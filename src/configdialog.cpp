#include "configdialog.h"
#include "ui_configdialog.h"
#include "settings.h"
#include "langselectdialog.h"
#include <QFileDialog>
#include <QStringList>
#include <QLocale>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    //ui->listWidget->setMinimumWidth(ui->listWidget->sizeHintForColumn(0));
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setToolTip(ui->listWidget->item(i)->text());
    init();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::accept()
{
    Settings *settings = Settings::instance();
    if (ui->radioButtonCuneiform->isChecked())
        settings->setSelectedEngine(UseCuneiform);
    else
        settings->setSelectedEngine(UseTesseract);
    settings->setTessdataPath(ui->lineEditTessData->text());
    if (ui->checkBoxSingleLang->isChecked()) {
        QStringList sl;
        sl << ui->comboBoxSingleLang->currentText();
        settings->setSelectedLanguages(sl);
        settings->setLanguage(settings->getShortLanguageName(sl[0]));
    }
    settings->setCropLoaded(ui->checkBoxCrop->isChecked());
    settings->setAutoDeskew(ui->checkBoxDeskew->isChecked());
    settings->setPreprocessed(ui->checkBoxPreprocess->isChecked());
    settings->setDoublePreprocessed(ui->checkBoxProcessAfterDeskew->isChecked());
    settings->setNoLocale(false);
    if (ui->checkBox->isChecked()) {
        settings->setNoLocale(true);
    } else {
        settings->setNoLocale(false);
    }
    if (ui->checkBox_2->isChecked())
        settings->setAutosaveInterval(ui->spinBox_2->value());
    else
        settings->setAutosaveInterval(30000);
    settings->setFontSize(ui->fontSizeSpinBox->value());
    settings->setMaxRecentProjects(ui->recentProjectsSpinBox->value());
    QDialog::accept();
}

void ConfigDialog::init()
{
    Settings *settings = Settings::instance();
    ui->radioButtonCuneiform->setChecked(settings->getSelectedEngine() == UseCuneiform);
    ui->radioButtonTesseract->setChecked(settings->getSelectedEngine() == UseTesseract);
    ui->lineEditTessData->setText(settings->getTessdataPath());
    QStringList sl = settings->getSelectedLanguages();
    ui->checkBoxSingleLang->setChecked(sl.count() == 1);

    QStringList sl2 = settings->fullLanguageNames();
    sl2.prepend(settings->getFullLanguageName(settings->getLanguage()));
    sl2.removeDuplicates();
    ui->comboBoxSingleLang->clear();
    ui->comboBoxSingleLang->addItems(sl2);
    ui->checkBoxCrop->setChecked(settings->getCropLoaded());
    ui->checkBoxDeskew->setChecked(settings->getAutoDeskew());
    ui->checkBoxPreprocess->setChecked(settings->getPreprocessed());
    ui->checkBoxProcessAfterDeskew->setChecked(settings->getDoublePreprocessed());
    if (settings->getAutosaveInterval() > 60) {
        ui->checkBox_2->setChecked(false);
        ui->spinBox_2->setValue(15);
        ui->spinBox_2->setEnabled(false);
    } else{
        ui->spinBox_2->setValue(settings->getAutosaveInterval());
        ui->spinBox_2->setEnabled(true);
    }

    QStringList sl3;
    if (settings->useNoLocale())
        sl3 << "English";
    sl3 << QLocale::languageToString(QLocale::system().language()) << "English" << "Russian";
    sl3.removeDuplicates();
    //ui->comboBoxInterfaceLang->addItems(sl3);

    ui->fontSizeSpinBox->setValue(settings->getFontSize());

    ui->recentProjectsSpinBox->setValue(settings->getMaxRecentProjects());

}



void ConfigDialog::on_pushButtonTessData_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, trUtf8("Tesseract Data Directory"), "/");
    if (dir != "") {
        if (dir.endsWith("tessdata/"))
            dir.truncate(dir.length()-9);
        if (dir.endsWith("tessdata"))
            dir.truncate(dir.length()-8);
        if (!dir.endsWith("/"))
            dir = dir + "/";
        ui->lineEditTessData->setText(dir);
    }
}

void ConfigDialog::on_pushButtonLangs_clicked()
{
    ui->checkBoxSingleLang->setChecked(false);
    LangSelectDialog lsd;
    lsd.exec();
}

void ConfigDialog::itemClicked(QListWidgetItem *item)
{
    ui->stackedWidget->setCurrentIndex(ui->listWidget->row(item));
}

void ConfigDialog::on_checkBoxProcessAfterDeskew_toggled(bool checked)
{
    if (ui->checkBoxProcessAfterDeskew->isChecked())
        ui->checkBoxDeskew->setChecked(true);
}

void ConfigDialog::on_checkBox_2_toggled(bool checked)
{
    ui->spinBox_2->setEnabled(ui->checkBox_2->isChecked());
}
