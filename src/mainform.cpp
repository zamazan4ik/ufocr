/*
   UFOCR - User-Friendly OCR
   Copyright (C) 2017 Alexander Zaitsev <zamazan4ik@tut.by>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "sidebar.h"
#include "droplabel.h"
#include "popplerdialog.h"
#include "pdfextractor.h"
#include "pdf2ppt.h"
#include "ghostscr.h"
#include "configdialog.h"
#include "mainform.h"
#include "tpagecollection.h"
#include "scanner.h"
#include "projectmanager.h"
#include "langselectdialog.h"
#include "tiffimporter.h"
#include "busyform.h"
#include "recognizerwrapper.h"
#include "recognitiondialog.h"
#include "extprocessdialog.h"
#include "saveprojectdialog.h"
#include "loadprojectdialog.h"
#include "djvu2pdf.h"
#include "menuaction.h"
#include "autosaver.h"
#include "waitwindow.h"
#include "globallock.h"
#include <signal.h>
#include <QComboBox>
#include <QLabel>
#include <QPixmap>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPainter>
#include <QSize>
#include <QStringList>
#include <QList>
#include <QRect>
#include <QRectF>
#include <QStatusBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QClipboard>
#include <QMap>
#include <QWidgetAction>
#include <QPushButton>
#include "qgraphicsinput.h"
#include "utils.h"
#include "qxtunixsignalcatcher.h"
#include <QUrl>
#include <QCheckBox>
#include <QEvent>
#include <QCursor>
#include <QLineEdit>
#include <QToolTip>
#include <QPoint>
#include <QAction>


MainForm::MainForm(QWidget* parent) : QMainWindow(parent)
{
    setupUi(this);

    pages = PageCollection::instance();

    setWindowTitle("YAGF");
    //spellChecker = new SpellChecker(textEdit);
    textEdit->enumerateDicts();

    frame->show();
    //toolBar->addWidget(label1);
    //toolBar->addWidget(selectLangsBox);
    graphicsInput = new QGraphicsInput(QRectF(0, 0, 2000, 2000), graphicsView);

    label->setListWidget(sideBar);
    pdfx = nullptr;
    epd = nullptr;
    dj2pf = nullptr;

    connect(sideBar, SIGNAL(pageSelected(int)), pages, SLOT(pageSelected(int)));
    connect(label, SIGNAL(pageRemoved(int)), pages, SLOT(pageRemoved(int)));

    statusBar()->show();
    useXSane = true;
    scanner = nullptr;
    //rotation = 0;
    m_menu = new QMenu(graphicsView);
    ifCounter = 0;
    dirty = false;

    connect(actionOpen, SIGNAL(triggered()), this, SLOT(loadImage()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(this, SIGNAL(windowShown()), this, SLOT(onShowWindow()), Qt::QueuedConnection);
    connect(actionScan, SIGNAL(triggered()), this, SLOT(scanImage()));
    connect(actionPreviousPage, SIGNAL(triggered()), this, SLOT(loadPreviousPage()));
    connect(actionNextPage, SIGNAL(triggered()), this, SLOT(loadNextPage()));
    connect(actionRecognize, SIGNAL(triggered()), this, SLOT(recognize()));
    connect(action_Save, SIGNAL(triggered()), this, SLOT(saveAllText()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDlg()));
    connect(actionOnlineHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(actionCopyToClipboard, SIGNAL(triggered()), textEdit, SLOT(copyClipboard()));
    connect(actionSelect_All_Text, SIGNAL(triggered()), textEdit, SLOT(selectAll()));
    connect(graphicsInput, SIGNAL(rightMouseClicked(int, int, bool)), this, SLOT(rightMouseClicked(int, int, bool)));
    connect(actionSelect_HTML_format, SIGNAL(triggered()), this, SLOT(selectHTMLformat()));

    connect(graphicsInput, SIGNAL(increaseMe()), this, SLOT(enlargeButtonClicked()));
    connect(graphicsInput, SIGNAL(decreaseMe()), this, SLOT(decreaseButtonClicked()));
    connect(sideBar, SIGNAL(filesDropped(QStringList)), SLOT(loadFiles(QStringList)));
    connect(pages, SIGNAL(loadPage(bool)), this, SLOT(loadPage(bool)));
    connect(graphicsInput, SIGNAL(blockCreated(QRect)), pages, SLOT(addBlock(QRect)));
    connect(graphicsInput, SIGNAL(blockCreated(QRect)), this, SLOT(markDirty()));
    connect(graphicsInput, SIGNAL(deleteBlock(QRect)), pages, SLOT(deleteBlock(QRect)));
    connect(graphicsInput, SIGNAL(deleteBlock(QRect)), this, SLOT(markDirty()));
    connect(sideBar, SIGNAL(fileRemoved(int)), pages, SLOT(pageRemoved(int)));
    connect(sideBar, SIGNAL(fileRemoved(int)), this, SLOT(markDirty()));
    connect(pages, SIGNAL(addSnippet(int)), this, SLOT(addSnippet(int)));
    connect(actionSelect_languages, SIGNAL(triggered()), this, SLOT(selectLanguages()));
    connect(graphicsInput, SIGNAL(clickMeAgain()), this, SLOT(clickMeAgain()), Qt::QueuedConnection);
    //connect(pdfx, SIGNAL(processStarted()), this, SLOT(extProcStarted()));

    selectLangsBox = new QComboBox();

    selectLangsBox->setToolTip(trUtf8("Recognition language"));

    // connect(selectLangsBox->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(LangTextChanged(QString)));

    initSettings();
    engineLabel = new QLabel();
    statusBar()->addPermanentWidget(engineLabel, 0);
    if (settings->getSelectedEngine() == UseCuneiform)
    {
        //fillLanguagesBoxCuneiform();
        engineLabel->setText(trUtf8("Using Cuneiform"));

    }
    if (settings->getSelectedEngine() == UseTesseract)
    {
        //fillLanguagesBoxTesseract();
        engineLabel->setText(trUtf8("Using Tesseract"));
    }

    slAction = toolBar_6->insertWidget(0, selectLangsBox);
    //selectLangsBox->setStyleSheet("border: 1px solid blue; padding: 2px 2px 2px 18px; min-width: 6em; background-color: white; selection-background-color:blue; QComboBox::drop-down: { width: 0px; border-style: none}");
    //selectLangsBox->setStyleSheet(label_2->text());
    langLabel = new QLabel();
    statusBar()->addPermanentWidget(langLabel);
    if (settings->getSelectedLanguages().count() == 1)
    {
        slAction->setVisible(false);
        langLabel->setText(
                trUtf8("Recognition Language") + ": " + settings->getFullLanguageName(settings->getLanguage()));
    }
    fillLangBox();
    delTmpFiles();
    QXtUnixSignalCatcher::connectUnixSignal(SIGUSR2);
    ba = new QByteArray();
    connect(QXtUnixSignalCatcher::catcher(), SIGNAL(unixSignal(int)), this, SLOT(readyRead(int)));

    QPixmap l_cursor;
    l_cursor.load(":/resize.png");
    resizeCursor = new QCursor(l_cursor);
    graphicsInput->setMagnifierCursor(resizeCursor);
    l_cursor.load(":/resize_block.png");
    resizeBlockCursor = new QCursor(l_cursor);


    this->sideBar->show();

    connect(actionRecognize_All_Pages, SIGNAL(triggered()), this, SLOT(recognize()));

    QPixmap pm;
    pm.load(":/align.png");
    pm.load(":/undo.png");

    loadFromCommandLine();
    emit windowShown();

    if (findProgram("pdftoppm"))
    {
        pdfx = new PDF2PPT();
    }
    else if (findProgram("gs"))
    {
        pdfx = new GhostScr();
    }
    dj2pf = new Djvu2PDF();
    epd = new ExtProcessDialog(this);
    epd->hide();

    if (pdfx)
    {
        connect(pdfx, SIGNAL(processStarted()), this, SLOT(extProcStarted()));
        connect(pdfx, SIGNAL(processFinished(bool)), this, SLOT(showPDFprogress()));
        connect(pdfx, SIGNAL(addPage(QString, int, int)), this, SLOT(addPDFPage(QString, int, int)),
                Qt::DirectConnection);
        connect(pdfx, SIGNAL(extractingFinished()), this, SLOT(finishedPDF()));
        connect(epd, SIGNAL(rejected()), pdfx, SLOT(cancelProcess()));
        connect(pdfx, SIGNAL(error(QString)), this, SLOT(reportError(QString)), Qt::QueuedConnection);
    }

    if (dj2pf)
    {
        connect(dj2pf, SIGNAL(started()), this, SLOT(djvuStarted()));
        connect(dj2pf, SIGNAL(finished()), this, SLOT(djvuFinished()));
        connect(dj2pf, SIGNAL(error(QString)), this, SLOT(reportError(QString)), Qt::QueuedConnection);
    }
    rw = 0;
    rd = 0;

    createRecentMenu();

    _asm = new AutoSaveManager();
    connect(_asm, SIGNAL(startedAutoSave()), this, SLOT(startedAutoSave()), Qt::QueuedConnection);
    connect(_asm, SIGNAL(finishedAutoSave()), this, SLOT(autosaveFinished()));
    connect(_asm, SIGNAL(reportError(QString)), this, SLOT(reportError(QString)));

    connect(this, SIGNAL(callAfterConstructor()), this, SLOT(afterConstructor()), Qt::QueuedConnection);
    emit callAfterConstructor();
    timerId = startTimer(settings->getAutosaveInterval() * 60000);

}


void MainForm::onShowWindow()
{
    actionKeep_Lines->setChecked(settings->getKeepLines());
    connect(selectLangsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newLanguageSelected(int)));
    selectLangsBox->setCurrentIndex(selectLangsBox->findData(QVariant(settings->getLanguage())));
}


void MainForm::loadFromCommandLine()
{
    QStringList sl = QApplication::arguments();
    if (sl.count() > 1)
    {
        if (QFile::exists(sl.at(1)))
        {
            loadFile(sl.at(1));
        }
        for (int i = 2; i < sl.count(); i++)
        {
            QApplication::processEvents();
            if (QFile::exists(sl.at(i)))
            {
                loadFile(sl.at(i));
            }
        }
        sideBar->select(sl.at(1));
    }
}

void MainForm::loadFiles(const QStringList& files)
{
    if (files.count() == 1)
    {
        if (QFile::exists(files.at(0)))
        {
            if (files.at(0).endsWith(".tiff", Qt::CaseInsensitive) || files.at(0).endsWith(".tif", Qt::CaseInsensitive))
            {
                loadTIFF(files.at(0));
            }
            else
            {
                if (files.at(0).endsWith(".pdf", Qt::CaseInsensitive))
                {
                    importPDF(files.at(0));
                }
                else
                {
                    if (files.at(0).endsWith(".djvu", Qt::CaseInsensitive))
                    {
                        importDjVu(files.at(0));
                    }
                    else
                    {
                        loadFile(files.at(0));
                    }
                }
            }
        }
        return;
    }
    QProgressDialog pd(this);
    pd.setWindowTitle("YAGF");
    pd.setLabelText(trUtf8("Loading files..."));
    pd.setRange(1, files.count());
    pd.setValue(1);
    pd.show();
    for (int i = 0; i < files.count(); i++)
    {
        if (QFile::exists(files.at(i)))
        {
            if (files.at(i).endsWith(".tiff", Qt::CaseInsensitive) || files.at(i).endsWith(".tif", Qt::CaseInsensitive))
            {
                loadTIFF(files.at(i));
            }
            else
            {
                if (files.at(i).endsWith(".pdf", Qt::CaseInsensitive))
                {
                    importPDF(files.at(i));
                }
                else
                {
                    if (files.at(i).endsWith(".djvu", Qt::CaseInsensitive))
                    {
                        importDjVu(files.at(i));
                    }
                    else
                    {
                        loadFile(files.at(i));
                    }
                }
            }
        }
        pd.setValue(i + 1);
        QApplication::processEvents();
        if (pd.wasCanceled())
        {
            // connectTC(true);
            break;
        }
    }
    forbidAutoSave = false;
}

void MainForm::LangTextChanged(const QString& text)
{
    if (selectLangsBox->findText(text, Qt::MatchStartsWith) < 0)
    {
        selectLangsBox->lineEdit()->setText("");
    }
}

void MainForm::showConfigDlg()
{
    ConfigDialog dialog(this);
    if (dialog.exec())
    {

        //if (settings->getSelectedEngine() != ose) {
        QString oldLang = selectLangsBox->currentText();
        selectLangsBox->clear();
        if (settings->getSelectedEngine() == UseCuneiform)
        {
            engineLabel->setText(trUtf8("Using Cuneiform"));
            if (settings->selectedLanguagesAvailableTo("cuneiform").count() == 0)
            {
                styledWarningMessage(this,
                                     trUtf8("Cuneiform doesn't support any of selected recognition langualges.\nFalling back to tesseract. Please install tesseract."));
                settings->setSelectedEngine(UseTesseract);
                engineLabel->setText(trUtf8("Using Tesseract"));
            }
        }
        if (settings->getSelectedEngine() == UseTesseract)
        {
            engineLabel->setText(trUtf8("Using Tesseract"));
            if (settings->selectedLanguagesAvailableTo("tesseract").count() == 0)
            {
                styledWarningMessage(this,
                                     trUtf8("Tesseract doesn't support any of selected recognition langualges.\nFalling back to cueniform. Please install cuneiform."));
                settings->setSelectedEngine(UseCuneiform);
                engineLabel->setText(trUtf8("Using Cuneiform"));
            }
        }
        fillLangBox();
        int newIndex = selectLangsBox->findText(oldLang);
        if (newIndex >= 0)
        {
            selectLangsBox->setCurrentIndex(newIndex);
            settings->setLanguage(selectLangsBox->itemData(newIndex).toString());
        }
        else
        {
            settings->setLanguage("eng");
            for (int i = 0; i < selectLangsBox->count(); i++)
            {
                QString s = selectLangsBox->itemData(i).toString();
                if (s == "eng")
                {
                    newLanguageSelected(i);
                    selectLangsBox->setCurrentIndex(i);
                    break;
                }
            }
        }

        //} else         fillLangBox();


        toolBar->setIconSize(settings->getIconSize());
        if (selectLangsBox->count() > 1)
        {
            selectLangsBox->setStyleSheet("");
            slAction->setVisible(true);
            langLabel->setText("");
        }
        else
        {
            if (settings->getSelectedLanguages().count() == 1)
            {
                slAction->setVisible(false);
                langLabel->setText(
                        trUtf8("Recognition Language") + ": " + settings->getFullLanguageName(settings->getLanguage()));
            }
        }
        killTimer(timerId);
        timerId = startTimer(settings->getAutosaveInterval() * 60000);
    }
}

void MainForm::importPDF(const QString& fileName)
{
    if (!pdfx)
    {
        styledCriticalMessage(this,
                              trUtf8("No compatible PDF converter software could be found. Please install either the pdftoppm utility or the GhostScript package (from this the gs command will be required)."));
        return;
    }
    PopplerDialog dialog(this);
    dialog.setPDFFile(fileName);
    if (dialog.exec())
    {
        pdfx->setSourcePDF(dialog.getPDFFile());
        if (pdfx->getSourcePDF().isEmpty())
        {
            styledCriticalMessage(this, trUtf8("PDF file name may not be empty"));
            return;
        }
        pdfx->setStartPage(dialog.getStartPage());
        pdfx->setStopPage(dialog.getStopPage());
        pdfx->setOutputDir();
        globalDeskew = settings->getAutoDeskew();
        settings->setAutoDeskew(dialog.getDeskew());
        QApplication::processEvents();
        pdfx->run();
    }
}

void MainForm::importDjVu(const QString& fileName)
{
    dj2pf->convert(fileName);
}

void MainForm::addPDFPage(QString pageName, int current, int total)
{
    Unlocker unlocker(GlobalLock::instance()->lock());
    if (pdfPD == 0)
    {
        return;
    }
    QFile fl(pageName);
    while (!fl.exists())
    {
        if (pdfPD == 0)
        {
            return;
        }
        else if (!pdfPD->isVisible())
        {
            return;
        }
    }
    while (!pages->appendPage(pageName))
    {
        if (pdfPD == 0)
        {
            return;
        }
        else if (!pdfPD->isVisible())
        {
            return;
        }
    }
    pages->setDeskewed(true);
    if (total != 0)
    {
        int ratio = (current * 100) / total;
        pdfPD->setValue(ratio);
    }
    else
    {
        pdfPD->setValue(pdfPD->value() + 1);
    }
}

void MainForm::finishedPDF()
{
    delete pdfPD;
    pdfPD = 0;
    settings->setAutoDeskew(globalDeskew);
}

#include <QDebug>

void MainForm::loadImage()
{
    if (!GlobalLock::instance()->lock())
    {
        return;
    }
    Unlocker unlocker(true);
    QFileDialog dialog(this,
                       trUtf8("Open Image"), settings->getLastDir(),
                       trUtf8("Image Files (*.png *.jpg *.jpeg *.bmp *.tiff *.tif *.gif *.pnm *.pgm *.pbm *.ppm *.pdf *.djvu)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec())
    {
        QStringList fileNames;
        fileNames = dialog.selectedFiles();
        qDebug() << fileNames;
        settings->setLastDir(dialog.directory().path());
                foreach(QString fn, fileNames)
            {
                if (fn.endsWith(".pdf", Qt::CaseInsensitive))
                {
                    importPDF(fn);
                }
            }
        for (int i = fileNames.count() - 1; i >= 0; i--)
        {
            if (fileNames.at(i).endsWith(".pdf", Qt::CaseInsensitive))
            {
                fileNames.removeAt(i);
            }
        }
        if (fileNames.count() > 0)
        {
            loadFiles(fileNames);
        }
    }
}

void MainForm::singleColumnButtonClicked()
{
    //singleColumn = singleColumnButton->isChecked();
}

void MainForm::closeEvent(QCloseEvent* event)
{
    killTimer(timerId);
    if (scanner)
    {
        delete scanner;
        scanner = nullptr;
    }
    settings->setSize(size());
    settings->setPosition(pos());
    settings->setFullScreen(isFullScreen());
    settings->writeSettings();
    if (dirty)
    {
        QPixmap icon;
        icon.load(":/images/question.png");
        QMessageBox messageBox(QMessageBox::NoIcon, "YAGF",
                               trUtf8("There is an unsaved data. Do you want to save or to discard it?"),
                               QMessageBox::Save | QMessageBox::Ignore, this);
        messageBox.setIconPixmap(icon);
        int result = messageBox.exec();
        if (result == QMessageBox::Save)
        {
            if (projectName.isEmpty())
            {
                SaveProjectDialog spd(settings->getProjectDir(), this);
                spd.exec();
                projectName = spd.projectPath();
            }
            if (!projectName.isEmpty())
            {
                ProjectSaver ps;
                ps.save(projectName);
            }
        }
    }
    delTmpFiles();
    delAutoSaveFiles();
    event->accept();
    QXtUnixSignalCatcher::catcher()->disconnectUnixSugnals();
    pages->clear();
}

void MainForm::rotateCWButtonClicked()
{
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    pages->rotate90CW();
    setCursor(oldCursor);
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
    }
}

void MainForm::rotateCCWButtonClicked()
{
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    pages->rotate90CCW();
    setCursor(oldCursor);
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
    }
}

void MainForm::rotate180ButtonClicked()
{
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    pages->rotate180();
    setCursor(oldCursor);
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
    }
}

void MainForm::enlargeButtonClicked()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
    }
    pages->makeLarger();
}

void MainForm::decreaseButtonClicked()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
    }
    pages->makeSmaller();
}

void MainForm::initSettings()
{
    settings = Settings::instance();
    if (settings->getFullScreen())
    {
        showFullScreen();
    }
    else
    {
        move(settings->getPosition());
        resize(settings->getSize());
    }
    actionCheck_spelling->setChecked(settings->getCheckSpelling());
    actionSelect_HTML_format->setChecked(settings->getOutputFormat() != "text");

    QList<int> li;
    li.append(1);
    li.append(1);
    splitter->setSizes(li);
    toolBar->setIconSize(settings->getIconSize());
}

void MainForm::newLanguageSelected(int index)
{
    if (index < 0)
    { return; }
    settings->setLanguage(selectLangsBox->itemData(index).toString());
    actionCheck_spelling->setEnabled(textEdit->hasDict(settings->getLanguage()));
    if (settings->getCheckSpelling())
    {
        settings->setCheckSpelling(textEdit->spellCheck(settings->getLanguage()));
        //actionCheck_spelling->setEnabled(checkSpelling);
        actionCheck_spelling->setChecked(settings->getCheckSpelling());
    }

}

void MainForm::scanImage()
{

    if (useXSane)
    {
        if (scanner)
        {
            delete scanner;
        }
        ScannerFactory* sf = new ScannerFactory();
        scanner = sf->createScannerFE("xsane");
        if (scanner == nullptr)
        {
            styledWarningMessage(this,
                                 trUtf8("Scanning is impossible. No scanning front-end is found.\nPlease install XSane in order to perform scanning."));
            return;
        }
        scanner->setOutputFile(settings->workingDir() + settings->getScanOutputFile());
        delete sf;
        scanner->exec();

    }
}

void MainForm::loadFile(const QString& fn, bool loadIntoView)
{
    // dirty = true;
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    if (pages->appendPage(fn))
    {
        if (loadIntoView)
        {
            pages->makePageCurrent(pages->count() - 1);
            textEdit->clear();
            loadPage(true);
            sideBar->item(sideBar->count() - 1)->setSelected(true);
        }
    }
    else
    {
        styledWarningMessage(this, trUtf8("Failed to load image %1").arg(fn));
    }
    setCursor(oldCursor);
}

void MainForm::loadTIFF(const QString& fn, bool loadIntoView)
{
    TiffImporter ti(fn);
    ti.exec();
    QStringList files = ti.extractedFiles();
    if (!files.count())
    {
        QMessageBox mb;
        mb.setWindowTitle("YAGF");
        mb.setIconPixmap(QPixmap(":/critical.png"));
        mb.setText(trUtf8("Cannot open file %1. Make sure imagemagick and tifftopnm are installed.").arg(fn));
        mb.addButton(QMessageBox::Close);
        mb.exec();
        return;
    }
    loadFiles(files);
}

void MainForm::delTmpFiles()
{

    QDir dir(settings->workingDir());
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    for (uint i = 0; i < dir.count(); i++)
    {
        if (dir[i].endsWith("jpg") || dir[i].endsWith("bmp") || dir[i].endsWith("png") || dir[i].endsWith("txt") ||
            dir[i].endsWith("ygf"))
        {
            dir.remove(dir[i]);
        }
    }
    if (pdfx)
    {
        pdfx->setOutputDir();
    }
    delTmpDir();
}

void MainForm::delAutoSaveFiles()
{
    QString autosaveDir = settings->workingDir() + "autosave/";
    QDir dir1(autosaveDir);
            foreach(QString s, dir1.entryList())
        {
            QFile::remove(autosaveDir + s);
        }
}

void MainForm::loadNextPage()
{
}

void MainForm::loadPreviousPage()
{
}

void MainForm::showAboutDlg()
{
    QPixmap icon;
    icon.load(":/yagf.png");
    QMessageBox aboutBox(QMessageBox::NoIcon, trUtf8("About UFOCR"),
                         trUtf8("<p align=\"center\"><b>UFOCR - User-Friendly OCR</b></p><p align=\"center\">Version %1</p> <p align=\"center\">Ⓒ 2017 Alexander Zaitsev</p> This is a free software distributed under GPL v3. Visit <a href=\"https://github.com/ZaMaZaN4iK/ufocr\">https://github.com/ZaMaZaN4iK/ufocr</a> for more details.").arg(
                                 version), QMessageBox::Ok);
    aboutBox.setIconPixmap(icon);
    QList<QLabel*> labels = aboutBox.findChildren<QLabel*>();
    for (int i = 0; i < labels.count(); i++)
    {
        QLabel* lab = labels.at(i);
        lab->setTextInteractionFlags(Qt::TextBrowserInteraction);
    }
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.exec();
}

void MainForm::showHelp()
{
    QDesktopServices::openUrl(QUrl(trUtf8("https://github.com/ZaMaZaN4iK/ufocr")));
}

void MainForm::readyRead(int sig)
{
    QFile f(settings->workingDir() + settings->getScanOutputFile());
    QString newName = QString(settings->workingDir() + "scan-input-%1.png").arg(ifCounter);
    ifCounter++;
    QFileInfo fi(newName);
    if (fi.exists())
    {
        QFile f2(newName);
        f2.remove();
    }
    f.rename(newName);
    loadFile(newName);
}


void MainForm::delTmpDir()
{
    QDir dir;
    dir.setPath(settings->workingDir() + "output_files");
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    for (uint i = 0; i < dir.count(); i++)
    {
        if (dir[i].endsWith("jpg") || dir[i].endsWith("bmp") || dir[i].endsWith("ygf"))
        {
            dir.remove(dir[i]);
        }
    }
    dir.rmdir(settings->workingDir() + "output_files");

}


void MainForm::fillLangBox()
{
    if (!slAction->isVisible())
    {
        if (settings->getSelectedLanguages().count() > 1)
        {
            slAction->setVisible(true);
        }
    }
    else
    {
        if (settings->getSelectedLanguages().count() == 1)
        {
            slAction->setVisible(false);
        }
    }
    disconnect(selectLangsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newLanguageSelected(int)));
    QStringList sl = Settings::instance()->getSelectedLanguages();
    settings->startLangPair();
    QString full;
    QString abbr;
    selectLangsBox->clear();
    while (settings->getLangPair(full, abbr))
    {
        if (sl.contains(full) || (sl.count() == 0))
        {
            selectLangsBox->addItem(full, QVariant(abbr));
        }
    }
    selectLangsBox->setCurrentIndex(-1);
    connect(selectLangsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newLanguageSelected(int)));
    if (selectLangsBox->model()->rowCount())
    {
        int index = 0;
        QString s = Settings::instance()->getLanguage();
        for (int i = 0; i < selectLangsBox->count(); i++)
        {
            if (selectLangsBox->itemData(i) == s)
            {
                index = i;
                break;
            }
        }
        selectLangsBox->setCurrentIndex(index);
    }
}

void MainForm::createRW()
{

    rw = new RecognizerWrapper(this);
    connect(rw, SIGNAL(finished(int)), this, SLOT(recognitionFinished()), Qt::QueuedConnection);
    connect(rw, SIGNAL(error(QString)), this, SLOT(recognitionError(QString)), Qt::QueuedConnection);
    connect(rw, SIGNAL(readOutput(QString, QChar)), this, SLOT(readOutput(QString, QChar)), Qt::QueuedConnection);
    rd = new RecognitionDialog(this);
    connect(rd, SIGNAL(rejected()), this, SLOT(cancelRecognition()));
    connect(rw, SIGNAL(blockRecognized(int)), rd, SLOT(blockRecognized(int)));
    rd->show();
}


void MainForm::createRecentMenu()
{
    QMenu* project = new QMenu();
    QStringList sl = settings->getRecentProjects();
            foreach (QString s, sl)
        {
            MenuAction* ma = new MenuAction(s);
            connect(ma, SIGNAL(triggered(QString)), this, SLOT(menuTriggered(QString)));
            project->addAction(ma);
        }
    actionRecent_Projects->setMenu(project);
}

void MainForm::loadProjectInternal(const QString& path)
{
    WaitWindow* ww = new WaitWindow(this);
    ww->setWindowOpacity(0);
    ww->show();
    QApplication::processEvents();
    QApplication::processEvents();
    pages->clear();
    ProjectLoader pl;
    if (!pl.load(path))
    {
        styledWarningMessage(this, trUtf8("Failed to load project."));
    }
    else
    {
        projectName = path;
    }
    delete ww;
}

void MainForm::connectTC(bool doIt)
{
    if (doIt)
    {
        connect(textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()), Qt::UniqueConnection);
    }
    else
    {
        disconnect(textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()));
    }
}

void MainForm::loadAutoSaved()
{
    loadProjectInternal(settings->workingDir() + "autosave/");
    dirty = true;
}

void MainForm::saveTextInternal(bool allText)
{
    if (pages->count() == 0)
    {
        styledWarningMessage(this, trUtf8("Nothing to save"));
        return;
    }

    Settings* settings = Settings::instance();
    QString filter;
    //if (settings->getOutputFormat() == "text")
    filter = trUtf8("Text Files (*.txt)");
    //else
    //    filter = trUtf8("HTML Files (*.html)");
    QString title = allText ? trUtf8("Save All Text") : trUtf8("Save Current Page Text");
    QFileDialog dialog(this,
                       title, settings->getLastOutputDir(), filter);
//    if (settings->getOutputFormat() == "text")
    dialog.setDefaultSuffix("txt");
//    else
//        dialog.setDefaultSuffix("html");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec())
    {
        QStringList fileNames;
        fileNames = dialog.selectedFiles();
        settings->setLastOutputDir(dialog.directory().path());
        //if (settings->getOutputFormat() == "text") {
        if (allText)
        {
            PageCollection::instance()->saveAllText(fileNames.at(0), true);
        }
        else
        {
            PageCollection::instance()->SaveCurrentPageText(fileNames.at(0), true);
        }
        //}
        // else
        //     saveHtml(&textFile);
    }
}

void MainForm::clickMeAgain()
{
    QToolTip::showText(actPos, trUtf8("Click Me again!"));
}

void MainForm::setResizingCusor()
{
    //scrollArea->widget()->setCursor(*resizeBlockCursor);
}

void MainForm::setUnresizingCusor()
{
    //scrollArea->widget()->setCursor(QCursor(Qt::ArrowCursor));
}

void MainForm::loadPage(bool show)
{
    bool wasDirty = dirty;
    connectTC(false);
    textEdit->clear();
    try
    {
        if (!pages->currentText().isEmpty())
        {
            textEdit->setText(pages->currentText());
        }
    }
    catch (...)
    {
        textEdit->clear();
    }
    if (show)
    {
        graphicsInput->loadImage(pages->pixmap());
        graphicsInput->clearBlocks();
    }
    connectTC(true);


    QApplication::processEvents();
    if (show)
    {
        for (int i = 0; i < pages->blockCount(); i++)
            graphicsInput->addBlockColliding(pages->getBlock(i));
    }
    QFileInfo fi(pages->originalFileName());
    setWindowTitle(QString("YAGF - %1").arg(fi.fileName()));
    dirty = wasDirty;
}

/*void MainForm::recognizeAll()
{
    if (pages->count() == 0)
        return;
    QProgressDialog progress(trUtf8("Recognizing pages..."), trUtf8("Abort"), 0, pages->count(), this);
    progress.setWindowTitle("YAGF");
    progress.show();
    progress.setValue(0);
    pages->makePageCurrent(-1);
    for (int i = 0; i < pages->count(); i++) {
        progress.setValue(i);
        if (progress.wasCanceled())
            break;
        pages->makeNextPageCurrent();
        recognize();
    }
}*/

