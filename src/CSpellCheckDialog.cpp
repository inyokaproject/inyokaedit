/**
 * \file CSpellCheckDialog.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
#include "./CSpellCheckDialog.h"
#include "ui_CSpellCheckDialog.h"

#include "./CSpellChecker.h"

CSpellCheckDialog::CSpellCheckDialog(CSpellChecker *spellChecker,
                                     QWidget *parent)
    : QDialog(parent),
      m_pUi(new Ui::CSpellCheckDialog) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi->setupUi(this);
    _spellChecker = spellChecker;

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
            this, SLOT(reject()));
}

CSpellCheckDialog::~CSpellCheckDialog() {
    if (NULL != m_pUi) {
        delete m_pUi;
    }
    m_pUi = NULL;
}

CSpellCheckDialog::SpellCheckAction CSpellCheckDialog::checkWord(const QString &sWord) {
    _unkownWord = sWord;
    m_pUi->lblUnknownWord->setText(QString("<b>%1</b>").arg(_unkownWord));

    m_pUi->ledtReplaceWith->clear();

    QStringList suggestions = _spellChecker->suggest(sWord);
    m_pUi->listWidget->clear();
    m_pUi->listWidget->addItems(suggestions);

    if (suggestions.count() > 0) {
        m_pUi->listWidget->setCurrentRow(0, QItemSelectionModel::Select);
    }

    _returnCode = AbortCheck;
    QDialog::exec();
    return _returnCode;
}

QString CSpellCheckDialog::replacement() const {
    return m_pUi->ledtReplaceWith->text();
}

void CSpellCheckDialog::ignoreOnce() {
    _returnCode = IgnoreOnce;
    accept();
}

void CSpellCheckDialog::ignoreAll() {
    _spellChecker->ignoreWord(_unkownWord);
    _returnCode = IgnoreAll;
    accept();
}

void CSpellCheckDialog::replaceOnce() {
    _returnCode = ReplaceOnce;
    accept();
}

void CSpellCheckDialog::replaceAll() {
    _returnCode = ReplaceAll;
    accept();
}

void CSpellCheckDialog::addToDict() {
    _spellChecker->addToUserWordlist(_unkownWord);
    _returnCode = AddToDict;
    accept();
}
