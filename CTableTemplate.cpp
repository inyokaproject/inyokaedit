/**
 * \file CTableTemplate.cpp
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
 * Shows a modal window with table template.
 */

#include "CTableTemplate.h"
#include "ui_CTableTemplate.h"

#include <QFile>
#include <QString>
#include <QTextDocument>
#include <QTextStream>

#include <iostream>

#include "CParser.h"
#include "CTextEditor.h"

CTableTemplate::CTableTemplate( CTextEditor *pEditor,
                                const QDir tmpFileOutputDir,
                                const QDir tmpImgDir,
                                const QList<QStringList> sListIWiki,
                                const QList<QStringList> sListIWikiUrl,
                                const QString &sAppName,
                                const QString &sAppDirPath,
                                CSettings *pSettings,
                                QWidget *pParent )
    : QDialog(pParent)
    , m_pEditor(pEditor)
    , m_pTextDocument(new QTextDocument(this))
{
    qDebug() << "Start" << Q_FUNC_INFO;

    // Build UI
    m_pUi = new Ui::CTableTemplateClass();
    m_pUi->setupUi(this);
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    this->setModal( true );

    // Table styles
    m_sListTableStyles << "Human" << "KDE" << "Xfce" << "Edubuntu" <<
                          "Ubuntu Studio" << "Lubuntu";
    m_sListTableStylesPrefix << "" << "kde-" << "xfce-" << "edu-" <<
                                "studio-" << "lxde-";
    m_pUi->tableStyleBox->addItems( m_sListTableStyles );

    connect( m_pUi->previewButton, SIGNAL(pressed()),
                this, SLOT(preview()) );

    connect( m_pUi->buttonBox, SIGNAL(accepted()),
                this, SLOT(accept()) );

    // Setup parser
    m_pParser = new CParser( m_pTextDocument,
                             tmpFileOutputDir,
                             tmpImgDir,
                             sListIWiki,
                             sListIWikiUrl,
                             sAppName,
                             sAppDirPath,
                             pSettings);

    connect( m_pParser, SIGNAL(callShowPreview(QString)),
                this, SLOT(tableParseFinished(QString)) );

    qDebug() << "Stop" << Q_FUNC_INFO;
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::newTable(){
    qDebug() << "Start" << Q_FUNC_INFO;

    m_pUi->tableStyleBox->setCurrentIndex(0);
    m_pUi->showHeadBox->setChecked( false );
    m_pUi->showTitleBox->setChecked( false );
    m_pUi->HighlightSecondBox->setChecked( false );
    m_pUi->colsNum->setValue( m_pUi->colsNum->minimum() );
    m_pUi->rowsNum->setValue( m_pUi->rowsNum->minimum() );

    m_sTableString = "";
    m_pUi->previewBox->setHtml( "" );
    this->show();
    this->exec();

    qDebug() << "Stop" << Q_FUNC_INFO;
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::preview(){
    qDebug() << "Start" << Q_FUNC_INFO;

    this->generateTable();
    m_pTextDocument->setPlainText(m_sTableString);

    m_pParser->genOutput("");

    qDebug() << "Stop" << Q_FUNC_INFO;
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::tableParseFinished(QString sHtmlFilePath){
    qDebug() << "Start" << Q_FUNC_INFO;

    QFile file(sHtmlFilePath);
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QMessageBox::warning( 0, "Warning", tr("Could not load table preview!") );
        qWarning() << "Could not open table preview:" << sHtmlFilePath;
    }
    else
    {
        QString sContent( file.readAll() );

        // Remove for preview useless elements
        sContent.remove( QRegExp("<h1 class=\"pagetitle\">.*</h1>") );
        sContent.remove( QRegExp("<p class=\"meta\">.*</p>") );
        sContent.replace( "</style>", "#page table{margin:0px;}</style>");

        QFileInfo fi(file);
        m_pUi->previewBox->setHtml( sContent, QUrl::fromLocalFile(fi.absoluteFilePath()) );
        file.close();
    }

    qDebug() << "Stop" << Q_FUNC_INFO;
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::generateTable(){
    qDebug() << "Start" << Q_FUNC_INFO;

    int colsNum = m_pUi->colsNum->value();
    int rowsNum = m_pUi->rowsNum->value();

    m_sTableString = "{{{#!" + m_pParser->getTransTemplate().toLower() + " " + m_pParser->getTransTable() + "\n";

    // Create title if set
    if ( m_pUi->showTitleBox->isChecked()  ) {
         m_sTableString += QString("<rowclass=\"%1titel\"-%2> %3\n+++\n")
                .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()])
                .arg(colsNum)
                .arg(tr("Title"));
    }

    // Create head if set
    if ( m_pUi->showHeadBox->isChecked() ) {
         m_sTableString += QString("<rowclass=\"%1kopf\"> ")
                .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()]);

        for ( int i=0; i<colsNum; i++ )
            m_sTableString += QString(tr("Head %1 \n")).arg(i+1);

        m_sTableString += "+++\n";
    }

    // Create body
    for ( int i=0; i<rowsNum; i++ ) {
        if ( m_pUi->HighlightSecondBox->isChecked() && 1 == i % 2 ) {
            m_sTableString += QString("<rowclass=\"%1highlight\">")
                    .arg(m_sListTableStylesPrefix[m_pUi->tableStyleBox->currentIndex()]);;
        }
        for( int j=0; j<colsNum; j++ ) {
			m_sTableString += "\n";
        }
		if(i != rowsNum-1)
            m_sTableString += "+++\n";
	}

    m_sTableString += "}}}\n";

    qDebug() << "Stop" << Q_FUNC_INFO;
}

void CTableTemplate::accept(){
    qDebug() << "Start" << Q_FUNC_INFO;

    this->generateTable();
    m_pEditor->insertPlainText(m_sTableString);
    done(Accepted);

    qDebug() << "Stop" << Q_FUNC_INFO;
}
