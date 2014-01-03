/**
 * \file CSpellCheckDialog.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2014 The InyokaEdit developers
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
 * Class definition spell checker dialog.
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#ifndef INYOKAEDIT_CSPELLCHECKDIALOG_H_
#define INYOKAEDIT_CSPELLCHECKDIALOG_H_

#include <QDialog>

class CSpellChecker;

namespace Ui {
    class CSpellCheckDialog;
}

/**
 * \class CSpellCheckDialog
 * \brief Spell checker dialog.
 */
class CSpellCheckDialog : public QDialog {
    Q_OBJECT

  public:
    enum SpellCheckAction {
        AbortCheck, IgnoreOnce, IgnoreAll, ReplaceOnce, ReplaceAll, AddToDict
    };

    explicit CSpellCheckDialog(CSpellChecker *pSpellChecker,
                               QWidget *pParent = 0);
    ~CSpellCheckDialog();

    QString replacement() const;

  public slots:
    SpellCheckAction checkWord(const QString &sWord);

  protected slots:
    void ignoreOnce();
    void ignoreAll();
    void replaceOnce();
    void replaceAll();
    void addToDict();

  private:
    Ui::CSpellCheckDialog *m_pUi;
    CSpellChecker *m_pSpellChecker;
    QString m_sUnkownWord;
    SpellCheckAction m_returnCode;
};

#endif  // INYOKAEDIT_CSPELLCHECKDIALOG_H_
