// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_FINDREPLACE_H_
#define APPLICATION_FINDREPLACE_H_

#include <QDialog>
#include <QTextCursor>

class QCloseEvent;
class QTextEdit;
class QShowEvent;

namespace Ui {
class FindReplace;
}

class FindReplace : public QDialog {
  Q_OBJECT

 public:
  explicit FindReplace(QWidget *parent = nullptr);
  ~FindReplace();

  void setEditor(QTextEdit *pEditor);

 public slots:
  void callFind();
  void callReplace();
  void findNext();
  void findPrevious();

 protected:
  void showEvent(QShowEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

 private slots:
  void textSearchChanged();
  void replace();
  void replaceAll();

 private:
  void find(const bool bForward);
  void toggleSearchReplace(bool bReplace);

  Ui::FindReplace *m_pUi;
  QTextEdit *m_pEditor;
  QTextCursor m_TextCursor;
};

#endif  // APPLICATION_FINDREPLACE_H_
