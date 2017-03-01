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

#include "page.h"
#include "settings.h"
#include "core/ccbuilder.h"
#include "core/rotationcropper.h"
#include "core/pageanalysis.h"
#include "core/analysis.h"
#include "core/imageprocessor.h"
#include "core/subimagepp.h"
#include "globallock.h"
#include <QSize>
#include <QRect>
#include <QFile>
#include <QApplication>
#include <cmath>

Page::Page(const int pid, QObject *parent) :
    QObject(parent), selectedBlock(0,0,0,0)
{
    imageLoaded = false;
    loadedBefore = false;
    ccbuilder = NULL;
    rotation = 0;
    deskewed = false;
    preprocessed = false;
    cropped = false;
    originalFN.clear();
    mFileName.clear();
    this->pid = pid;
    scale = 1;
    text = "";
}

Page::~Page()
{
    delete ccbuilder;
}

bool Page::loadFile(QString fileName, int tiled, bool loadIntoView)
{

    if (fileName == "") {
        if (mFileName.isEmpty()) return false;
        fileName = mFileName;
    }
    if (!fileName.endsWith(".ygf", Qt::CaseInsensitive))
        originalFN = fileName;
    rotation = 0;
    crop1.setX(0);
    crop1.setY(0);
    crop1.setWidth(0);
    crop1.setHeight(0);
    scale = 0.5;
    ImageProcessor ip;
    try {
        img = ip.loadFromFile(fileName);
    } catch (...) {
        return false;
    }

    imageLoaded = !img.isNull();
    if (!imageLoaded)
        return false;
    if (img.format() != QImage::Format_ARGB32)
        img = img.convertToFormat(QImage::Format_ARGB32);
    if (ccbuilder) {
        delete ccbuilder;
        ccbuilder = 0;
    }
    ip.loadImage(img);
    settings = Settings::instance();
    if (settings->getCropLoaded()) {
        if (!cropped) {
            ip.crop();
            setCropped(true);
        }

    }
    img = ip.gsImage();
    if (settings->getPreprocessed()&&(!preprocessed)) {
        ip.loadImage(img);

        ip.binarize();
        img = ip.gsImage();
        preprocessed = true;
    }

    if (Settings::instance()->getAutoDeskew()) {
        if (textHorizontal())
            if (deskew()) {
                if (mFileName.isEmpty()) {
                    ImageProcessor ip;
                    QImage img1 = ip.loadFromFile(fileName);
                    if (img1.format() != QImage::Format_ARGB32)
                        img1 = img1.convertToFormat(QImage::Format_ARGB32);
                    ip.loadImage(img1);
                    ip.binarize();
                    mFileName = Settings::instance()->tmpYGFFileName();
                    ip.saveYGF(ip.gsImage(), mFileName);
                }
                return true;
            }
    }



    rotateImageInternal(img, rotation);
    mFileName = saveTmpPage("YGF");

    loadedBefore = true;
    return true;
}

QPixmap Page::displayPixmap()
{
    return QPixmap::fromImage(currentImage());
}

QImage Page::thumbnail()
{
    return img.scaled(img.width()*0.125, img.height()*0.125);
}

QString Page::recognizedText()
{
    return text;
}

void Page::setRecognizedText(const QString &t)
{
    text = t;
}

bool Page::makeLarger()
{
    clearIntersected();
    if (scale >= 2.0) return false;
    if (scale < 0.2) {
        scale = 0.2;
        return true;
    }
    if (scale < 0.25) {
        scale = 0.25;
        return true;
    }
    if (scale < 0.3) {
        scale = 0.3;
        return true;
    }
    if (scale < 0.5) {
        scale = 0.5;
        return true;
    }
    if (scale < 0.75) {
        scale = 0.75;
        return true;
    }
    if (scale < 0.85) {
        scale = 0.85;
        return true;
    }
    if (scale < 1.0) {
        scale = 1.0;
        return true;
    }
    if (scale < 1.5) {
        scale = 1.5;
        return true;
    }
    if (scale < 2.0) {
        scale = 2.0;
        return true;
    }
    return true;
}

bool Page::makeSmaller()
{
    clearIntersected();
    if (scale <= 0.125) {
        return false;
    }
    if (scale > 0.75) {
        scale = 0.75;
        return true;
    }
    if (scale > 0.5) {
        scale = 0.5;
        return true;
    }
    if (scale > 0.3) {
        scale = 0.3;
        return true;
    }
    if (scale > 0.25) {
        scale = 0.25;
        return true;
    }
    if (scale > 0.2) {
        scale = 0.2;
        return true;
    }
    if (scale > 0.125)
        scale = 0.125;
    return true;
}

