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

#include "qgraphicsinput.h"
#include "qxtgraphicsview.h"
#include "qxtgraphicsproxywidget.h"
#include "core/pageanalysis.h"
#include "math.h"
#include "ycommon.h"
#include "tblock.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QToolBar>
#include <QLayout>
#include <QGraphicsTextItem>


QGraphicsInput::QGraphicsInput(const QRectF &sceneRect, QGraphicsView *view) :
    QGraphicsScene(sceneRect)
{
    setView(view);
    m_image = 0;
    selecting  = NoSelect;
    hasImage = false;
    m_LastSelected = 0;
    buttonPressed = Qt::NoButton;
    near_res = 0;
    magnifierCursor = new QCursor(Qt::SizeAllCursor);
    redRect.setX(0);
    redRect.setY(0);
    redRect.setWidth(0);
    redRect.setHeight(0);
    xred = false;
    deskewMode = false;
    deskLine = 0;
    deskewing = false;
}

QGraphicsInput::~QGraphicsInput()
{
    delete magnifierCursor;
}

bool QGraphicsInput::loadImage(const QPixmap &pixmap)
{
    clear();
    m_LastSelected = 0;
    m_CurrentBlockRect = 0;
    m_image = addPixmap(pixmap);
    setSceneRect(pixmap.rect());
    m_image->setZValue(-1);
    QApplication::processEvents();
    this->setFocus();
    m_image->setFocus();
    m_image->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
    m_image->setAcceptHoverEvents(true);
    m_image->setData(1, "image");
    if (m_view) {
        m_view->centerOn(0, 0);
        m_view->show();
        update();
        hasImage = true;
        return true;
    } else
        return false;
}

void QGraphicsInput::setDeskewMode(bool value)
{
    deskewMode = value;
}

bool QGraphicsInput::getDeskewMode()
{
    return deskewMode;
}

void QGraphicsInput::setView(QGraphicsView *view)
{
    m_view = view;
    if (m_view)
        m_view->setScene(this);

}

void QGraphicsInput::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
    // QMessageBox::critical(0, "MOUS111", "MOUSE");
    if (!hasImage)
        return;
    if (buttonPressed == Qt::LeftButton) {
        if (event->buttons() != Qt::LeftButton)
            mouseReleaseEvent(event);
        event->accept();
        return;
    }
    if (event->buttons() == Qt::LeftButton) {
        if (deskewMode) {
            if (deskewing) {
                setDeskLine(event->scenePos().x(), event->scenePos().y());
                return;
            }
            if (deskLine) {
                clearDeskLine();
            } else {
                setDeskLine(event->scenePos().x(), event->scenePos().y());
                deskewing = true;
                return;
            }
        } else {
            if (deskLine)
                clearDeskLine();
        }
        buttonPressed = Qt::LeftButton;
        if (selecting == NoSelect) {
            if ((near_res = nearActiveBorder(event->scenePos().x(), event->scenePos().y())) != 0) {
                m_CurrentBlockRect = m_LastSelected;
                selecting = Selecting;
                blockRect = m_CurrentBlockRect->rect();
                emit deleteBlock(QRectF2Rect(blockRect));
            }  else {
                selecting = StartSelect;
                blockRect.setLeft(event->lastScenePos().x());
                blockRect.setTop(event->lastScenePos().y());
                blockRect.setWidth(10);
                blockRect.setHeight(10);
            }
        } else {
            //TODO!!!
        }
    } else
        buttonPressed = Qt::RightButton;
}

void QGraphicsInput::deleteBlockRect(QGraphicsRectItem *item)
{
    if (item == 0)
        return;
    if (item == m_CurrentBlockRect)
        m_CurrentBlockRect = 0;
    if (item == m_LastSelected)
        m_LastSelected = 0;
    removeItem(item);
}

void QGraphicsInput::setDeskLine(int x, int y)
{
    if (deskLine == 0) {
        deskLine = new QGraphicsLineItem(m_image);
        QPen pen = QPen(QColor(80, 80, 0));
        pen.setWidth(2);
        deskLine->setPen(pen);
        deskLine->setLine(x, y, x, y);
    } else {
        QLineF l = deskLine->line();
        deskLine->setLine(l.x1(), l.y1(), x, y);
        desData = deskLine->line();
    }

    deskLine->show();
}

QLineF QGraphicsInput::getDeskData()
{
    return desData;
}

void QGraphicsInput::clearDeskLine()
{
    delete deskLine;
    deskLine = 0;
    deskewMode = false;
    deskewing = false;
    desData = QLineF(0,0,0,0);
}

