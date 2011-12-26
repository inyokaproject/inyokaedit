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
* File Name:  CSpellChecker.h
* Purpose:    Class definition spell checker using hunspell
***************************************************************************/

// Original code form http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell

#ifndef CSPELLCHECKER_H
#define CSPELLCHECKER_H

#include <QString>

class Hunspell;

class CSpellChecker
{
public:
    CSpellChecker(const QString &dictionaryPath, const QString &userDictionary);
    ~CSpellChecker();

    bool spell(const QString &word);
    QStringList suggest(const QString &word);
    void ignoreWord(const QString &word);
    void addToUserWordlist(const QString &word);

private:
    void put_word(const QString &word);
    Hunspell *_hunspell;
    QString _userDictionary;
    QString _encoding;
    QTextCodec *_codec;
};

#endif // CSPELLCHECKER_H
