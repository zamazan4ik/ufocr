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

#ifndef QXTUNIXSIGNALCTACHER_H
#define QXTUNIXSIGNALCTACHER_H

#include <QObject>

class QMutex;

class QXtUnixSignalCatcherInternal;

class QXtUnixSignalCatcher : public QObject
{
    Q_OBJECT
public:
    static QXtUnixSignalCatcher *catcher();
    static bool connectUnixSignal(int sig_num);
    void disconnectUnixSugnals();
signals:
    void unixSignal(int sig_num);
private slots:
    void doEmit(int sig_num);

private:
    QXtUnixSignalCatcher(QObject *parent = 0);
    QXtUnixSignalCatcher();
    QXtUnixSignalCatcher(QXtUnixSignalCatcher &);
    ~QXtUnixSignalCatcher();
    static void signalHandler(int sig_num);
    void emitSignal(int sig_num);
    bool connectUnixSignalInternal(int sig_num);
private:
    static QXtUnixSignalCatcher *sc;
    static QMutex *mutex;
    QXtUnixSignalCatcherInternal *sci;
};

#endif // QXTUNIXSIGNALCTACHER_H


