#include "server.h"
#include <QDebug>

Server::Server()
{
    //READ: Iinicializar la bandera en false, enlazar o conectar el evento o señal
    // de newConnection que genere una instancia de esta clase y que la reciba y procese
    // esta misma instancia por el SLOT newConnection ..
    // Limpiar la lista de conexiones
    // Hacer que el apuntador al TextEdit(Log) sea nulo
    this->server_started = false;
    connect(this, SIGNAL(newConnection()), this, SLOT(newConnection()));
    this->Lista_Conexiones.clear();
    this->casillas = 0; // llleva un recuento si ya se llenaron las 64 casillas
    this->log = 0;
    for (int i=0; i<8 ; i++)
      for (int j=0; j<8 ; j++)
          this->TABLERO[i][j]=-1;
    this->repetido=false;

}
void Server::reset()
{
    for (int i=0; i<8 ; i++)
      for (int j=0; j<8 ; j++)
          this->TABLERO[i][j]=-1;
    this->casillas=0;
     int i=0;
    while (i<this->Lista_Conexiones.count())
   {
        if(this->Lista_Conexiones.at(i)!=0){
            this->Lista_Conexiones.at(i)->veces=0;
        }
        i++;
   }

}

Server::~Server()
{
    //READ: Simplemente llamar al metodo STOP que hace toda la limpieza
    this->stop();
}

void Server::start(int port,int cuantas_conexiones)
{
    //READ: Bueno iniciar el server siempre y cuando NO se haya iniciado previamente
    // para eso se revisa la bandera. Luego ejecutamos el metodo LISTEN para escuchar
    // en el puerto que recibe de parametro, en caso que no pueda escuchar en ese
    // puerto porque por ejemplo puede que ya este ocupado entonces, mostrar un mensaje
    // y dejar en false la bandera.
    // Si logra arranca, entonces colocar la bandera en True, instancia la nickList
    // Limpia la lista de conexiones y muestra un mensaje en el LOG.
    if (this->server_started==false)
    {
        if (listen(QHostAddress::Any, port))
        {
           this->server_started = true;
           this->nickList = new QStringList();
           this->Lista_Conexiones.clear();
           this->log->append(tr("Servidor iniciado en puerto :%1 ").arg(port));
           this->Cantidad_Conexiones = cuantas_conexiones;
        }
        else
        {
            this->server_started = false;
            this->log->append(tr("Servidor no se pudo iniciar :%1 ").arg(this->errorString()));
        }
    }

}
void Server::stop()
{
    if (this->server_started)
    {

      this->server_started = false;
      delete this->nickList;
      // Limpiar la lista de conexiones

       while (!this->Lista_Conexiones.isEmpty())
         {
            // this->Lista_Conexiones.at(0)->disconnect(); //Desconectar la conexion

             Connection *cTemp= this->Lista_Conexiones.takeFirst();  // Libera el apuntador
             cTemp->sendMessage("SERVERSTOP:\n\r");
             cTemp->socket->close();
             delete cTemp;
         }
        //Cerrar el socket del server
        this->close();
        this->Cantidad_Conexiones=0;
        this->casillas=0;
        this->log->append(tr("Servidor detenido!!! "));
    }

}
bool Server::getStatus()
{
    return this->server_started;
}
void Server::newConnection() {
        // Cuando se crea la conexion la enlazo agregandola a lista de conexiones

    if(this->Lista_Conexiones.count()<this->Cantidad_Conexiones)
    {
        Connection *con = new Connection(nextPendingConnection());
         // Agregarla a la lista de conexiones

        int val=this->conexionesDisponibles();
        qDebug()<<"Valor"<<val;
        if(val==-1){
            qDebug()<<this->Lista_Conexiones.count()<<"aniadido";
            this->Lista_Conexiones.push_back(con);
             con->indice_lista_conexion = this->Lista_Conexiones.count()-1;
         }else{
             qDebug()<<"Reemplazar"<<val<<con->nick;
             this->Lista_Conexiones.replace(val,con);
             con->indice_lista_conexion=val;
             qDebug()<<"paso de acá";
         }
        connect(con,SIGNAL(newProcess(Connection*,QString)),this,SLOT(procesar(Connection*,QString)));
        connect(con,SIGNAL(newViewer(Connection*,QString)),this,SLOT(procesarViewer(Connection*,QString)));
        connect(con,SIGNAL(newMovePieza(Connection*,QString)),this,SLOT(procesarMovimientoPieza(Connection*,QString)));
        connect(con, SIGNAL(newMessage(Connection*,QString)), this, SLOT(procesarMensaje(Connection*,QString)));
        connect(con, SIGNAL(newMove(Connection*,QString)), this, SLOT(procesarMovimiento(Connection*,QString)));
        connect(con,SIGNAL(newChallenge(Connection*,QString)),this,SLOT(procesarChallenges(Connection*,QString)));
        // connect(this, SIGNAL(newMessage(QString)), con, SLOT(sendMessage(QString)));
        connect(con, SIGNAL(connected(Connection*, QString)), this, SLOT(connected(Connection*, QString)));
        connect(con, SIGNAL(disconnected(Connection*, QString)), this , SLOT(disconnected(Connection*, QString)));
        connect(con,SIGNAL(modificarListas(Connection*,QString)),this,SLOT(procesarCambiosListas(Connection*,QString)));

        this->log->append("Nueva conexion..." + con->socket->peerAddress().toString());
    }
    else
    {
        this->nextPendingConnection()->close();
        this->log->append("Conexion rechazada, se llego al limite..." );
     }
}
void Server::procesar(Connection *con, QString mensaje){
    if(mensaje.startsWith("CLOSEGAME:")){
        mensaje.remove("CLOSEGAME:");

        this->sendMessageTo(mensaje,"CLOSEGAME:"+con->nick+"\n\r");

        if(juegos.contains(mensaje,con->nick)){
            juegos.remove(mensaje,con->nick);
            this->sendMessage("SACARLISTA:"+mensaje+" vs "+con->nick+"\n\r");
        }else{
            juegos.remove(con->nick,mensaje);
            this->sendMessage("SACARLISTA:"+con->nick+" vs "+mensaje+"\n\r");
        }
        QString la="";
        la=mensaje+" vs "+con->nick;
        fichas.remove(la);
        la=con->nick+" vs "+mensaje;
        fichas.remove(la);
    }
    if(mensaje.startsWith("CERRARVISTA:")){
        mensaje.remove("CERRARVISTA:");
        this->Viewers.remove(con->nick,mensaje);
    }
    if(mensaje.startsWith("CLOSEVIEWS:")){
        mensaje.remove("CLOSEVIEWS:");
        qDebug()<<"CLOSEVIEWS"<<mensaje;
        QList<QString> temp=this->Viewers.keys(mensaje);
        for(int i=0;i<temp.length();i++){
            qDebug()<<temp[i]<<mensaje;
            this->sendMessageTo(temp[i],"CERRARVISTA:"+mensaje+"\n\r");
        }
    }
}

