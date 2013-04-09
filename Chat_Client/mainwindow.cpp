#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->Conexion = new ClientConnection();


    connect(Conexion, SIGNAL(newMessage(ClientConnection*,QString,QString)),
            this, SLOT(procesarMensaje(ClientConnection*,QString,QString)));

    connect(Conexion, SIGNAL(connected(ClientConnection*, QString)),
            this, SLOT(seConecto(ClientConnection*, QString)));

    connect(Conexion, SIGNAL(disconnected(ClientConnection*, QString)),
            this , SLOT(seDesconecto(ClientConnection*, QString)));

    connect(Conexion, SIGNAL(ConnectionError(ClientConnection*, QString,QString)),
            this , SLOT(errorConexion(ClientConnection*, QString,QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete this->Conexion;
}

void MainWindow::on_btnSalir_clicked()
{
    this->close();
}

void MainWindow::on_btnConnect_clicked()
{
    if(   this->ui->lineEdit_Nick->text().trimmed()==""
       || this->ui->lineEdit_Server->text().trimmed()==""
       || this->ui->lineEdit_Port->text().trimmed()==""
       )
    {
         QMessageBox::information(this,"Datos Vacios","Nick, Server o puerto vacios");
    }
    else
    {
        this->Conexion->conectarse(this->ui->lineEdit_Server->text(),
                                   this->ui->lineEdit_Port->text().toInt(),
                                   this->ui->lineEdit_Nick->text()
                                   );

    }
}

//----------------PUBLIC SLOTS ----------------------------
void MainWindow::procesarMensaje(ClientConnection *con, QString comando, QString mensaje)
{
    qDebug()<<"COMANDO"<<comando;
    //Dependiendo del comando hacer ciertas cosas, por ejemplo
//    this->ui->textEdit->append("Comando recibido:" + comando + ">>" + mensaje);
    if(comando=="NICKLIST")
    {
       // this->ui->textEdit->append("Recibi la lista de Nicks.");
        //Viene la lista de nicks, sin el mio, este viene en otro mensaje.
        // Estos vienen separados por comas
        QStringList listtemp = mensaje.split(",");
        QString temporal = listtemp.at(0);
        if(temporal=="X") return; //La lista esta vacia.
        int i=0;
        while(i<listtemp.count())
        {
            // Agregarla al listwidget
            this->ui->listWidget->addItem(listtemp.at(i));

            i++;
        }


    }
    if(comando=="NEWNICK")
    {
          //Hay un nuevo nick
          //Agregarlo a la lista, al final
          this->ui->listWidget->addItem(mensaje);
         // this->ui->textEdit->append("Nuevo nick ha llegado :" + mensaje);


    }
    if(comando=="OLDNICK")
    {
        //Revisar otros casos y validaciones
        int i=0;
        while(i<this->ui->listWidget->count())
        {
            if(mensaje==this->ui->listWidget->item(i)->text())
                break;

            i++;
        }
       delete this->ui->listWidget->takeItem(i);

    }
    if(comando=="MSGFROM")
    {
        //Obtener el nick
        QString elNick;
        int end = mensaje.indexOf(":");
        elNick=mensaje.mid(0,end);
        mensaje = mensaje.mid(end+1);
        this->ui->textEdit->append(elNick+">>"+mensaje);

    }
    if(comando=="SERVERSTOP")
    {
        this->ui->textEdit->append("El server se detuvo");

    }
     if(comando=="RESETGAME")
    {
        this->ui->textEdit->append("Reset el Game");

    }
    if(comando=="INFO")
    {
        this->ui->textEdit->append(mensaje);

    }
    if(comando=="MOVEPIEZA"){
        int fPiezaM=mensaje.mid(0,1).toInt();
        int cPiezaM=mensaje.mid(2,1).toInt();
        int fPiezaU=mensaje.mid(4,1).toInt();
        int cPiezaU=mensaje.mid(6,1).toInt();
        QString nombre=mensaje.mid(8);
        nombre=this->Conexion->nick+nombre;
        Tablero *temp=this->ventanas[nombre];
        qDebug()<<abs(fPiezaM-7)<<abs(cPiezaM-7)<<abs(fPiezaU-7)<<abs(cPiezaU-7);
        nombre=temp->area->getValor(abs(fPiezaM-7),abs(cPiezaM-7));
        temp->area->setValor(abs(fPiezaM-7),abs(cPiezaM-7),"0");
        temp->area->setValor(abs(fPiezaU-7),abs(cPiezaU-7),nombre);
        temp->area->update();
        temp->cambiarTurno();
        temp->turno=true;
    }
    if(comando=="VMOVEPIEZA"){
        int fPiezaM=mensaje.mid(0,1).toInt();
        int cPiezaM=mensaje.mid(2,1).toInt();
        int fPiezaU=mensaje.mid(4,1).toInt();
        int cPiezaU=mensaje.mid(6,1).toInt();
        QString nombre=mensaje.mid(8);
        Tablero*temp=this->views[nombre.mid(1)];
        if(mensaje.mid(8,1)=="2"){
            fPiezaM=abs(fPiezaM-7);
            cPiezaM=abs(cPiezaM-7);
            fPiezaU=abs(fPiezaU-7);
            cPiezaU=abs(cPiezaU-7);
        }
        nombre=temp->area->getValor(fPiezaM,cPiezaM);
        temp->area->setValor(fPiezaM,cPiezaM,"0");
        temp->area->setValor(fPiezaU,cPiezaU,nombre);
        temp->area->update();
        temp->turno=true;
    }
    if(comando=="MOVE")
    {
        int fila,col;
        fila=mensaje.mid(0,1).toInt();
        col=mensaje.mid(2,1).toInt();
        QString nombre=mensaje.mid(4);
        nombre=this->Conexion->nick+nombre;
        qDebug()<<"BuscarJuego"<<nombre;
        Tablero *temp=this->ventanas[nombre];
        qDebug()<<"Colocar"<<abs(fila-7)<<abs(col-7);
        temp->area->setCurrentCellOponent(abs(fila-7),abs(col-7));
        temp->area->update();
        qDebug()<<"coloco";
    }
    if(comando=="VMOVE"){
        int fila,col;
        fila=mensaje.mid(0,1).toInt();
        col=mensaje.mid(2,1).toInt();
        QString nombre=mensaje.mid(4);
        Tablero *temp=this->views[nombre.mid(1)];
        if(mensaje.mid(4,1)=="2"){
            fila=abs(fila-7);
            col=abs(col-7);
            temp->area->setCurrentCellOponent(fila,col);
        }else{
            temp->area->setCurrentCell(fila,col);
        }
        temp->area->update();
    }
    if(comando=="RETO"){
        this->ui->listWidgetRetos->addItem(mensaje);
    }
    if(comando=="RETOINVALIDO"){
        QMessageBox::information(this,"Reto inválido",mensaje);
    }
    if(comando=="NEWGAME"){
        QString color=mensaje.at(0);
        QString nombre=this->Conexion->nick+mensaje.mid(1);
        Tablero *temp=new Tablero(color,mensaje.mid(1),this->Conexion);
        mensaje="Has iniciado un nuevo juego con el jugador "+mensaje.mid(1);
        ui->textEdit->append(mensaje);
        qDebug()<<"Nombre"<<nombre;
        ventanas.insert(nombre,temp);
        connect(ventanas[nombre],SIGNAL(cerrar(QString,bool)),this,SLOT(cerrarVentana(QString,bool)));
        ventanas[nombre]->show();
    }
    if(comando=="JUEGOINVALIDO"){
        QMessageBox::information(this,"Juego inválido",mensaje);
    }
    if(comando=="ANADIRGAME"){
        ui->listWidgetGames->addItem(mensaje);
    }
    if(comando=="BORRARRETO"){
        int val=ui->listWidgetRetos->count();
        qDebug()<<mensaje;
        mensaje="Reto del jugador "+mensaje;
        for(int i=0;i<val;i++){
            if(ui->listWidgetRetos->item(i)->text()==mensaje){
                delete ui->listWidgetRetos->item(i);
                break;
            }
        }
    }

    if(comando=="SACARLISTA"){
        for(int i=0;i<ui->listWidgetGames->count();i++){
            QString texto=ui->listWidgetGames->item(i)->text().mid(13);
            qDebug()<<texto;
            if(texto==mensaje){
                delete ui->listWidgetGames->item(i);
            }
        }
    }
    if(comando=="BORRARGAME"){
        QStringList nicks=mensaje.split("|[]|");
        for(int i=0;i<nicks.count()-1;i++){
            for(int e=0;e<ui->listWidgetGames->count();e++){
                qDebug()<<"for";
                QString texto=ui->listWidgetGames->item(e)->text().mid(13);
                qDebug()<<texto<<nicks.at(i);
                if(texto.startsWith(nicks.at(i))){
                    qDebug()<<"Delete";
                    delete ui->listWidgetGames->item(e);
                    qDebug()<<"Delete hecho";
                    e=0;
                    break;
                }
            }
        }
    }
    if(comando=="CLOSEGAME"){
        QString name=this->Conexion->nick+mensaje;
        qDebug()<<"Close game"<<name;
        qDebug()<<this->ventanas.contains(name);
        Tablero *temp=this->ventanas[name];
        temp->cerrarVentana();
        qDebug()<<"Borrar game";
        delete temp;
        qDebug()<<"Game borrado";
    }
    if(comando=="CERRARVISTA"){
        qDebug()<<"ENTRO ACA CERRARVISTA"<<mensaje;
        Tablero*temp=this->views[mensaje];
        temp->cerrarVentana();
        delete temp;
    }

    if(comando=="VIEWEREXISTE"){
        qDebug()<<mensaje;
        if(mensaje=="SAME"){
            QMessageBox::information(this,"Acción inválida","No puedes ser un viewer de uno de tus propias juegos ya en curso.");
            return;
        }
        QMessageBox::information(this,"Acción Inválida","Ya eres viewer de esa partida actualmente.");
    }
    if(comando=="NEWVIEWER"){
        qDebug()<<"VIEWERENTRO";
        QString nom=mensaje.mid(0,mensaje.indexOf(":"));
        mensaje.remove((nom+":"));
        QStringList partes=mensaje.split(":otro:");
        QStringList player1=partes.at(0).split(":");
        QStringList player2=partes.at(1).split(":");
        QStringList templ=nom.split(" vs ");
        Tablero *temp=new Tablero("0",templ.at(1),this->Conexion,0,true,templ.at(0));
        this->views.insert(nom,temp);
        int tam=player1.length();
        qDebug()<<"Stats"<<player1<<player2<<templ;
        qDebug()<<"For1";
        for(int i=0;i<tam;i++){
            int f=player1.at(i).mid(0,1).toInt();
            int c=player1.at(i).mid(1,1).toInt();
            QString val=player1.at(i).mid(2,player1.at(i).length());
            qDebug()<<f<<c<<val;
            views[nom]->area->setValor(f,c,val);
        }
        qDebug()<<"For1 superado";
        qDebug()<<"For2";
        tam=player2.length();
        for(int i=0;i<tam;i++){
            int f=player2.at(i).mid(0,1).toInt();
            int c=player2.at(i).mid(1,1).toInt();
            QString val=player2.at(i).mid(2,player2.at(i).length());
            qDebug()<<abs(f-7)<<abs(c-7)<<val;
            views[nom]->area->setValor(abs(f-7),abs(c-7),val);
        }
        qDebug()<<"For2 superado";
        views[nom]->show();
    }


}

void MainWindow::seConecto(ClientConnection *con, QString nick)
{
    this->ui->textEdit->append("Confirmando conexion .....");
    this->ui->btnConnect->setEnabled(false);
    this->ui->btnDisconnect->setEnabled(true);
    con->estaConectado = true;


}
void MainWindow::seDesconecto(ClientConnection *con, QString nick)
{
    this->ui->btnConnect->setEnabled(true);
    this->ui->btnDisconnect->setEnabled(false);
    this->ui->textEdit->append("Conexion cerrada ..." + nick);
    con->nick="";
    this->ui->listWidget->clear();
    this->ui->listWidgetRetos->clear();
    this->ui->listWidgetGames->clear();

}
void MainWindow::errorConexion(ClientConnection*, QString titulo,QString mensaje)
{
    this->ui->textEdit->append("ERROR:"+titulo + ">>"+ mensaje);

}

void MainWindow::on_lineEdit_Mensaje_returnPressed()
{
    if(this->Conexion->estaConectado)
    {
       if(this->ui->lineEdit_Mensaje->text().trimmed()!="")
          this->Conexion->sendMessage("MSG:" +
                                      this->ui->lineEdit_Mensaje->text().trimmed() +
                                      "\n\r");
       this->ui->lineEdit_Mensaje->clear();


    }
    if(this->ui->lineEdit_Mensaje->text().trimmed()!="")
      this->ui->webView->setUrl("http://www.google.com/search?q=" + this->ui->lineEdit_Mensaje->text().trimmed());

}

void MainWindow::on_btnDisconnect_clicked()
{
    this->Conexion->disconnect();
    qDebug()<<"ENTRO POR ACA";
    QMap<QString,Tablero*>::ConstIterator it;
    for(it=this->ventanas.constBegin();it!=ventanas.constEnd();it++){
        qDebug()<<"FOR1"<<it.value();
        Tablero *temp=it.value();
        temp->muerte=true;
        temp->cerrarVentana();
        delete temp;
        //this->ventanas.erase(it);
    }
    qDebug()<<"tamanio"<<views.count();
    for(it=this->views.constBegin();it!=views.constEnd();it++){
        qDebug()<<"FOR2"<<it.value();
        Tablero *temp=it.value();
        temp->muerte=true;
        temp->cerrarVentana();
        delete temp;
        //this->views.erase(it);
    }


}


void MainWindow::on_lineEdit_editingFinished()
{
  this->ui->webView->setUrl("http://www.google.com/search?q=" + this->ui->lineEdit->text());
}



void MainWindow::on_btnRetar_clicked()
{

    if(ui->listWidget->currentRow()==-1){
        QMessageBox::information(this,"Jugador Inválido","Debe seleccionar un jugador a retar.");
    }else{
        QString temp=ui->listWidget->currentItem()->text();
        this->Conexion->sendMessage("RETAR:"+temp+"\n\r");
    }
}

void MainWindow::on_btnAceptarReto_clicked()
{
    if(ui->listWidgetRetos->currentRow()==-1){
        QMessageBox::information(this,"Reto inválido",
                                 "Debe seleccionar un reto primero.");
    }else{
        QString temp=ui->listWidgetRetos->currentItem()->text().remove("Reto del jugador ");
        delete ui->listWidgetRetos->takeItem(ui->listWidgetRetos->currentRow());
        this->Conexion->sendMessage("ACEPTARRETO:"+temp+"\n\r");


    }
}

void MainWindow::on_btnRechazarReto_clicked()
{

    if(ui->listWidgetRetos->currentRow()==-1){
        QMessageBox::information(this,"Reto inválido",
                                 "Debe seleccionar un reto primero.");
    }else{
        QString temp=ui->listWidgetRetos->currentItem()->text().remove("Reto del jugador ");
        delete ui->listWidgetRetos->takeItem(ui->listWidgetRetos->currentRow());
        this->Conexion->sendMessage("RECHAZARRETO:"+temp+"\n\r");
    }
}

void MainWindow::on_btnVisualizarGame_clicked()
{
    if(ui->listWidgetGames->currentRow()==-1){
        QMessageBox::information(this,"Juego inválido","Debe seleccionar un juego a visualizar.");
        return;
    }
    QString juego=ui->listWidgetGames->currentItem()->text();
    qDebug()<<"Envio"<<juego;
    this->Conexion->sendMessage("NEWVIEWER:"+juego+"\n\r");

}
void MainWindow::cerrarVentana(QString vent,bool val){
    if(!val){
        delete this->ventanas[vent];
        this->ventanas.remove(vent);
    }else{
        delete this->views[vent];
        this->views.remove(vent);
    }
}
