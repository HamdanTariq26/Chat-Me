/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: connectserver.h
   Description:  
*/
#ifndef CONNECTSERVER_H
#define CONNECTSERVER_H
#include "pch.h"

namespace Ui {
class ConnectServer;
}

class ConnectServer : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectServer(QWidget *parent = nullptr);
    ~ConnectServer();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

signals:
    void connectToBy(bool isLan,QString ip,QString port);

private:
    Ui::ConnectServer *ui;
    QString serverip;
    QString port;
    bool onLan;

};

#endif // CONNECTSERVER_H

