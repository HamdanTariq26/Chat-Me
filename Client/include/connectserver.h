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
