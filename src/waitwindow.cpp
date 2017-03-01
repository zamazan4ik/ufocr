#include "waitwindow.h"
#include "ui_waitwindow.h"

WaitWindow::WaitWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaitWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

WaitWindow::~WaitWindow()
{
    delete ui;
}