void MainForm::unalignButtonClicked()
{
    /*if (((QSelectionLabel *) scrollArea->widget())->pixmap()->isNull())
        return;
    int rot = ((FileToolBar *) m_toolBar)->getRotation();
    int rrot = ((rot + 45)/90);
    rrot *=90;
    rotateImage(rrot - rot);
    rotation = rrot;*/
}

void MainForm::on_ActionClearAllBlocks_activated()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
        return;
    }
    pages->clearBlocks();
    loadPage(true);
}

void MainForm::rightMouseClicked(int x, int y, bool inTheBlock)
{
    m_menu->clear();
    m_menu->addAction(ActionClearAllBlocks);
    if (inTheBlock)
    {
        m_menu->addAction(ActionDeleteBlock);
        m_menu->addAction(actionRecognize_block);
        m_menu->addAction(actionSave_block);
        m_menu->addAction(actionDeskew_by_Block);
        m_menu->addAction(actionSelect_Table);
    }
    else
    {
        m_menu->addAction(actionSelect_Text_Area);
        m_menu->addAction(actionSelect_multiple_blocks);
    }
    QPoint p = graphicsView->mapToGlobal(QPoint(x, y));
    m_menu->move(p);
    m_menu->show();
}

void MainForm::setupPDFPD()
{
    pdfPD->setWindowTitle("YAGF");
    pdfPD->setLabelText(trUtf8("Importing pages from the PDF document..."));
    pdfPD->setCancelButton(0);
    // pdfPD->setCancelButton(new QPushButton());
    //pdfPD->setCancelButtonText(trUtf8("Cancel"));
    pdfPD->setMinimum(-1);
    pdfPD->setMaximum(-1);
    pdfPD->setModal(true);
    pdfPD->setWindowIcon(QIcon(":/yagf.png"));
    /*if (pdfx) {
        connect(pdfPD, SIGNAL(canceled()), pdfx, SLOT(cancel()));
        connect(pdfPD, SIGNAL(canceled()), this, SLOT(cancelPDF()));
    }*/
}

