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
* File Name:  CProgessDialog.cpp
* Purpose:    Shows a modal window with download progress messages
***************************************************************************/

#include "CProgressDialog.h"
#include "ui_CProgressDialog.h"

CProgressDialog::CProgressDialog(const QString &sScriptname, QStringList sListArguments, const QString &sAppName, QWidget *pParent) :
    QDialog(pParent),
    m_pUi(new Ui::CProgressDialog)
{
    m_pUi->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if (sListArguments.size() == 0){
        sListArguments << QDir::homePath() + "/." + sAppName;
    }

    try
    {
        m_myProc = new QProcess();
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: Caught bad_alloc in \"CProgressDialog\": " << ba.what() << std::endl;
      QMessageBox::critical(0, sAppName, "Error while memory allocation: CProgressDialog");
      exit (-1);
    }
    m_myProc->start(sScriptname, sListArguments);

    // Show output
    connect(m_myProc, SIGNAL(readyReadStandardOutput()),this, SLOT(ShowMessage()) );
    connect(m_myProc, SIGNAL(readyReadStandardError()), this, SLOT(ShowErrorMessage()) );

    connect(m_myProc, SIGNAL(finished(int)), this, SLOT(DownloadScriptFinished()) );
}

CProgressDialog::~CProgressDialog()
{
    m_myProc->kill();
    delete m_myProc;
    m_myProc = NULL;
    delete m_pUi;
    m_pUi = NULL;
}

// -----------------------------------------------------------------------------------------------

void CProgressDialog::closeEvent(QCloseEvent *pEvent)
{
    m_myProc->kill();   // Kill download process
    pEvent->accept();  // Close window
}

// -----------------------------------------------------------------------------------------------

// Show message
void CProgressDialog::ShowMessage()
{
    QByteArray strdata = m_myProc->readAllStandardOutput();
    m_pUi->textEditProcessOut->setTextColor(Qt::black);
    m_pUi->textEditProcessOut->append(strdata);
}

// Show error message
void CProgressDialog::ShowErrorMessage()
{
    QByteArray strdata = m_myProc->readAllStandardError();
    m_pUi->textEditProcessOut->setTextColor(Qt::darkGray);
    m_pUi->textEditProcessOut->append(strdata);
}

// -----------------------------------------------------------------------------------------------

void CProgressDialog::DownloadScriptFinished()
{
    this->close();
}

// -----------------------------------------------------------------------------------------------

// Click on cancel button
void CProgressDialog::ClickedCloseProcess()
{
    this->close();  // Send close event
}
