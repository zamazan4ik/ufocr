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

#include "tpagecollection.h"
#include "core/imageprocessor.h"
#include "qsnippet.h"
#include "settings.h"
#include <QApplication>
#include <QFile>
#include <QImage>
#include <QMutex>
#include <QFile>

static QMutex lsGate;

PageCollection *PageCollection::m_instance = NULL;

PageCollection::PageCollection(QObject *parent) :
    QObject(parent),
    index(-1),
    pid(0)
{
}

PageCollection::PageCollection(const PageCollection &)
{
}

PageCollection::~PageCollection()
{
    clear();
}

bool PageCollection::appendPage(const QString &fileName)
{


    unloadAll();
    Page *p = new Page(++pid);
    connect(p,SIGNAL(refreshView(bool)), this, SIGNAL(loadPage(bool)));
    if (p->loadFile(fileName, 1, false)) {

        pages.append(p);
        index = pages.count() - 1;
        // if (Settings::instance()->getAutoDeskew()) {
        //   deskew();

        //p->cropYGF();
        //p->reSaveTmpPage();
        //   }
        emit addSnippet(index);
        connect(p, SIGNAL(textOut(QString)), SLOT(textOut(QString)));
        return true;
    } else {
        delete p;
        pid--;
        return false;
    }

}

Page * PageCollection::newPage(const QString &fileName, qreal rotation, bool preprocessed, bool deskewed, bool cropped)
{
    if (cp())
        cp()->unload();
    Page *p = new Page(++pid);
    connect(p,SIGNAL(refreshView(bool)), this, SIGNAL(loadPage(bool)));
    p->setDeskewed(deskewed);
    p->setPreprocessed(preprocessed);
    p->setCropped(cropped);
    if (p->loadFile(fileName, 1)) {
        pages.append(p);
        p->rotate(rotation);
        index = pages.count() - 1;
        emit addSnippet(index);
        makePageCurrent(index);        
        return p;
    }
    return NULL;
}



int PageCollection::count()
{
    return pages.count();
}

QString PageCollection::text()
{
    if (cp())
        return cp()->recognizedText();
    return "";
}

void PageCollection::setText(const QString &t)
{
    if (cp())
        cp()->setRecognizedText(t);
    mTextSaved = false;
}

bool PageCollection::makePageCurrent(int index)
{
    if (cp()) {
        cp()->unload();
    }
    this->index = index;
    return index < pages.count();
}

bool PageCollection::makePageCurrentByID(int id)
{
    return makePageCurrent(id2Index(id)) >= 0;
}

void PageCollection::setBeforeFirst()
{
    index = -1;
}

bool PageCollection::makeNextPageCurrent()
{
    if (cp()) {
        cp()->unload();
    }
    index++;
    if (index < count())
        return true;
    index = -1;
    return false;
}

bool PageCollection::textAtNextId(int &cid, QString &text)
{
    int tmpcid = 0x1000;
    int idx = 0;
    for(int i = 0; i < pages.count(); i++) {
        int pid = pages.at(i)->pageID();
        if (pid > cid) {
            if (pid <= tmpcid) {
                tmpcid = pid;
                idx = i;
            }
        }
    }
    if (tmpcid != 0x1000) {
        text = pages.at(idx)->recognizedText();
        cid = tmpcid;
        return true;
    }
    return false;
}

QSnippet *PageCollection::snippet()
{
    if (!cp()) return NULL;
    QSnippet *s = new QSnippet();
    s->setPage(cp()->pageID(), cp()->originalFileName(), cp()->thumbnail());
    return s;
}

QPixmap PageCollection::pixmap()
{
    if (!cp()) return QPixmap();
    return cp()->displayPixmap();
}

void PageCollection::savePageForRecognition(const QString &fileName)
{
    if (!cp()) return;
    cp()->savePageForRecognition(fileName);
}

void PageCollection::SaveCurrentPageText(const QString &fileName, bool truncate)
{
    if (!cp())
        return;
    QFile textFile(fileName);
    if (truncate)
        textFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    else
        textFile.open(QIODevice::ReadWrite);
    //textFile.seek()
    textFile.write(currentText().toUtf8());
    textFile.close();
    if (pages.count() ==1)
        mTextSaved = true;

}

