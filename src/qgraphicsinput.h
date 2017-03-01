/*
    YAGF - cuneiform OCR graphical front-end
    Copyright (C) 2009-2011 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef QGRAPHICSINPUT_H
#define QGRAPHICSINPUT_H

#include "sidebar.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRectF>
#include <QAction>
#include <QList>
#include <QPointer>

typedef QList<QAction *> ActionList;
class Block;

class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QPixmap;
class QCursor;
class QRectF;
class QCursor;
class QToolBar;
class QActions;

enum SelectStates {
    NoSelect = 0,
    StartSelect,
    Selecting
};


class QGraphicsInput : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit QGraphicsInput(const QRectF &sceneRect, QGraphicsView *view = 0);
    ~QGraphicsInput();
    void setView(QGraphicsView *view);
    QRect getActiveBlock();
    QRect getCurrentBlock();
    void deleteBlock(int index);
    void deleteCurrentBlock();
    void deleteActiveBlock();
    void clearBlocks();
    bool addBlock(const QRectF &rect, bool removeObstacles = true);
    void addBlockColliding(Block block);
    void drawLine(int x1, int y1, int x2, int y2);
    void imageOrigin(QPoint &p);
    QPixmap getCurrentImage();

    void cropWhiteFrame();

    void setMagnifierCursor(QCursor *cursor);
    QGraphicsRectItem *newBlock(QRectF rect);
    bool loadImage(const QPixmap &pixmap);
    void setDeskewMode(bool value);
    bool getDeskewMode();
    QLineF getDeskData();
    void clearDeskLine();

    //setMagnifierCursor(QCursor * cursor = );
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
    virtual void keyReleaseEvent(QKeyEvent *keyEvent);
    virtual void keyPressEvent(QKeyEvent *keyEvent);
signals:
    void leftMouseClicked(int x, int y, bool blockSelected);
    void rightMouseClicked(int x, int y, bool inTheBlock);
    void keyPressed(int key);
    void increaseMe();
    void decreaseMe();
    void blockCreated(QRect rect);
    void deleteBlock(QRect rect);
    void clickMeAgain();
private slots:
private:
    void leftMouseRelease(qreal x, qreal y);
    void rightMouseRelease(qreal x, qreal y);
    int nearActiveBorder(qreal x, qreal y);
    void clearTransform();
    void deleteBlockRect(QGraphicsRectItem *item);
    void setDeskLine(int x, int y);
    QGraphicsView *m_view;
    QGraphicsPixmapItem *m_image;
    QGraphicsRectItem *m_CurrentBlockRect;
    QGraphicsRectItem *m_LastSelected;
    QGraphicsLineItem *deskLine;
    SelectStates selecting;
    QRectF blockRect;
    QRectF selBlockRect;
    bool hasImage;
    Qt::MouseButton buttonPressed;
    QCursor *magnifierCursor;
    int near_res;
    QRect redRect;
    bool xred;
    bool deskewMode;
    bool deskewing;
    QLineF desData;
};

#endif // QGRAPHICSINPUT_H