void Server::connected(Connection* con, QString nick) {

    if(!(nick.isEmpty() || this->nickList->contains(nick))){
        con->sendNicks(this->nickList);
        this->log->append("Nicks enviados al resto...");
        con->nick=nick;
        nickList->push_back(nick);
        // Enviar un mensaje a todos de este nuevo Nick
        this->sendMessage("NEWNICK:"+nick + "\n\r");
        this->log->append(nick + " [" + con->socket->peerAddress().toString() + "] connected");
        QMultiMap<QString,QString>::ConstIterator it;
        for(it=this->juegos.constBegin();it!=juegos.constEnd();it++){
            QString mensaje="Juego entre: "+it.key()+" vs "+it.value();
            mensaje="ANADIRGAME:"+mensaje+"\n\r";
            qDebug()<<nick<<mensaje;
            con->sendMessage(mensaje);
            qDebug()<<"Mensaje enviado";
        }
    }else{

        this->repetido=true;
        con->socket->close();
    }

}
int Server::conexionesDisponibles(){
    for(int i=0;i<this->Lista_Conexiones.count();i++){
        if(Lista_Conexiones.at(i)==0){
            return i;
        }
    }
    return -1;
}

void Server::disconnected(Connection* con, QString nick) {
        //con->deleteLater();
    if(!repetido){
        qDebug()<<"NO ES REPETIDO";
        int index = nickList->indexOf(nick);
        qDebug()<<"Index"<<index;
        if(index!=-1){
            qDebug()<<"Remover";
            nickList->removeAt(index);
        }
        qDebug()<<"eliminarinfo";
        this->eliminarInfo(nick);
        // Remover de la lista de conexiones.
        this->log->append(nick + " disconnected");
    }
    qDebug()<<"Es repetido";
    int pos=con->indice_lista_conexion;
    qDebug()<<"Posicion"<<pos;
    if(!repetido)
        this->Lista_Conexiones.at(pos)->socket->close();
    //Avisar al resto que se desconecto
    if(!repetido){
        this->sendMessage("OLDNICK:"+nick + "\n\r");

    }
    qDebug()<<"Delete"<<pos;
    delete this->Lista_Conexiones.value(pos);
    this->Lista_Conexiones.replace(pos,0);
    qDebug()<<"Reemplazado"<<pos;
    this->repetido=false;
}
void Server::sendMessage(QString mensaje)
{ // Recorrer la lista de conexiones y enviar
   int i=0;
    while (i<this->Lista_Conexiones.count())
   {
        if(this->Lista_Conexiones.at(i)!=0){
            if(this->Lista_Conexiones.at(i)->socket->isValid())
               this->Lista_Conexiones.at(i)->sendMessage(mensaje);
        }
    //TODO: Deberiamos validar el estado de la conexion
        // Para no tratar de enviar en Conexiones cerradas
        // O en el disconnect asegurarse de poner en estado no valida
        // la conexion. Porque sino se pierde el indice.

    i++;
   }


}
void Server::procesarMensaje(Connection *con, QString mensaje)
{
    int i=0;
    while (i<this->Lista_Conexiones.count())
   {
        // Enviar al resto lo que recibimos de con, inclusive al mismo (ECHO)
        if(this->Lista_Conexiones.at(i)!=0){
            if(this->Lista_Conexiones.at(i)->socket->isValid())
            {
                this->log->append(con->nick+">>Enviando:(" +this->Lista_Conexiones.at(i)->nick+"):" + mensaje);
                this->Lista_Conexiones.at(i)->sendMessage(mensaje);
             }
        }
    i++;
   }

}
void Server::procesarMovimientoPieza(Connection *con, QString mensaje){
    QString nombre=mensaje.mid(8);
    int fPiezaM=mensaje.mid(0,1).toInt();
    int cPiezaM=mensaje.mid(2,1).toInt();
    int fPiezaU=mensaje.mid(4,1).toInt();
    int cPiezaU=mensaje.mid(6,1).toInt();
    QString msgView="VMOVEPIEZA:"+mensaje.mid(0,8);
    mensaje=mensaje.mid(0,8)+con->nick;
    mensaje="MOVEPIEZA:"+mensaje+"\n\r";
    qDebug()<<"Move Pieza"<<mensaje;
    this->sendMessageTo(nombre,mensaje);
    this->moveViewers(con->nick,nombre,msgView);
    nombre=con->nick+" vs "+nombre;
    this->moverPieza(nombre,fPiezaM,cPiezaM,fPiezaU,cPiezaU);
}
void Server::moveViewers(QString nombre1,QString nombre2, QString msg){
    QString value=nombre1+" vs "+nombre2;
    QList<QString> nombres=this->Viewers.keys(value);
    for(int i=0;i<nombres.length();i++){
        QString temp=msg+"1"+value+"\n\r";
        qDebug()<<nombres.at(i)<<temp;
        this->sendMessageTo(nombres.at(i),temp);
    }
    value=nombre2+" vs "+nombre1;
    nombres=this->Viewers.keys(value);
    for(int i=0;i<nombres.length();i++){
        QString temp=msg+"2"+value+"\n\r";
        qDebug()<<nombres.at(i)<<temp;
        this->sendMessageTo(nombres.at(i),temp);
    }
}

