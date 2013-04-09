#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QMessageBox>
#include <Qlist>
//READ: Incluir la client connection
#include "clientconnection.h"
#include "gamearea.h"
#include "tablero.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //READ: Declarar un apuntador a una conexion
    ClientConnection *Conexion;
    QMap<QString,Tablero*> ventanas;
    QMap<QString,Tablero*> views;

public slots:
     void procesarMensaje(ClientConnection *con, QString comando, QString mensaje);
     void seConecto(ClientConnection *con, QString nick);
     void seDesconecto(ClientConnection *con, QString nick);
     void errorConexion(ClientConnection*, QString titulo,QString mensaje);

private slots:

    void on_btnVisualizarGame_clicked();
    void on_btnRechazarReto_clicked();
    void on_btnAceptarReto_clicked();
    void on_btnRetar_clicked();
    void on_lineEdit_editingFinished();
    void on_btnDisconnect_clicked();
    void on_lineEdit_Mensaje_returnPressed();
    void on_btnConnect_clicked();
    void on_btnSalir_clicked();
    void cerrarVentana(QString vent,bool val);

};

#endif // MAINWINDOW_H