void PageCollection::saveAllText(const QString &fileName, bool truncate)
{
    if (!cp())
        return;
    int cid = currentId;
    QFile textFile(fileName);
    if (truncate)
        textFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    else
        textFile.open(QIODevice::ReadWrite);
    int _pid =-1;
    QString txt;
    while(textAtNextId(_pid, txt))
        textFile.write(txt.toUtf8());
    textFile.close();
    makePageCurrentByID(cid);
    mTextSaved = true;
}

void PageCollection::saveRawBlockForRecognition(QRect r, const QString &fileName)
{
    if (!cp()) return;
    cp()->saveRawBlockForRecognition(r, fileName);
}

void PageCollection::saveBlockForRecognition(QRect r, const QString &fileName, const QString &format)
{
    if (!cp()) return;
    cp()->saveBlockForRecognition(r, fileName, format);
}

void PageCollection::saveBlockForRecognition(int index, const QString &fileName)
{
    if (!cp()) return;
    if (index == 0)
        cp()->sortBlocksInternal();
    cp()->saveBlockForRecognition(index, fileName);
}

int PageCollection::blockCount()
{
    if (!cp()) return 0;
    return cp()->blockCount();
}

Block PageCollection::getBlock(const QRect &r)
{
    Block block(0,0,0,0);
    if (!cp()) return block;
    return cp()->getBlock(r);
}

Block PageCollection::getBlock(int index)
{
    Block block(0,0,0,0);
    if (!cp()) return block;
    return cp()->getBlock(index);
}

void PageCollection::selectBlock(const QRect &r)
{
    if (!cp()) return;
    cp()->selectBlock(r);
}

Block PageCollection::getSelectedBlock()
{
    Block block(0,0,0,0);
    if (!cp()) return block;
    return cp()->getSelectedBlock();
}

bool PageCollection::pageValid()
{
    return cp() != 0;
}

QString PageCollection::fileName()
{
    if (!cp())
        return "";
    return cp()->fileName();
}

QString PageCollection::originalFileName()
{
    if (!cp())
        return "";
    return cp()->originalFileName();
}

bool PageCollection::hasPage()
{
    return (cp() != 0);
}

bool PageCollection::savePageAsImage(const QString &fileName, const QString &format)
{
    if (!cp())
        return false;
    return cp()->savePageAsImage(fileName, format);
}

bool PageCollection::isDeskewed()
{
    if (cp())
        return cp()->isDeskewed();
    return false;
}

bool PageCollection::isPreprocessed()
{
    if (cp())
        return cp()->isPreprocessed();
    return false;
}

bool PageCollection::isCropped()
{
    if (cp())
        return cp()->isCropped();
    return false;
}

qreal PageCollection::getRotation()
{
    if (cp())
        return cp()->getRotation();
    return 0;
}

void PageCollection::setRotation(const qreal value)
{
    if (cp())
        cp()->rotate(value);
}

void PageCollection::setDeskewed(const bool value)
{
    if (cp())
        cp()->setDeskewed(value);
}

void PageCollection::setPreprocessed(const bool value)
{
    if (cp())
        cp()->setPreprocessed(value);
}

void PageCollection::reloadPage()
{
    emit loadPage(true);
}

void PageCollection::unloadAll()
{
    foreach(Page *p, pages) {
        p->unload();
    }
}

void PageCollection::deskew(int x1, int y1, int x2, int y2)
{
    if (cp())
        cp()->deskew(x1,y1,x2,y2);
    emit loadPage(true);
}

QRect PageCollection::scaleRect(QRect &rect)
{
    if (cp())
        return cp()->scaleRect(rect);
    return QRect(0,0,0,0);
}

QRect PageCollection::scaleRect(QRect &rect, qreal scale)
{
    QRect result(0,0,0,0);
    if (cp()) {
        result = cp()->scaleTo(rect, scale);
    }
    return result;
}

