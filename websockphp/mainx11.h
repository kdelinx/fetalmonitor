#ifndef MAINX11_H
#define MAINX11_H

#include <QMainWindow>
#include <QWebSocket>
#include <QBluetoothSocket>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>

namespace Ui {
    class mainX11;
}

class QAuthenticator;

class mainX11 : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainX11(QWidget *parent = 0);
    ~mainX11();
private slots:
    void onConnected();
    void onTextMessageReceived(const QString &message);
    void stateChanged(QAbstractSocket::SocketState state);
    void on_send_clicked();
    void startScan();
    void addDevice(const QBluetoothDeviceInfo &device);
    void finishScan();
    void displayPairingMenu(const QPoint &pos);
    void pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

private:
    Ui::mainX11 *ui;
    QWebSocket *mSocket;
    QString str_sha1;
    QByteArray *blob;
    QBluetoothLocalDevice *localDevice;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *btSocket;
};

#endif // MAINX11_H
