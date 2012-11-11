/**
 * \file CSpellChecker.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
 *
 * This file is part of InyokaEdit.
 *
 * InyokaEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * InyokaEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Class definition for spell checker using hunspell.
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#ifndef INYOKAEDIT_CSPELLCHECKER_H_
#define INYOKAEDIT_CSPELLCHECKER_H_

#include <QString>

#include "./CSpellCheckDialog.h"
#include "./CTextEditor.h"

#if defined _WIN32
    #include "../windows_files/hunspell-mingw/include/hunspell.hxx"
#else
    #include <hunspell/hunspell.hxx>
#endif

/**
 * \class CSpellChecker
 * \brief Spell checker using hunspell.
 */
class CSpellChecker {
  public:
    CSpellChecker(const QString &dictionaryPath, const QString &userDictionary,
                  QWidget *pParent);
    ~CSpellChecker();

    void start(CTextEditor *pEdito);
    bool spell(const QString &sWord);
    QStringList suggest(const QString &sWord);
    void ignoreWord(const QString &sWord);
    void addToUserWordlist(const QString &sWord);

  private:
    void put_word(const QString &sWord);
    Hunspell *_hunspell;
    QString _userDictionary;
    QString _encoding;
    QTextCodec *_codec;
    CSpellCheckDialog *m_pCheckDialog;
};

#endif  // INYOKAEDIT_CSPELLCHECKER_H_
