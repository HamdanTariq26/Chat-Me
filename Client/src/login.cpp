/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: login.cpp
   Description: 
*/
#include "ui_login.h"
#include "pch.h"
#include "login.h"

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    ui->reconnectPushButton->hide();
    ui->connectPushButton->hide();
    ui->SignUpPushButton->hide();
    ui->connectionStatusLabel->setText("Connecting to server...");
    this->setWindowTitle("Login");
    registerWindow = new Register(this);


    QString qss = R"(

QWidget {
    background-color: #e5e5e5;
}

QWidget {
    border-radius: 12px;
    background-color: rgba(255, 255, 255, 0.98);
}

QLabel {
    font-weight: 500;
    color: #333333;
    font-size: 14px;
}


QLineEdit {
    padding: 8px 12px;
    border: 1px solid #c0c0c0;
    border-radius: 6px;
    background-color: #ffffff;
    color: #000000;
}

QLineEdit:focus {
    border: 1px solid #0078d4;
}



QCheckBox::indicator {
    font-size: 13px;
    color: #333333;
    spacing: 5px;
}

QCheckBox::indicator:checked {
  background-color: #0078d4;
    border: 1px solid #005a9e;
}

QCheckBox::indicator:hover {
    border: 1px solid blue;
}


QPushButton {
    padding: 8px 18px;
    border: 1px solid #b0b0b0;
    border-radius: 8px;
    background-color: #0078d4;
    color: white;
    font-weight: 600;
}

QPushButton:pressed {
    background-color: #005a9e;
}


QPushButton:!enabled {
    background-color: #cccccc;
    color: #666666;
    border: 1px solid #999999;
}

)";

    this->setStyleSheet(qss);


}


Login::~Login()
{
    delete registerWindow;
    delete ui;
}

void Login::on_showPasswordCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked){
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->passwordEdit->setEchoMode(QLineEdit::Password);
    }
}


void Login::on_connectPushButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    if(!username.isEmpty() && !username.contains(" ") && !password.isEmpty() && !password.contains(" ")){
        emit send(username.toStdString(),password.toStdString());
    }else{
        QMessageBox::warning(this,"Invalid username/password","Username/password must not contain whitespaces");
    }
}


void Login::on_exitPushButton_clicked()
{
    QApplication::exit();
}


void Login::on_SignUpPushButton_clicked()
{
    registerWindow->setModal(true);
    registerWindow->setWindowTitle("Signup");
    connect(registerWindow,&Register::onClose,this,[this](){this->show();});
    connect(registerWindow,&Register::send,this,&Login::registerSend);
    connect(this,&Login::wasRegisterSuccessful,registerWindow,&Register::onRegisterationTry);
    connect(registerWindow,&Register::autoFill,this,[this](QString username,QString password){
        this->ui->usernameEdit->clear();
        this->ui->usernameEdit->setText(username);
        this->ui->passwordEdit->clear();
        this->ui->passwordEdit->setText(password);
        this->show();
    });
    this->hide();
    registerWindow->show();

}

void Login::registerSend(std::string username,std::string password){
    emit send(username,password,true);
}

void Login::onRegisteration(bool wasSuccessful)
{
    emit wasRegisterSuccessful(wasSuccessful);
}

void Login::closeEvent(QCloseEvent *event){
    if(event->spontaneous()){
       QApplication::quit();
    }else{
        event->accept();
    }

}

void Login::onConnect(bool arg){
    if(arg){
        ui->connectPushButton->show();
        ui->SignUpPushButton->show();
        ui->connectionStatusLabel->setText("Connected to server");
        ui->reconnectPushButton->hide();
    }else{
        ui->connectPushButton->hide();
        ui->SignUpPushButton->hide();
        ui->reconnectPushButton->show();
        ui->connectionStatusLabel->setText("Couldn't connect to server");
    }
}


void Login::on_reconnectPushButton_clicked()
{
    emit reconnect();
}


