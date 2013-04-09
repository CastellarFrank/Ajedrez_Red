#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QTcpSocket>

class ClientConnection : public QObject
{
   Q_OBJECT
public:

        ClientConnection();
        //READ: El socket que mantiene la comunicacion y un array de bytes para enviar
        // y leer mensajes
        QTcpSocket *socket;
        bool estaConectado;
        QByteArray message;
        //READ: el nick asociado a esta conexion
        QString nick;
         //READ: Metodo que se usa para enviar un mensaje en esta conexion al socket respectivo
        void sendMessage(QString);
        void conectarse(QString host,int port,QString nick);

private:
        //READ: Este es de los metodos mas importantes, ya que es el metodo que analiza
        // el mensaje para saber que comandos ejecuto un server
        void parseMessage(QString msg);

private slots:
        //READ: Este metodo privado es el que se ejecuta cuando el SOCKET de esta
        //conexion recibe la señal de nuevos datos en la conexion y este metodo revisa
        // hasta encontrar \n\r y luego llama a ParseMessage
        void recv();
        //READ; metodo que se ejecuta cuando se confirma la conexion del server
        void conectado();
        void displayError(QAbstractSocket::SocketError socketError);


public slots:

        //READ: Metodo que se usa cuando el socket de esta conexion recibe una señal de
        // Desconexion.
        void disconnect();

signals:
        //READ: Estas son las señales que una conexion puede EMITIR
        //estas señales se emiten a la pantalla principal
        // TODAS estas señales DEBEN indicar la conexion en curso que emite la señal
        void newMessage(ClientConnection*,QString,QString);
        void connected(ClientConnection*, QString);
        void disconnected(ClientConnection*, QString);
        void ConnectionError(ClientConnection*, QString,QString);
    };

#endif // CLIENTCONNECTION_H
