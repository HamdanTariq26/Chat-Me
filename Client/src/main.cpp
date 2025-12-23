/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: main.cpp
   Description: 
*/

#include "mainwindow.h"
#include "login.h"
#include "pch.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QThread *networkWorker = new QThread;
    Client* client = new Client();
    Login login;
    MainWindow w;
    client->moveToThread(networkWorker);
    networkWorker->start();


    QObject::connect(networkWorker,&QThread::started,client,&Client::tryConnect);
    QObject::connect(client,&Client::connectionStatus,&login,&Login::onConnect);
    QObject::connect(&login,&Login::reconnect,client,&Client::reconnect);
    QObject::connect(client,&Client::successful,&w,[&w,&login](){w.show();login.close();});
    QObject::connect(&login,&Login::send,client,&Client::SendLoginRegister);
    QObject::connect(client,&Client::updateClient,&w,&MainWindow::updateUserList);
    QObject::connect(&w,&MainWindow::SendMsg,client,&Client::SendMsg);
    QObject::connect(client,&Client::updateMsg,&w,&MainWindow::updateMsg);
    QObject::connect(client,&Client::handle,&w,&MainWindow::ShowUser);
    QObject::connect(&w,&MainWindow::SendFile,client,&Client::SendFile);
    QObject::connect(client,&Client::fileReceived,&w,&MainWindow::fileRecived);
    QObject::connect(client,&Client::wasRegisterationSuccessful,&login,&Login::onRegisteration);
    QObject::connect(client,&Client::loginUnSuccessful,&login,[&login](){
        QMessageBox::information(&login,"Login Unsuccessful","Password or username incorrect");
    });
    QObject::connect(&w,&MainWindow::broadcastMsg,client,&Client::SendMsgToEveryone);

    login.show();
    return a.exec();
    delete networkWorker;
    delete client;
}

