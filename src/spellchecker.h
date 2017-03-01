/*
    YAGF - cuneiform and tesseract OCR graphical front-end
    Copyright (C) 2009-2012 Andrei Borovsky <anb@symmetrica.net>

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

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QString>
#include <QMap>
#include <aspell.h>

typedef QMap<QString, QString> StringMap;

class QTextEdit;
class QRegExp;
class QTextCursor;
class QStringList;

class SpellChecker
{
public:
    SpellChecker(QTextEdit *textEdit);
    ~SpellChecker();
    void unSpellCheck();
    void setLanguage(const QString &lang);
    bool spellCheck();  //Returns false only if the dictionary not found. Otherwise always true.
    void checkWord();
    bool hasHyphen(QTextCursor *cursor);
    bool hasLongHyphen(QTextCursor *cursor);
    QString checkConcatenation(QTextCursor *cursor);
    void enumerateDicts();
    bool hasDict(const QString &shname);
    QStringList suggestions();
private:
    void _checkWord(QTextCursor *cursor);
    bool checkWordSpelling(const QString &word);
    QTextEdit *m_textEdit;
    QRegExp *m_regExp;
    QString m_lang1;
    QString m_lang2;
    StringMap *m_map;
    AspellConfig *spell_config1;
    AspellConfig *spell_config2;
    AspellSpeller *spell_checker1;
    AspellSpeller *spell_checker2;
    QStringList *dictList;
    QString bad_language;
};

#endif // SPELLCHECKER_H