void Server::procesarMovimiento(Connection *con, QString mensaje)
{
        QString msgView="VMOVE:"+mensaje.mid(0,4);
        qDebug()<<"Mensaje Move"<<mensaje;
        QString nombre=mensaje.mid(4);
        qDebug()<<"Nombre"<<nombre;
        mensaje=mensaje.mid(0,4)+con->nick;
        mensaje="MOVE:"+mensaje+"\n\r";
        this->sendMessageTo(nombre,mensaje);
        this->moveViewers(con->nick,nombre,msgView);
}

void Server::procesarChallenges(Connection *con, QString retado){
    QString mensaje;
    mensaje="RETO:"+con->nick+"\n\r";
    int i=0;
    if(!(this->buscarSolicitud(retado,con->nick) || con->nick==retado ||this->buscarJuego(con->nick,retado))){
        while(i<this->Lista_Conexiones.count()){
            if(this->Lista_Conexiones.at(i)!=0){
                if(this->Lista_Conexiones.at(i)->nick==retado){
                    this->solicitudes.insert(retado,con->nick);
                    this->Lista_Conexiones.at(i)->sendMessage(mensaje);
                    break;
                }
            }
            i++;
        }
    }else{
        con->sendMessage("RETOINVALIDO:\n\r");
    }

}
bool Server::buscarJuego(QString jugador1, QString jugador2){
    if(this->juegos.contains(jugador1,jugador2)||
       this->juegos.contains(jugador2,jugador1)){
        return true;
    }else{
        return false;
    }

}
bool Server::buscarSolicitud(QString retado, QString retador){
    if(this->solicitudes.contains(retado,retador)||
       this->solicitudes.contains(retador,retado)){
        return true;
    }else{
        return false;
    }
}
void Server::procesarCambiosListas(Connection *con, QString mensaje){
    qDebug()<<"CambiosListas"<<mensaje;
    QString msg;
    if(mensaje.startsWith("ACEPTARRETO:")){
        qDebug()<<"Entro";
        msg=mensaje.mid(12);
        this->solicitudes.remove(con->nick,msg);
        qDebug()<<"buscar"<<con->nick<<msg;
        if(!this->buscarJuego(con->nick,msg)){
            int i=0;
            while(i<this->Lista_Conexiones.count()){
                if(this->Lista_Conexiones.at(i)!=0){
                    qDebug()<<"Compara"<<this->Lista_Conexiones.at(i)->nick<<msg;
                    if(this->Lista_Conexiones.at(i)->nick==msg){
                        this->juegos.insert(con->nick,msg);
                        qDebug()<<"Enviar señales";
                        con->sendMessage("NEWGAME:0"+msg+"\n\r");
                        this->Lista_Conexiones.at(i)->sendMessage("NEWGAME:1"+con->nick+"\n\r");
                        QString mensaje="Juego entre: "+con->nick+" vs "+msg;
                        this->sendMessage("ANADIRGAME:"+mensaje+"\n\r");
                        QString nombres=con->nick+" vs "+msg;
                        qDebug()<<"Guarda"<<nombres;
                        this->agregarJuego(nombres,0);
                        nombres=msg+" vs "+con->nick;
                        qDebug()<<"Guarda"<<nombres;
                        this->agregarJuego(nombres,1);
                        break;
                    }
                }
                i++;
            }
        }else{
            con->sendMessage("JUEGOINVALIDO:"+msg+"\n\r");
        }
    }
    if(mensaje.startsWith("RECHAZARRETO:")){
        msg=mensaje.mid(13);
        this->solicitudes.remove(con->nick,msg);
    }
}
void Server::agregarJuego(QString jugadores,int C){
    QList<Ficha> lista;
    QString color;
    if(C==0){
        color="W";
    }else{
        color="B";
    }
    lista.append(Ficha(C,7,0,color+"T"));
    lista.append(Ficha(C,7,1,color+"C"));
    lista.append(Ficha(C,7,2,color+"A"));
    lista.append(Ficha(C,7,3,color+"V"));
    lista.append(Ficha(C,7,4,color+"R"));
    lista.append(Ficha(C,7,5,color+"A"));
    lista.append(Ficha(C,7,6,color+"C"));
    lista.append(Ficha(C,7,7,color+"T"));
    for(int i=0;i<8;i++){
        lista.append(Ficha(C,6,i,color+"P"));
    }
    this->fichas.insert(jugadores,lista);
}
void Server::moverPieza(QString nombreJuego, int fPiezaM, int cPiezaM, int fPiezaU, int cPiezaU){
    int tam=this->fichas[nombreJuego].length();
    qDebug()<<"valores"<<fPiezaM<<cPiezaM<<fPiezaU<<cPiezaU;
    for(int i=0;i<tam;i++){
        if(fichas[nombreJuego].at(i).posX==fPiezaM && fichas[nombreJuego].at(i).posY==cPiezaM){
            qDebug()<<"MovPieza"<<fichas[nombreJuego].at(i).posX<<fichas[nombreJuego].at(i).posY;
            fichas[nombreJuego].operator [](i).posX=fPiezaU;
            fichas[nombreJuego].operator [](i).posY=cPiezaU;
            qDebug()<<"MovPieza2"<<fichas[nombreJuego].at(i).posX<<fichas[nombreJuego].at(i).posY;
            break;
        }
    }


}

