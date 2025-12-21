#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "pch.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->MsgLineEdit->setPlaceholderText("Type message...");

    connect(ui->usersAreaListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item){
        ui->chatAreaListWidget->clear();
        currentUser = item->text();

        if(userChat.contains(currentUser)){
            for(const QString& msg : userChat[currentUser]){
                bool isMe = msg.startsWith("Me");
                QString displayMsg = msg;
                if(isMe){
                    displayMsg = msg.mid(msg.indexOf(":") + 1).trimmed();
                }
                QString time = QTime::currentTime().toString("hh:mm");
                QWidget* messageWidget = createChatMessage(isMe ? "Me" : currentUser, displayMsg, time, isMe);

                QListWidgetItem* chatItem = new QListWidgetItem(ui->chatAreaListWidget);
                chatItem->setSizeHint(messageWidget->sizeHint());
                ui->chatAreaListWidget->addItem(chatItem);
                ui->chatAreaListWidget->setItemWidget(chatItem, messageWidget);
            }
        }

        ui->chatAreaListWidget->scrollToBottom();

        items = ui->usersAreaListWidget->findItems(currentUser, Qt::MatchExactly);
        if(!items.isEmpty()){
            QFont font = items.first()->font();
            font.setBold(false);
            items.first()->setFont(font);
        }
    });

    this->setWindowTitle("Chat Me");
}

MainWindow::~MainWindow()
{
    delete ui;
}


QWidget* MainWindow::createChatMessage(const QString& sender, const QString& msg, const QString& time, bool isMe)
{
    const int MAX_WIDTH = 450;
    const int MIN_WIDTH = 110;

    QWidget* rowWidget = new QWidget();
    rowWidget->setStyleSheet("background: transparent; border: none;");

    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(10, 2, 10, 2);
    rowLayout->setSpacing(0);

    QFrame* bubbleFrame = new QFrame();
    QString bg = isMe ? "#dcf8c6" : "#ffffff";
    QString corners = isMe ? "border-top-right-radius: 0px;" : "border-top-left-radius: 0px;";

    bubbleFrame->setStyleSheet(QString(
                                   "QFrame { "
                                   "  background-color: %1; "
                                   "  border-radius: 12px; "
                                   "  %2 "
                                   "  border: 1px solid #dcdcdc; "
                                   "}"
                                   "QLabel { border: none; background: transparent; padding: 0px; }"
                                   ).arg(bg).arg(corners));

    QVBoxLayout* bubbleLayout = new QVBoxLayout(bubbleFrame);
    bubbleLayout->setContentsMargins(12, 10, 12, 10);
    bubbleLayout->setSpacing(4);


    QLabel* nameLabel = new QLabel();
    if (isMe) {
        nameLabel->setText("Me");

        nameLabel->setStyleSheet("color: #007bff; font-weight: bold; font-size: 9pt; letter-spacing: 0.5px;");
    } else {
        nameLabel->setText(sender);
        nameLabel->setStyleSheet("color: #e542a3; font-weight: bold; font-size: 9pt;");
    }
    bubbleLayout->addWidget(nameLabel);

    QLabel* msgLabel = new QLabel(msg);
    msgLabel->setWordWrap(true);
    msgLabel->setStyleSheet("font-size: 11pt; color: black;");

    QFontMetrics fm(msgLabel->font());
    int textWidthOnOneLine = fm.horizontalAdvance(msg);

    int finalWidth;
    if (textWidthOnOneLine + 40 < MAX_WIDTH) {
        finalWidth = std::max(textWidthOnOneLine + 10, MIN_WIDTH);
    } else {
        finalWidth = MAX_WIDTH;
    }

    msgLabel->setFixedWidth(finalWidth);
    int finalHeight = msgLabel->sizeHint().height() + 5;
    msgLabel->setFixedHeight(finalHeight);

    bubbleLayout->addWidget(msgLabel);

    QLabel* timeLabel = new QLabel(time);
    timeLabel->setAlignment(Qt::AlignRight);
    timeLabel->setStyleSheet("color: #808080; font-size: 8pt;");
    bubbleLayout->addWidget(timeLabel);

    bubbleFrame->setFixedWidth(finalWidth + 24);

    if (isMe) {
        rowLayout->addStretch();
        rowLayout->addWidget(bubbleFrame);
    } else {
        rowLayout->addWidget(bubbleFrame);
        rowLayout->addStretch();
    }

    rowWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    return rowWidget;
}




void MainWindow::updateUserList(std::string username,bool remove){
    QString Qusername = QString::fromStdString(username);
    if(!remove){
        ui->usersAreaListWidget->addItem(Qusername);
    }else{
        for(int i = 0; i < ui->usersAreaListWidget->count(); i++){
            if(ui->usersAreaListWidget->item(i)->text() == username){
                delete ui->usersAreaListWidget->takeItem(i);
                break;
            }
        }
    }

}


