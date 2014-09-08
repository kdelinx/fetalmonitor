#ifndef MAINX11_H
#define MAINX11_H

#include <QMainWindow>

namespace Ui {
class mainX11;
}

class mainX11 : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainX11(QWidget *parent = 0);
    ~mainX11();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent * event);

private slots:
    void gen_code();
    void red();
    void green();
    void yellow();

private:
    Ui::mainX11 *ui;
};

#endif // MAINX11_H
