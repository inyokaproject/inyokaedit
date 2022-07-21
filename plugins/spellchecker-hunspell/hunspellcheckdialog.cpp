/**
 * \file hunspellcheckdialog.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
 * Show spell checker dialog with spelling suggestions.
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
