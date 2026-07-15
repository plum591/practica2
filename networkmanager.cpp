#include "networkmanager.h"

static const char MSG_END = '\n';

NetworkManager::NetworkManager(QObject* parent)
    : QObject(parent),
      m_server(nullptr),
      m_isHost(false)
{
}

// Хост

bool NetworkManager::startHost(quint16 port) {
    m_isHost = true;

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection,
            this, &NetworkManager::onNewConnection);


    if (!m_server->listen(QHostAddress::Any, port)) {
        emit errorOccurred("Не удалось открыть порт " + QString::number(port) +
                           ": " + m_server->errorString());
        return false;
    }
    return true;
}

// Гость

void NetworkManager::connectToHost(const QString& ip, quint16 port) {
    m_isHost = false;

    QTcpSocket* socket = new QTcpSocket(this);
    m_sockets.append(socket);

    connect(socket, &QTcpSocket::connected,
            this, &NetworkManager::onConnectedToHost);
    connect(socket, &QTcpSocket::readyRead,
            this, &NetworkManager::onReadyRead);
    connect(socket, &QTcpSocket::disconnected,
            this, &NetworkManager::onSocketDisconnected);

    // Обработка ошибки подключения (неверный IP, хост недоступен и т.п.).
    connect(socket, &QTcpSocket::errorOccurred,
            this, [this, socket](QAbstractSocket::SocketError) {
        emit errorOccurred("Ошибка подключения: " + socket->errorString());
    });

    socket->connectToHost(ip, port);
}

// Отправка

void NetworkManager::sendMessage(const QString& text) {
    for (QTcpSocket* socket : m_sockets) {
        sendToSocket(socket, text);
    }
}

void NetworkManager::sendToSocket(QTcpSocket* socket, const QString& text) {
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = text.toUtf8();
        data.append(MSG_END);
        socket->write(data);
    }
}


bool NetworkManager::isHost() const {
    return m_isHost;
}

bool NetworkManager::isConnected() const {
    for (QTcpSocket* socket : m_sockets) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            return true;
        }
    }
    return false;
}

void NetworkManager::disconnectAll() {
    for (QTcpSocket* socket : m_sockets) {
        socket->disconnectFromHost();
    }
    m_sockets.clear();

    if (m_server) {
        m_server->close();
    }
}


// У хоста: подключился новый гость.
void NetworkManager::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket* socket = m_server->nextPendingConnection();
        m_sockets.append(socket);

        connect(socket, &QTcpSocket::readyRead,
                this, &NetworkManager::onReadyRead);
        connect(socket, &QTcpSocket::disconnected,
                this, &NetworkManager::onSocketDisconnected);

        emit peerConnected();
    }
}

// У гостя: успешно соединились с хостом.
void NetworkManager::onConnectedToHost() {
    emit peerConnected();
}

// Пришли данные
void NetworkManager::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine();
        QString text = QString::fromUtf8(line).trimmed();
        if (!text.isEmpty()) {
            emit messageReceived(text);
        }
    }
}

// Кто-то отключился
void NetworkManager::onSocketDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_sockets.removeAll(socket);
        socket->deleteLater();
    }
    emit peerDisconnected();
}
