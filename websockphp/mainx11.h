#ifndef MAINX11_H
#define MAINX11_H

#include <QMainWindow>
#include <QWebSocket>
#include <QFileDialog>

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
    void readChannelFinished();
    void onTextMessageReceived(const QString &message);
    void stateChanged(QAbstractSocket::SocketState state);
    void on_send_clicked();

private:
    Ui::mainX11 *ui;
    QFileDialog *fileDialog;
    QWebSocket *mSocket;
    QString str_sha1;
    QString *browse;
    QByteArray *blob;
};

#endif // MAINX11_H
