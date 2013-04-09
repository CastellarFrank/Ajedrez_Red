#include "gamearea.h"


GameArea::GameArea(QString color,QWidget *parent,bool view) : QWidget(parent) // llamar constructor clase base
{
    this->guess=view;
    this->fichaSeleccionada=false;
    this->color=color;
    setBackgroundRole( QPalette::Base );
    setAutoFillBackground( true );
    fila_actual=7;
    columna_actual=0;
    this->filaOponente=0;
    this->columnaOponente=7;
    this->filaMarcada=-1;
    this->columnaMarcada=-1;
    this->reset();

    imagen_tablero.load(":/images/tablero.png");
}
void GameArea::setMarcada(int fila, int columna){
    this->filaMarcada=fila;
    this->columnaMarcada=columna;
}

void GameArea::reset()
{
    for (int i=0; i<8 ; i++)
     for (int j=0; j<8 ; j++)
         this->TABLERO_INTERNO[i][j]="0";

    if(!this->guess){
        QString bando1,bando2;
        if(color=="0"){
            bando1="W";
            bando2="B";
        }else{
            bando1="B";
            bando2="W";
        }

        this->TABLERO_INTERNO[7][0]=bando1+"T";
        this->TABLERO_INTERNO[7][1]=bando1+"C";
        this->TABLERO_INTERNO[7][2]=bando1+"A";
        this->TABLERO_INTERNO[7][3]=bando1+"V";
        this->TABLERO_INTERNO[7][4]=bando1+"R";
        this->TABLERO_INTERNO[7][5]=bando1+"A";
        this->TABLERO_INTERNO[7][6]=bando1+"C";
        this->TABLERO_INTERNO[7][7]=bando1+"T";
        for(int i=0;i<8;i++){
            this->TABLERO_INTERNO[6][i]=bando1+"P";
        }

        this->TABLERO_INTERNO[0][0]=bando2+"T";
        this->TABLERO_INTERNO[0][1]=bando2+"C";
        this->TABLERO_INTERNO[0][2]=bando2+"A";
        this->TABLERO_INTERNO[0][3]=bando2+"R";
        this->TABLERO_INTERNO[0][4]=bando2+"V";
        this->TABLERO_INTERNO[0][5]=bando2+"A";
        this->TABLERO_INTERNO[0][6]=bando2+"C";
        this->TABLERO_INTERNO[0][7]=bando2+"T";
        for(int i=0;i<8;i++){
            this->TABLERO_INTERNO[1][i]=bando2+"P";
        }
    }
}
void GameArea::setValor(int fila, int col, QString valor)
{
    // Se deberia validar fila y col pero asumamos que vienen bien
    this->TABLERO_INTERNO[fila][col]=valor;

}
 QString GameArea::getValor(int fila, int col)
 {
     // Igual deberia validar fila y col
     return this->TABLERO_INTERNO[fila][col];

 }
void GameArea::focusOutEvent ( QFocusEvent * event )
{
  this->releaseKeyboard();
}
void GameArea::focusInEvent ( QFocusEvent * event )
{
  this->grabKeyboard();
}
void GameArea::enterEvent ( QEvent * event )
{
    this->setFocus();
}

QPixmap GameArea::retorna_imagen()
{
  return this->imagen_tablero;

}


void GameArea::paintEvent( QPaintEvent * event ){
    QPainter painter( this ); // Inicializar el Painter
    // Dibujar el tablero primero SIEMPRE

    painter.drawPixmap(0,0,this->retorna_imagen());
    QPen pen(Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen( pen);
    painter.setBrush(Qt::transparent); // Y la brocha transparente.
    painter.drawRect(columna_actual*59+15,fila_actual*59+15,59,59);
    if(this->fichaSeleccionada)
        painter.drawRect(this->columnaMarcada*59+15,this->filaMarcada*59+15,59,59);
    QPen pen2(Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen( pen2);
    painter.drawRect(this->columnaOponente*59+15,this->filaOponente*59+15,59,59);
    // Redibujar el tablero
    for (int i=0; i<8 ; i++){
        for (int j=0; j<8 ; j++)
        {
            if(this->TABLERO_INTERNO[i][j]!="0")
           {
                QString path=":images/";
                path=path+this->TABLERO_INTERNO[i][j]+".png";
                this->imagen.load(path);
                painter.drawPixmap(j*59+15,i*59+15,59,59,imagen);
            }
        }
    }
}

int GameArea::getCurrentfila()
{
    return fila_actual;
}
int GameArea::getCurrentcolumna()
{
    return columna_actual;
}

void GameArea::setCurrentCell(int fila, int col)
{
    if((fila>=0 && fila<=7)&&(col>=0 && col<=7))
    {
      fila_actual = fila;
      columna_actual = col;
    }

}
void GameArea::setCurrentCellOponent(int fila, int col){
    if((fila>=0 && fila<=7)&&(col>=0 && col<=7))
    {
      this->filaOponente = fila;
      this->columnaOponente = col;
    }
}

void GameArea::setConexion( ClientConnection *con)
{
    this->referenciaConexion = con;

}

