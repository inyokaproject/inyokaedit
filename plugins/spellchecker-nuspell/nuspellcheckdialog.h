/**
 * \file nuspellcheckdialog.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Class definition spell checker dialog.
 *
 * \section SOURCE
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * Copyright (c) 2011, Volker Götz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell
 */

#ifndef PLUGINS_SPELLCHECKER_NUSPELL_NUSPELLCHECKDIALOG_H_
#define PLUGINS_SPELLCHECKER_NUSPELL_NUSPELLCHECKDIALOG_H_

#include <QDialog>

class SpellChecker_Nuspell;

namespace Ui {
class NuspellCheckDialog;
}

/**
 * \class NuspellCheckDialog
 * \brief Spell checker dialog.
 */
class NuspellCheckDialog : public QDialog {
  Q_OBJECT

 public:
  enum SpellCheckAction {
    None,
    AbortCheck,
    IgnoreOnce,
    IgnoreAll,
    ReplaceOnce,
    ReplaceAll,
    AddToDict
  };

  explicit NuspellCheckDialog(SpellChecker_Nuspell *pSpellChecker,
                              QWidget *pParent = nullptr);
  ~NuspellCheckDialog();

  auto replacement() const -> QString;

 public slots:
  NuspellCheckDialog::SpellCheckAction checkWord(const QString &sWord);

 protected slots:
  void ignoreOnce();
  void ignoreAll();
  void replaceOnce();
  void replaceAll();
  void addToDict();

 private slots:
  void changeLanguage(int nIndex);
  void closeDialog();

 private:
  Ui::NuspellCheckDialog *m_pUi;
  SpellChecker_Nuspell *m_pSpellChecker;
  QString m_sUnkownWord;
  SpellCheckAction m_returnCode;
};

#endif  // PLUGINS_SPELLCHECKER_NUSPELL_NUSPELLCHECKDIALOG_H_
