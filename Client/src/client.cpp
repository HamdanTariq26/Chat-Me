#include "client.h"
#include "pch.h"




Client::Client(QObject *parent)
    : QObject{parent}, m_timerLoop(m_context)

{
    connect(&m_discovery, &ClientDiscovery::serverFound, this, [this](QString ip1) {
        if (m_connectedOnce.exchange(true))
            return;

        qDebug() << "Found server at:" << ip1;
        ip = ip1;

        if (!Connect(ip.toStdString(), 60000)) {
            m_connectedOnce = false;
            return;
        }

        std::thread([this]() { this->process(); }).detach();
    });
}


void Client::process(){

    while(IsConnected()){
        Incoming().wait();
        while(!Incoming().empty()){
            auto msg = this->Incoming().pop_front();
            processMsg(msg.msg);
        }
    }

}

void Client::processMsg(olc::net::message<MessageType> &msg){
    olc::net::message<MessageType> reply;
    switch(msg.header.id){
    case MessageType::validationSuccessful:
        m_bValidated = true;
        emit connectionStatus(true);
        startChecking();
        break;

    case MessageType::validationUnsuccessful:
        m_bValidated = false;
        emit connectionStatus(false); // validation failed
        break;

    case MessageType::loginSuccessful:
        emit successful();
        break;
    case MessageType::loginUnSuccessful:
        emit loginUnSuccessful();
        break;
    case MessageType::registerationSuccessful:
        emit wasRegisterationSuccessful(true);
        break;
    case MessageType::usernameTaken:
        emit wasRegisterationSuccessful(false);
        break;
    case MessageType::newClient:
    {
        std::string username;
        msg >> username;
        emit updateClient(username);
        break;
    }
    case MessageType::loginDuplicate:

        break;

    case MessageType::removeClient:
    {
        std::string username;
        msg >> username;
        emit updateClient(username,true);
    }
        break;

    case MessageType::message:
    {
        std::string s_username, text;
        msg >> text >> s_username;
        emit updateMsg(s_username,text,false);
    }
        break;
    case MessageType::fileInfo: {
        std::string senderUsername;
        std::string filename;
        uint32_t filesize;

        msg >> senderUsername >> filename >> filesize;


        QString qFilename = QString::fromStdString(filename);

        if (m_outputFile) {
            delete m_outputFile;
        }
        m_outputFile = new QFile(qFilename);

        if (!m_outputFile->open(QIODevice::WriteOnly)) {
            qDebug() << "CRITICAL: Failed to open file for writing in current directory:"
                     << qFilename << "Error:" << m_outputFile->errorString();

            delete m_outputFile;
            m_outputFile = nullptr;
            receivingFile = false;
            break;
        }
        remainingBytes = filesize;
        receivingFile = true;
        break;
    }

    case MessageType::fileChunk: {
        std::string senderUsername;
        msg >> senderUsername;

        if (receivingFile && m_outputFile && m_outputFile->isOpen()) {
            qint64 bytesWritten = m_outputFile->write(
                reinterpret_cast<const char*>(msg.body.data()),
                msg.body.size()
                );

            if (bytesWritten == -1) {
                qDebug() << "Error writing file chunk:" << m_outputFile->errorString();
            }

            remainingBytes -= msg.body.size();
        }
        break;
    }

    case MessageType::fileComplete: {
        std::string senderUsername;
        msg >> senderUsername;

        if (receivingFile && m_outputFile && m_outputFile->isOpen()) {
            m_outputFile->close();
            delete m_outputFile;
            m_outputFile = nullptr;
            receivingFile = false;
            emit fileReceived(senderUsername);
        }
        break;
    }
    case MessageType::messageAll:
    {
        std::string s_username, text;
        msg >> text >> s_username;
        emit updateMsg(s_username,text,true);
    }
        break;


    default:

        break;
    }
}

void Client::tryConnect(){
    m_discovery.discover();
    QTimer::singleShot(5000, this, [this]() {
        if (!m_bValidated) {
            emit connectionStatus(false);
        }
    });

}

void Client::reconnect(){
    QString program = QCoreApplication::applicationFilePath();
    QStringList args = QCoreApplication::arguments();

    QProcess::startDetached(program, args);
    QCoreApplication::quit();
}

void Client::SendLoginRegister(std::string username,std::string password,bool forRegister){
    olc::net::message<MessageType>msg;
    if(forRegister){
        msg.header.id = MessageType::registeration;
        msg << username << password;
        Send(msg);
    }else{
        msg.header.id = MessageType::login;
        msg << username << password;
        Send(msg);
    }

}

void Client::SendMsg(std::string username,std::string msg){
    olc::net::message<MessageType>msgtoclient;
    msgtoclient.header.id = MessageType::message;
    msgtoclient << username << msg;
    Send(msgtoclient);
}

void Client::startChecking(){
    m_timerLoop.expires_after(m_checkInterval);
    checkConnectedToServer();
}

void Client::onDisconnectFromServer(){
    m_timerLoop.cancel();
    emit handle("Disconnected","Your are disconnect from server.\nPlease try again later",true);
}

void Client::checkConnectedToServer(){
    m_timerLoop.async_wait([this](std::error_code ec){
        if(!ec){
            m_timerLoop.expires_after(m_checkInterval);
            if(!IsConnected() && !alerted){
                alerted = true;
                onDisconnectFromServer();
            }
            checkConnectedToServer();
        }else{
        emit handle("An unexpected error occurred",ec.message());
        }
    });
}

void Client::SendNextChunk() {

    if (m_fileStream.eof() || m_remainingFileSize <= 0) {
        olc::net::message<MessageType> done;
        done.header.id = MessageType::fileComplete;
        done << m_receiverUsername;
        Send(done);

        m_fileStream.close();
        m_bFileSending = false;
        disconnect(this, &Client::chunkSent, this, &Client::SendNextChunk);
        return;
    }

    const size_t CHUNK = 65536;
    std::vector<char> buffer(CHUNK);

    std::streamsize bytes = std::min<std::streamsize>(m_remainingFileSize, (std::streamsize)CHUNK);
    m_fileStream.read(buffer.data(), bytes);
    m_remainingFileSize -= bytes;

    olc::net::message<MessageType> chunk;
    chunk.header.id = MessageType::fileChunk;
    chunk.body.assign(buffer.begin(), buffer.begin() + bytes);
    chunk << m_receiverUsername;
    Send(chunk);

    QTimer::singleShot(0, this, [this]() {
        emit chunkSent();
    });
}

void Client::SendFile(const std::string path, const std::string reciverUsername) {
    if (m_bFileSending) return;
    m_fileStream.open(path, std::ios::binary | std::ios::ate);
    if (!m_fileStream.is_open()) return;

    m_bFileSending = true;
    m_receiverUsername = reciverUsername;
    m_remainingFileSize = m_fileStream.tellg();
    m_fileStream.seekg(0);

    olc::net::message<MessageType> info;
    info.header.id = MessageType::fileInfo;
    QFileInfo fileInfo(QString::fromStdString(path));
    std::string filename_only = fileInfo.fileName().toStdString();
    info << (uint32_t)m_remainingFileSize;
    info << filename_only;
    info << reciverUsername;
    Send(info);

    connect(this, &Client::chunkSent, this, &Client::SendNextChunk, Qt::QueuedConnection);

    emit chunkSent();
}

void Client::SendMsgToEveryone(std::string msg)
{
    olc::net::message<MessageType>broadcast;
    broadcast.header.id = MessageType::messageAll;
    broadcast << msg;
    Send(broadcast);
}

