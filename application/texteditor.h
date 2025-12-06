// SPDX-FileCopyrightText: 2011 Nokia Corporation and/or its subsidiary(-ies).
// SPDX-License-Identifier: BSD-3-Clause

#ifndef APPLICATION_TEXTEDITOR_H_
#define APPLICATION_TEXTEDITOR_H_

#include <QTextEdit>

class QCompleter;

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