void MainForm::on_ActionDeleteBlock_activated()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
        return;
    }
    QRect r = graphicsInput->getCurrentBlock();
    if (r.width() == 0)
    {
        r = graphicsInput->getActiveBlock();
    }
    graphicsInput->deleteCurrentBlock();
    graphicsInput->deleteActiveBlock();
    pages->deleteBlock(r);
}


void MainForm::on_actionRecognize_block_activated()
{
    connectTC(false);
    if (!RecognizerWrapper::findEngine(true))
    {
        styledWarningMessage(this, trUtf8("Selected recognition engine not found."));
        return;
    }
    if (graphicsInput->getCurrentBlock().isNull())
    {
        return;
    }
    clearTmpFiles();
    QRect r = graphicsInput->getCurrentBlock();
    r = pages->scaleRect(r);
    pages->saveRawBlockForRecognition(r, settings->workingDir() + settings->getRecognizeInputFile());
    createRW();
    rw->startSingleBlock();
}

void MainForm::recognize()
{
    createRW();
    rw->start();
}

/*void MainForm::on_actionRecognize_activated()
{

}*/


void MainForm::on_actionCheck_spelling_triggered()
{
    settings->setCheckSpelling(actionCheck_spelling->isChecked());
}

void MainForm::on_actionSave_current_image_activated()
{
    QCursor oc = cursor();
    setCursor(Qt::WaitCursor);
    QString format;
    QString fn = getFileNameToSaveImage(format);
    if (!fn.isEmpty())
    {
        if (!(pages->savePageAsImage(fn, format)))
        {
            styledWarningMessage(this, QObject::trUtf8("Failed to save the image"));
        }
    }
    setCursor(oc);
}

