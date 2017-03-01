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

#include "recognizerwrapper.h"
#include "recognizerwrapperinternal.h"

RecognizerWrapper::RecognizerWrapper(QObject *parent) :
    QObject(parent)
{
    rwi = new RecognizerWrapperInternal(this);
    rwi->init();
    connect(rwi, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
    connect(rwi, SIGNAL(finished(int)), this, SIGNAL(finished(int)));
    connect(rwi, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError()));
    connect(rwi, SIGNAL(blockRecognized(int)), this, SIGNAL(blockRecognized(int)));
    connect(rwi, SIGNAL(readOutput(QString, QChar)), this, SIGNAL(readOutput(QString, QChar)));
}

RecognizerWrapper::~RecognizerWrapper()
{
    delete rwi;
}

bool RecognizerWrapper::findEngine(bool selected)
{
    return RecognizerWrapperInternal::findEngine(selected);
}

void RecognizerWrapper::start()
{
    rwi->recognize();
}

void RecognizerWrapper::startSingleBlock()
{
    rwi->recognizeInternal();
}


void RecognizerWrapper::cancel()
{
    rwi->supressErros = true;
    rwi->proc.kill();
}

void RecognizerWrapper::onError()
{
    if (!rwi->supressErros)
        emit error(trUtf8("Recognizer error."));
}


