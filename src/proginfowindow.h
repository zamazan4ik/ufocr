#ifndef PROGINFOWINDOW_H
#define PROGINFOWINDOW_H

#include <QDialog>

namespace Ui {
class ProgInfoWindow;
}

class ProgInfoWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProgInfoWindow(QWidget *parent = 0);
    ~ProgInfoWindow();
    
private:
    Ui::ProgInfoWindow *ui;
};

#endif // PROGINFOWINDOW_H
