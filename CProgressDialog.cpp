/**
 * \file CProgressDialog.cpp
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
 * Shows a modal window with download progress messages.
 */

#include "CProgressDialog.h"
#include "ui_CProgressDialog.h"

CProgressDialog::CProgressDialog( const QString &sScriptname, QStringList sListArguments, const QString &sAppName, QWidget *pParent ) :
    QDialog(pParent),
    m_pUi(new Ui::CProgressDialog)
{
    m_pUi->setupUi(this);
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );

    if ( sListArguments.size() == 0 )
    {
        sListArguments << QDir::homePath() + "/." + sAppName;
    }

    m_myProc = new QProcess();
    m_myProc->start( sScriptname, sListArguments );

    // Show output
    connect( m_myProc, SIGNAL(readyReadStandardOutput()),
             this, SLOT(showMessage()) );
    connect( m_myProc, SIGNAL(readyReadStandardError()),
             this, SLOT(showErrorMessage()) );

    connect( m_myProc, SIGNAL(finished(int)),
             this, SLOT(downloadScriptFinished()) );
}

CProgressDialog::~CProgressDialog()
{
    m_myProc->kill();

    if (  m_myProc != NULL )
    {
        delete  m_myProc;
    }
     m_myProc = NULL;

    if ( m_pUi != NULL )
    {
        delete m_pUi;
    }
    m_pUi = NULL;
}

// -----------------------------------------------------------------------------------------------

void CProgressDialog::closeEvent( QCloseEvent *pEvent )
{
    m_myProc->kill();  // Kill download process
    pEvent->accept();  // Close window
}

// -----------------------------------------------------------------------------------------------

// Show message
void CProgressDialog::showMessage()
{
    QByteArray strdata = m_myProc->readAllStandardOutput();
    m_pUi->textEditProcessOut->setTextColor(Qt::black);
    m_pUi->textEditProcessOut->append(strdata);
}

// Show error message
void CProgressDialog::showErrorMessage()
{
    QByteArray strdata = m_myProc->readAllStandardError();
    m_pUi->textEditProcessOut->setTextColor(Qt::darkGray);
    m_pUi->textEditProcessOut->append(strdata);
}

// -----------------------------------------------------------------------------------------------

void CProgressDialog::downloadScriptFinished()
{
#if defined _WIN32
    QProcess killWinBash;
    // Kill bash if button "Cancel" was clicked, needed because bash.exe is started via batch file...
    killWinBash.start( "taskkill", QStringList() << "/F" << "/IM" << "bash.exe" );
#endif

    this->close();
}

// -----------------------------------------------------------------------------------------------

// Click on cancel button
void CProgressDialog::on_pushButtonClosProc_clicked()
{
    this->close();  // Send close event
}
