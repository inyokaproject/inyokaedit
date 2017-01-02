/**
 * \file CSpellCheckDialog.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Show spell checker dialog with spelling suggestions.
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#include <QDebug>
#include <QTextEdit>

#include "./CSpellCheckDialog.h"
#include "ui_CSpellCheckDialog.h"
#include "./CSpellChecker.h"

CSpellCheckDialog::CSpellCheckDialog(CSpellChecker *pSpellChecker,
                                     QWidget *pParent)
    : QDialog(pParent),
      m_pUi(new Ui::CSpellCheckDialog) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi->setupUi(this);
    m_pSpellChecker = pSpellChecker;

    this->setWindowFlags(this->windowFlags()
                         & ~Qt::WindowContextHelpButtonHint);

    connect(m_pUi->listWidget, SIGNAL(currentTextChanged(QString)),
            m_pUi->ledtReplaceWith, SLOT(setText(QString)));

    connect(m_pUi->btnAddToDict, SIGNAL(clicked()),
            this, SLOT(addToDict()));
    connect(m_pUi->btnReplaceOnce, SIGNAL(clicked()),
            this, SLOT(replaceOnce()));
    connect(m_pUi->btnReplaceAll, SIGNAL(clicked()),
            this, SLOT(replaceAll()));
    connect(m_pUi->btnIgnoreOnce, SIGNAL(clicked()),
            this, SLOT(ignoreOnce()));
    connect(m_pUi->btnIgnoreAll, SIGNAL(clicked()),
            this, SLOT(ignoreAll()));
    connect(m_pUi->btnCancel, SIGNAL(clicked()),
            this, SLOT(closeDialog()));

    // Add items before connect(), otherwise indexChanged is emitted!
    m_pUi->comboBoxLang->addItems(m_pSpellChecker->m_sListDicts);
    connect(m_pUi->comboBoxLang, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(changeLanguage(QString)));

    if (-1 != m_pUi->comboBoxLang->findText(m_pSpellChecker->m_sDictLang)) {
        m_pUi->comboBoxLang->setCurrentIndex(
                    m_pUi->comboBoxLang->findText(
                        m_pSpellChecker->m_sDictLang));
    }
}

CSpellCheckDialog::~CSpellCheckDialog() {
    if (NULL != m_pUi) {
        delete m_pUi;
    }
    m_pUi = NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CSpellCheckDialog::SpellCheckAction CSpellCheckDialog::checkWord(
        const QString &sWord) {
    m_sUnkownWord = sWord;
    m_pUi->lblUnknownWord->setText(QString("<b>%1</b>").arg(m_sUnkownWord));

    m_pUi->ledtReplaceWith->clear();

    QStringList sListSuggestions = m_pSpellChecker->suggest(sWord);
    m_pUi->listWidget->clear();
    m_pUi->listWidget->addItems(sListSuggestions);

    if (sListSuggestions.count() > 0) {
        m_pUi->listWidget->setCurrentRow(0, QItemSelectionModel::Select);
    }

    m_returnCode = AbortCheck;
    QDialog::exec();
    return m_returnCode;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CSpellCheckDialog::replacement() const {
    return m_pUi->ledtReplaceWith->text();
}

void CSpellCheckDialog::ignoreOnce() {
    m_returnCode = IgnoreOnce;
    this->accept();
}

void CSpellCheckDialog::ignoreAll() {
    m_pSpellChecker->ignoreWord(m_sUnkownWord);
    m_returnCode = IgnoreAll;
    this->accept();
}

void CSpellCheckDialog::replaceOnce() {
    m_returnCode = ReplaceOnce;
    this->accept();
}

void CSpellCheckDialog::replaceAll() {
    m_returnCode = ReplaceAll;
    this->accept();
}

void CSpellCheckDialog::closeDialog() {
    m_pSpellChecker->m_sDictLang = m_pUi->comboBoxLang->currentText();
    m_pSpellChecker->m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
    m_pSpellChecker->m_pSettings->setValue("SpellCheckerLanguage",
                                           m_pSpellChecker->m_sDictLang);
    m_pSpellChecker->m_pSettings->endGroup();
    this->reject();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSpellCheckDialog::changeLanguage(const QString &sLanguage) {
    m_pSpellChecker->m_sDictLang = sLanguage;  // Before initDictionaries() !
    m_pSpellChecker->initDictionaries();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSpellCheckDialog::addToDict() {
    m_pSpellChecker->addToUserWordlist(m_sUnkownWord);
    m_returnCode = AddToDict;
    this->accept();
}
