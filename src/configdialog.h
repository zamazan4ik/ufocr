#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui
{
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    void accept();
private slots:
    void on_pushButtonTessData_clicked();

    void on_pushButtonLangs_clicked();

    void itemClicked(QListWidgetItem *item);
    void on_checkBoxProcessAfterDeskew_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

private:
    void init();
private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