void Page::clearIntersected()
{
    foreach (Block b, blocks) {
        QRect r = b;
        Block ir = includes(r);
        if (ir.width() != 0) {
            deleteBlock( (b.blockNumber() < ir.blockNumber() ? b : ir));
            break;
        }
    }
}

void Page::rotate(qreal angle)
{
    rotateImageInternal(img, angle);
    rotation += angle;
    clearBlocks();
}

void Page::unload()
{
    if (ccbuilder) {
        delete ccbuilder;
        ccbuilder = 0;
    }
    img = QImage(0,0,QImage::Format_ARGB32);
    imageLoaded = false;
}

inline bool qrects_equal(QRect &r1, QRect &r2)
{
    if (abs(r1.x() - r2.x()) > 2)
        return false;
    if (abs(r1.y() - r2.y()) > 2)
        return false;
    if (abs(r1.width() - r2.width()) > 2)
        return false;
    if (abs(r1.height() - r2.height()) > 2)
        return false;
    return true;
}

Block Page::includes(const QRect &rect)
{
    foreach (Block b, blocks) {
        QRect r = b;
        if (rect != r) {
            if (rect.intersected(r).width()*rect.intersected(r).height() > 64) return r;
        }
    }
    return QRect(0,0,0,0);
}

void Page::addBlock(Block block, int blocknum, int inTable)
{
    QRect r = block;
    //normalizeRect(r);
    scaleRect(r);
    bool add = true;
    foreach (Block b, blocks) {
        QRect r1 = b;
        if (r == r1) {
            add = false;
            break;
        }
        QRect ir = includes(r);
        if (ir.width() != 0) {
            deleteBlock(r);
        }
    }
    if (blocknum != 0)
            block.setBlockNumber(blocknum);
    if (add) {
        block.setRect(r.x(), r.y(), r.width(), r.height());
        blocks.append(block);
    }
    if (blocknum == 0) {
        sortBlocksInternal();
        renumberBlocks();
    }
    if (inTable == 1) {
        block.setTableCell(true);
    }
}

void Page::deleteBlock(const Block &b)
{
    blocks.removeOne(b);
    sortBlocksInternal();
    renumberBlocks();
}

void Page::deleteBlock(const QRect &r)
{
    QRect rx = r;
    scaleRect(rx);
    foreach (Block b, blocks) {
        QRect r1 = b;
        if (qrects_equal(rx, r1)) {
            blocks.removeAll(b);
            break;
        }
    }
    sortBlocksInternal();
    renumberBlocks();
}

Block Page::getBlock(const QRect &r)
{
    QRect rn =r;
    scaleRectToScale(rn);
    //normalizeRect(rn);
    foreach (Block b, blocks) {
        QRect r1 = b;
        if (qrects_equal(rn,r1)) {
            scaleRect(b);
            return b;
        }
    }
    return Block(0,0,0,0);
}

Block Page::getBlock(int index)
{
    Block b = blocks.at(index);
    scaleRectToScale(b);
    return b;
}

int Page::blockCount()
{
    return blocks.count();
}

void Page::clearBlocks()
{
    blocks.clear();
}

void Page::savePageForRecognition(const QString &fileName)
{
    if(Settings::instance()->getCropLoaded()) {
            ImageProcessor ip;
            ip.loadImage(img);
            ip.crop();
            ip.gsImage().save(fileName, "BMP");
    } else
        img.save(fileName, "BMP");
}

bool Page::savePageAsImage(const QString &fileName, const QString &format)
{
    return img.save(fileName, format.toAscii().data());
}

void Page::saveRawBlockForRecognition(QRect r, const QString &fileName)
{

    saveBlockForRecognition(r, fileName, "BMP");
}

void Page::saveSubimage(QRect r, const QString &fileName)
{
    QImage image = img.copy(r);
    if (Settings::instance()->getUpscale()) {
        ImageProcessor ip;
        image = ip.upScale(image, true);
    }
    image.save(fileName);
}

void Page::saveBlockForRecognition(QRect r, const QString &fileName, const QString &format)
{
    clearIntersected();
    //QRect rs = scaleRect(r);
    QImage image = img.copy(r);
    if (Settings::instance()->getUpscale()) {
        ImageProcessor ip;
        image = ip.upScale(image, true);
    }
    SubimagePP spp(image);
    spp.fillComponents();
    spp.removeBars();
    spp.removeNoise();
    //applyTransforms(image, 1);
    image.save(fileName, format.toAscii().data());
}

