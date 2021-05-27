#include "tcpserver.h"
#include "ui_tcpserver.h"

TcpServer::TcpServer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    //Init the server
    initServer();
    //Random messages
    for (int i = 0; i < 20; i++) {
        randomMessages << "Mensaje aleatorio " + QString::number(i);
    }
    //connects
    connect(ui->btn_close, &QAbstractButton::clicked, this, &QWidget::close);
    //connect for the server socket
    connect(qTcpServer, &QTcpServer::newConnection, this, &TcpServer::sendRandomMessage);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::initServer()
{
    qInfo() << "init server\n";
    qTcpServer = new QTcpServer(this);

    if(!qTcpServer->listen()){
        qInfo() << "Server error: " + qTcpServer->errorString();
        return;
    }

    QString port = QString::number(qTcpServer->serverPort(), 10);
    QString ipAddress;
    QList<QHostAddress> ipAddressList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressList.size(); i++) {
        if(ipAddressList.at(i) != QHostAddress::LocalHost && ipAddressList.at(i).toIPv4Address()){
            ipAddress = ipAddressList.at(i).toString();
            qInfo() << "ip: " << ipAddress << Qt::endl;
            break;
        }
    }
    if(ipAddressList.isEmpty()){
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    QString info = "ip: " + ipAddress + ", puerto: " + port;
    ui->lbl_info->setText(info);
}

void TcpServer::sendRandomMessage()
{
    qInfo() << "Send random message\n";
    QByteArray qByteArray;
    QDataStream qDataSteam(&qByteArray, QIODevice::WriteOnly);
    QString randomMessage = randomMessages[QRandomGenerator::global()->bounded(randomMessages.size())];
    qDataSteam << randomMessage;
    QTcpSocket *clientConnection = qTcpServer->nextPendingConnection();
    clientConnection->write(qByteArray);
    clientConnection->disconnectFromHost();
    ui->lbl_message->setText(randomMessage);
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection,&QObject::deleteLater);
}
