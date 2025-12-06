// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-FileCopyrightText: 2011 Volker Götz
// SPDX-License-Identifier: GPL-3.0-or-later AND BSD-2-Clause

// Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell

#include "./nuspellcheckdialog.h"

#include <QDebug>
#include <QSettings>

#include "./spellchecker-nuspell.h"
#include "ui_nuspellcheckdialog.h"

NuspellCheckDialog::NuspellCheckDialog(SpellChecker_Nuspell *pSpellChecker,
                                       QWidget *pParent)
    : QDialog(pParent), m_pUi(new Ui::NuspellCheckDialog), m_returnCode(None) {
  qDebug() << "Calling" << Q_FUNC_INFO;

  m_pUi->setupUi(this);
  m_pSpellChecker = pSpellChecker;

  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

  connect(m_pUi->listWidget, &QListWidget::currentTextChanged,
          m_pUi->ledtReplaceWith, &QLineEdit::setText);

  connect(m_pUi->btnAddToDict, &QPushButton::clicked, this,
          &NuspellCheckDialog::addToDict);
  connect(m_pUi->btnReplaceOnce, &QPushButton::clicked, this,
          &NuspellCheckDialog::replaceOnce);
  connect(m_pUi->btnReplaceAll, &QPushButton::clicked, this,
          &NuspellCheckDialog::replaceAll);
  connect(m_pUi->btnIgnoreOnce, &QPushButton::clicked, this,
          &NuspellCheckDialog::ignoreOnce);
  connect(m_pUi->btnIgnoreAll, &QPushButton::clicked, this,
          &NuspellCheckDialog::ignoreAll);
  connect(m_pUi->btnCancel, &QPushButton::clicked, this,
          &NuspellCheckDialog::closeDialog);

  // Add items before connect(), otherwise indexChanged is emitted!
  m_pUi->comboBoxLang->addItems(m_pSpellChecker->m_sListDicts);
  connect(
      m_pUi->comboBoxLang,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &NuspellCheckDialog::changeLanguage);

  if (-1 != m_pUi->comboBoxLang->findText(m_pSpellChecker->m_sDictLang)) {
    m_pUi->comboBoxLang->setCurrentIndex(
        m_pUi->comboBoxLang->findText(m_pSpellChecker->m_sDictLang));
  }
}

NuspellCheckDialog::~NuspellCheckDialog() {
  delete m_pUi;
  m_pUi = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

NuspellCheckDialog::SpellCheckAction NuspellCheckDialog::checkWord(
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

auto NuspellCheckDialog::replacement() const -> QString {
  return m_pUi->ledtReplaceWith->text();
}

void NuspellCheckDialog::ignoreOnce() {
  m_returnCode = IgnoreOnce;
  this->accept();
}

void NuspellCheckDialog::ignoreAll() {
  m_pSpellChecker->ignoreWord(m_sUnkownWord);
  m_returnCode = IgnoreAll;
  this->accept();
}

void NuspellCheckDialog::replaceOnce() {
  m_returnCode = ReplaceOnce;
  this->accept();
}

void NuspellCheckDialog::replaceAll() {
  m_returnCode = ReplaceAll;
  this->accept();
}

void NuspellCheckDialog::closeDialog() {
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

void NuspellCheckDialog::changeLanguage(int nIndex) {
  // Before initDictionaries() !
  m_pSpellChecker->m_sDictLang = m_pUi->comboBoxLang->itemText(nIndex);
  m_pSpellChecker->initDictionaries();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void NuspellCheckDialog::addToDict() {
  m_pSpellChecker->addToUserWordlist(m_sUnkownWord);
  m_returnCode = AddToDict;
  this->accept();
}