void QGraphicsInput::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    if (deskewing)
        emit clickMeAgain();
    if (buttonPressed == Qt::LeftButton) {
        if (deskewing)
            emit clickMeAgain();
        if (selecting == Selecting) {
            selecting = NoSelect;
            if ((blockRect.width() < 12) || (blockRect.height() < 12)) {
                if (m_CurrentBlockRect == m_LastSelected)
                    m_LastSelected = 0;
                deleteBlockRect(m_CurrentBlockRect);
                //clik!!!
                leftMouseRelease(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
            } else emit blockCreated(QRectF2Rect(m_CurrentBlockRect->rect()));
            if (xred)
                emit deleteBlock(redRect);
            xred = false;
            m_CurrentBlockRect = 0;
        }        
        if (selecting == StartSelect) {
            selecting = NoSelect;
            m_CurrentBlockRect = 0;
            leftMouseRelease(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
        }
    }
    deskewing = false;
    if (buttonPressed == Qt::RightButton) {
        this->rightMouseRelease(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
    }
    buttonPressed = Qt::NoButton;
}


QGraphicsRectItem *QGraphicsInput::newBlock(QRectF rect)
{
    QPen p(Qt::SolidLine);
    QBrush b(Qt::SolidPattern);
    b.setColor(QColor(0, 0, 127, 127));
    p.setWidth(2);
    p.setColor(QColor(0, 0, 255));
    QGraphicsRectItem *res;
    if (rect.width() + rect.x() > m_image->boundingRect().width())
        rect.setWidth(m_image->boundingRect().width() - rect.x()-1);
    if (rect.x() < 0)
        rect.setX(0);
    if (rect.height() + rect.y() > m_image->boundingRect().height())
        rect.setHeight(m_image->boundingRect().height() - rect.y()-1);
    if (rect.y() < 0)
        rect.setY(0);
    res = this->addRect(rect, p, b);
    res->setAcceptHoverEvents(true);
    res->setZValue(1);
    res->setData(1, "block");
    res->setData(2, "no");
    return res;
}

bool QGraphicsInput::addBlock(const QRectF &rect, bool removeObstacles)
{
    QGraphicsRectItem *block = newBlock(rect);
    if (!removeObstacles) {
        if (block->collidingItems().size() > 0) {
            deleteBlockRect(block);
            return false;
        }
    } else {
        for (int i = block->collidingItems().size() - 1; i >= 0; i--) {
            if (block->collidingItems().at(i)->data(1) == "block")
                deleteBlockRect((QGraphicsRectItem *) block->collidingItems().at(i));
        }
    }
    m_CurrentBlockRect = block;
    return true;
}

void QGraphicsInput::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    if (deskewing) {
        setDeskLine(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
    }
    if (selecting == StartSelect) {
        selecting = Selecting;
        m_CurrentBlockRect = newBlock(blockRect);
    }

    if ((mouseEvent->modifiers() & Qt::ControlModifier) == 0)
        if (mouseEvent->buttons() == Qt::NoButton) {
            near_res = nearActiveBorder(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
            switch (near_res) {
                case 0:
                    m_view->viewport()->setCursor(Qt::ArrowCursor);
                    break;
                case 1:
                    m_view->viewport()->setCursor(Qt::SplitHCursor);
                    break;
                case 2:
                    m_view->viewport()->setCursor(Qt::SplitVCursor);
                    break;
                case 3:
                    m_view->viewport()->setCursor(Qt::SplitHCursor);
                    break;
                case 4:
                    m_view->viewport()->setCursor(Qt::SplitVCursor);
                    break;
                default:
                    break;
            }

        }
    QRectF newRect;
    if (near_res && (mouseEvent->buttons()&Qt::LeftButton)) {
        if (!xred)
            redRect = QRectF2Rect(m_LastSelected->rect());
        xred = true;
        QRectF newRect = m_LastSelected->mapRectToScene(m_LastSelected->rect());
        switch (near_res) {
            case 1:
                newRect.setLeft(mouseEvent->lastScenePos().x());
                break;
            case 2:
                newRect.setTop(mouseEvent->lastScenePos().y());
                break;
            case 3:
                newRect.setRight(mouseEvent->lastScenePos().x());
                break;
            case 4:
                newRect.setBottom(mouseEvent->lastScenePos().y());
                break;
            default:
                break;
        }
        m_CurrentBlockRect = m_LastSelected;
        m_CurrentBlockRect->setRect(m_LastSelected->mapRectFromScene(newRect));
        for (int i = 0; i < m_CurrentBlockRect->collidingItems().size(); ++i)
            if (m_CurrentBlockRect->collidingItems().at(i)->data(1) == "block") {
                m_CurrentBlockRect->setRect(m_LastSelected->mapRectFromScene(selBlockRect));
                return;
            }
        selBlockRect = newRect;
        return;
    }
    if (selecting == Selecting) {
        newRect = blockRect;
        if (newRect.left() < mouseEvent->lastScenePos().x())
            newRect.setRight(mouseEvent->lastScenePos().x());
        else
            newRect.setLeft(mouseEvent->lastScenePos().x());
        if (newRect.top() < mouseEvent->lastScenePos().y())
            newRect.setBottom(mouseEvent->lastScenePos().y());
        else
            newRect.setTop(mouseEvent->lastScenePos().y());
        m_CurrentBlockRect->setRect(newRect);
        for (int i = 0; i < m_CurrentBlockRect->collidingItems().size(); ++i)
            if (m_CurrentBlockRect->collidingItems().at(i)->data(1) == "block") {
                m_CurrentBlockRect->setRect(blockRect);
                return;
            }
        blockRect = newRect;
        return;
    }

}

void QGraphicsInput::leftMouseRelease(qreal x, qreal y)
{
    QGraphicsItem *it = this->itemAt(x, y);
    if (it) {
        if (it->data(1).toString() == "block") {
            QGraphicsRectItem *r = (QGraphicsRectItem *) it;
            QPen p(Qt::SolidLine);
            QBrush b(Qt::SolidPattern);
            b.setColor(QColor(0, 0, 127, 127));
            p.setColor(QColor(0, 0, 255));
            p.setWidth(2);
            if (r->data(2).toString() == "no") {
                //select block!!!!
                if (m_LastSelected) {
                    m_LastSelected->setPen(p);
                    m_LastSelected->setBrush(b);
                    m_LastSelected->setData(2, "no");
                }
                b.setColor(QColor(127, 0, 0, 127));
                p.setColor(QColor(255, 0, 0));
                r->setData(2, "yes");
                m_LastSelected = r;
                selBlockRect = m_LastSelected->rect();
                redRect = QRectF2Rect(selBlockRect); // ATT

//                emit addBlock(QRectF2Rect(selBlockRect));
            } else {
                m_LastSelected = 0;
                r->setData(2, "no");
            }
            r->setPen(p);
            r->setBrush(b);
            //         m_CurrentBlockRect = r;
        }
    } else
        m_CurrentBlockRect = 0;
    emit leftMouseClicked(m_view->mapFromScene(x, y).x(), m_view->mapFromScene(x, y).y(), m_CurrentBlockRect != 0);
}


void QGraphicsInput::rightMouseRelease(qreal x, qreal y)
{
    QGraphicsItem *it = this->itemAt(x, y);
    if (it) {
        if (it->data(1).toString() == "block") {
            m_CurrentBlockRect = (QGraphicsRectItem *) it;
        }
    } else
        m_CurrentBlockRect = 0;
    emit rightMouseClicked(m_view->mapFromScene(x, y).x(), m_view->mapFromScene(x, y).y(), m_CurrentBlockRect != 0);
}


int QGraphicsInput::nearActiveBorder(qreal x, qreal y)
{
    if (m_LastSelected == 0)
        return 0;
    x = m_LastSelected->mapFromScene(x, y).x();
    y = m_LastSelected->mapFromScene(x, y).y();
    qreal xcenter = m_LastSelected->rect().center().x();
    qreal ycenter = m_LastSelected->rect().center().y();
    qreal xcd = abs(m_LastSelected->rect().right() - xcenter) + 8;
    qreal ycd = abs(m_LastSelected->rect().bottom() - ycenter) + 8;
    if ((abs(x - m_LastSelected->rect().left()) <= 4)) {
        if (abs(y - ycenter) < ycd)
            return 1;
        else
            return 0;
    }
    if ((abs(m_LastSelected->rect().top() - y) <= 4)) {
        if (abs(x - xcenter) < xcd)
            return 2;
        else
            return 0;
    }
    if ((abs(x - m_LastSelected->rect().right()) <= 4)) {
        if (abs(y - ycenter) < ycd)
            return 3;
        else
            return 0;
    }
    if ((abs(m_LastSelected->rect().bottom() - y) <= 4)) {
        if (abs(x - xcenter) < xcd)
            return 4;
        else
            return 0;
    }
    return 0;
}

QRect QGraphicsInput::getActiveBlock()
{
    if (m_LastSelected)
        return QRectF2Rect(m_LastSelected->rect());
    return QRectF2Rect(QRect(0,0,0,0));
}

QRect QGraphicsInput::getCurrentBlock()
{
    if (m_CurrentBlockRect) {
        int x =m_CurrentBlockRect->rect().x();
        int y = m_CurrentBlockRect->rect().y();
        int w = m_CurrentBlockRect->rect().width();
        int h = m_CurrentBlockRect->rect().height();
        bool reframe = false;
        if (x < 0) {
            x = 0;
            reframe = true;
        }
        if (y < 0) {
            y = 0;
            reframe = true;
        }
        if (x >= m_image->boundingRect().width()) {
            x = 0;
            reframe = true;
        }
        if (y >= m_image->boundingRect().height()) {
            y = 0;
            reframe = true;
        }
        if (w+x >= m_image->boundingRect().width()) {
            w = m_image->boundingRect().width() - x -1;
            reframe = true;
        }
        if (w+x < 0) {
            w = -x + 1;
            reframe = true;
        }
        if (h+y >= m_image->boundingRect().height()) {
            w = m_image->boundingRect().height() - y - 1;
            reframe = true;
        }
        if (w+y < 0) {
            w = -y + 1;
            reframe = true;
        }
        if (reframe) {
            return QRect(x, y, w, h);
        }

        return QRectF2Rect(m_CurrentBlockRect->rect());
    }
    return QRectF2Rect(QRect(0,0,0,0));
}

void QGraphicsInput::deleteCurrentBlock()
{
    if (m_CurrentBlockRect != 0)
        deleteBlockRect(m_CurrentBlockRect);
}

void QGraphicsInput::deleteActiveBlock()
{
    if (m_LastSelected)
        deleteBlockRect(m_LastSelected);
    m_LastSelected = 0;
}

void QGraphicsInput::deleteBlock(int index)
{
    int count = 0;
    for (int i = 0; i < items().count(); i++) {
        if (items().at(i)->data(1) == "block") {
            if (index == count) {
                deleteBlockRect((QGraphicsRectItem *)items().at(i));
                return;
            }
            count++;
        }
    }
}

void QGraphicsInput::clearBlocks() // KEEP
{
    for (int i = items().count() - 1; i >= 0; i--) {
        if (items().at(i)->data(1) == "block") {
            deleteBlockRect((QGraphicsRectItem *)items().at(i));
        }
    }
}

void QGraphicsInput::setMagnifierCursor(QCursor *cursor)
{
    delete magnifierCursor;
    magnifierCursor = new QCursor(cursor->pixmap());
}

void QGraphicsInput::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    if (wheelEvent->modifiers() == Qt::ControlModifier) {
        int delta = wheelEvent->delta();
        qreal coeff = delta < 0 ? 1 / (1 - delta / (360.)) : 1 + delta / (240.);
        if (coeff >= 1)
            emit increaseMe();
        else
            emit decreaseMe();
        wheelEvent->accept();
        m_view->viewport()->setCursor(*magnifierCursor);
    } else
        QGraphicsScene::wheelEvent(wheelEvent);
}

void QGraphicsInput::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Control)
        m_view->viewport()->setCursor(Qt::ArrowCursor);
    if (keyEvent->modifiers() & Qt::ControlModifier) {
        if ((keyEvent->key() == Qt::Key_Plus) || (keyEvent->key() == Qt::Key_Equal)) {
            emit increaseMe();
            return;
        }
        if ((keyEvent->key() == Qt::Key_Minus) || (keyEvent->key() == Qt::Key_Underscore)) {
            emit decreaseMe();
            return;
        }

    }
    if (keyEvent->key() > Qt::Key_F1) {
        emit keyPressed((int)keyEvent->key());
    }
}

void QGraphicsInput::clearTransform()
{
    if (m_view) {
        QTransform tr = m_view->transform();
        tr.reset();
        m_view->setTransform(tr);
    }
}

void QGraphicsInput::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Control) {
        m_view->viewport()->setCursor(*magnifierCursor);
        //QApplication::
    }

}

void QGraphicsInput::drawLine(int x1, int y1, int x2, int y2)
{

    QPen pen(QColor("red"));
    pen.setWidth(2);
    this->addLine(x1, y1, x2, y2, pen);
}

void QGraphicsInput::imageOrigin(QPoint &p)
{
    p.setX(m_image->mapToScene(0,0).x());
    p.setY(m_image->mapToScene(0,0).y());
}

QPixmap QGraphicsInput::getCurrentImage()
{
    return (m_image->pixmap());
}

void QGraphicsInput::addBlockColliding(Block block)
{
    QGraphicsRectItem *gi = newBlock(block);
    m_CurrentBlockRect = gi;
    QGraphicsTextItem *gte = new QGraphicsTextItem(QString::number(block.blockNumber()), gi);
    gte->setFont(QFont("Arial", 16));
    gte->setDefaultTextColor(QColor("white"));
    gte->moveBy(block.x(), block.y());
}


