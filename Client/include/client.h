#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "olc_net.h"
#include "types.h"
#include <fstream>
#include <QFile>
#include "clientdiscovery.h"

class Client : public QObject , public olc::net::client_interface<MessageType>
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

signals:
    void connectionStatus(bool arg);
    void successful();
    void updateClient(std::string username,bool remove = false);
    void updateMsg(std::string s_username,std::string msg,bool isBroadcast = false);
    void handle(std::string title,std::string msg,bool closeApp = false);
    void fileReceived(std::string username);
    void chunkSent();
    void wasRegisterationSuccessful(bool);
    void loginUnSuccessful();
public slots:
    void tryConnect();//bool isLan,QString serverip,QString port
    void reconnect();
    void SendLoginRegister(std::string username,std::string password,bool forRegister = false);
    void SendMsg(std::string username,std::string msg);
    void SendFile(const std::string path,const std::string reciverUsername);
    void SendMsgToEveryone(std::string msg);
public:
    void process();
    void processMsg(olc::net::message<MessageType>&);
    void checkConnectedToServer();
    void startChecking();
    void onDisconnectFromServer();
    void SendNextChunk();

private:
    bool m_bValidated = false;
    asio::steady_timer m_timerLoop;
    const std::chrono::milliseconds m_checkInterval = std::chrono::milliseconds(7000);
    QFile* m_outputFile = nullptr;
    std::ifstream m_fileStream;
    std::string m_receiverUsername;
    std::streamsize m_remainingFileSize;
    std::atomic<bool> m_bFileSending = false;
    bool receivingFile = false;
    uint32_t remainingBytes = 0;
    bool alerted = false;
    ClientDiscovery m_discovery;
    QString ip;
};

#endif // CLIENT_H
