#include "mainx11.h"
#include <stdio.h>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QVariant>
#include <QByteArray>
#include <QProcess>
#include <QString>
#include <QFile>
#include <QCryptographicHash>
#include "ui_mainx11.h"

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
    QUrl ba = QString("ws://79.136.138.123:8000");
    mSocket = new QWebSocket();
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(textMessageReceived(QString)), SLOT(onTextMessageReceived(QString)));
    connect(mSocket, SIGNAL(readChannelFinished()), SLOT(readChannelFinished()));

    mSocket->open(ba);
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
    mSocket->sendTextMessage(str_sha1);
}


void mainX11::stateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << state;
}

QString mainX11::on_browse_clicked()
{
    QString browse = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(), tr("All files (*.*)"));
    QFile file(browse);
    ui->path->setText(browse);
    if(!file.open(QFile::ReadOnly)) {
        return 0;
    }
    QByteArray blob = file.readAll();
    mSocket->sendBinaryMessage(blob);

    qDebug() << "file size - " << blob.size();
}
