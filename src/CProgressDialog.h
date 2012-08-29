/**
 * \file CProgressDialog.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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
 * Class definition of download progress dialog.
 */

#ifndef CPROGRESSDIALOG_H
#define CPROGRESSDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QtGui/QCloseEvent>
#include <QDir>

#include "CInyokaEdit.h"

namespace Ui {
    class CProgressDialog;
}

class QCloseEvent;

/**
 * \class CProgressDialog
 * \brief Showing a progress dialog while downloading styles or article images.
 */
class CProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CProgressDialog(const QString &sScriptname, QStringList sListArguments, const QString &sAppname, QWidget *pParent = 0);
    ~CProgressDialog();

private slots:
    void showMessage();
    void showErrorMessage();

    void downloadScriptFinished();
    void on_pushButtonClosProc_clicked();

protected:
    void closeEvent(QCloseEvent *pEvent);

private:
    Ui::CProgressDialog *m_pUi;
    QString m_sAppName;
    QProcess *m_myProc;
};

#endif // CPROGRESSDIALOG_H
