// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-FileCopyrightText: 2011 Volker Götz
// SPDX-License-Identifier: GPL-3.0-or-later AND BSD-2-Clause

// Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell

#ifndef PLUGINS_SPELLCHECKER_NUSPELL_NUSPELLCHECKDIALOG_H_
#define PLUGINS_SPELLCHECKER_NUSPELL_NUSPELLCHECKDIALOG_H_

#include <QDialog>

class SpellChecker_Nuspell;

namespace Ui {
class NuspellCheckDialog;
}

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
