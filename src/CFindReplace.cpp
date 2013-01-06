/**
 * \file CFindReplace.cpp
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
 * Find/replace dialog.
 */

#include <QDebug>
#include <QTextDocument>

#include "./CFindReplace.h"
#include "ui_CFindReplace.h"

CFindReplace::CFindReplace(CSettings *pSettings, QWidget *parent)
    : QDialog(parent),
      m_pSettings(pSettings),
      m_pUi(new Ui::CFindReplace),
      m_pEditor(NULL) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_pUi->lbl_Error->setStyleSheet("font-weight: bold; color: red;");
    m_pUi->lbl_Error->clear();

    connect(m_pUi->button_Find, SIGNAL(clicked()),
            this, SLOT(find()));
    connect(m_pUi->text_Search, SIGNAL(textChanged(QString)),
            this, SLOT(textSearchChanged()));
    connect(m_pUi->button_Replace, SIGNAL(clicked()),
            this, SLOT(replace()));
    connect(m_pUi->button_ReplaceAll, SIGNAL(clicked()),
            this, SLOT(replaceAll()));
    connect(m_pUi->button_Cancel, SIGNAL(clicked()),
            this, SLOT(close()));
}

CFindReplace::~CFindReplace() {
    delete m_pUi;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFindReplace::setEditor(QTextEdit *pEditor) {
    m_pEditor = pEditor;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFindReplace::showEvent(QShowEvent *event) {
    m_pUi->button_Find->setEnabled(m_pUi->text_Search->text().size() > 0);
    m_pUi->button_Replace->setEnabled(m_pUi->text_Search->text().size() > 0);
    m_pUi->button_ReplaceAll->setEnabled(m_pUi->text_Search->text().size() > 0);

    m_pUi->text_Search->setText(m_pSettings->getTextFind());
    m_pUi->text_Replace->setText(m_pSettings->getTextReplace());
    m_pUi->radio_Forward->setChecked(m_pSettings->getSearchForwardState());
    m_pUi->radio_Backward->setChecked(!m_pSettings->getSearchForwardState());
    m_pUi->check_Case->setChecked(m_pSettings->getCaseState());
    m_pUi->check_WholeWord->setChecked(m_pSettings->getWholeWordState());
    m_pUi->check_Regexp->setChecked(m_pSettings->getUseRegExpState());

    // Always select search input box
    m_pUi->text_Search->selectAll();
    m_pUi->text_Search->setFocus();

    // Check if editor was set before
    if (!m_pEditor) {
        QMessageBox::warning(0, "Warning", "Couldn't access editor object.");
        qWarning() << Q_FUNC_INFO << "- m_pEditor = NULL";
        event->ignore();
        this->close();
    } else {
        event->accept();
    }

}

// ----------------------------------------------------------------------------

void CFindReplace::closeEvent(QCloseEvent *event) {
    m_pSettings->setTextFind(m_pUi->text_Search->text());
    m_pSettings->setTextReplace(m_pUi->text_Replace->text());
    m_pSettings->setSearchForwardState(m_pUi->radio_Forward->isChecked());
    m_pSettings->setCaseState(m_pUi->check_Case->isChecked());
    m_pSettings->setWholeWordState(m_pUi->check_WholeWord->isChecked());
    m_pSettings->setUseRegExpState(m_pUi->check_Regexp->isChecked());

    event->accept();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFindReplace::toggleSearchReplace(bool bReplace) {
    if (!bReplace) {
        this->setWindowTitle(tr("Search"));
        m_pUi->lbl_Replace->setVisible(false);
        m_pUi->text_Replace->setVisible(false);
        m_pUi->button_Replace->setVisible(false);
        m_pUi->button_ReplaceAll->setVisible(false);
    } else {
        this->setWindowTitle(tr("Search / Replace"));
        m_pUi->lbl_Replace->setVisible(true);
        m_pUi->text_Replace->setVisible(true);
        m_pUi->button_Replace->setVisible(true);
        m_pUi->button_ReplaceAll->setVisible(true);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/****************************************************************************
*****************************************************************************
**
** Copyright (C) 2009  Lorenzo Bettini <http://www.lorenzobettini.it>
** All rights reserved.
**
** The following code is part of QtFindReplaceDialog.
**
** QtFindReplaceDialog is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; version 2.1 of the License.
**
** QtFindReplaceDialog is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Lesser General Public License for more details.
**
****************************************************************************/

void CFindReplace::textSearchChanged() {
    m_pUi->button_Find->setEnabled(m_pUi->text_Search->text().size() > 0);
    m_pUi->button_Replace->setEnabled(m_pUi->text_Search->text().size() > 0);
    m_pUi->button_ReplaceAll->setEnabled(m_pUi->text_Search->text().size() > 0);

    // Check regular expression syntax
    if (m_pUi->check_Regexp->isChecked() &&
            m_pUi->text_Search->text().size() > 0) {
        QRegExp regexp(m_pUi->text_Search->text(),
                       m_pUi->check_Case->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

        if (!regexp.isValid()) {
            m_pUi->lbl_Error->setText(regexp.errorString());
        }
    } else {
        m_pUi->lbl_Error->setText("");
    }
}

// ----------------------------------------------------------------------------

void CFindReplace::find() {
    const bool bForward = m_pUi->radio_Forward->isChecked();
    const bool bCaseSens = m_pUi->check_Case->isChecked();
    const bool bWholeWord = m_pUi->check_WholeWord->isChecked();
    const bool bUseRegexp = m_pUi->check_Regexp->isChecked();
    const QString sSearched = m_pUi->text_Search->text();
    QTextDocument::FindFlags searchFlags;
    bool bResult = false;

    if (!bForward) {
        searchFlags |= QTextDocument::FindBackward;
    }
    if (bCaseSens) {
        searchFlags |= QTextDocument::FindCaseSensitively;
    }
    if (bWholeWord) {
        searchFlags |= QTextDocument::FindWholeWords;
    }

    if (bUseRegexp) {
        QRegExp searchExp(sSearched,
                          (bCaseSens ? Qt::CaseSensitive : Qt::CaseInsensitive));

        m_TextCursor = m_pEditor->document()->find(searchExp, m_TextCursor,
                                                   searchFlags);
        m_pEditor->setTextCursor(m_TextCursor);
        bResult = (!m_TextCursor.isNull());
    } else {
        bResult = m_pEditor->find(sSearched, searchFlags);
    }

    if (bResult) {
        m_pUi->lbl_Error->setText("");
    } else {
        m_pUi->lbl_Error->setText(tr("Could not find your expression"));
        // Move to the beginning of the document for the next search cycle
        m_TextCursor.setPosition(0);
        m_pEditor->setTextCursor(m_TextCursor);

        // Workaround for LP: #941308
        m_pEditor->setText(m_pEditor->toPlainText());
    }
}

// ----------------------------------------------------------------------------

void CFindReplace::replace() {
    if (!m_pEditor->textCursor().hasSelection()) {
        this->find();
    } else {
        m_pEditor->textCursor().insertText(m_pUi->text_Replace->text());
        this->find();
    }
}

// ----------------------------------------------------------------------------

void CFindReplace::replaceAll() {
    quint32 nReplaced = 0;
    while (m_pEditor->textCursor().hasSelection()) {
        m_pEditor->textCursor().insertText(m_pUi->text_Replace->text());
        this->find();
        nReplaced++;
    }
    m_pUi->lbl_Error->setText(tr("Replaced expressions: %1").arg(nReplaced));
}