QRect PageCollection::scaleRectToPage(QRect &rect)
{
    QRect result(0,0,0,0);
    if (cp()) {
        result = cp()->scaleRectToScale(rect);
    }
    return result;
}

void PageCollection::splitTable()
{
    if (cp())
        cp()->splitTable();
}


void PageCollection::saveCurrentText(const QString &text)
{
       if (cp())
           cp()->setRecognizedText(text);
}

QString PageCollection::currentText()
{
    if (cp())
        return cp()->recognizedText();
    return "";
}

void PageCollection::storeCurrentIndex()
{
    if (cp())
        currentId = cp()->pageID();
    else {
        if (pages.count() > 0) {
            makePageCurrent(0);
            currentId = cp()->pageID();
        }
    }
}

void PageCollection::restoreCurrentIndex()
{
    makePageCurrentByID(currentId);
    emit loadPage(true);
}


void PageCollection::makeLarger()
{
    if (!cp()) return;
    if (lsGate.tryLock()) {
        cp()->makeLarger();
        emit loadPage(true);
        lsGate.unlock();
    }
}

void PageCollection::makeSmaller()
{
    if (!cp()) return;
    if (lsGate.tryLock()) {
        cp()->makeSmaller();
        emit loadPage(true);
        lsGate.unlock();
    }
}

void PageCollection::rotate90CW()
{
    if (!cp()) return;
    cp()->rotate90CW();
    emit loadPage(true);
}

void PageCollection::rotate90CCW()
{
    if (!cp()) return;
    cp()->rotate90CCW();
    emit loadPage(true);
}

void PageCollection::rotate180()
{
    if (!cp()) return;
    cp()->rotate180();
    emit loadPage(true);
}

void PageCollection::deskew()
{
    if (!cp()) return;
    if (cp()->textHorizontal())
        cp()->deskew();
    emit loadPage(true);
}

void PageCollection::blockAllText()
{
    if (!cp()) return;
    cp()->blockAllText();
    emit loadPage(true);
}

bool PageCollection::splitPage(bool preprocess)
{
    if (!cp()) return false;
    bool res = cp()->splitPage(preprocess);
    emit loadPage(true);
    return res;
}

void PageCollection::addBlock(const QRect &rect)
{
    if (!cp()) return;
    Block block(rect.x(), rect.y(), rect.width(), rect.height());
    cp()->addBlock(block);
}

void PageCollection::addBlock(const QRect &rect, int num)
{
    if (!cp()) return;
    Block block(rect.x(), rect.y(), rect.width(), rect.height());
    cp()->addBlock(block, num);
}

void PageCollection::deleteBlock(const QRect &rect)
{
    if (!cp()) return;
    cp()->deleteBlock(rect);
}

void PageCollection::clearBlocks()
{
    if (!cp()) return;
    cp()->clearBlocks();
}

void PageCollection::clear()
{
    foreach (Page * p, pages) {
        delete p;
    }
    pages.clear();
    emit cleared();
    index = -1;
    pid = 0;
}


Page *PageCollection::cp()
{
    if ((index < 0)|| (index >= count()))
        return (Page *) 0;
    return pages.at(index);
}

int PageCollection::id2Index(int id)
{
    foreach (Page *p, pages) {
        if (p->pageID() == id)
            return pages.indexOf(p);
    }
    return -1;
}

void PageCollection::pageSelected(int id)
{
    makePageCurrent(id2Index(id));
    emit loadPage(true);
}

void PageCollection::pageRemoved(int id)
{
    int index = id2Index(id);
    if (index >= 0) {
        delete pages.at(index);
        pages.remove(index);
    }
    if (index >= pages.count())
        index = pages.count() - 1;
    makePageCurrent(index);
    emit loadPage(true);
}

void PageCollection::textOut(const QString &msg)
{
    emit messagePosted(msg);
}

PageCollection *PageCollection::instance()
{
    if (!m_instance)
        m_instance = new PageCollection();
    return m_instance;
}

void PageCollection::clearCollection()
{
    if (m_instance)
        m_instance->clear();
}

void PageCollection::setOriginalFileName(const QString &fn)
{
    if (cp())
        cp()->setOriginalFileName(fn);
}
