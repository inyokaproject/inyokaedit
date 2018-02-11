/**
 * \file findreplace.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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

#include "./findreplace.h"

#include <QDebug>
#include <QMessageBox>
#include <QTextDocument>

#include "ui_findreplace.h"

FindReplace::FindReplace(QWidget *parent)
  : QDialog(parent),
    m_pUi(new Ui::FindReplace),
    m_pEditor(NULL) {
  m_pUi->setupUi(this);
  this->setWindowFlags(this->windowFlags()
                       & ~Qt::WindowContextHelpButtonHint);

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

FindReplace::~FindReplace() {
  delete m_pUi;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::setEditor(QTextEdit *pEditor) {
  m_pEditor = pEditor;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::showEvent(QShowEvent *event) {
  m_pUi->button_Find->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_Replace->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_ReplaceAll->setEnabled(m_pUi->text_Search->text().size() > 0);

  // Check if editor was set before
  if (!m_pEditor) {
    QMessageBox::warning(0, "Warning", "Couldn't access editor object.");
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

void FindReplace::callFind() {
  this->toggleSearchReplace(false);
}

void FindReplace::callReplace() {
  this->toggleSearchReplace(true);
}

void FindReplace::toggleSearchReplace(bool bReplace) {
  if (!bReplace) {
    this->setWindowTitle(trUtf8("Search"));
    m_pUi->lbl_Replace->setVisible(false);
    m_pUi->text_Replace->setVisible(false);
    m_pUi->button_Replace->setVisible(false);
    m_pUi->button_ReplaceAll->setVisible(false);
  } else {
    this->setWindowTitle(trUtf8("Search / Replace"));
    m_pUi->lbl_Replace->setVisible(true);
    m_pUi->text_Replace->setVisible(true);
    m_pUi->button_Replace->setVisible(true);
    m_pUi->button_ReplaceAll->setVisible(true);
  }
  this->show();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FindReplace::closeEvent(QCloseEvent *event) {
  event->accept();
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

void FindReplace::textSearchChanged() {
  m_pUi->button_Find->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_Replace->setEnabled(m_pUi->text_Search->text().size() > 0);
  m_pUi->button_ReplaceAll->setEnabled(m_pUi->text_Search->text().size() > 0);

  // Check regular expression syntax
  if (m_pUi->check_Regexp->isChecked() &&
      m_pUi->text_Search->text().size() > 0) {
    QRegExp regexp(m_pUi->text_Search->text(),
                   m_pUi->check_Case->isChecked()
                   ? Qt::CaseSensitive : Qt::CaseInsensitive);

    if (!regexp.isValid()) {
      m_pUi->lbl_Error->setText(regexp.errorString());
    }
  } else {
    m_pUi->lbl_Error->setText("");
  }
}

// ----------------------------------------------------------------------------

void FindReplace::find() {
  this->find(m_pUi->radio_Forward->isChecked());
}

void FindReplace::findNext() {
  this->find(true);
}

void FindReplace::findPrevious() {
  this->find(false);
}

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
    QRegExp searchExp(
          sSearched,
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
    m_pUi->lbl_Error->setText(trUtf8("Could not find your expression"));
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
    this->find();
  } else {
    m_pEditor->textCursor().insertText(m_pUi->text_Replace->text());
    this->find();
  }
}

// ----------------------------------------------------------------------------

void FindReplace::replaceAll() {
  quint32 nReplaced = 0;
  while (m_pEditor->textCursor().hasSelection()) {
    m_pEditor->textCursor().insertText(m_pUi->text_Replace->text());
    this->find();
    nReplaced++;
  }
  m_pUi->lbl_Error->setText(
        trUtf8("Replaced expressions: %1").arg(nReplaced));
}
