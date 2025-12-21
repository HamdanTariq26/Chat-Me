#include "register.h"
#include "ui_register.h"
#include "pch.h"
Register::Register(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);
}

Register::~Register()
{
    delete ui;
}

void Register::on_backPushButton_clicked()
{
    emit onClose();
    this->close();
}


void Register::on_showPasswordCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked){
        ui->newPasswordLineEdit->setEchoMode(QLineEdit::Normal);
        ui->confirmLineEdit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->newPasswordLineEdit->setEchoMode(QLineEdit::Password);
        ui->confirmLineEdit->setEchoMode(QLineEdit::Password);
    }
}


void Register::on_registerPushButton_clicked()
{

    QString username = ui->userNameLineEdit->text().trimmed();
    QString newPassword = ui->newPasswordLineEdit->text().trimmed();
    QString confirmPassword = ui->confirmLineEdit->text().trimmed();


    if (username.isEmpty()) {
        QMessageBox::warning(this, "Username Invalid", "Username cannot be empty.");
        return;
    }
    if (username.contains(" ")) {
        QMessageBox::warning(this, "Username Invalid", "Username should not contain whitespaces.");
        return;
    }

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Password Invalid", "Password cannot be empty.");
        return;
    }
    if (newPassword.contains(" ") || confirmPassword.contains(" ")) {
        QMessageBox::warning(this, "Password Invalid", "Password should not contain whitespaces.");
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "Password Invalid", "Passwords do not match.");
        return;
    }

    emit send(username.toStdString(),newPassword.toStdString());
}

void Register::onRegisterationTry(bool wasSuccessful)
{
    if(wasSuccessful){
        QMessageBox::information(this,"Registeration","Registeration was successful");
        emit autoFill(ui->userNameLineEdit->text(),ui->newPasswordLineEdit->text());
        this->close();
    }else{
        QMessageBox::information(this,"Registeration","Registeration was unsuccessful.\nUsername taken");
    }

}


