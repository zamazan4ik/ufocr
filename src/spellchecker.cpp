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

#include <QTextEdit>
#include <QRegExp>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QMap>
#include <QMessageBox>
#include <QStringList>
#include "spellchecker.h"


SpellChecker::SpellChecker(QTextEdit *textEdit): m_textEdit(textEdit)
{
    m_regExp = new QRegExp("[^\\s]*");
    //m_cursor= new QTextCursor(m_textEdit->document());
    spell_config1 = new_aspell_config();
    spell_config2 = new_aspell_config();
    aspell_config_replace(spell_config1, "lang", m_lang1.toAscii());
    aspell_config_replace(spell_config2, "lang", m_lang2.toAscii());
    aspell_config_replace(spell_config1, "encoding", "utf-8");
    aspell_config_replace(spell_config2, "encoding", "utf-8");
    m_map = new StringMap();
    m_map->insert("ara", "ar");
    m_map->insert("ruseng", "ru");
    m_map->insert("rus", "ru");
    m_map->insert("bul", "bg");
    m_map->insert("cze", "cs");
    m_map->insert("dan", "da");
    m_map->insert("dut", "nl");
    m_map->insert("nld", "nl");
    m_map->insert("ell", "el");
    m_map->insert("eng", "en");
    m_map->insert("est", "et");
    m_map->insert("fin", "fi");
    m_map->insert("fra", "fr");
    m_map->insert("ger", "de");
    m_map->insert("deu", "de");
    m_map->insert("deu-frak", "de-alt");
    m_map->insert("heb", "he");
    m_map->insert("hin", "hi");
    m_map->insert("hrv", "hr");
    m_map->insert("hun", "hu");
    m_map->insert("ind", "in");
    m_map->insert("isl", "is");
    m_map->insert("ita", "it");
    m_map->insert("lav", "lv");
    m_map->insert("lit", "lt");
    m_map->insert("mkd", "mk");
    m_map->insert("msa", "ms");
    m_map->insert("nor", "no");
    m_map->insert("pol", "pl");
    m_map->insert("por", "pt_PT");
    m_map->insert("rum", "ro");
    //m_map->insert("rus", "ru");
    m_map->insert("ron", "ro");
    m_map->insert("slo", "sl");
    m_map->insert("slk", "sk");
    m_map->insert("spa", "es");
    m_map->insert("srp", "sr");
    m_map->insert("swa", "sw");
    m_map->insert("swe", "sv");
    m_map->insert("swef", "sv");
    m_map->insert("tur", "tr");
    m_map->insert("ukr", "uk");
    m_map->insert("vie", "vi");
    spell_checker1 = 0;
    spell_checker2 = 0;
    setLanguage("ruseng");
    dictList = new QStringList();
}


void SpellChecker::enumerateDicts()
{
    AspellConfig *config;
    AspellDictInfoList *dlist;
    AspellDictInfoEnumeration *dels;
    const AspellDictInfo *entry;

    config = new_aspell_config();

    /* the returned pointer should _not_ need to be deleted */
    dlist = get_aspell_dict_info_list(config);

    /* config is no longer needed */
    delete_aspell_config(config);

    dels = aspell_dict_info_list_elements(dlist);

    while ((entry = aspell_dict_info_enumeration_next(dels)) != 0) {
        dictList->append(entry->code);
    }

    delete_aspell_dict_info_enumeration(dels);

}

bool SpellChecker::hasDict(const QString &shname)
{
    if (shname.contains("+")) {
        return dictList->contains(m_map->value(shname.split("+")[0]));
    }
    return dictList->contains(m_map->value(shname));
}

SpellChecker::~SpellChecker()
{
    delete m_regExp;
    delete m_map;

    delete_aspell_speller(spell_checker1);
    delete_aspell_speller(spell_checker2);

    delete_aspell_config(spell_config1);
    delete_aspell_config(spell_config2);
    delete dictList;
}

