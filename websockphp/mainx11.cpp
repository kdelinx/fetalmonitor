#include "mainx11.h"
#include <QMenu>
#include <QDebug>
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

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();

    connect(ui->connect, SIGNAL(clicked()), this, SLOT(onConnected()));
    connect(ui->connect, SIGNAL(clicked()), this, SLOT(startScan()));

    connect(ui->connect, SIGNAL(clicked()), this, SLOT(startScan()));
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(finishScan()));

    ui->list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(displayPairingMenu(QPoint)));
    connect(localDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),
            this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
}

mainX11::~mainX11()
{
    delete ui;
    delete mSocket;
    delete discoveryAgent;
}

void mainX11::onConnected() {
    QUrl ba = QString("ws://10.10.10.160:8000");
    mSocket = new QWebSocket();
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(textMessageReceived(QString)), SLOT(onTextMessageReceived(QString)));
    connect(mSocket, SIGNAL(readChannelFinished()), SLOT(readChannelFinished()));

    mSocket->open(ba);
}

void mainX11::startScan() {
    ui->connect->setEnabled(false);
    localDevice = new QBluetoothLocalDevice();
    QString localDeviceName;

    if(localDevice->isValid()) {
        localDevice->powerOn();
        localDeviceName = localDevice->name();
        localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);
    }
    // start scan
    discoveryAgent->start();
    ui->devname->setText(localDeviceName);
}

void mainX11::addDevice(const QBluetoothDeviceInfo &device) {
    QString label = QString("%1%2").arg(device.address().toString()).arg(device.name());
    QList<QListWidgetItem *> items = ui->list->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(device.address());
        if (pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired) {
            item->setTextColor(QColor(Qt::green));
        }else{
            item->setTextColor(QColor(Qt::red));
        }
        ui->list->addItem(item);
    }
}

void mainX11::finishScan() {
    ui->connect->setEnabled(true);
}

void mainX11::displayPairingMenu(const QPoint &pos)
{
    QMenu menu(this);
    QAction *pairAction = menu.addAction("Pair");
    QAction *removePairAction = menu.addAction("Remove Pairing");
    QAction *chosenAction = menu.exec(ui->list->viewport()->mapToGlobal(pos));
    QListWidgetItem *currentItem = ui->list->currentItem();

    QString text = currentItem->text();
    int index = text.indexOf(' ');
    if (index == -1)
        return;

    QBluetoothAddress address (text.left(index));
    if (chosenAction == pairAction) {
        localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);
    } else if (chosenAction == removePairAction) {
        localDevice->requestPairing(address, QBluetoothLocalDevice::Unpaired);
    }
}

void mainX11::pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
{
    QList<QListWidgetItem *> items = ui->list->findItems(address.toString(), Qt::MatchContains);

    if (pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired ) {
        for (int var = 0; var < items.count(); ++var) {
            QListWidgetItem *item = items.at(var);
            item->setTextColor(QColor(Qt::green));
        }
    } else {
        for (int var = 0; var < items.count(); ++var) {
            QListWidgetItem *item = items.at(var);
            item->setTextColor(QColor(Qt::red));
        }
    }
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
