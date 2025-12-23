/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: clientdiscovery.h
   Description:
*/
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