QString MainForm::getFileNameToSaveImage(QString& format)
{
    QString jpegFilter = QObject::trUtf8("JPEG Files (*.jpg)");
    QString pngFilter = QObject::trUtf8("PNG Files (*.png)");
    QStringList filters;
    format = "JPEG";
    filters << jpegFilter << pngFilter;
    QFileDialog dialog(this,
                       trUtf8("Save Image"), settings->getLastOutputDir());
    dialog.setNameFilters(filters);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("jpg");
    if (dialog.exec())
    {
        if (dialog.selectedNameFilter() == jpegFilter)
        {
            format = "JPEG";
            dialog.setDefaultSuffix("jpg");
        }
        else if (dialog.selectedNameFilter() == pngFilter)
        {
            format = "PNG";
            dialog.setDefaultSuffix("png");
        }
        QStringList fileNames;
        fileNames = dialog.selectedFiles();
        settings->setLastOutputDir(dialog.directory().path());
        return fileNames.at(0);
    }
    return "";
}


MainForm::~MainForm()
{
    closeAll();
    delete _asm;
    delete resizeBlockCursor;
    delete resizeCursor;
    delete graphicsInput;
    delete ba;
    delete pdfx;
    delete dj2pf;
    delete epd;
}

void MainForm::on_actionSave_block_activated()
{
    QString format;
    QString fn = getFileNameToSaveImage(format);
    if (!fn.isEmpty())
    {
        pages->saveBlockForRecognition(graphicsInput->getCurrentBlock(), fn, format);
    }
}

