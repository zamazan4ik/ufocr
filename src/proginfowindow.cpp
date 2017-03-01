#include "proginfowindow.h"
#include "ui_proginfowindow.h"

ProgInfoWindow::ProgInfoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgInfoWindow)
{
    ui->setupUi(this);
}

ProgInfoWindow::~ProgInfoWindow()
{
    delete ui;
}
