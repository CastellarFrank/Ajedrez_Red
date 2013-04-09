#ifndef GAMEAREA_H
#define GAMEAREA_H


#include <QWidget>
#include <QPainter>
#include <QString>
#include <Qlist>
#include "clientconnection.h"


class GameArea : public QWidget
{
public:
     GameArea(QString color,QWidget *parent = 0,bool view=false); // Este es el constructor por default
     void setCurrentCell(int fila, int col);
     void setCurrentCellOponent(int fila,int col);
     void setConexion(ClientConnection *con);
     void setValor(int fila, int col, QString valor);
     QString getValor(int fila, int col);
     int getCurrentfila();
     int getCurrentcolumna();
     void setMarcada(int fila,int columna);
     void reset();
     QString color;
     bool fichaSeleccionada;

private:
     bool guess;
    int filaMarcada, columnaMarcada;
    int fila_actual, columna_actual;
    int filaOponente, columnaOponente;
    ClientConnection *referenciaConexion;
    QPixmap imagen_tablero;
    QString TABLERO_INTERNO[8][8];

protected:
    void paintEvent(QPaintEvent *event);
    void focusOutEvent ( QFocusEvent * event );
    void focusInEvent ( QFocusEvent * event );
    void enterEvent ( QEvent * event ) ;
    QPixmap retorna_imagen();
    QPixmap imagen;
};

#endif // GAMEAREA_H