void MainForm::on_actionCheck_spelling_activated()
{
    settings->setCheckSpelling(actionCheck_spelling->isChecked());
    if (settings->getCheckSpelling())
    {
        actionCheck_spelling->setChecked(textEdit->spellCheck(settings->getLanguage()));
    }
    else
    {
        textEdit->unSpellCheck();
    }
}

/*void MainForm::on_alignButton_clicked()
{
    this->AnalizePage();
}*/

void MainForm::upscale()
{
    Settings::instance()->setUpscale(!(Settings::instance()->getUpscale()));
}

void MainForm::on_actionSelect_HTML_format_activated()
{
    if (actionSelect_HTML_format->isChecked())
    {
        settings->setOutputFormat("html");
    }
    else
    {
        settings->setOutputFormat("text");
    }
}

void MainForm::pasteimage()
{
    QClipboard* clipboard = QApplication::clipboard();
    QPixmap pm = clipboard->pixmap();
    if (pm.isNull())
    {
        QMessageBox mb(this);
        mb.setIconPixmap(QPixmap(":/warning.png"));
        mb.setWindowTitle(trUtf8("Warning"));
        mb.setText(trUtf8("Clipboard doesn't contain an image."));
        mb.setButtonText(0, trUtf8("OK"));
        mb.exec();
        return;
    }
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    QString tmpFile = settings->tmpFileName() + ".png";
    pm.save(tmpFile, "PNG");
    loadFile(tmpFile);
    setCursor(oldCursor);
    dirty = true;
}

