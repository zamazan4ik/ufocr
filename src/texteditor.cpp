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

#include "texteditor.h"
#include "settings.h"
#include "ycommon.h"
#include "utils.h"
#include <QClipboard>
#include <QFileDialog>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QTextCursor>

TextEditor::TextEditor(QWidget *parent) :
    QTextEdit(parent), spellChecker(this)
{
    hasCopy = false;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    connect(document(), SIGNAL(cursorPositionChanged(const QTextCursor &)), this, SLOT(updateSP()));
    connect(this, SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailable(bool)));
    connect(this, SIGNAL(textChanged()), this, SLOT(textChanged()));
}

TextEditor::~TextEditor()
{
}

bool TextEditor::spellCheck(const QString &lang)
{
    spellChecker.setLanguage(lang);
    return spellChecker.spellCheck();
}

void TextEditor::unSpellCheck()
{
    spellChecker.unSpellCheck();
}

void TextEditor::enumerateDicts()
{
    spellChecker.enumerateDicts();
}

bool TextEditor::hasDict(const QString &shname)
{
    return spellChecker.hasDict(shname);
}


void TextEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if ((e->key() == Qt::Key_Plus) || (e->key() == Qt::Key_Equal)) {
            enlargeFont();
            e->accept();
            return;
        } else if (e->key() == Qt::Key_Minus) {
            decreaseFont();
            e->accept();
            return;
        }
    }
    QTextEdit::keyPressEvent(e);
}

void TextEditor::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
            enlargeFont();
        else
            decreaseFont();
        e->accept();
        return;
    }
    QTextEdit::wheelEvent(e);
}

void TextEditor::replaceWord()
{
    QAction *action =  (QAction *) sender();
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.removeSelectedText();
    cursor.insertText(action->text());
}

void TextEditor::copyAvailable(bool yes)
{
    hasCopy = yes;
}

void TextEditor::textChanged()
{
    Settings *settings = Settings::instance();
    QFont f(font());
    f.setPointSize(settings->getFontSize());
    setFont(f);
}

void TextEditor::copyClipboard()
{
    if (!hasCopy) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(toPlainText(), QClipboard::Clipboard);
    } else
        copy();
}

void TextEditor::saveHtml(QFile *file)
{
    QString text = document()->toHtml().toUtf8();
    QString newDir = extractFilePath(file->fileName()) + extractFileName(file->fileName()) + ".files";
    text.replace("<meta name=\"qrichtext\" content=\"1\" />", "<meta content=\"text/html; charset=utf-8\" http-equiv=\"content-type\" />");
    /*text.replace(workingDir + "output_files",  newDir);
    text.replace("[img src=", "<img src=");
    text.replace(".bmp\"]", ".bmp\">");
    text.replace(".bmp]", ".bmp>");
    QDir dir(workingDir+"output_files");
    dir.rename(workingDir+"output_files", newDir);*/
    file->write(text.toAscii());
}

void TextEditor::globalReplace(const QString &what, const QString &with)
{
    moveCursor(QTextCursor::Start);
    while (find(what)) {
        QTextCursor cursor = textCursor();
        cursor.removeSelectedText();
        cursor.insertText(what);
    }
}

void TextEditor::contextMenuRequested(const QPoint &point)
{
    QAction *action;
    QMenu *menu = new QMenu(this);
    QStringList sl = spellChecker.suggestions();
    //if (sl.count() == 0) {

    action = new QAction(trUtf8("Undo\tCtrl+Z"), this);
    action->setShortcut(QKeySequence("Ctrl+Z"));
    connect(action, SIGNAL(triggered()), this, SLOT(undo()));
    menu->addAction(action);
    action = new QAction(trUtf8("Redo\tCtrl+Shift+Z"), this);
    action->setShortcut(QKeySequence("Ctrl+Shift+Z"));
    connect(action, SIGNAL(triggered()), this, SLOT(redo()));
    menu->addAction(action);
    action = new QAction("separator", this);
    action->setText("");
    action->setSeparator(true);
    menu->addAction(action);
    action = new QAction(trUtf8("Select All\tCtrl+A"), this);
    action->setShortcut(QKeySequence("Ctrl+A"));
    connect(action, SIGNAL(triggered()), this, SLOT(selectAll()));
    menu->addAction(action);
    action = new QAction(trUtf8("Cut\tCtrl+X"), this);
    action->setShortcut(QKeySequence("Ctrl+X"));
    connect(action, SIGNAL(triggered()), this, SLOT(cut()));
    menu->addAction(action);
    action = new QAction(trUtf8("Copy\tCtrl+C"), this);
    action->setShortcut(QKeySequence("Ctrl+C"));
    connect(action, SIGNAL(triggered()), this, SLOT(copyClipboard()));
    menu->addAction(action);
    action = new QAction(trUtf8("Paste\tCtrl+V"), this);
    action->setShortcut(QKeySequence("Ctrl+V"));
    connect(action, SIGNAL(triggered()), this, SLOT(paste()));
    menu->addAction(action);
    action = new QAction("separator", this);
    action->setText("");
    action->setSeparator(true);
    menu->addAction(action);
    action = new QAction(trUtf8("Larger Font\tCtrl++"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(enlargeFont()));
    menu->addAction(action);
    action = new QAction(trUtf8("Smaller Font\tCtrl+-"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(decreaseFont()));
    menu->addAction(action);

    //}
    if (sl.count() > 0)
        menu->addSeparator();
    foreach(QString str, sl) {
        QAction *action = menu->addAction(str);
        connect(action, SIGNAL(triggered()), this, SLOT(replaceWord()));
    }
    menu->exec(mapToGlobal(point));
    delete menu;
}

void TextEditor::enlargeFont()
{
    Settings *settings = Settings::instance();
    int fontSize = font().pointSize();
    fontSize++;
    QFont f(font());
    f.setPointSize(fontSize);
    setFont(f);
    settings->setFontSize(fontSize);
}

void TextEditor::decreaseFont()
{
    Settings *settings = Settings::instance();
    int fontSize = font().pointSize();
    if (fontSize > 1) fontSize--;
    QFont f(font());
    f.setPointSize(fontSize);
    setFont(f);
    settings->setFontSize(fontSize);
}

void TextEditor::updateSP()
{
    Settings *settings = Settings::instance();
    if (settings->getCheckSpelling())
        spellChecker.checkWord();
}
