#include "mainx11.h"
#include <stdio.h>
#include <QDebug>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonValue>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QJsonDocument>
#include <QVariant>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QCryptographicHash>
#include "ui_mainx11.h"
#define MSG_NOSIGNAL 0x2000 /* don't raise SIGPIPE */

mainX11::mainX11(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainX11)
{
    ui->setupUi(this);

    connect(ui->connect, SIGNAL(clicked()), this, SLOT(onConnected()));
}

mainX11::~mainX11()
{
    delete ui;
    delete mSocket;
}

void mainX11::onConnected() {
    QUrl ba = QString("ws://10.10.10.160:8000");
    mSocket = new QWebSocket();
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(textMessageReceived(QString)), SLOT(onTextMessageReceived(QString)));
    connect(mSocket, SIGNAL(readChannelFinished()), SLOT(readChannelFinished()));

    mSocket->open(ba);

    QBluetoothLocalDevice localDevice;
    QString localDeviceName;

    if(localDevice.isValid()) {
        localDevice.powerOn();
        localDeviceName = localDevice.name();
        localDevice.setHostMode(QBluetoothLocalDevice::HostConnectable);
        QList<QBluetoothAddress> remoteDevices;
        remoteDevices = localDevice.connectedDevices();
    }
    qDebug() << "Device name - " << localDeviceName;
}

void mainX11::startDeviceDiscovery() {
    QBluetoothDeviceDiscoveryAgent *discoverAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoverAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    discoverAgent->start(); // start scaning!
    qDebug() << "Scanning devices...";
}

void mainX11::deviceDiscovered(const QBluetoothDeviceInfo &info) {
    qDebug() << "Found the devices " << info.name() << "(" << info.address().toString() << ")";
}

void mainX11::readChannelFinished() {

}

void mainX11::onTextMessageReceived(const QString& message) {
    QByteArray jsonArr(message.toLatin1().data());
    QJsonParseError *err = new QJsonParseError();
    QJsonDocument doc  = QJsonDocument::fromJson(jsonArr, err);
    if(err->error != 0) {
        qDebug() << err->errorString();
        return;
    }

    QString jsonRAW = doc.toVariant().toMap()["data"].toString();
    qDebug() << jsonRAW;

    QString salt = QString("%1%2").arg("01").arg("BOOM").append(jsonRAW);
    qDebug() << "Salt" << salt;
    str_sha1 = QString(QCryptographicHash::hash(salt.toLatin1(),QCryptographicHash::Sha1).toHex());
    qDebug() << "str_sha1"  << str_sha1;
    quint64 i = 2000000;
    mSocket->setReadBufferSize(i);
    mSocket->sendTextMessage(str_sha1);
}


void mainX11::stateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << state;
}

void mainX11::on_send_clicked()
{
    QFile *file = new QFile();
    file->setFileName("/home/kdelinx/shakira.mp3");
    ui->path->setText(file->fileName());
    if(!file->open(QFile::ReadWrite|QIODevice::ReadOnly)) {
        return;
    }
    mSocket->bytesWritten(file->size() / 10);
    mSocket->readBufferSize();
    QByteArray blob = file->readAll();
    qDebug() << "sizeof before - " << blob;
    mSocket->sendBinaryMessage(blob);
    qDebug() << "file size - " << blob.size();
    qDebug() << "sizeof after - " << sizeof(blob);
    mSocket->ping(blob);
}