void MainForm::deskewByBlock()
{
    pages->deskew();
}

void MainForm::selectTextArea()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
        return;
    }
    pages->blockAllText();
}

void MainForm::addSnippet(int index)
{
    sideBar->addItem(pages->snippet());
}

void MainForm::preprocessPage()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
        return;
    }
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    if (!pages->splitPage(true))
    {
        styledWarningMessage(this,
                             trUtf8("Failed to detect text areas on this page.\nThe page possibly lacks contrast. Try to select blocks manually."));
    }
    setCursor(oldCursor);
}

void MainForm::saveProjectAs()
{
    SaveProjectDialog sp(settings->getProjectDir(), this);
    sp.exec();
    QString pp = sp.projectPath();
    if (pp == "")
    { return; }
    QDir dir(pp);
    if (!dir.mkpath(pp))
    {
        styledCriticalMessage(this, trUtf8("Cannot create the project directory "));
        return;
    }
    QCursor oldCursor = cursor();
    dirty = false;
    _asm->startAutoSave();
    projectName = pp;
    setCursor(oldCursor);
}

void MainForm::saveProject()
{
    if (projectName.contains(".config/yagf/autosave"))
    {
        projectName = "";
    }
    if (projectName != "")
    {
        dirty = false;
        _asm->startAutoSave();
    }
    else
    {
        saveProjectAs();
    }
}