void Server::eliminarSolicitudes(QString nick){
    QList<QString>temp=this->solicitudes.keys(nick);
    int tam=temp.count();
    for(int i=0;i<tam;i++){
        QString mensaje="BORRARRETO:"+nick+"\n\r";
        qDebug()<<temp[i]<<mensaje;
        this->sendMessageTo(temp[i],mensaje);
        this->solicitudes.remove(temp[i],nick);
    }
    temp=this->solicitudes.values(nick);
    tam=temp.count();
    for(int i=0;i<tam;i++){
        this->solicitudes.remove(nick,temp[i]);
    }
}
void Server::eliminarJuegos(QString nick){
    QList<QString>temp=this->juegos.keys(nick);
    int tam=temp.count();
    QString mensaje;
    for(int i=0;i<tam;i++){
        qDebug()<<"First"<<temp[i]<<"CLOSEGAME: "+nick+"\n\r";
        this->sendMessageTo(temp[i],"CLOSEGAME:"+nick+"\n\r");
        this->juegos.remove(temp[i],nick);
        QString la=temp[i]+" vs "+nick;
        mensaje+=la+"|[]|";
        fichas.remove(la);
        la=nick+" vs "+temp[i];
        fichas.remove(la);
    }
    temp=this->juegos.values(nick);
    tam=temp.count();
    for(int i=0;i<tam;i++){
        qDebug()<<"Second"<<temp[i]<<"CLOSEGAME: "+nick+"\n\r";
        this->sendMessageTo(temp[i],"CLOSEGAME:"+nick+"\n\r");
        this->juegos.remove(nick,temp[i]);
        QString la=nick+" vs "+temp[i];
        mensaje+=la+"|[]|";
        fichas.remove(la);
        la=temp[i]+" vs "+nick;
        fichas.remove(la);
    }
    if(!mensaje.isNull()){
        mensaje="BORRARGAME:"+mensaje+"\n\r";
        this->sendMessage(mensaje);
    }
}


