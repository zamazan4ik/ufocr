#ifndef WAITWINDOW_H
#define WAITWINDOW_H

#include <QWidget>

namespace Ui {
class WaitWindow;
}

class WaitWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit WaitWindow(QWidget *parent = 0);
    ~WaitWindow();
    
private:
    Ui::WaitWindow *ui;
};

#endif // WAITWINDOW_H
