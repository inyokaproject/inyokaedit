/**
 * \file spellcheckdialog.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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

#ifndef PLUGINS_SPELLCHECKER_SPELLCHECKDIALOG_H_
#define PLUGINS_SPELLCHECKER_SPELLCHECKDIALOG_H_

#include <QDialog>

class SpellChecker;

namespace Ui {
class SpellCheckDialog;
}

/**
 * \class SpellCheckDialog
 * \brief Spell checker dialog.
 */
class SpellCheckDialog : public QDialog {
  Q_OBJECT

 public:
    enum SpellCheckAction {
      None, AbortCheck, IgnoreOnce, IgnoreAll,
      ReplaceOnce, ReplaceAll, AddToDict
    };

    explicit SpellCheckDialog(SpellChecker *pSpellChecker,
                              QWidget *pParent = nullptr);
    ~SpellCheckDialog();

    QString replacement() const;

 public slots:
    SpellCheckDialog::SpellCheckAction checkWord(const QString &sWord);

 protected slots:
    void ignoreOnce();
    void ignoreAll();
    void replaceOnce();
    void replaceAll();
    void addToDict();

 private slots:
    void changeLanguage(const QString &sLanguage);
    void closeDialog();

 private:
    Ui::SpellCheckDialog *m_pUi;
    SpellChecker *m_pSpellChecker;
    QString m_sUnkownWord;
    SpellCheckAction m_returnCode;
};

#endif  // PLUGINS_SPELLCHECKER_SPELLCHECKDIALOG_H_