void MainWindow::on_sendMsgPushButton_clicked()
{
    QString msg = ui->MsgLineEdit->text().trimmed();
    if(msg.isEmpty() || currentUser.isEmpty()) return;

    QString time = QTime::currentTime().toString("hh:mm");

    userChat[currentUser].append("Me: " + msg);
    emit SendMsg(currentUser.toStdString(), msg.toStdString());

    QWidget* messageWidget = createChatMessage("Me", msg, time, true);
    QListWidgetItem* item = new QListWidgetItem(ui->chatAreaListWidget);
    item->setSizeHint(messageWidget->sizeHint());
    ui->chatAreaListWidget->addItem(item);
    ui->chatAreaListWidget->setItemWidget(item, messageWidget);
    ui->chatAreaListWidget->scrollToBottom();

    ui->MsgLineEdit->clear();
}

void MainWindow::updateMsg(std::string s_username, std::string msg, bool isBroadcast)
{
    QString senderUsername = QString::fromStdString(s_username);
    QString senderMsg = QString::fromStdString(msg);
    QString time = QTime::currentTime().toString("hh:mm");

    items = ui->usersAreaListWidget->findItems(senderUsername, Qt::MatchExactly);
    if(!items.isEmpty() && currentUser != senderUsername){
        QFont font = items.first()->font();
        font.setBold(true);
        items.first()->setFont(font);
    }

    userChat[senderUsername].append(senderMsg);

    if(currentUser == senderUsername || isBroadcast){
        QWidget* messageWidget = createChatMessage(isBroadcast ? senderUsername + " (Broadcast)" : senderUsername, senderMsg, time, false);
        QListWidgetItem* item = new QListWidgetItem(ui->chatAreaListWidget);
        item->setSizeHint(messageWidget->sizeHint());
        ui->chatAreaListWidget->addItem(item);
        ui->chatAreaListWidget->setItemWidget(item, messageWidget);
        ui->chatAreaListWidget->scrollToBottom();
    }
}

void MainWindow::ShowUser(std::string title, std::string msg, bool closeApp)
{
    QString Qtitle = QString::fromStdString(title);
    QString Qmsg = QString::fromStdString(msg);
    QMessageBox::warning(this,Qtitle,Qmsg);
    if(closeApp){
        QString program = QCoreApplication::applicationFilePath();
        QStringList args = QCoreApplication::arguments();
        QProcess::startDetached(program, args);
        QCoreApplication::quit();
    }
}

void MainWindow::on_filePushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Select file to send");
    if(path.isEmpty() || currentUser.isEmpty()) return;

    QString time = QTime::currentTime().toString("hh:mm");
    QString fileName = QFileInfo(path).fileName();
    QString msgText = "Sent file: " + fileName;

    userChat[currentUser].append("Me: " + msgText);

    QWidget* messageWidget = createChatMessage("Me", msgText, time, true);
    QListWidgetItem* item = new QListWidgetItem(ui->chatAreaListWidget);
    item->setSizeHint(messageWidget->sizeHint());
    ui->chatAreaListWidget->addItem(item);
    ui->chatAreaListWidget->setItemWidget(item, messageWidget);
    ui->chatAreaListWidget->scrollToBottom();

    emit SendFile(path.toStdString(), currentUser.toStdString());
}

void MainWindow::fileRecived(std::string username)
{
    QString Qusername = QString::fromStdString(username);
    QString time = QTime::currentTime().toString("hh:mm");
    QString msgText = "Sent a file";

    userChat[Qusername].append(msgText);

    if(currentUser == Qusername){
        QWidget* messageWidget = createChatMessage(Qusername, msgText, time, false);
        QListWidgetItem* item = new QListWidgetItem(ui->chatAreaListWidget);
        item->setSizeHint(messageWidget->sizeHint());
        ui->chatAreaListWidget->addItem(item);
        ui->chatAreaListWidget->setItemWidget(item, messageWidget);
        ui->chatAreaListWidget->scrollToBottom();
    }
}

