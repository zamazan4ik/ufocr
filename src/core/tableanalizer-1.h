#ifndef TABLEANALIZER_H
#define TABLEANALIZER_H

#include "../utils.h"
#include <QList>
#include <QRect>

class QIPBlackAndWhiteImage;

class TableAnalizer
{
public:
    TableAnalizer(QIPBlackAndWhiteImage *image);
    QList<Rect> splitTable(const QRect &bounds);
    QList<Rect> splitTableForce(const QRect &bounds);
private:
    QIPBlackAndWhiteImage * img;
    Rect maxRect;
    QList<Rect> blocks;
private:
    bool findRect(const QRect &bounds);
    void updateComponents(int x, int y, int label);
    bool findMax(const QRect &bounds);
    void clearImage();
    void clearGarbage();
    void drawHorzLine(int y, int minx, int maxx);
    void drawVertLine(int x, int miny, int maxy);
    void addBars(const QRect &bounds);
    bool findHorzLine(qint32 y, qint32 xstart, qint32 xstop, int count);
};

#endif // TABLEANALIZER_H
