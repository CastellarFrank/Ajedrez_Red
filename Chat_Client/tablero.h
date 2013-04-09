#ifndef TABLERO_H
#define TABLERO_H

#include <QMainWindow>
#include "gamearea.h"
#include "clientconnection.h"
#include <QKeyEvent>
#include <QMessageBox>

namespace Ui {
    class Tablero;
}

class Tablero : public QMainWindow
{
    Q_OBJECT

public:
    explicit Tablero(QString color,QString oponent,ClientConnection *con,QWidget *parent = 0,bool view=false,QString player="0");
    ClientConnection *Conexion;
    GameArea *area;
    bool turno;
    void cerrarVentana();
    void cambiarTurno();
    bool muerte;
    ~Tablero();

private:
    int filaMarcada,columnaMarcada;
    QString color;
    QString oponent;
    QString player;
    Ui::Tablero *ui;
    bool recibido;
    bool guess;


signals:
    void cerrar(QString ,bool);

protected:
     virtual void keyPressEvent( QKeyEvent *event );
     void closeEvent(QCloseEvent *);
};

#endif // TABLERO_H