void MainWindow::on_actionSend_a_broadcast_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Broadcast Message");
    dialog.setMinimumWidth(520);

    QLineEdit* edit = new QLineEdit(&dialog);
    edit->setPlaceholderText("Type message...");
    edit->setStyleSheet("QLineEdit { border: 1px solid #cccccc; background: white; border-radius: 18px; padding: 8px 14px; font-size: 14px;}");

    QPushButton* sendBtn = new QPushButton("Send", &dialog);
    sendBtn->setFixedHeight(36);
    sendBtn->setStyleSheet("QPushButton { background-color: #25d366; color: white; border: none; border-radius: 18px; padding: 0 14px; font-size: 14px; } QPushButton:hover { background-color: #1aae55;}");

    QPushButton* cancelBtn = new QPushButton("Cancel", &dialog);
    cancelBtn->setFixedHeight(36);
    cancelBtn->setStyleSheet("QPushButton { background-color: #ffd6d6; border: none; border-radius: 18px; padding: 0 14px; font-size: 14px; } QPushButton:hover { background-color: #ffbdbd;}");

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(edit, 1);
    hLayout->addWidget(sendBtn);
    hLayout->addWidget(cancelBtn);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(hLayout);

    QObject::connect(sendBtn, &QPushButton::clicked, [&]() { dialog.accept(); });
    QObject::connect(cancelBtn, &QPushButton::clicked, [&]() { dialog.reject(); });

    if(dialog.exec() == QDialog::Accepted){
        QString msg = edit->text().trimmed();
        if(!msg.isEmpty()){
            QString time = QTime::currentTime().toString("hh:mm");

            for(int i = 0; i < ui->usersAreaListWidget->count(); i++){
                QListWidgetItem* item = ui->usersAreaListWidget->item(i);
                QString username = item->text();

                userChat[username].append("Me (Broadcast): " + msg);

                if(currentUser == username){
                    QWidget* messageWidget = createChatMessage("Me (Broadcast)", msg, time, true);
                    QListWidgetItem* chatItem = new QListWidgetItem(ui->chatAreaListWidget);
                    chatItem->setSizeHint(messageWidget->sizeHint());
                    ui->chatAreaListWidget->addItem(chatItem);
                    ui->chatAreaListWidget->setItemWidget(chatItem, messageWidget);
                    ui->chatAreaListWidget->scrollToBottom();
                }
            }

            emit broadcastMsg(msg.toStdString());
        }
    }

    delete edit;
    delete sendBtn;
    delete cancelBtn;
    delete hLayout;
    delete mainLayout;
}

void MainWindow::on_actionOne_lone_coder_triggered()
{
    QMessageBox::about(
        this,
        "About / Acknowledgement",
        "I would like to express my appreciation to OneLoneCoder (javidx9) for his "
        "educational content on C++ networking and ASIO. My understanding of "
        "asynchronous networking, client–server models, and message handling was "
        "mainly developed by watching his tutorials and following his "
        "implementation step-by-step.\n\n"

        "The networking framework used in this project was coded while following "
        "his videos, and his explanations played an essential role in helping me "
        "implement asynchronous communication. Although the code has been adapted "
        "for my own project requirements, the core concepts and motivation came "
        "from his work.\n\n"

        "— Thank you, One Lone Coder."
        );

}


void MainWindow::on_actionC_triggered()
{
    QMessageBox::about(this,
                       "About C++",
                       "This project is built using C++. C++ is a high-performance, general-purpose programming language "
                       "that supports procedural, object-oriented, and generic programming paradigms. "
                       "It offers fine-grained control over system resources and memory management, "
                       "making it ideal for applications that require efficiency, speed, and reliability. "
                       "C++ is widely used in software development, game engines, real-time systems, "
                       "and networked applications.");

}


void MainWindow::on_actionAsio_triggered()
{
    QMessageBox::about(this,
                       "About ASIO",
                       "This project uses ASIO (Asynchronous Input/Output) for network communication. "
                       "ASIO is a cross-platform C++ library that provides low-level networking and asynchronous I/O capabilities. "
                       "It allows developers to efficiently handle sockets, timers, and other I/O objects without blocking, "
                       "enabling the creation of high-performance, responsive, and scalable network applications.");

}


void MainWindow::on_actionQt_triggered()
{
    QMessageBox::aboutQt(this);
}


void MainWindow::on_actionProject_triggered()
{
    QMessageBox::about(this,
                       "About Project",
                       "This project is a fully functional LAN chat application, developed entirely in C++ using the ASIO networking library. "
                       "It is designed to provide a simple yet powerful platform for real-time communication between users connected over a local network. "
                       "The application combines efficient networking with a clean, intuitive GUI to ensure smooth and reliable communication.\n\n"
                       "Key Features:\n"
                       "• Real-Time Messaging: Send and receive messages instantly, with clear identification of sender and recipient.\n"
                       "• Visual Chat Interface: Messages are displayed in chat bubbles, color-coded to differentiate between sent and received messages, "
                       "and each message includes a timestamp for reference.\n"
                       "• File Transfer: Share files seamlessly with other users on the network. Received files are displayed with proper labels and timestamps.\n"
                       "• Broadcast Messaging: Send a message to all connected users simultaneously, allowing for announcements or group notifications.\n"
                       "• User List and Status: Displays currently connected users, highlights unread messages, and visually indicates active conversations.\n"
                       "• Clean and Modern Design: The interface is inspired by popular chat applications, providing a familiar experience while maintaining simplicity.\n\n"
                       "This application is built using C++, leveraging the performance, memory control, and object-oriented features of the language. "
                       "ASIO is used for robust and efficient networking, enabling asynchronous communication that ensures responsiveness and reliability. "
                       "The combination of these technologies demonstrates a practical implementation of modern networking and GUI design concepts, "
                       "and serves as a foundation for further enhancements such as encryption, multimedia support, or scalability beyond LAN environments.\n\n"
                       "Overall, this project showcases the integration of C++ programming, network communication using ASIO, "
                       "and user interface design to create a professional-grade local chat application, suitable for both learning and practical usage.");

}


