#include "transport.h"

Transport::Transport()
{
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, 7817);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &Transport::Ping);
    timer->start(5000);

}


void Transport::sendInvite(QJsonObject jobject)
{
    jobject["action"] = "invite";
    jobject["machine"] = QSysInfo::machineUniqueId().data();
    QJsonDocument doc( jobject );
    socket->writeDatagram(doc.toJson(QJsonDocument::Compact), remote, 7818);
}


void Transport::setServerAddress(QHostInfo address)
{

    if (!address.addresses().isEmpty()) {
        remote = QHostAddress(address.addresses().first());
        qDebug()<< "host" << QHostAddress(remote).toString();
    } else {
        // TODO: open settings
    }
}

void Transport::setServer(QString address)
{
    QHostInfo::lookupHost(address, this, SLOT(setServerAddress(QHostInfo)));
}

void Transport::readyRead()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    socket->readDatagram(buffer.data(), buffer.size(),
                         &sender, &senderPort);

    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
    //QJsonDocument doc = QJsonDocument::fromBinaryData(buffer);
    QJsonDocument doc = QJsonDocument::fromJson(buffer);
    emit onMessage(doc.object());

}

void Transport::Ping()
{
    QJsonObject jobject;

    jobject["action"] = "ping";
    jobject["machine"] = QSysInfo::machineUniqueId().data();
    QJsonDocument doc( jobject );

    socket->writeDatagram(doc.toJson(QJsonDocument::Compact), remote, 7818);
    //socket->writeDatagram(doc.toBinaryData(), remote, 7818);

}

