#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //READ: Instanciar el Server de Char
    this->ChatServer = new Server();
    //READ: Definir al server de Chat, el LOG (textEdit)
    this->ChatServer->log = this->ui->textEdit;
}

MainWindow::~MainWindow()
{
    delete ui;
    //READ: Liberar el ChatServer
    delete this->ChatServer;
}



void MainWindow::on_btnStart_clicked()
{
    //READ: Iniciar el server en el puerto 7777 o cualquiera que deseen
    // El metodo start recibe de parametro el puerto donde escuchar
    this->ChatServer->start(7777,this->ui->spinBox->value());
}

void MainWindow::on_btnStop_clicked()
{
    //READ:Detener el server
    this->ChatServer->stop();
}

void MainWindow::on_btnQuit_clicked()
{
    //READ: Cerrar la ventana pero antes de eso, primero hacer un STOP
    //del server y luego cerrarse
    this->ChatServer->stop();
    this->close();
}

void MainWindow::on_btnReset_clicked()
{
    if(this->ChatServer->getStatus()) // Server esta levantado
    {
        this->ChatServer->sendMessage("RESETGAME: \n\r");
        this->ChatServer->reset();
    }
}