void Page::saveBlockForRecognition(int index, const QString &fileName)
{
    foreach(Block b, blocks) {
        if (b.blockNumber() == index+1) {
            saveBlockForRecognition(b, fileName, "BMP");
            return;
        }
    }
    saveBlockForRecognition(blocks.at(index), fileName, "BMP");
}

void Page::selectBlock(const QRect &r)
{
    QRect rn =r;
    //normalizeRect(rn);
    foreach (Block b, blocks) {
        QRect r1 = b;
        if (rn == r1) {
            selectedBlock = b;
            break;
        }
    }
}

Block Page::getSelectedBlock()
{
    return selectedBlock;
}

bool Page::deskew(bool recreateCB)
{
    if (deskewed) return false;
    if (imageLoaded) {
        prepareCCBuilder();
        CCAnalysis *an = new CCAnalysis(ccbuilder);
        qreal angle = 0;
        if (an->analize()) {
            {
                QImage timg;
                timg = tryRotate(img, -atan(an->getK())*360/6.283);
                CCBuilder *cb2 = new CCBuilder(timg);
                cb2->labelCCs();
                CCAnalysis *an2 = new CCAnalysis(cb2);
                an2->analize(true); // If use bars
                Bars bars = an2->getBars();
                qreal barsAngle = 0.0;
                if (bars.count()) {
                    Rect bar = bars[0];
                    int maxdim = abs(bar.x2 - bar.x1) > abs(bar.y2 - bar.y1) ? abs(bar.x2 - bar.x1) : abs(bar.y2 - bar.y1);
                    for (int i = 1; i < bars.count(); i++) {
                        int maxdim1 = abs(bars[i].x2 - bars[i].x1) > abs(bars[i].y2 - bars[i].y1) ? abs(bars[i].x2 - bars[i].x1) : abs(bars[i].y2 - bars[i].y1);
                        if (maxdim1 > maxdim) bar = bars[i];
                    }
                    if ((bar.x2 - bar.x1 != 0) && (bar.y2 - bar.y1 !=0)) {
                        if (abs(bar.x2 - bar.x1) > abs(bar.y2 - bar.y1))
                            barsAngle = atan(float(bar.y2 - bar.y1)/float(bar.x2 - bar.x1));
                        else
                            barsAngle = atan(float(bar.x2 - bar.x1)/float(bar.y2 - bar.y1));
                    }
                    barsAngle=barsAngle*360/6.283;
                }
                angle = -atan(an2->getK())*360/6.283;
                delete an2;
                delete cb2;

                if (abs(angle*10) >= abs(3))
                    angle += (-atan(an->getK())*360/6.283);
                else
                    angle = -atan(an->getK())*360/6.283;
                if ((barsAngle != 0)&&(angle == 0))
                    angle = -barsAngle;
                if (abs(angle*100) < 1) {
                    deskewed = true;
                    return false;
                }
                rotation = angle;
            }
            if (settings->getDoublePreprocessed()) {
                ImageProcessor ip;
                QImage img1 = ip.loadFromFile(originalFileName());
                if (img1.format() != QImage::Format_ARGB32)
                    img1 = img1.convertToFormat(QImage::Format_ARGB32);
                rotateImageInternal(img1, rotation);
                ip.loadImage(img1);
                ip.binarize();
                img = ip.gsImage();
            } else {
                rotate(angle);
            }

            ImageProcessor::cropAngles(img);
            QString fn = saveTmpPage("YGF");
            deskewed = true;

            loadFile(fn, 1);

            delete ccbuilder;
            ccbuilder = 0;
            if (recreateCB)
                prepareCCBuilder();
        }
        delete an;
    }
    return true;
}



void Page::deskew(int x1, int y1, int x2, int y2)
{
    float dx = x2-x1;
    if (dx == 0) return;
    float dy = y2-y1;

    float angle = -atan(dy/dx)*360/6.283;
    if (abs(angle) >= 45)
        return;
    deskewed = true;
    if (abs(angle*100) < 1)
        return;
    rotate(angle);
    rotation = angle;
    ImageProcessor::cropAngles(img);
    QString fn = saveTmpPage("YGF");
    deskewed = true;
    loadFile(fn, 1);

}

void Page::rotate90CW()
{
    deskewed = false;
    rotate(90);
}

void Page::rotate90CCW()
{
    deskewed = false;
    rotate(-90);
}

void Page::rotate180()
{
    deskewed = false;
    rotate(180);
}

void Page::blockAllText()
{
    prepareCCBuilder();
    clearBlocks();
    BlockSplitter bs;
    bs.setImage(img, rotation, scale);
    QRect r = bs.getRootBlock(currentImage());
    addBlock(r);
}

