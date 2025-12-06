// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-FileCopyrightText: 2011 Volker Götz
// SPDX-License-Identifier: GPL-3.0-or-later AND BSD-2-Clause

// Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell

#ifndef PLUGINS_SPELLCHECKER_HUNSPELL_HUNSPELLCHECKDIALOG_H_
#define PLUGINS_SPELLCHECKER_HUNSPELL_HUNSPELLCHECKDIALOG_H_

#include <QDialog>

class SpellChecker_Hunspell;

namespace Ui {
class HunspellCheckDialog;
}

class HunspellCheckDialog : public QDialog {
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

  explicit HunspellCheckDialog(SpellChecker_Hunspell *pSpellChecker,
                               QWidget *pParent = nullptr);
  ~HunspellCheckDialog();

  auto replacement() const -> QString;

 public slots:
  HunspellCheckDialog::SpellCheckAction checkWord(const QString &sWord);

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
  Ui::HunspellCheckDialog *m_pUi;
  SpellChecker_Hunspell *m_pSpellChecker;
  QString m_sUnkownWord;
  SpellCheckAction m_returnCode;
};

#endif  // PLUGINS_SPELLCHECKER_HUNSPELL_HUNSPELLCHECKDIALOG_H_
