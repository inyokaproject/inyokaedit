/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CSpellChecker.cpp
* Purpose:    Spell checker (using hunspell)
***************************************************************************/

// Original code form http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell

#include "CSpellChecker.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>

#include <hunspell/hunspell.hxx>

CSpellChecker::CSpellChecker(const QString &dictionaryPath, const QString &userDictionary)
{
    _userDictionary = userDictionary;

    QString dictFile = dictionaryPath + ".dic";
    QString affixFile = dictionaryPath + ".aff";
    QByteArray dictFilePathBA = dictFile.toLocal8Bit();
    QByteArray affixFilePathBA = affixFile.toLocal8Bit();
    _hunspell = new Hunspell(affixFilePathBA.constData(), dictFilePathBA.constData());

    // Detect encoding analyzing the SET option in the affix file
    _encoding = "ISO8859-1";
    QFile _affixFile(affixFile);
    if (_affixFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&_affixFile);
        QRegExp enc_detector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*", Qt::CaseInsensitive);
        for(QString line = stream.readLine(); !line.isEmpty(); line = stream.readLine()) {
            if (enc_detector.indexIn(line) > -1) {
                _encoding = enc_detector.cap(1);
                //qDebug() << QString("Encoding set to ") + _encoding;
                break;
            }
        }
        _affixFile.close();
    }
    _codec = QTextCodec::codecForName(this->_encoding.toLatin1().constData());

    if(!_userDictionary.isEmpty()) {
        QFile userDictonaryFile(_userDictionary);
        if(userDictonaryFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&userDictonaryFile);
            for(QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine())
                put_word(word);
            userDictonaryFile.close();
        } else {
            QMessageBox::warning(0, "Spell checker", "User dictionary in " + _userDictionary + " could not be opened.");
            //qWarning() << "User dictionary in " << _userDictionary << "could not be opened";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}


CSpellChecker::~CSpellChecker()
{
    delete _hunspell;
}


bool CSpellChecker::spell(const QString &word)
{
    // Encode from Unicode to the encoding used by current dictionary
    return _hunspell->spell(_codec->fromUnicode(word).constData()) != 0;
}


QStringList CSpellChecker::suggest(const QString &word)
{
    char **suggestWordList;

    // Encode from Unicode to the encoding used by current dictionary
    int numSuggestions = _hunspell->suggest(&suggestWordList, _codec->fromUnicode(word).constData());
    QStringList suggestions;
    for(int i=0; i < numSuggestions; ++i) {
        suggestions << _codec->toUnicode(suggestWordList[i]);
        free(suggestWordList[i]);
    }
    return suggestions;
}


void CSpellChecker::ignoreWord(const QString &word)
{
    put_word(word);
}


void CSpellChecker::put_word(const QString &word)
{
    _hunspell->add(_codec->fromUnicode(word).constData());
}


void CSpellChecker::addToUserWordlist(const QString &word)
{
    put_word(word);
    if(!_userDictionary.isEmpty()) {
        QFile userDictonaryFile(_userDictionary);
        if(userDictonaryFile.open(QIODevice::Append)) {
            QTextStream stream(&userDictonaryFile);
            stream << word << "\n";
            userDictonaryFile.close();
        } else {
            QMessageBox::warning(0, "Spell checker", "User dictionary in " + _userDictionary + " could not be opened for appending a new word.");
            //qWarning() << "User dictionary in " << _userDictionary << "could not be opened for appending a new word";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}
