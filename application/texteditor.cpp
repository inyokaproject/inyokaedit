/**
 * \file texteditor.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (qt-info@nokia.com)
 *
 * This file is part of the examples of the Qt Toolkit.
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 *
 * \section DESCRIPTION
 * Extend QTextEdit (editor widget) to popup code completition for Inyoka
 * syntax elements.
 */

#include "./texteditor.h"

#include <QAbstractItemView>
#include <QFile>
#include <QKeyEvent>
#include <QScrollBar>

TextEditor::TextEditor(const QStringList &sListTplMacros,
                       const QString &sTransTemplate,
                       QWidget *pParent)
  : QTextEdit(pParent),
    m_sFileName(""),
    m_bCodeCompletion(false),
    m_sListCompleter(sListTplMacros) {
  for (int i = 0; i < m_sListCompleter.size(); i++) {
    if (!m_sListCompleter[i].startsWith('[') &&
        !m_sListCompleter[i].startsWith('{')) {
      m_sListCompleter[i].clear();
    }
    m_sListCompleter[i].replace("\\n", "\n");
    m_listPosCompleter << QPoint(m_sListCompleter[i].indexOf("%%"),
                                 m_sListCompleter[i].lastIndexOf("%%"));
    m_sListCompleter[i].remove("%%");
  }
  m_sListCompleter.push_front("[[" + sTransTemplate + "(");
  m_sListCompleter.push_front("{{{#!" + sTransTemplate.toLower() + " ");
  m_listPosCompleter.push_front(QPoint(-1, -1));
  m_listPosCompleter.push_front(QPoint(-1, -1));
  m_pCompleter = new QCompleter(m_sListCompleter, this);
  this->setCompleter(m_pCompleter);

  this->setAcceptRichText(false);  // Paste plain text only

  // Text changed
  connect(this->document(), &QTextDocument::modificationChanged,
          this, &TextEditor::documentChanged);
}

TextEditor::~TextEditor() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void TextEditor::updateTextEditorSettings(const bool bCompleter) {
  m_bCodeCompletion = bCompleter;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void TextEditor::setCompleter(QCompleter *completer) {
  if (m_pCompleter) {
    QObject::disconnect(m_pCompleter, nullptr, this, nullptr);
  }
  m_pCompleter = completer;
  if (!m_pCompleter) {
    return;
  }

  m_pCompleter->setWidget(this);
  m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
  m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
  m_pCompleter->setWrapAround(false);
  connect(m_pCompleter,
          static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated),
          this, &TextEditor::insertCompletion);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void TextEditor::insertCompletion(const QString &sCompletion) {
  if (m_pCompleter->widget() != this) {
    return;
  }

  QTextCursor tc = textCursor();
  int extra = sCompletion.length() - m_pCompleter->completionPrefix().length();
  tc.movePosition(QTextCursor::Left);
  tc.movePosition(QTextCursor::EndOfWord);
  setTextCursor(tc);
  int nCurrentPos = tc.position();
  int nPosInCompletion = sCompletion.length() - extra;
  tc.insertText(sCompletion.right(extra));

  // Select placeholder
  int nIndex(m_sListCompleter.indexOf(sCompletion));
  if (-1 != nIndex &&
      nIndex < m_listPosCompleter.length()) {
    if ((m_listPosCompleter.at(nIndex).x() != m_listPosCompleter.at(nIndex).y()) &&
        m_listPosCompleter.at(nIndex).x() >= 0 &&
        m_listPosCompleter.at(nIndex).y() >= 0) {
      tc.setPosition(
            nCurrentPos - nPosInCompletion + m_listPosCompleter.at(nIndex).x());
      tc.setPosition(
            nCurrentPos - nPosInCompletion + m_listPosCompleter.at(nIndex).y() - 2,
            QTextCursor::KeepAnchor);
      setTextCursor(tc);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString TextEditor::getLineUnderCursor() {
  QTextCursor tc = textCursor();
  tc.select(QTextCursor::LineUnderCursor);
  return tc.selectedText();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void TextEditor::focusInEvent(QFocusEvent *e) {
  if (m_pCompleter) {
    m_pCompleter->setWidget(this);
  }
  QTextEdit::focusInEvent(e);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void TextEditor::keyPressEvent(QKeyEvent *e) {
  if (m_pCompleter && m_pCompleter->popup()->isVisible()) {
    // The following keys are forwarded by the completer to the widget
    switch (e->key()) {
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Escape:
      case Qt::Key_Tab:
      case Qt::Key_Backtab:
        e->ignore();
        return;  // Let the completer do default behavior
      default:
        break;
    }
  }

  bool isShortcut = ((e->modifiers() & Qt::ControlModifier)
                     && e->key() == Qt::Key_E);  // CTRL+E
  // Do not process the shortcut when we have a completer
  if (!m_pCompleter || !isShortcut) {
    QTextEdit::keyPressEvent(e);
  }

  const bool ctrlOrShift = e->modifiers() &
                           (Qt::ControlModifier | Qt::ShiftModifier);
  if (!m_pCompleter || (ctrlOrShift && e->text().isEmpty())) {
    return;
  }

  static QString eow("~@$%^&*_+|:\"<>?,./;'\\-=");  // End of word
  bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
  QString completionPrefix = this->getLineUnderCursor();

  if (false == m_bCodeCompletion) {
    m_pCompleter->popup()->hide();
    return;
  } else if (!isShortcut && (hasModifier ||
                             e->text().isEmpty() ||
                             completionPrefix.length() < 3 ||
                             eow.contains(e->text().right(1)))) {
    m_pCompleter->popup()->hide();
    return;
  }

  if (completionPrefix != m_pCompleter->completionPrefix()) {
    m_pCompleter->setCompletionPrefix(completionPrefix);
    m_pCompleter->popup()->setCurrentIndex(
          m_pCompleter->completionModel()->index(0, 0));
  }
  QRect cr = cursorRect();
  cr.setWidth(m_pCompleter->popup()->sizeHintForColumn(0)
              + m_pCompleter->popup()->verticalScrollBar()->sizeHint().width());
  m_pCompleter->complete(cr);  // Show popup
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void TextEditor::setFileName(const QString &sFileName) {
  m_sFileName = sFileName;
}
QString TextEditor::getFileName() {
  return m_sFileName;
}

bool TextEditor::isUndoAvailable() {
  return this->document()->isUndoAvailable();
}
bool TextEditor::isRedoAvailable() {
  return this->document()->isRedoAvailable();
}