QList<Rect> Page::splitInternal()
{
    clearBlocks();
    BlockSplitter bs;
    //rotation  = 0;
    bs.setImage(img, 0, 1.0);// sideBar->getScale());
    bs.splitBlocks();
    return bs.getBlocks();
}

void Page::prepareCCBuilder()
{
    if (!ccbuilder) {
        ccbuilder = new CCBuilder(img);
        ccbuilder->labelCCs();
    }
}

bool Page::splitPage(bool preprocess)
{
    QList<Rect> blocks;
    prepareCCBuilder();
    if (preprocess) {
        QString fn = saveTmpPage("YGF");
        loadedBefore = false;
        loadFile(fn, 1);
        blocks = splitInternal();
        /*if (blocks.count() == 0) {
            deskew();
            fn =Settings::instance()->workingDir() + QString::fromUtf8("tmp-%1.bmp").arg((quint64)img2.data_ptr());
            saveTmpPage(fn, true, false);
            loadedBefore = false;
            loadFile(fn);
            blocks = splitInternal();
        }*/
        preprocessed = true;
    } else {
        deskew();
        blocks = splitInternal();
    }
    qreal sf = scale;
    foreach (Rect block, blocks) {
        QRect r;
        block.x1 *=sf;
        block.y1 *=sf;
        block.x2 *= sf;
        block.y2 *=sf;

        block.x1 -=4;
        block.y1 +=6;
        block.x2 -= 4;
        block.y2 +=6;


        r.setX(block.x1);
        r.setY(block.y1);
        r.setWidth(block.x2 - block.x1);
        r.setHeight(block.y2 - block.y1);
        addBlock(r);
    }
    return blocks.count() != 0;
}

bool Page::textHorizontal()
{
    return ImageProcessor::isTextHorizontal(img);
}

QString Page::fileName()
{
    return mFileName;
}

QString Page::originalFileName() const
{
    if (originalFN.isEmpty())
        return mFileName;
    return originalFN;
}

void Page::setOriginalFileName(const QString &fn)
{
    originalFN = fn;
}

int Page::pageID()
{
    return pid;
}

void Page::sortBlocksInternal()
{
    bool allBlocksNumbered = true;
    foreach(Block b, blocks)
        if (b.blockNumber() == 0)
            allBlocksNumbered = false;
    if (allBlocksNumbered)
        return;
    sortBlocks(blocks);
}

bool Page::isDeskewed()
{
    return deskewed;
}

bool Page::isCropped()
{
    return cropped;
}

void Page::setCropped(bool value)
{
    cropped = value;
}

bool Page::isPreprocessed()
{
    return preprocessed;
}

qreal Page::getRotation()
{
    return rotation;
}

void Page::setDeskewed(bool value)
{
    deskewed = value;
}

void Page::setPreprocessed(bool value)
{
    preprocessed = value;
}


void Page::applyTransforms(QImage &image, qreal scale)
{
    scale = scale*2;
    if (!isCropped()) {
        QRect crop;
        crop.setX(crop1.x()*scale);
        crop.setY(crop1.y()*scale);
        crop.setWidth(crop1.width()*scale);
        crop.setHeight(crop1.height()*scale);
        image = image.copy(crop);
    }
    rotateImageInternal(image, rotation);
    setCropped(true);
}

void Page::rotateImageInternal(QImage &image, qreal angle)
{
    qreal x = image.width() / 2;
    qreal y = image.height() / 2;
    image = image.transformed(QTransform().translate(-x, -y).rotate(angle).translate(x, y), Qt::SmoothTransformation);
}

QRect Page::scaleRect(QRect &rect)
{
    qreal iscale = 1./scale;
    qreal oldw = rect.width();
    qreal oldh = rect.height();
    rect.setX(rect.x()*iscale);
    rect.setY(rect.y()*iscale);
    rect.setWidth(oldw*iscale);
    rect.setHeight(oldh*iscale);
    return rect;
}

QRect Page::scaleTo(QRect &rect, qreal newScale)
{
    qreal iscale = newScale/scale;
    qreal oldw = rect.width();
    qreal oldh = rect.height();
    rect.setX(rect.x()*iscale);
    rect.setY(rect.y()*iscale);
    rect.setWidth(oldw*iscale);
    rect.setHeight(oldh*iscale);
    return rect;
}

bool blocksLessThan( Block &b1, Block &b2)
{
    if ((b1.top()/8) < (b2.top()/8))
        return true;
    else
        if ((b1.top()/8) > (b2.top()/8))
            return false;
    if ((b1.left()) < (b2.left()))
        return true;
    return false;
}

