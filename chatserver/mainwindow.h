#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "server.h"

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
    //READ: Declarar un apuntador a un Server llamado ChatServer
    Server *ChatServer;



private slots:
     void on_btnReset_clicked();
    void on_btnQuit_clicked();
    void on_btnStop_clicked();
    void on_btnStart_clicked();

};

#endif // MAINWINDOW_H
