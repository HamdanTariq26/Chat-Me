/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: register.h
   Description: 
*/
#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>

namespace Ui {
class Register;
}

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

signals:
    void onClose();
    void send(std::string username,std::string password);
    void autoFill(QString username,QString password);

private slots:
    void on_backPushButton_clicked();

    void on_showPasswordCheckBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_registerPushButton_clicked();

public slots:
    void onRegisterationTry(bool wasSuccessful);


private:
    Ui::Register *ui;
};

#endif // REGISTER_H

