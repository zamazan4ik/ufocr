/*
    YAGF - cuneiform and tesseract OCR graphical front-end
    Copyright (C) 2009-2012 Andrei Borovsky <anb@symmetrica.net>

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

#include "settings.h"
#include <QMainWindow>
#include <QString>
#include "ui_mainform.h"
#include <QProgressDialog>
#include <QMap>
//#include "ui_popplerdialog.h"

class QComboBox;

class QCheckBox;

class QLabel;

class QCloseEvent;

class QPixmap;

class QProcess;

class QByteArray;

class QFile;

class SpellChecker;

class QCursor;

class QGraphicsInput;

class QMenu;

class PDFExtractor;

class ccbuilder;

class QLabel;

class QAction;

class QPoint;

class RecognizerWrapper;

class RecognitionDialog;

class ExtProcessDialog;

class Djvu2PDF;

class AutoSaveManager;

const QString version = "0.9.5";

class PageCollection;

class ScannerBase;

class MainForm : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
    MainForm(QWidget* parent = 0);

    ~MainForm();

signals:

    void windowShown();

    void callAfterConstructor();

private slots:

    //void on_actionRecognize_activated();
    void on_actionSelect_HTML_format_activated();

    void upscale();

    //void on_alignButton_clicked();
    void on_actionCheck_spelling_activated();

    void on_actionSave_block_activated();

    void on_actionSave_current_image_activated();

    void on_actionCheck_spelling_triggered();

    void on_actionRecognize_block_activated();

    void recognize();

    void on_ActionDeleteBlock_activated();

    void on_ActionClearAllBlocks_activated();

    void loadImage();

    void rotateCWButtonClicked();

    void rotateCCWButtonClicked();

    void rotate180ButtonClicked();

    void enlargeButtonClicked();

    void decreaseButtonClicked();

    void singleColumnButtonClicked();

    void newLanguageSelected(int index);

    void scanImage();

    void loadNextPage();

    void loadPreviousPage();

    //void recognizeAll();
    void showAboutDlg();

    void showHelp();

    void unalignButtonClicked();

    void importPDF(const QString& fileName);

    void importDjVu(const QString& fileName);

    void showConfigDlg();

    void addPDFPage(QString pageName, int current, int total);

    void finishedPDF();

    void pasteimage();

    void deskewByBlock();

    void selectTextArea();

    void selectBlocks();

    void selectHTMLformat();

    void loadFiles(const QStringList& files);

    void LangTextChanged(const QString& text);

    void SelectRecognitionLanguages();

    void selectLanguages();

    void deskewByLine();

protected:
    void timerEvent(QTimerEvent* e);

private:
    virtual void closeEvent(QCloseEvent* event);

    void initSettings();

    void loadFile(const QString& fn, bool loadIntoView = true);

    void loadTIFF(const QString& fn, bool loadIntoView = true);

    void delTmpFiles();

    void delAutoSaveFiles();

    void delTmpDir();

    QString getFileNameToSaveImage(QString& format);

    void loadFromCommandLine();

    void fillLangBox();

    void createRW();

    void createRecentMenu();

    void loadProjectInternal(const QString& path);

    void connectTC(bool doIt);

    void loadAutoSaved();

    void saveTextInternal(bool allText);

private:
    QComboBox* selectLangsBox;
    QGraphicsInput* graphicsInput;
    QString fileName;
    QCursor* resizeCursor;
    QCursor* resizeBlockCursor;
    bool useXSane;
    ScannerBase* scanner;
    QByteArray* ba;
    //SpellChecker *spellChecker;
    QMenu* m_menu;
    PDFExtractor* pdfx;
    Djvu2PDF* dj2pf;
    QProgressDialog* pdfPD;
    int ifCounter;
    Settings* settings;
    PageCollection* pages;
    QLabel* engineLabel;
    QLabel* langLabel;
    QAction* slAction;
    bool globalDeskew;
    QPoint actPos;
    QString oldooltip;
    int timerId;
    RecognizerWrapper* rw;
    RecognitionDialog* rd;
    ExtProcessDialog* epd;
    AutoSaveManager* _asm;
    QString projectName;
    bool dirty;
    bool forbidAutoSave;
private slots:

    void clickMeAgain();

    void readyRead(int sig);

    void setResizingCusor();

    void setUnresizingCusor();

    void loadPage(bool show);

    void rightMouseClicked(int x, int y, bool inTheBlock);

    void setupPDFPD();

    void onShowWindow();

    void addSnippet(int index);

    void preprocessPage();

    void saveProjectAs();

    void saveProject();

    void loadProject();

    void on_actionKeep_Lines_toggled(bool arg1);

    void readOutput(QString text, QChar separator);

    void recognitionFinished();

    void recognitionError(const QString& text);

    void reportError(const QString& text);

    void cancelRecognition();

    void splitTable();

    void showPDFprogress();

    void extProcStarted();

    void djvuStarted();

    void djvuFinished();

    void menuTriggered(const QString& text);

    void closeAll();

    void startedAutoSave();

    void autosaveFinished();

    void testslot();

    void textChanged();

    void afterConstructor();

    void markDirty();

    void saveAllText();

    void saveCurrentText();
};
