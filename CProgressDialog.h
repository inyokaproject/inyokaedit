/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CProgressDialog.h
* Purpose:    Class definition of style download progress dialog
***************************************************************************/

#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H

#include <QDialog>
#include <QProcess>

#include "CInyokaEdit.h"

namespace Ui {
    class CProgressDialog;
}

class QCloseEvent;

class CProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CProgressDialog(const QString &sScriptname, const QString &sAppname, QWidget *parent = 0, QString sDownloadFolder = "");
    ~CProgressDialog();

private slots:
    void showMessage();
    void showErrorMessage();

    void DownloadScriptFinished();
    void on_pushButtonClosProc_clicked();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::CProgressDialog *ui;
    QProcess *myProc;
};

#endif // CPROGRESSDIALOG_H
