#pragma once
#include <QObject>
#include <QUdpSocket>

class ClientDiscovery : public QObject
{
    Q_OBJECT
public:
    explicit ClientDiscovery(QObject *parent = nullptr);
    void discover();

signals:
    void serverFound(const QString& ip);

private slots:
    void onReadyRead();

private:
    QUdpSocket udp;
    bool multicastWorked = false;
};
