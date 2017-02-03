/**
 * \file CProgressDialog.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Shows a modal window with download progress messages.
 */

#include <QDebug>
#include <QDir>
#include <QMessageBox>

#include "./CProgressDialog.h"
#include "ui_CProgressDialog.h"

CProgressDialog::CProgressDialog(const QString &sScriptname,
                                 QStringList sListArguments)
  : QDialog(),
    m_pUi(new Ui::CProgressDialog) {
  qDebug() << "Calling" << Q_FUNC_INFO;

  m_pUi->setupUi(this);
  this->setWindowFlags((this->windowFlags()
                        & ~Qt::WindowContextHelpButtonHint)
                       | Qt::WindowStaysOnTopHint);

  m_pProcess = new QProcess();
  m_pProcess->start(sScriptname, sListArguments);

  // Show output
  connect(m_pProcess, SIGNAL(readyReadStandardOutput()),
          this, SLOT(showMessage()));
  connect(m_pProcess, SIGNAL(readyReadStandardError()),
          this, SLOT(showErrorMessage()));

  connect(m_pProcess, SIGNAL(finished(int)),
          this, SLOT(downloadScriptFinished()));
}

CProgressDialog::~CProgressDialog() {
  m_pProcess->kill();

  if (NULL != m_pProcess) {
    delete  m_pProcess;
  }
  m_pProcess = NULL;

  if (NULL != m_pUi) {
    delete m_pUi;
  }
  m_pUi = NULL;
}

// ----------------------------------------------------------------------------

void CProgressDialog::closeEvent(QCloseEvent *pEvent) {
  m_pProcess->kill();  // Kill download process
  pEvent->accept();    // Close window
}

// ----------------------------------------------------------------------------

// Show message
void CProgressDialog::showMessage() {
  QByteArray strdata = m_pProcess->readAllStandardOutput();
  m_pUi->textEditProcessOut->setTextColor(Qt::black);
  m_pUi->textEditProcessOut->append(strdata);
}

// Show error message
void CProgressDialog::showErrorMessage() {
  QByteArray strdata = m_pProcess->readAllStandardError();
  m_pUi->textEditProcessOut->setTextColor(Qt::darkGray);
  m_pUi->textEditProcessOut->append(strdata);
}

// ----------------------------------------------------------------------------

void CProgressDialog::downloadScriptFinished() {
  QMessageBox::information(0, qApp->applicationName(), "Download finished.");
  this->close();
}

// ----------------------------------------------------------------------------

// Click on cancel button
void CProgressDialog::on_pushButtonClosProc_clicked() {
  this->close();  // Send close event
}
