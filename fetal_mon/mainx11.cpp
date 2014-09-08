#include <QDesktopWidget>
#include <QTime>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QKeyEvent>
#include "mainx11.h"
#include "ui_mainx11.h"

mainX11::mainX11(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainX11)
{
    ui->setupUi(this);

    setWindowTitle(tr("Фетальный монитор"));
    setWindowModified(true);

    connect(ui->pushButton, SIGNAL(clicked()), SLOT(gen_code()));
}

mainX11::~mainX11()
{
    delete ui;
}

void mainX11::gen_code()
{
    double x = 2.1;

            ui->show_status->setText(QString::number(x));

            if (ui->show_status->text().toDouble() <= 1.40) {
                green();
            }
            if (ui->show_status->text().toDouble() >= 1.50 && ui->show_status->text().toDouble() <= 1.80) {
                yellow();
            }
            if (ui->show_status->text().toDouble() >= 1.90) {
                red();
            }
            ui->freq_heart->setText(QString::number(x*10));
}

void mainX11::red() {
    QImage pm;
    pm.load(":/pic/red-circle-md.png");
    ui->stat_1->setPixmap(QPixmap::fromImage(pm));
}

void mainX11::yellow() {
    QImage pm;
    pm.load(":/pic/speech_circle_yellow.png");
    ui->stat_2->setPixmap(QPixmap::fromImage(pm));
}

void mainX11::green() {
    QImage pm;
    pm.load(":/pic/green-circle-md.png");
    ui->stat_3->setPixmap(QPixmap::fromImage(pm));
}

void mainX11::keyPressEvent(QKeyEvent *event) {
    if(event->DeactivateControl == Qt::Key_Control)
        QMessageBox::warning(0, "Info", "Нажата кнопка popup_menu");
        event->accept();
    // emit keyPressEvent(event);

}

void mainX11::mousePressEvent(QMouseEvent * event) {
    setFocusPolicy(Qt::ClickFocus);
}
