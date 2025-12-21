#include "clientdiscovery.h"
#include <QNetworkInterface>
#include <QTimer>

static const QHostAddress MULTICAST_GROUP("239.255.0.1");
static const quint16 DISCOVERY_PORT = 8888;

ClientDiscovery::ClientDiscovery(QObject *parent)
    : QObject(parent)
{
    udp.bind(QHostAddress::AnyIPv4, 0,
             QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    for (const QNetworkInterface& iface : QNetworkInterface::allInterfaces()) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            iface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            udp.joinMulticastGroup(MULTICAST_GROUP, iface);
            break;
        }
    }

    connect(&udp, &QUdpSocket::readyRead,
            this, &ClientDiscovery::onReadyRead);
}

void ClientDiscovery::discover()
{
    QByteArray msg = "DISCOVER_SERVER";

    udp.writeDatagram(msg, MULTICAST_GROUP, DISCOVERY_PORT);

    QTimer::singleShot(1000, this, [this, msg]() {
        if (!multicastWorked) {
            udp.writeDatagram(msg, QHostAddress::Broadcast, DISCOVERY_PORT);
        }
    });

}

void ClientDiscovery::onReadyRead()
{
    while (udp.hasPendingDatagrams()) {
        QByteArray data;
        data.resize(udp.pendingDatagramSize());

        QHostAddress sender;
        udp.readDatagram(data.data(), data.size(), &sender);

        if (data == "SERVER_HERE") {
            multicastWorked = true;
            emit serverFound(sender.toString());
        }
    }
}
