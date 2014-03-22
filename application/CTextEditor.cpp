/**
 * \file CTextEditor.cpp
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

#include <QAbstractItemView>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QScrollBar>

#include "./CTextEditor.h"

CTextEditor::CTextEditor(QStringList sListTplMacros,
                         QString sUserAppDir,
                         QWidget *pParent)
    : QTextEdit(pParent),
      m_UserAppDir(sUserAppDir),
      m_bCodeCompState(false),
      m_sListCompleter(sListTplMacros),
      nTimeMultiplier(1000) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QStringList sListTemp = m_sListCompleter;
    m_sListCompleter.clear();
    for (int i = 0; i < sListTemp.size(); i++) {
        if (sListTemp[i].startsWith("[[")) {
            // "[[" can not be handled by completer
            m_sListCompleter << sListTemp[i].remove("[[");
            // Remove markers for description
            m_sListCompleter.last() = sListTemp[i].remove("%%");
            m_sListCompleter.last() = m_sListCompleter.last().replace("\\n",
                                                                      "\n");
        }
    }

    m_pCompleter = new QCompleter(m_sListCompleter, this);

    this->setCompleter(m_pCompleter);
    this->setAcceptRichText(false);  // Paste plain text only

    // Text changed
    connect(this->document(), SIGNAL(contentsChanged()),
            pParent, SLOT(documentWasModified()));

    // Install auto save timer
    m_pTimerAutosave = new QTimer(this);
    connect(m_pTimerAutosave, SIGNAL(timeout()),
            this, SLOT(saveArticleAuto()));
}

CTextEditor::~CTextEditor() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTextEditor::updateTextEditorSettings(const bool bCompleter,
                                           const quint16 nAutosave) {
    m_bCodeCompState = bCompleter;

    m_pTimerAutosave->stop();
    if (0 != nAutosave) {
        m_pTimerAutosave->start(nAutosave * nTimeMultiplier);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/**
 * \todo Move completer to plugin -> TPL macros from template library?
 */

void CTextEditor::setCompleter(QCompleter *completer) {
    if (m_pCompleter) {
        QObject::disconnect(m_pCompleter, 0, this, 0);
    }

    m_pCompleter = completer;

    if (!m_pCompleter) {
        return;
    }

    m_pCompleter->setWidget(this);
    m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
    m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pCompleter->setWrapAround(false);
    QObject::connect(m_pCompleter, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QCompleter *CTextEditor::completer() const {
    return m_pCompleter;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTextEditor::insertCompletion(const QString &sCompletion) {
    if ( m_pCompleter->widget() != this ) {
        return;
    }
    QTextCursor tc = textCursor();
    int extra = sCompletion.length()-m_pCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(sCompletion.right(extra));
    setTextCursor(tc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CTextEditor::textUnderCursor() const {
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTextEditor::focusInEvent(QFocusEvent *e) {
    if (m_pCompleter) {
        m_pCompleter->setWidget(this);
    }
    QTextEdit::focusInEvent(e);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CTextEditor::keyPressEvent(QKeyEvent *e) {
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

    const bool ctrlOrShift = e->modifiers()
            & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_pCompleter || (ctrlOrShift && e->text().isEmpty())) {
        return;
    }

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");  // End of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    /*
    if (!isShortcut && (hasModifier || e->text().isEmpty()
                        || completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }
    */

    if (false == m_bCodeCompState) {
        m_pCompleter->popup()->hide();
        return;
    } else if (!isShortcut && (hasModifier
                               || e->text().isEmpty()
                               || completionPrefix.length() < 3
                               || eow.contains(e->text().right(1)))) {
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

void CTextEditor::saveArticleAuto() {
    QFile fAutoSave(m_UserAppDir + "/AutoSave.bak~");
    QTextStream outStream(&fAutoSave);

    outStream.setCodec("UTF-8");
    outStream.setAutoDetectUnicode(true);

    // No write permission
    if (!fAutoSave.open(QFile::WriteOnly | QFile::Text)) {
        qWarning() << "Could not open AutoSave.bak file!";
        return;
    }

    outStream << this->toPlainText();
    fAutoSave.close();
}
