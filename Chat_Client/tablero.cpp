#include "tablero.h"
#include "ui_tablero.h"
#include <QRect>

Tablero::Tablero(QString color,QString oponent,ClientConnection *con, QWidget *parent, bool view,QString player) :
    QMainWindow(parent),
    ui(new Ui::Tablero)
{
    ui->setupUi(this);
    this->color=color;;
    this->recibido=false;
    this->muerte=false;
    this->oponent=oponent;
    this->player=player;
    this->Conexion=con;
    this->guess=view;
    area=new GameArea(color,0,view);
    area->setConexion(this->Conexion);
    area->setFocusPolicy(Qt::TabFocus);
    ui->gridLayout->addWidget(area,0,0,1,1);
    QString nick;
    if(guess){
        nick=this->player;
    }else{
        nick=this->Conexion->nick;
    }
    ui->lblPlayerBlancas->setText(nick);
    ui->lblPlayerNegro->setText(this->oponent);
    if (color.toInt()==0){
        ui->lblTurno->setText("Turno "+nick);
        turno=true;
    }else{
        ui->lblTurno->setText("Turno "+this->oponent);
        turno=false;
    }
    if(guess){
        ui->lblTurno->setText("VIEWER");
        this->setWindowTitle("Visualización: "+this->player+" vs "+this->oponent+" - Industrias BuenRecord");
    }else{
        this->setWindowTitle("Juego contra "+this->oponent+" - Industrias BuenRecord");
    }

}
void Tablero::cambiarTurno(){
    if(guess)
        return;
    if(ui->lblTurno->text().mid(6)==this->Conexion->nick){
        ui->lblTurno->setText("Turno "+this->oponent);
    }else{
        ui->lblTurno->setText("Turno "+this->Conexion->nick);
    }
}

void Tablero::keyPressEvent(QKeyEvent *event){
    int f,c;
    QString tempf,tempc;
    qDebug()<<"Oponente"<<this->oponent;
    if(!guess){
        switch(event->key())
        {

           case Qt::Key_Left :

            f = this->area->getCurrentfila();
            c = this->area->getCurrentcolumna()-1;
            this->area->setCurrentCell(f,c);
            if(f<8 && c<8){
                tempf.setNum(f,10);
                tempc.setNum(c,10);
                this->Conexion->sendMessage("MOVE:"+tempf+":"+tempc +":"+oponent+ "\n\r");
            }
            break;

           case Qt::Key_Right :

            f = this->area->getCurrentfila();
            c = this->area->getCurrentcolumna()+1;
            this->area->setCurrentCell(f,c);
            if(f<8 && c<8){
                tempf.setNum(f,10);
                tempc.setNum(c,10);
                this->Conexion->sendMessage("MOVE:"+tempf+":"+tempc +":"+oponent+ "\n\r");
            }
            break;

           case Qt::Key_Up :

            f = this->area->getCurrentfila()-1;
            c = this->area->getCurrentcolumna();
            this->area->setCurrentCell(f,c);
            if(f<8 && c<8){
                tempf.setNum(f,10);
                tempc.setNum(c,10);
                this->Conexion->sendMessage("MOVE:"+tempf+":"+tempc +":"+oponent+ "\n\r");
            }
            break;

            case Qt::Key_Down :

            f = this->area->getCurrentfila()+1;
            c = this->area->getCurrentcolumna();
            this->area->setCurrentCell(f,c);
            if(f<8 && c<8){
                tempf.setNum(f,10);
                tempc.setNum(c,10);
                this->Conexion->sendMessage("MOVE:"+tempf+":"+tempc +":"+oponent+ "\n\r");
            }
            break;
           // Cuando de enter hay que mandar el mensaje del movimiento
           case Qt::Key_Return :
            if(turno){
                f = this->area->getCurrentfila();
                c = this->area->getCurrentcolumna();
                QString valor=area->getValor(f,c);
                if(area->fichaSeleccionada){
                    if(valor!="0"){
                        qDebug()<<valor;
                        QMessageBox::information(this,"Movimiento inválido","No puede mover la ficha ahí.");
                        area->fichaSeleccionada=false;
                        area->update();
                    }else{
                        qDebug()<<valor;
                        QString val=area->getValor(filaMarcada,columnaMarcada);
                        qDebug()<<" val"<<val<<"Fila"<<filaMarcada<<"Col"<<columnaMarcada;
                        area->setValor(filaMarcada,columnaMarcada,"0");
                        qDebug()<<"fila"<<f<<"col"<<c<<"val"<<val;
                        area->setValor(f,c,val);
                        area->fichaSeleccionada=false;
                        area->update();
                        QString fPiezaM,cPiezaM,fPiezaU,cPiezaU;
                        fPiezaM.setNum(filaMarcada); cPiezaM.setNum(columnaMarcada);
                        fPiezaU.setNum(f); cPiezaU.setNum(c);
                        this->Conexion->sendMessage("MOVEPIEZA:"+fPiezaM+":"+cPiezaM+
                                                    ":"+fPiezaU+":"+cPiezaU+":"+
                                                    this->oponent+"\n\r");
                        this->cambiarTurno();
                        this->turno=false;
                    }
                }else{
                    if(valor!="0" && (valor.at(0)=='W'?0:1)==this->color.toInt()){
                          area->setMarcada(f,c);
                          filaMarcada=f;
                          columnaMarcada=c;
                          area->fichaSeleccionada=true;
                          area->update();
                    }
                }
            }else{
                QMessageBox::information(this,"Acción inválida","Aún no es tu turno.\nNo puedes realizar ese movimiento.");
            }




                 // this->Conexion->sendMessage("MOVE:"+tempf+":"+tempc + "\n\r");
                //  this->ui->textEdit->append("MOVE:"+tempf+":"+tempc + "\n\r");
                  // Importante notar que aun asi, NO se cambia nada de tablero
                  // mientras no recibamos el Ok del server (MOVEOK)
               break;
       }
    }
    this->area->update();
}
void Tablero::closeEvent(QCloseEvent *val){
    if(!this->guess){
        if(this->recibido){
            if(this->muerte){
                val->accept();
                return;
            }
            QString temp;
            if(color=="0"){
                temp=this->Conexion->nick+" vs "+this->oponent;
            }else{
                temp=this->oponent+" vs "+this->Conexion->nick;
            }
            qDebug()<<"val"<<temp;
            this->Conexion->sendMessage("CLOSEVIEWS:"+temp+"\n\r");
            QMessageBox::information(this,"Oponente Deserto","Tu oponente ha abandonado la partida.");
            val->accept();
            return;
        }
        QMessageBox::StandardButton replay=QMessageBox::question(this,"Cerrar Juego","¿Realmente desea cerrar el juego?",
                              QMessageBox::Yes | QMessageBox::No,QMessageBox::No);

        if(replay==QMessageBox::No){
            val->ignore();
            return;
        }
        this->Conexion->sendMessage("CLOSEGAME:"+this->oponent+"\n\r");
        val->accept();
        emit cerrar(this->Conexion->nick+this->oponent,false);
    }else{
        if(this->muerte){
            val->accept();
            return;
        }
        if(this->recibido){
            QMessageBox::information(this,"Juego terminado","El juego ha terminado.");
        }
        QString temp=this->player+" vs "+this->oponent;
        qDebug()<<temp;
        this->Conexion->sendMessage("CERRARVISTA:"+temp+"\n\r");
        val->accept();
        emit cerrar(temp,true);
    }

}
void Tablero::cerrarVentana(){
    this->recibido=true;
    this->close();
}

Tablero::~Tablero()
{
    delete ui;
    delete this->area;
}