void MainForm::loadProject()
{
    closeAll();
    LoadProjectDialog lpd(settings->getProjectDir(), this);
    lpd.exec();
    QString project = lpd.projectPath();
    if (project != "")
    {
        loadProjectInternal(project);
    }
    dirty = false;
}

void MainForm::selectBlocks()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
        return;
    }
    QCursor oldCursor = cursor();
    setCursor(Qt::WaitCursor);
    if (!pages->splitPage(false))
    {
        styledWarningMessage(this,
                             trUtf8("Failed to detect text areas on this page.\nThe page possibly lacks contrast. Try to select blocks manually."));
    }
    setCursor(oldCursor);
}

void MainForm::selectHTMLformat()
{
    if (actionSelect_HTML_format->isChecked())
    {
        settings->setOutputFormat("html");
    }
    else
    {
        settings->setOutputFormat("text");
    }

}


void MainForm::SelectRecognitionLanguages()
{
    LangSelectDialog lsd;
    if (lsd.exec() == QDialog::Accepted)
    {
        fillLangBox();
    }
}


void MainForm::selectLanguages()
{
    LangSelectDialog lsd(this); // ;)
    lsd.exec();
    fillLangBox();
}

void MainForm::deskewByLine()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
    }
    if (!graphicsInput->getDeskewMode())
    {
        pages->clearBlocks();
        graphicsInput->setDeskewMode(true);
        actPos = QCursor::pos();
        QPoint gp = graphicsView->mapToGlobal(QPoint(16, 16));
        QToolTip::showText(gp, trUtf8("Draw the line along a string of text with left mouse button"), graphicsView);
    }
    else
    {
        QLineF l = graphicsInput->getDeskData();
        graphicsInput->clearDeskLine();
        pages->deskew(l.x1(), l.y1(), l.x2(), l.y2());

    }
}

