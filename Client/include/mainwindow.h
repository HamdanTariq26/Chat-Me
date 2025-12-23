/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: mainwindow.h
   Description:  
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QWidgetItem>
#include "login.h"
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void showLogin();
    ~MainWindow();


private slots:

    void on_sendMsgPushButton_clicked();

    void on_filePushButton_clicked();

    void on_actionSend_a_broadcast_triggered();

    void on_actionOne_lone_coder_triggered();

    void on_actionC_triggered();

    void on_actionAsio_triggered();

    void on_actionQt_triggered();

    void on_actionProject_triggered();


public slots:
    void updateUserList(std::string username,bool remove = false);
    void updateMsg(std::string s_username,std::string msg,bool isBroadcast = false);
    void ShowUser(std::string title,std::string msg,bool closeApp = false);
    void fileRecived(std::string username);

signals:
    void SendMsg(std::string username,std::string msg);
    void SendFile(const std::string path,const std::string reciverUsername);
    void broadcastMsg(std::string msg);

private:
    Ui::MainWindow *ui;
    QMap<QString,QStringList> userChat;
    QString currentUser;
    QList<QListWidgetItem*> items;
    QWidget *createChatMessage(const QString &sender, const QString &msg, const QString &time, bool isMe);
};
#endif // MAINWINDOW_H