int findLine(QVector<Block> table, int startFrom)
{
    if (startFrom > table.count())
        return 0;
    int baseY = table.at(startFrom - 1).y()/8;
    for (int i = startFrom; i < table.count(); i++) {
        int curY = table.at(i).y()/8;
        if (curY != baseY)
            return i;
    }
    return table.count();
}

void sortBlocks2(QVector<Block> &blocks)
{
    qSort(blocks.begin(), blocks.end(), blocksLessThan);
}


void Page::splitTable()
{

    Block bblock(0,0,0,0);
    if (blockCount() == 1) {
        bblock = blocks.at(0);
        clearBlocks();
    }
    if (bblock.x() < 0)
            bblock.setX(0);
    if (bblock.y() < 0)
            bblock.setY(0);
    if (bblock.x() + bblock.width() > img.width())
            bblock.setWidth(img.width() - 1 - bblock.x());
    if (bblock.y() + bblock.height() > img.height())
            bblock.setHeight(img.height() - 1 - bblock.y());
    if (blockCount() == 1) {

    }

    ImageProcessor ip0;
    ip0.loadImage(img);
    QRect r = ip0.deskewByTable(bblock);
    deskew(r.x(), r.y(), r.x()+r.width(), r.y() + r.height());

    ImageProcessor ip;
    ip.loadImage(img);
    QList<Rect> b;



    b = ip.splitTable(bblock);
    if (b.count() < 3) {
        ImageProcessor ip1;
        ip1.loadImage(img);
        b = ip1.splitTableForce(bblock);
    }
    Blocks bs;
    for (int i = 0; i <  b.count(); i++) {
        Rect r = b.at(i);
        Block block(r.x1, r.y1, r.x2-r.x1, r.y2-r.y1);
        scaleRectToScale(block);
        bs.append(block);
    }
    QVector<Block> vb;
    for (int i = 0; i <  bs.count(); i++)
        vb.append(bs.at(i));
    sortBlocks2(vb);
    for (int i = 0; i < vb.count(); i++) {
        addBlock(vb.at(i), i+1, 1);
    }
    vb.last().setRowEnd(true);
    emit refreshView(true);
}

QRect Page::scaleRectToScale(QRect &rect)
{
    qreal oldw = rect.width();
    qreal oldh = rect.height();
    rect.setX(rect.x()*scale);
    rect.setY(rect.y()*scale);
    rect.setWidth(oldw*scale);
    rect.setHeight(oldh*scale);
    return rect;
}


QRect Page::shiftRectBeforeScale(QRect &rect, int sx, int sy)
{
    int w = rect.width();
    int h = rect.height();
    int tx = rect.x() + sx;
    int ty = rect.y() + sy;
    rect.setX(tx);
    rect.setY(ty);
    rect.setWidth(w);
    rect.setHeight(h);
    return rect;
}

QImage Page::tryRotate(QImage image, qreal angle)
{
    qreal x = image.width() / 2;
    qreal y = image.height() / 2;
    return image.transformed(QTransform().translate(-x, -y).rotate(angle).translate(x, y), Qt::SmoothTransformation);
}

QImage Page::currentImage()
{
    if (!imageLoaded) {
        ImageProcessor ip;
        img = ip.loadYGF(mFileName);
        if (img.isNull())
            img.load(mFileName);
        applyTransforms(img, 0.5);
        imageLoaded=true;
    }
    try {
        if (!img.isNull())
            return img.scaled(img.width()*scale, img.height()*scale);
        else {
            ImageProcessor ip;
            img = ip.loadYGF(mFileName);
            if (img.isNull())
                img.load(mFileName);
            applyTransforms(img, scale);
            return img;
        }
    } catch (...) {
        return img;
    }
}
#include <QTemporaryFile>
QString Page::saveTmpPage(const QString &format)
{
    QString fileName =Settings::instance()->tmpFileName();
    //    booster.flatten(&image);
    if (format == "BMP") {
        fileName = fileName +".bmp";
        img.save(fileName, "BMP");
    } else {
        fileName = fileName +".ygf";
        ImageProcessor ip;
        ip.saveYGF(img, fileName);
    }
    return fileName;
}

void Page::reSaveTmpPage()
{
    ImageProcessor ip;
    ip.saveYGF(img, mFileName);
}



void Page::renumberBlocks()
{
    for (int i = 0; i < blocks.count(); i++) {
        blocks[i].setBlockNumber(i+1);
    }
}