void SpellChecker::setLanguage(const QString &lang)
{
    if (lang.isEmpty()) return;
    delete_aspell_speller(spell_checker1);
    delete_aspell_speller(spell_checker2);
    bad_language.clear();

    m_lang2 = "en";
    m_lang1 = m_map->value(lang, QString("en"));
    if (lang.contains("+")) {
        QStringList sl = lang.split("+");
        m_lang1 = m_map->value(sl[0], QString("en"));
        m_lang2 = m_map->value(sl[1], QString("en"));
        if ((m_lang1 == "deu")||(lang == "ger")) {
            m_lang1 = "de_DE";
        }
        if ((m_lang2 == "deu")||(lang == "ger")) {
            m_lang2 = "de_DE";
        }
    }
    if (lang == "rus_fra") {
        m_lang1 = "ru";
        m_lang2 = "fr";
    } else if (lang == "rus_ger") {
        m_lang1 = "ru";
        m_lang2 = "de";
    } else if (lang == "rus_spa") {
        m_lang1 = "ru";
        m_lang2 = "es";
    }
    if ((lang == "deu")||(lang == "ger")) {
        m_lang1 = "de_DE";
        m_lang2 = "de_AT";

    }
    if (lang == "ruseng") {
        m_lang1 = "ru";
        m_lang2 = "en";

    }
    aspell_config_replace(spell_config1, "lang", m_lang1.toAscii());
    aspell_config_replace(spell_config2, "lang", m_lang2.toAscii());
    AspellCanHaveError *possible_err = new_aspell_speller(spell_config1);
    spell_checker1 = 0;
    if (aspell_error_number(possible_err) == 0)
        spell_checker1 = to_aspell_speller(possible_err);
    else
        delete_aspell_can_have_error(possible_err);
    possible_err = new_aspell_speller(spell_config2);
    spell_checker2 = 0;
    if (aspell_error_number(possible_err) == 0)
        spell_checker2 = to_aspell_speller(possible_err);
    else
        delete_aspell_can_have_error(possible_err);

    // Check absent dictionary
    if (spell_checker1 == 0)
        bad_language = m_lang1;
    if (spell_checker2 == 0)
        bad_language = m_lang2;

}

bool SpellChecker::spellCheck()
{
    if ((spell_checker1 == 0) && (spell_checker2 == 0)) {
        QPixmap icon;
        icon.load(":/warning.png");
        QMessageBox messageBox(QMessageBox::NoIcon, "YAGF", QObject::trUtf8("Required spelling dictionary (%1) is not found.\nSpell-checking is disabled.\nTry to install an appropriate aspell dictionary.").arg(bad_language),
                               QMessageBox::Ok, 0);
        messageBox.setIconPixmap(icon);
        messageBox.exec();
        return false;
    }
    QTextCursor cursor(m_textEdit->document());
    while (!cursor.isNull() && !cursor.atEnd()) {
        if (hasLongHyphen(&cursor)) {
            cursor.select(QTextCursor::WordUnderCursor);
            QString word1 = cursor.selectedText();
            word1.truncate(word1.length()-1);
            cursor.movePosition(QTextCursor::NextWord);
            cursor.select(QTextCursor::WordUnderCursor);
            QString word2 = cursor.selectedText();
            word1 = word1 +word2;
            if (checkWordSpelling(word1)) {
                cursor.movePosition(QTextCursor::PreviousWord);
                cursor.select(QTextCursor::WordUnderCursor);
                cursor.removeSelectedText();
                cursor.select(QTextCursor::WordUnderCursor);
                cursor.removeSelectedText();
                cursor.insertText(word1);
            }
        }
        cursor.select(QTextCursor::WordUnderCursor);
        QString word = cursor.selectedText();
        if (word == QString::fromUtf8("вЂ”")) {
            cursor.removeSelectedText();
            cursor.insertText(QString::fromUtf8("—"));
        }
        if (hasHyphen(&cursor)) {
            QString cc = checkConcatenation(&cursor);
            if (!cc.isEmpty()) {
                cursor.movePosition(QTextCursor::PreviousWord);
                cursor.movePosition(QTextCursor::PreviousWord);
                cursor.select(QTextCursor::WordUnderCursor);
                cursor.removeSelectedText();
                cursor.select(QTextCursor::WordUnderCursor);
                cursor.removeSelectedText();
                cursor.movePosition(QTextCursor::NextWord);
                cursor.select(QTextCursor::WordUnderCursor);
                cursor.removeSelectedText();
                cursor.insertText(cc);
            }
        }
        _checkWord(&cursor);
        QTextCursor oldc = cursor;
        if (!cursor.movePosition(QTextCursor::NextWord,
                                 QTextCursor::MoveAnchor))
            break;
//cursor.movePosition(QTextCursor::EndOfWord,  QTextCursor::MoveAnchor);

        //cursor = m_textEdit->document()->find(*m_regExp, cursor);
        int oldpos = oldc.position();
        int newpos = cursor.position();
        if (abs(newpos - oldpos) < 3)
            cursor.setPosition(newpos + 1);
    }
    if (!cursor.isNull())
        _checkWord(&cursor);
    return true;
}

