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

#include "globallock.h"
#include <QAtomicInt>
#include <QApplication>

GlobalLock* GlobalLock::m_instance = nullptr;

static QAtomicInt state;

GlobalLock* GlobalLock::instance()
{
    if (!m_instance)
    {
        m_instance = new GlobalLock();
    }
    return m_instance;
}

bool GlobalLock::lock()
{
    return state.testAndSetOrdered(0, 1);
}

void GlobalLock::unlock()
{
    state.fetchAndStoreOrdered(0);
}

bool GlobalLock::isLocked()
{
    return state == 1;
}

GlobalLock::GlobalLock()
{
}

GlobalLock::GlobalLock(const GlobalLock&)
{
}

Unlocker::Unlocker(bool locked) : unlock(locked)
{
}

Unlocker::~Unlocker()
{
    if (unlock)
    {
        GlobalLock::instance()->unlock();
    }
}
