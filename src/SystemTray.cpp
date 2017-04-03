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

#include "SystemTray.hpp"
#include <QtWidgets>
//#include "MainWindow.h"

// ----------------------------------------------------------------------
SystemTray::SystemTray(QWidget* pwgt /*=0*/) : QWidget(pwgt)
{
    setWindowTitle(tr("System Tray"));

    QAction* pactShowHide =
            new QAction(tr("&Show/Hide Application Window"), this);

    connect(pactShowHide, SIGNAL(triggered()),
            this,         SLOT(slotShowHide())
    );

    QAction* pactQuit = new QAction(tr("&Quit"), this);
    connect(pactQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    m_ptrayIconMenu = new QMenu(this);
    m_ptrayIconMenu->addAction(pactShowHide);
    m_ptrayIconMenu->addAction(pactQuit);

    m_ptrayIcon = new QSystemTrayIcon(this);
    m_ptrayIcon->setContextMenu(m_ptrayIconMenu);
    m_ptrayIcon->setToolTip(QCoreApplication::applicationName());

    m_ptrayIcon->setIcon(QPixmap(":/yagf.png"));

    m_ptrayIcon->show();
}

// ----------------------------------------------------------------------
/*virtual*/void SystemTray::closeEvent(QCloseEvent*)
{
    if (m_ptrayIcon->isVisible())
    {
        this->parentWidget()->hide();
    }
}

// ----------------------------------------------------------------------
void SystemTray::slotShowHide()
{
    this->parentWidget()->setVisible(!isVisible());
}