void SpellChecker::unSpellCheck()
{
    QTextCursor cursor(m_textEdit->document());
    QTextCharFormat fmt = cursor.charFormat();
    fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(fmt);
    cursor.clearSelection();
}

void SpellChecker::_checkWord(QTextCursor *cursor)
{
    cursor->select(QTextCursor::WordUnderCursor);
    QString selText = cursor->selectedText();
    static const QRegExp nonDigits("\\D");
    if (!selText.contains(nonDigits))
        return;
    selText = selText.remove(QString::fromUtf8("«"));
    selText = selText.remove(QString::fromUtf8("»"));
    //selText = selText.remove("\"");
    selText = selText.remove(QString::fromUtf8("("));
    selText = selText.remove(QString::fromUtf8(")"));

    if (!checkWordSpelling(selText)) {
        QTextCharFormat fmt = cursor->charFormat();
        fmt.setUnderlineColor(QColor(Qt::red));
        fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
        cursor->setCharFormat(fmt);
    } else {
        QTextCharFormat fmt = cursor->charFormat();
        fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
        cursor->setCharFormat(fmt);
    }
    cursor->clearSelection();
}

bool SpellChecker::checkWordSpelling(const QString &word)
{
    QString tmp = word;
    tmp = tmp.remove(QString::fromUtf8("»"));
    tmp = tmp.remove(QString::fromUtf8("«"));

    QByteArray ba = tmp.toUtf8();
    return (aspell_speller_check(spell_checker1, ba.data(), ba.size()) != 0) ||
           (aspell_speller_check((spell_checker2 != NULL ? spell_checker2 : spell_checker1), ba.data(), ba.size()) != 0);
}

void SpellChecker::checkWord()
{
    if ((spell_checker1 == 0) && (spell_checker2 == 0))
        return;
    QTextCursor cursor = m_textEdit->textCursor();
    _checkWord(&cursor);
}

bool SpellChecker::hasHyphen(QTextCursor *cursor)
{
    if ((spell_checker1 == 0) || (spell_checker2 == 0))
        return false;
    cursor->movePosition(QTextCursor::EndOfWord);
    //cursor->movePosition(QTextCursor::NextCharacter);
    cursor->select(QTextCursor::WordUnderCursor);
    QString selText = cursor->selectedText();
    cursor->movePosition(QTextCursor::PreviousWord);
    if (selText.endsWith(QString::fromUtf8("-")))
        return true;
    return false;
}

bool SpellChecker::hasLongHyphen(QTextCursor *cursor)
{
    cursor->select(QTextCursor::WordUnderCursor);
    QString selText = cursor->selectedText();
    return selText.endsWith(QString::fromUtf8("—"));
}

QString SpellChecker::checkConcatenation(QTextCursor *cursor)
{
    cursor->movePosition(QTextCursor::PreviousWord);
    cursor->select(QTextCursor::WordUnderCursor);
    QString word1 = cursor->selectedText();
    cursor->movePosition(QTextCursor::NextWord);
    //cursor->movePosition(QTextCursor::NextWord);
    cursor->select(QTextCursor::WordUnderCursor);
    QString word2 = cursor->selectedText();
    cursor->movePosition(QTextCursor::PreviousWord);
    QString word = word1+word2;
    if (checkWordSpelling(word))
        return word;
    return "";
}

QStringList SpellChecker::suggestions()
{
    QStringList sl;
    if ((spell_checker1 == 0) || (spell_checker2 == 0))
        return sl;
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString word = cursor.selectedText();
    QByteArray ba = word.toUtf8();
    if ((aspell_speller_check(spell_checker2, ba.data(), ba.size()) != 0)||(aspell_speller_check(spell_checker1, ba.data(), ba.size()) != 0))
        return sl;
    const struct AspellWordList *awl = aspell_speller_suggest(spell_checker1, ba.data(), ba.size());
    if (aspell_word_list_size(awl) > 0) {
        struct AspellStringEnumeration *ase = aspell_word_list_elements(awl);
        int i  = 0;
        while ((!aspell_string_enumeration_at_end(ase))&&(i < 10)) {
            const char *text = aspell_string_enumeration_next(ase);
            sl << QString::fromUtf8(text);
            i++;
        }
        delete_aspell_string_enumeration(ase);
    }
    return sl;
}
