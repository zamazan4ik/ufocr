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

#ifndef TPAGE_H
#define TPAGE_H

#include "tblock.h"
#include "core/analysis.h"
#include <QObject>
#include <QRect>
#include <QImage>
#include <QPixmap>

class CCBuilder;
class Settings;

class Page : public QObject
{
    Q_OBJECT
public:
    explicit Page(const int pid, QObject *parent = 0);
    ~Page();
    bool loadFile(QString fileName, int tiled, bool loadIntoView = true);
    QPixmap displayPixmap();
    QImage thumbnail();
    QString recognizedText();
    void setRecognizedText(const QString &t);
    bool makeLarger();
    bool makeSmaller();
    void rotate(qreal angle);
    void unload();
    void addBlock(Block block, int blocknum = 0, int inTable = 0);
    void deleteBlock(const Block &b);
    void deleteBlock(const QRect &r);
    Block getBlock(const QRect &r);
    Block getBlock(int index);
    int blockCount();
    void clearBlocks();
    void savePageForRecognition(const QString &fileName);
    bool savePageAsImage(const QString &fileName, const QString &format);
    void saveRawBlockForRecognition(QRect r, const QString &fileName);
    void saveSubimage(QRect r, const QString &fileName);
    void saveBlockForRecognition(QRect r, const QString &fileName, const QString &format);
    void saveBlockForRecognition(int index, const QString &fileName);
    void selectBlock(const QRect &r);
    Block getSelectedBlock();
    bool deskew(bool recreateCB = true);
    void deskew(int x1, int y1, int x2, int y2);
    void rotate90CW();
    void rotate90CCW();
    void rotate180();
    void blockAllText();
    bool splitPage(bool preprocess);
    bool textHorizontal();
    QString fileName();
    QString originalFileName() const;
    void setOriginalFileName(const QString &fn);
    int pageID();
    void sortBlocksInternal();
    bool isDeskewed();
    bool isCropped();
    void setCropped(bool value);
    bool isPreprocessed();
    qreal getRotation();
    void setDeskewed(bool value);
    void setPreprocessed(bool value);
    void reSaveTmpPage();
    QRect scaleRect(QRect &rect);
    QRect scaleTo(QRect &rect, qreal newScale);
    QRect scaleRectToScale(QRect &rect);
    void splitTable();
signals:
    void refreshView(bool show);
    void textOut(const QString &msg);
public slots:
private:
    void renumberBlocks();
    void applyTransforms(QImage &image, qreal scale);
    void rotateImageInternal(QImage &image, qreal angle);
    QRect shiftRectBeforeScale(QRect &rect, int sx, int sy);
    QImage tryRotate(QImage image, qreal angle);
    QImage currentImage();
    QString saveTmpPage(const QString &format);
    QList<Rect> splitInternal();
    void prepareCCBuilder();
    Block includes(const QRect &rect);
    void clearIntersected();
private:
    QString originalFN;
    qreal scale;
    qreal rotation;
    QRect crop1;
    QRect crop2;
    bool deskewed;
    bool cropped;
    QImage img;
    Blocks blocks;
    bool imageLoaded;
    bool loadedBefore;
    bool preprocessed;
    QString mFileName;
    CCBuilder *ccbuilder;
    Settings *settings;
    int blockPointer;
    int pid;
    Block selectedBlock;
    QString text;
};

#endif // TPAGE_H
