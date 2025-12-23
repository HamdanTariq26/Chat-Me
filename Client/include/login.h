/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: login.h
   Description:  
*/
#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "register.h"


namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

signals:
    void reconnect();
    void send(std::string username,std::string password,bool forRegister = false);
    void wasRegisterSuccessful(bool);



private slots:
    void on_showPasswordCheckBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_connectPushButton_clicked();

    void on_exitPushButton_clicked();

    void on_SignUpPushButton_clicked();

    void on_reconnectPushButton_clicked();

    void registerSend(std::string username,std::string password);

public slots:
    void onRegisteration(bool wasSuccessful);

public slots:
    void onConnect(bool arg);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Login *ui;
    Register *registerWindow;
};

#endif // LOGIN_H

