#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QUdpSocket>
#include <QObject>
#include <QTimer>
#include <QJsonObject>

#include <QJsonDocument>
#include <QSysInfo>
#include <QHostInfo>

class Transport : public QObject
{
    Q_OBJECT
public:
    Transport();

public slots:
    void setServer(QString address);
    void sendInvite(QJsonObject message);

private slots:
    void readyRead();
    void setServerAddress(QHostInfo address);

signals:
    void onMessage(QJsonObject message);

private:
    void Ping();
    QUdpSocket *socket;
    QHostAddress remote = QHostAddress::LocalHost;
    QTimer *timer;
};

#endif // TRANSPORT_H
