#include "globallock.h"
#include <QAtomicInt>
#include <QApplication>

GlobalLock * GlobalLock::m_instance = NULL;

static QAtomicInt state;

GlobalLock *GlobalLock::instance()
{
    if (!m_instance)
        m_instance = new GlobalLock();
    return m_instance;
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

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

GlobalLock::GlobalLock(const GlobalLock &)
{
}

Unlocker::Unlocker(bool locked): unlock(locked)
{
}

Unlocker::~Unlocker()
{
    if (unlock) GlobalLock::instance()->unlock();
}
