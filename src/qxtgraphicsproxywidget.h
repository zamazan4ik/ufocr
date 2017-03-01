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

#ifndef QXTGRAPHICSPROXYWIDGET_H
#define QXTGRAPHICSPROXYWIDGET_H

#include <QGraphicsProxyWidget>

class QXtGraphicsView;

class QXtGraphicsProxyWidget : public QGraphicsProxyWidget
{
    Q_OBJECT
public:

    QXtGraphicsProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    void setView(QXtGraphicsView *view);

public slots:

    void viewScrolled();

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QXtGraphicsView *mview;
};


#endif // QXTGRAPHICSPROXYWIDGET_H
