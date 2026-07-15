#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QList>


class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject* parent = nullptr);

    // Для хоста
    bool startHost(quint16 port = 55555);

    // Для присоединившегося
    void connectToHost(const QString& ip, quint16 port = 55555);

    void sendMessage(const QString& text);

    bool isHost() const;
    bool isConnected() const;

    void disconnectAll();

signals:

    void peerConnected();

    void peerDisconnected();

    void messageReceived(const QString& text);

    void errorOccurred(const QString& text);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onSocketDisconnected();
    void onConnectedToHost();
private:
    void sendToSocket(QTcpSocket* socket, const QString& text);

    QTcpServer*          m_server;
    QList<QTcpSocket*>   m_sockets;
    bool                 m_isHost;
};

#endif
