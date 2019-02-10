/**
 * \file findreplace.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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
 * Class definition find/replace dialog.
 */

#ifndef APPLICATION_FINDREPLACE_H_
#define APPLICATION_FINDREPLACE_H_

#include <QCloseEvent>
#include <QDialog>
#include <QShowEvent>
#include <QTextCursor>
#include <QTextEdit>

namespace Ui {
class FindReplace;
}

class FindReplace : public QDialog {
  Q_OBJECT

 public:
    explicit FindReplace(QWidget *parent = 0);
    ~FindReplace();

    void setEditor(QTextEdit *pEditor);

 public slots:
    void callFind();
    void callReplace();
    void findNext();
    void findPrevious();

 protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

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
