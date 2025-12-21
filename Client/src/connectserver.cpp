#include "connectserver.h"
#include "ui_connectserver.h"

ConnectServer::ConnectServer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectServer)
{
    ui->setupUi(this);
    onLan = true;
    serverip = "";
    port = "";

}

ConnectServer::~ConnectServer()
{
    delete ui;
}

void ConnectServer::on_pushButton_clicked()
{
    onLan = true;
    serverip = "";
    port = "";
    emit connectToBy(onLan,"0","0");
}


void ConnectServer::on_pushButton_2_clicked()
{
    if(!ui->serverLineEdit->text().isEmpty() && !ui->portLineEdit->text().isEmpty()){
        serverip = ui->serverLineEdit->text();
        port = ui->portLineEdit->text();
        emit connectToBy(onLan,serverip,port);
    }else{
        QMessageBox::warning(this,"Warning","Server ip/port number cann't be empty..!");
    }
}


void ConnectServer::on_pushButton_3_clicked()
{
    QApplication::exit();
}