void Server::eliminarInfo(QString nick){
    this->eliminarSolicitudes(nick);
    this->eliminarJuegos(nick);
    this->eliminarViewers(nick);
}
void Server::eliminarViewers(QString nick){
    QList<QString>temp=this->Viewers.values(nick);
    int tam=temp.count();
    for(int i=0;i<tam;i++){
        this->Viewers.remove(nick,temp[i]);
    }
}

void Server::sendMessageTo(QString nick, QString mensaje){
    int i=0;
     while (i<this->Lista_Conexiones.count())
    {
         if(this->Lista_Conexiones.at(i)!=0){
             if(this->Lista_Conexiones.at(i)->socket->isValid()){
                 qDebug()<<"Compara"<<this->Lista_Conexiones.at(i)->nick<<nick;
                 if(this->Lista_Conexiones.at(i)->nick==nick){
                     qDebug()<<"Enviando mensaje"<<mensaje;
                     this->Lista_Conexiones.at(i)->sendMessage(mensaje);
                     qDebug()<<"Break";
                     break;
                 }
             }
         }
         i++;
     }
}
void Server::procesarViewer(Connection *con, QString mensaje){
    qDebug()<<"entroView"<<mensaje;
    mensaje.remove("Juego entre: ");
    QStringList val= mensaje.split(" vs ");
    if(this->buscarViewer(con->nick,mensaje)){
        con->sendMessage("VIEWEREXISTE:"+mensaje+"\n\r");
        return;
    }else if(con->nick==val.at(0) || con->nick==val.at(1)){
        con->sendMessage("VIEWEREXISTE:SAME\n\r");
        return;
    }
    int tam=this->fichas[mensaje].length();

    QString temp="";
    for(int i=0;i<tam;i++){
        QString val,val2;;
        temp+=val.setNum(fichas[mensaje].at(i).posX)+val2.setNum(fichas[mensaje].at(i).posY)+
              fichas[mensaje].at(i).tipo+":";
    }
    this->Viewers.insert(con->nick,mensaje);
    QStringList list=mensaje.split(" vs ");
    mensaje=list.at(1)+" vs "+list.at(0);
    tam=this->fichas[mensaje].length();
    temp+="otro:";
    for(int i=0;i<tam;i++){
        QString val,val2;
        temp+=val.setNum(fichas[mensaje].at(i).posX)+val2.setNum(fichas[mensaje].at(i).posY)+
              fichas[mensaje].at(i).tipo+":";
    }

    mensaje=list.value(0)+" vs "+list.value(1)+":"+temp.mid(0,temp.length()-1);
    qDebug()<<con->nick<<mensaje;
    con->sendMessage("NEWVIEWER:"+mensaje+"\n\r");
}
bool Server::buscarViewer(QString jugador, QString juego){
    return this->Viewers.contains(jugador,juego);
}


