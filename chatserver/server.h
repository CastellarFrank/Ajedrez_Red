#ifndef SERVER_H
#define SERVER_H

//READ: De las librerias ocupamos el TcpServer, la lista QList y por supuesto
//la libreria connection.h que es la que nosotros implementamos para administrar
// cada conexion
#include <QTcpServer>
#include <QStringList>
#include <QList>
#include <QTextEdit>
#include "connection.h"
#include <QMultiMap>
#include "ficha.h"

class Server : public QTcpServer
{
    Q_OBJECT

public:
    //READ: Constructor, destructor, metodos Start y Stop
    Server();
     ~Server();
    void start(int port,int cuantas_conexiones);
    void stop();
    //READ: Mantener una variable de Status
    bool getStatus();
    int Cuantas_Conexiones () { return this->Cantidad_Conexiones; }
    //READ: Este metodo envia mensajes a TODAS las conexiones abiertas y validaas
    // Recorre la lista de conexiones
    void sendMessage(QString mensaje);
    //READ: Tengamos una lista de conexiones, la hare publica para no hacer get ni set
    QList<Connection *> Lista_Conexiones;
     // Matriz de enteros que representa el tablero
    int TABLERO[8][8];
    int casillas;
    //READ: Un apuntador al textedit donde iremos colocando los mensajes
    QTextEdit *log;
    void reset();
    void sendMessageTo(QString nick,QString mensaje);

 //READ: Los public slots son los metodos que se llaman cuando se generan ciertos eventos
 // en QT estos eventos se declaran como SIGNALS.
public slots:
        //READ: Slot que se ejecuta cuando se detecta la señal o evento de una nueva conexion
        void newConnection();
        //READ: Slot que se ejecuta cuando el cliente ha confirmado la conexion enviando
        // su NICK
        void connected(Connection* con, QString nick);
        //READ: Slot que se ejecuta cuando un cliente se desconecta
        void disconnected(Connection *con, QString nick);
        //READ: Slot que se ejecuta cuando un cliente esta enviando mensajes al server
        // y estos mensajes deben enviarse al resto
        void procesarMensaje(Connection *con, QString mensaje);
        void procesarMovimiento(Connection *con, QString mensaje);
        void procesarMovimientoPieza(Connection *con,QString mensaje);
        void procesarChallenges(Connection *con,QString retado);
        void procesarCambiosListas(Connection*con,QString mensaje);
        void procesarViewer(Connection*con,QString mensaje);
        void procesar(Connection *con,QString mensaje);

private:
        //READ: Tenemos una lista de nicks por separado aunque podriamos haber usado
        // la lista de conexiones
        QStringList *nickList;
        QStringList *Games;
        //READ: Tenemos una variable booleana que nos indica si el server ha iniciado o no
        bool server_started;
        int Cantidad_Conexiones;
        int conexionesDisponibles();
        QMultiMap <QString,QString> juegos;
        QMultiMap<QString,QString>solicitudes;
        QMap<QString,QList<Ficha> > fichas;
        QMultiMap<QString,QString>Viewers;
        bool repetido;
        bool buscarSolicitud(QString retado,QString retador);
        bool buscarJuego(QString jugador1,QString jugador2);
        bool buscarViewer(QString jugador,QString juego);
        void agregarJuego(QString jugadores,int C);
        void eliminarInfo(QString nick);
        void eliminarSolicitudes(QString nick);
        void eliminarJuegos(QString nick);
        void eliminarViewers(QString nick);
        void moverPieza(QString nombreJuego,int fPiezaM,int cPiezaM,int fPiezaU,int cPiezaU);
        void moveViewers(QString nombre1,QString nombre2,QString msg);

};

#endif // SERVER_H
