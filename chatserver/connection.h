#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QTcpSocket>



class Connection : public QObject
{
    Q_OBJECT
public:
        //READ: Constructor con socket de parametro del server
        Connection(QTcpSocket *_socket);
        //READ: El socket que mantiene la comunicacion y un array de bytes para enviar
        // y leer mensajes
        QTcpSocket *socket;
        QByteArray message;
        //READ: el nick asociado a esta conexion asi como el indice dentro la lista
        // lista de conexiones que tiene el server
        QString nick;
        int indice_lista_conexion;
        int veces;
        //READ: Metodo publico para enviar la lista de nicks al resto de conexiones
        void sendNicks(QStringList *nicks);

 private:
        //READ: Este es de los metodos mas importantes, ya que es el metodo que analiza
        // el mensaje para saber que comandos ejecuto un cliente y partir el mensaje
        // de forma adecuada y luego levanta la señal de NewMessage para que el server
        // la procese y la envie al resto de conexiones siempre y sea necesario
        void parseMessage(QString msg);

private slots:
        //READ: Este metodo privado es el que se ejecuta cuando el SOCKET de esta
        //conexion recibe la señal de nuevos datos en la conexion y este metodo revisa
        // hasta encontrar \n\r y luego llama a ParseMessage
        void recv();


public slots:
        //READ: Metodo que se usa para enviar un mensaje en esta conexion al socket respectivo
        void sendMessage(QString);
        //READ: Metodo que se usa cuando el socket de esta conexion recibe una señal de
        // Desconexion.
        void disconnect();

signals:
        //READ: Estas son las señales que una conexion puede EMITIR
        //estas señales se emiten al server, ya sea que llego un nuevo mensaje
        // que se conecto un nuevo cliente o se desconecto un cliente
        // TODAS estas señales DEBEN indicar la conexion en curso que emite la señal
        void newMessage(Connection*,QString);
        void newMove(Connection*,QString);
        void newMovePieza(Connection*,QString);
        void connected(Connection*, QString);
        void disconnected(Connection*, QString);
        void newChallenge(Connection*,QString);
        void modificarListas(Connection*,QString);
        void newViewer(Connection*,QString);
        void newProcess(Connection*,QString);


};

#endif // CONNECTION_H
