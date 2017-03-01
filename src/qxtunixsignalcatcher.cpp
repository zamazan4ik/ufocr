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

#include "qxtunixsignalcatcher.h"
#include "qxtunixscinternal.h"
#include <stdlib.h>
#include <signal.h>
#include <QMutex>


QXtUnixSignalCatcher *QXtUnixSignalCatcher::sc = NULL;

QMutex *QXtUnixSignalCatcher::mutex = NULL;

QXtUnixSignalCatcher::QXtUnixSignalCatcher(QObject *parent) :
    QObject(parent)
{
    mutex = new QMutex();
    sci = new QXtUnixSignalCatcherInternal(0);
    connect(sci, SIGNAL(unixSignalInternal(int)), this, SLOT(doEmit(int)), Qt::QueuedConnection);
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGHUP);
    sigaddset(&newset, SIGTERM);
    sigaddset(&newset, SIGINT);
    sigaddset(&newset, SIGUSR1);
    sigaddset(&newset, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &newset, 0);
}

QXtUnixSignalCatcher::QXtUnixSignalCatcher() : QObject(0)
{
}

QXtUnixSignalCatcher::QXtUnixSignalCatcher(QXtUnixSignalCatcher &) : QObject(0)
{
}

QXtUnixSignalCatcher::~QXtUnixSignalCatcher()
{
}

void QXtUnixSignalCatcher::signalHandler(int sig_num)
{
    mutex->lock();
    sc->emitSignal(sig_num);
    mutex->unlock();
}

void QXtUnixSignalCatcher::emitSignal(int sig_num)
{
    sci->emitSignal(sig_num);
}

bool QXtUnixSignalCatcher::connectUnixSignalInternal(int sig_num)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signalHandler;
    return (sigaction(sig_num, &sa, 0) == 0);
}

bool QXtUnixSignalCatcher::connectUnixSignal(int sig_num)
{
    catcher();
    return sc->connectUnixSignalInternal(sig_num);
}

void QXtUnixSignalCatcher::disconnectUnixSugnals()
{
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGHUP);
    sigaddset(&newset, SIGTERM);
    sigaddset(&newset, SIGINT);
    sigaddset(&newset, SIGUSR1);
    sigaddset(&newset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &newset, 0);
    delete sci;
    sci = NULL;
    delete mutex;
    mutex = NULL;
}

void QXtUnixSignalCatcher::doEmit(int sig_num)
{
    emit unixSignal(sig_num);
}

QXtUnixSignalCatcher *QXtUnixSignalCatcher::catcher()
{
    if (!sc)
        sc = new QXtUnixSignalCatcher(0);
    return sc;
}
