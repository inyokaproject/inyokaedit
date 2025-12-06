// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./findreplace.h"

#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QShowEvent>
#include <QTextDocument>
#include <QTextEdit>

#include "ui_findreplace.h"

FindReplace::FindReplace(QWidget *parent)
    : QDialog(parent), m_pUi(new Ui::FindReplace), m_pEditor(nullptr) {
  m_pUi->setupUi(this);
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

  m_pUi->lbl_Error->setStyleSheet(
      QStringLiteral("font-weight: bold; color: red;"));
  m_pUi->lbl_Error->clear();

  connect(m_pUi->button_Find, &QPushButton::clicked, this,
          [this]() { this->find(m_pUi->radio_Forward->isChecked()); });
  connect(m_pUi->text_Search, &QLineEdit::textChanged, this,
          &FindReplace::textSearchChanged);
  connect(m_pUi->button_Replace, &QPushButton::clicked, this,
          &FindReplace::replace);
  connect(m_pUi->button_ReplaceAll, &QPushButton::clicked, this,
          &FindReplace::replaceAll);
  connect(m_pUi->button_Cancel, &QPushButton::clicked, this,
          &FindReplace::close);
}

FindReplace::~FindReplace() { delete m_pUi; }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::setEditor(QTextEdit *pEditor) { m_pEditor = pEditor; }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::showEvent(QShowEvent *event) {
  m_pUi->button_Find->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_Replace->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_ReplaceAll->setEnabled(m_pUi->text_Search->text().size() > 0);

  // Check if editor was set before
  if (!m_pEditor) {
    QMessageBox::warning(nullptr, QStringLiteral("Warning"),
                         QStringLiteral("Couldn't access editor object."));
    qWarning() << Q_FUNC_INFO << "- m_pEditor = NULL";
    event->ignore();
    this->close();
  } else {
    // If some text is selected in editor
    if (!m_pEditor->textCursor().selectedText().isEmpty()) {
      m_pUi->text_Search->setText(m_pEditor->textCursor().selectedText());
    }

    // Always select search input box
    m_pUi->text_Search->selectAll();
    m_pUi->text_Search->setFocus();

    event->accept();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::callFind() { this->toggleSearchReplace(false); }

void FindReplace::callReplace() { this->toggleSearchReplace(true); }

void FindReplace::toggleSearchReplace(bool bReplace) {
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
  this->show();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::closeEvent(QCloseEvent *event) { event->accept(); }

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

void FindReplace::textSearchChanged() {
  m_pUi->button_Find->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_Replace->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_ReplaceAll->setEnabled(m_pUi->text_Search->text().size() > 0);

  // Check regular expression syntax
  if (m_pUi->check_Regexp->isChecked() &&
      m_pUi->text_Search->text().size() > 0) {
    QRegularExpression regexp(m_pUi->text_Search->text(),
                              m_pUi->check_Case->isChecked()
                                  ? QRegularExpression::NoPatternOption
                                  : QRegularExpression::CaseInsensitiveOption);

    if (!regexp.isValid()) {
      m_pUi->lbl_Error->setText(regexp.errorString());
    }
  } else {
    m_pUi->lbl_Error->setText(QLatin1String(""));
  }
}

// ----------------------------------------------------------------------------

void FindReplace::findNext() { this->find(true); }

void FindReplace::findPrevious() { this->find(false); }

// ----------------------------------------------------------------------------

void FindReplace::find(const bool bForward) {
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
    QRegularExpression searchExp(
        sSearched, (bCaseSens ? QRegularExpression::NoPatternOption
                              : QRegularExpression::CaseInsensitiveOption));

    m_TextCursor =
        m_pEditor->document()->find(searchExp, m_TextCursor, searchFlags);
    m_pEditor->setTextCursor(m_TextCursor);
    bResult = (!m_TextCursor.isNull());
  } else {
    bResult = m_pEditor->find(sSearched, searchFlags);
  }

  if (bResult) {
    m_pUi->lbl_Error->setText(QLatin1String(""));
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

void FindReplace::replace() {
  if (!m_pEditor->textCursor().hasSelection()) {
    this->find(m_pUi->radio_Forward->isChecked());
  } else {
    m_pEditor->textCursor().insertText(m_pUi->text_Replace->text());
    this->find(m_pUi->radio_Forward->isChecked());
  }
}

// ----------------------------------------------------------------------------

void FindReplace::replaceAll() {
  quint32 nReplaced = 0;
  while (m_pEditor->textCursor().hasSelection()) {
    m_pEditor->textCursor().insertText(m_pUi->text_Replace->text());
    this->find(m_pUi->radio_Forward->isChecked());
    nReplaced++;
  }
  m_pUi->lbl_Error->setText(tr("Replaced expressions: %1").arg(nReplaced));
}
