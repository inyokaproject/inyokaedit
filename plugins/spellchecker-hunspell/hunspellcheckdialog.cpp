// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-FileCopyrightText: 2011 Volker Götz
// SPDX-License-Identifier: GPL-3.0-or-later AND BSD-2-Clause

// Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell

#include "./hunspellcheckdialog.h"

#include <QDebug>
#include <QSettings>

#include "./spellchecker-hunspell.h"
#include "ui_hunspellcheckdialog.h"

HunspellCheckDialog::HunspellCheckDialog(SpellChecker_Hunspell *pSpellChecker,
                                         QWidget *pParent)
    : QDialog(pParent), m_pUi(new Ui::HunspellCheckDialog), m_returnCode(None) {
  qDebug() << "Calling" << Q_FUNC_INFO;

  m_pUi->setupUi(this);
  m_pSpellChecker = pSpellChecker;

  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

  connect(m_pUi->listWidget, &QListWidget::currentTextChanged,
          m_pUi->ledtReplaceWith, &QLineEdit::setText);

  connect(m_pUi->btnAddToDict, &QPushButton::clicked, this,
          &HunspellCheckDialog::addToDict);
  connect(m_pUi->btnReplaceOnce, &QPushButton::clicked, this,
          &HunspellCheckDialog::replaceOnce);
  connect(m_pUi->btnReplaceAll, &QPushButton::clicked, this,
          &HunspellCheckDialog::replaceAll);
  connect(m_pUi->btnIgnoreOnce, &QPushButton::clicked, this,
          &HunspellCheckDialog::ignoreOnce);
  connect(m_pUi->btnIgnoreAll, &QPushButton::clicked, this,
          &HunspellCheckDialog::ignoreAll);
  connect(m_pUi->btnCancel, &QPushButton::clicked, this,
          &HunspellCheckDialog::closeDialog);

  // Add items before connect(), otherwise indexChanged is emitted!
  m_pUi->comboBoxLang->addItems(m_pSpellChecker->m_sListDicts);
  connect(
      m_pUi->comboBoxLang,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &HunspellCheckDialog::changeLanguage);

  if (-1 != m_pUi->comboBoxLang->findText(m_pSpellChecker->m_sDictLang)) {
    m_pUi->comboBoxLang->setCurrentIndex(
        m_pUi->comboBoxLang->findText(m_pSpellChecker->m_sDictLang));
  }
}

HunspellCheckDialog::~HunspellCheckDialog() {
  delete m_pUi;
  m_pUi = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

HunspellCheckDialog::SpellCheckAction HunspellCheckDialog::checkWord(
    const QString &sWord) {
  m_sUnkownWord = sWord;
  m_pUi->lblUnknownWord->setText(
      QStringLiteral("<b>%1</b>").arg(m_sUnkownWord));

  m_pUi->ledtReplaceWith->clear();

  QStringList sListSuggestions = m_pSpellChecker->suggest(sWord);
  m_pUi->listWidget->clear();
  m_pUi->listWidget->addItems(sListSuggestions);

  if (!sListSuggestions.isEmpty()) {
    m_pUi->listWidget->setCurrentRow(0, QItemSelectionModel::Select);
  }

  m_returnCode = AbortCheck;
  QDialog::exec();
  return m_returnCode;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto HunspellCheckDialog::replacement() const -> QString {
  return m_pUi->ledtReplaceWith->text();
}

void HunspellCheckDialog::ignoreOnce() {
  m_returnCode = IgnoreOnce;
  this->accept();
}

void HunspellCheckDialog::ignoreAll() {
  m_pSpellChecker->ignoreWord(m_sUnkownWord);
  m_returnCode = IgnoreAll;
  this->accept();
}

void HunspellCheckDialog::replaceOnce() {
  m_returnCode = ReplaceOnce;
  this->accept();
}

void HunspellCheckDialog::replaceAll() {
  m_returnCode = ReplaceAll;
  this->accept();
}

void HunspellCheckDialog::closeDialog() {
  m_pSpellChecker->m_sDictLang = m_pUi->comboBoxLang->currentText();
  m_pSpellChecker->m_pSettings->beginGroup("Plugin_" +
                                           QStringLiteral(PLUGIN_NAME));
  m_pSpellChecker->m_pSettings->setValue(QStringLiteral("SpellCheckerLanguage"),
                                         m_pSpellChecker->m_sDictLang);
  m_pSpellChecker->m_pSettings->endGroup();
  this->reject();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HunspellCheckDialog::changeLanguage(int nIndex) {
  // Before initDictionaries() !
  m_pSpellChecker->m_sDictLang = m_pUi->comboBoxLang->itemText(nIndex);
  m_pSpellChecker->initDictionaries();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void HunspellCheckDialog::addToDict() {
  m_pSpellChecker->addToUserWordlist(m_sUnkownWord);
  m_returnCode = AddToDict;
  this->accept();
}
