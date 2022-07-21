/**
 * \file texteditor.h
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
 * Class definition for extended QTextEdit (editor widget).
 */

#ifndef APPLICATION_TEXTEDITOR_H_
#define APPLICATION_TEXTEDITOR_H_

#include <QTextEdit>

class QCompleter;

/**
 * \class TextEditor
 * \brief Extended QTextEdit (editor widget) with simple code completition.
 */
class TextEditor : public QTextEdit {
  Q_OBJECT

 public:
  TextEditor(const QStringList &sListTplMacros, const QString &sTransTemplate,
             QWidget *pParent = nullptr);
  ~TextEditor();

  void setFileName(const QString &sFileName);
  auto getFileName() -> QString;

  auto isUndoAvailable() -> bool;
  auto isRedoAvailable() -> bool;

 signals:
  void documentChanged(bool);

 protected:
  void keyPressEvent(QKeyEvent *e) override;
  void focusInEvent(QFocusEvent *e) override;

 public slots:
  void updateTextEditorSettings(const bool bCompleter);

 private slots:
  void insertCompletion(const QString &sCompletion);

 private:
  auto getLineUnderCursor() -> QString;
  void setCompleter(QCompleter *c);

  QString m_sFileName;
  QCompleter *m_pCompleter;
  bool m_bCodeCompletion;
  QStringList m_sListCompleter;
  QList<QPoint> m_listPosCompleter;
};

#endif  // APPLICATION_TEXTEDITOR_H_
