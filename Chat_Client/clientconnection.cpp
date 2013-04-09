#include "clientconnection.h"

ClientConnection::ClientConnection()
{
    //READ: Instanciar Socket y conectar las señales, la
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(conectado()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(recv()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    this->nick="";
    this->estaConectado =false;
}
void ClientConnection::recv() {
        message.append(socket->readAll());

        int pos;
        while((pos = message.indexOf("\n\r")) > -1) {
                parseMessage(QString(message.left(pos+2)));
                message = message.mid(pos+2);
        }
}
void ClientConnection::conectarse(QString host, int port,QString nick)
{
  this->socket->abort();
  this->nick = nick;
  this->socket->connectToHost(host, port);
}

void ClientConnection::conectado()
{

    this->sendMessage("CON:" + this->nick + "\n\r");
   //Avisar a la pantalla principal que logre conectarme pero no confirmado el nick
    this->estaConectado=false; // Se confirma hasta que recibimos nuestro nick otra vez


}

void ClientConnection::parseMessage(QString msg) {
    qDebug()<<"MENSAJEP"<<msg;
    //READ: Emitir los comandos con sus respectivos mensajes
       if(msg.startsWith("NICKLIST:"))
       {
           msg = msg.mid(9);
           msg = msg.mid(0,msg.length()-2);
           qDebug()<<"Envio NickList"<<msg;
           emit newMessage(this,"NICKLIST",msg);
       }
       if(msg.startsWith("NEWNICK:"))
       {
           msg = msg.mid(8);
           msg = msg.mid(0,msg.length()-2);
           emit newMessage(this,"NEWNICK",msg);
           if (this->nick==msg){ // Es mi mismo nick, entonces confirmar
                emit connected(this, nick);
                qDebug()<<"Envio NewNick"<<nick;
            }
       }
       if(msg.startsWith("OLDNICK:"))
       {
           msg = msg.mid(8);
           msg = msg.mid(0,msg.length()-2);
           emit newMessage(this,"OLDNICK",msg);
       }
        if(msg.startsWith("MSGFROM:"))
       {
           msg = msg.mid(8);
           msg = msg.mid(0,msg.length()-2);
           qDebug()<<"Envio msgFrom"<<msg;
           emit newMessage(this,"MSGFROM",msg);
       }
       if(msg.startsWith("SERVERSTOP:"))
       {
           qDebug()<<"Envio ServerStop"<<"(si vacio)";
           emit newMessage(this,"SERVERSTOP","");
       }
       if(msg.startsWith("RESETGAME:"))
       {
           qDebug()<<"Envio ResetGame"<<"(si vacio)";
           emit newMessage(this,"RESETGAME","");
       }
       if(msg.startsWith("MOVE:"))
       {
           qDebug()<<msg;
           msg = msg.mid(5);
           msg = msg.mid(0,msg.length()-2);
           emit newMessage(this,"MOVE",msg);
       }
       if(msg.startsWith("VMOVE:")){
           msg=msg.mid(6);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"VMOVE",msg);
       }
       if(msg.startsWith("MOVEPIEZA:")){
           msg=msg.mid(10);
           msg=msg.mid(0,msg.length()-2);
           qDebug()<<msg<<"enviado";
           emit newMessage(this,"MOVEPIEZA",msg);
       }
       if(msg.startsWith("VMOVEPIEZA:")){
           msg=msg.mid(11);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"VMOVEPIEZA",msg);
       }
       if(msg.startsWith("INFO:"))
       {
           msg = msg.mid(5);
           msg = msg.mid(0,msg.length()-2);
           qDebug()<<"Envio Info"<<msg;
           emit newMessage(this,"INFO",msg);
       }
       if (msg.startsWith("RETO:")){
           msg=msg.mid(5);
           msg=msg.mid(0,msg.length()-2);
           msg="Reto del jugador "+msg;
           emit newMessage(this,"RETO",msg);
       }
       if(msg.startsWith("RETOINVALIDO:")){
           msg="Ya hay una solicitud de reto o juego en progreso, o bien te has retado a ti mismo (ForeverAlone).";
           emit newMessage(this,"RETOINVALIDO",msg);
       }
       if(msg.startsWith("NEWGAME:")){
           msg=msg.mid(8);
           msg=msg.mid(0,msg.length()-2);
           qDebug()<<"Emite"<<msg;
           emit newMessage(this,"NEWGAME",msg);
       }
       if(msg.startsWith("JUEGOINVALIDO:")){
           msg=msg.mid(14);
           msg=msg.mid(0,msg.length()-2);
           msg="Ya tienes un juego en curso con el jugador"+msg;
           emit newMessage(this,"JUEGOINVALIDO",msg);
       }
       if(msg.startsWith("ANADIRGAME:")){
           msg=msg.mid(11);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"ANADIRGAME",msg);
       }

       if(msg.startsWith("BORRARRETO:")){
           msg=msg.mid(11);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"BORRARRETO",msg);
       }
       if(msg.startsWith("BORRARGAME:")){
           msg=msg.mid(11);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"BORRARGAME",msg);
       }
       if(msg.startsWith("CLOSEGAME:")){
           msg=msg.mid(10);
           msg=msg.mid(0,msg.length()-2);
           qDebug()<<"emite"<<"CLOSEGAME"<<msg;
           emit newMessage(this,"CLOSEGAME",msg);
       }
       if(msg.startsWith("SACARLISTA:")){
           msg=msg.mid(11);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"SACARLISTA",msg);
       }
       if(msg.startsWith("VIEWEREXISTE:")){
           msg=msg.mid(13);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"VIEWEREXISTE",msg);
       }
       if(msg.startsWith("NEWVIEWER:")){
           msg=msg.mid(10);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"NEWVIEWER",msg);
       }
       if(msg.startsWith("CERRARVISTA:")){
           msg=msg.mid(12);
           msg=msg.mid(0,msg.length()-2);
           emit newMessage(this,"CERRARVISTA",msg);
       }




      /*          int start = msg.indexOf("Nick: ");
                int end = msg.indexOf("\n", start);
                QString newNick = msg.mid(start + 6, end - start - 6);
*/

}
void ClientConnection::sendMessage(QString msg) {
    //READ: Si el socket de la conexion es valido entonces enviar el mensaje
    // Pero convertirlo en ASCII
    qDebug()<<"Move enviado"<<msg;
    if (socket->isValid())
        socket->write(msg.toAscii());

}
void ClientConnection::disconnect() {
       //READ: Emitir una señal al server para desconectarse
     this->estaConectado =false;
     this->socket->close();
     emit disconnected(this, nick);

}
void ClientConnection::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
          emit ConnectionError(this, tr("Chat Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        emit ConnectionError(this, tr("Chat Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the Chat server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        emit ConnectionError(this, tr("Chat Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(socket->errorString()));
    }


}