void MainForm::timerEvent(QTimerEvent* e)
{
    if (!GlobalLock::instance()->lock())
    {
        e->accept();
        QMainWindow::timerEvent(e);
        return;
    }
    GlobalLock::instance()->unlock();
    if (pages->hasPage())
    {
        _asm->startAutoSave();
    }
    connectTC(false);

    e->accept();
    QMainWindow::timerEvent(e);
}

void MainForm::on_actionKeep_Lines_toggled(bool arg1)
{
    settings->setKeepLines(arg1);
}

void MainForm::readOutput(QString text, QChar separator)
{
    textEdit->append(text);
    //textEdit->append(QString(separator));
    if (settings->getCheckSpelling())
    {
        actionCheck_spelling->setChecked(textEdit->spellCheck(settings->getLanguage()));
    }
}

void MainForm::recognitionFinished()
{

    pages->setText(textEdit->toPlainText());
    connectTC(true);
    delete rw;
    rw = 0;
    if (rd)
    {
        delete (rd);
    }
    rd = 0;
    dirty = true;
}

void MainForm::recognitionError(const QString& text)
{
    connectTC(true);
    if (rd)
    {
        delete (rd);
    }
    rd = 0;
    styledWarningMessage(this, text);
    delete rw;
    rw = 0;

}

void MainForm::reportError(const QString& text)
{
    connectTC(true);
    styledWarningMessage(this, text);
}

void MainForm::cancelRecognition()
{
    connectTC(true);
    if (rw)
    {
        rw->cancel();
    }
    delete rw;
    rw = 0;
    if (rd)
    {
        delete (rd);
    }
    rd = 0;
}

void MainForm::splitTable()
{
    if (!pages->hasPage())
    {
        styledWarningMessage(this, trUtf8("No image loaded"));
        return;
    }
    pages->splitTable();
    loadPage(true);
    //textChanged();
}

void MainForm::showPDFprogress()
{
    epd->hide();
    pdfPD = new QProgressDialog(this, Qt::Dialog | Qt::WindowStaysOnTopHint);
    setupPDFPD();
    pdfPD->show();
    pdfPD->setMinimum(0);
    pdfPD->setMaximum(100);
    dirty = true;
}

void MainForm::extProcStarted()
{
    epd->setWindowTitle(trUtf8("Extracting PDF Pages"));
    epd->show();
}

void MainForm::djvuStarted()
{
    connect(epd, SIGNAL(rejected()), dj2pf, SLOT(cancel()));
    epd->setWindowTitle(trUtf8("Converting DjVu to PDF"));
    epd->show();
}

void MainForm::djvuFinished()
{
    disconnect(epd, SIGNAL(rejected()), dj2pf, SLOT(cancel()));
    epd->hide();
    importPDF(dj2pf->pdfName());
}

void MainForm::menuTriggered(const QString& text)
{
    loadProjectInternal(text);
}

void MainForm::closeAll()
{
    dirty = false;
    setWindowTitle("YAGF");
    pages->clear();
    sideBar->clear();
    graphicsInput->clear();
    textEdit->clear();
    projectName = "";
}

void MainForm::startedAutoSave()
{
    _asm->work(projectName);
    epd->setWindowTitle(trUtf8("Autosave"));
    epd->show();
}

void MainForm::autosaveFinished()
{
    // graphicsInput->clearBlocks();
    epd->hide();
    if (projectName != "")
    {
        dirty = false;
    }
    connectTC(true);
}

void MainForm::testslot()
{
    _asm->startAutoSave();
}

void MainForm::textChanged()
{
    dirty = true;
    if (pages->count() == 0)
    { dirty = false; }
    pages->setText(textEdit->toPlainText());
}

void MainForm::afterConstructor()
{

    QString autosaveDir = settings->workingDir() + "autosave/";
    QDir dir(autosaveDir);
    if (!dir.exists())
    {
        dir.mkdir(autosaveDir);
    }
    QFileInfo fi(autosaveDir + "yagf_project.xml");
    if (fi.exists())
    {
        QPixmap icon;
        icon.load(":/images/question.png");
        QMessageBox messageBox(QMessageBox::NoIcon, "YAGF",
                               trUtf8("There is an unsaved data left from the previous  session. Do you want to open or discard it?"),
                               QMessageBox::Open | QMessageBox::Ignore, this);
        messageBox.setIconPixmap(icon);
        int result = messageBox.exec();
        if (result == QMessageBox::Open)
        {
            loadAutoSaved();
        }
        delAutoSaveFiles();

    }
}

void MainForm::markDirty()
{
    dirty = true;
}

void MainForm::saveAllText()
{
    saveTextInternal(true);
}

void MainForm::saveCurrentText()
{
    saveTextInternal(false);
}


