#include "CTableTemplate.h"
#include "ui_CTableTemplate.h"

#include <QFile>
#include <QString>
#include <QTextDocument>
#include <QTextStream>

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

	connect( m_pUi->previewButton, SIGNAL(pressed()),
			this, SLOT(preview()) );

	connect( m_pUi->showTitleBox, SIGNAL(stateChanged(int)),
			this, SLOT(setTitle(int)) );

	connect( m_pUi->showHeadBox, SIGNAL(stateChanged(int)),
			this, SLOT(setHead(int)) );

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

void CTableTemplate::setTitle( int nHasTitle ){
	qDebug() << "Start" << Q_FUNC_INFO;

	switch (nHasTitle) {
	case Qt::Checked: m_bHasTitle = true; break;
	case Qt::Unchecked: m_bHasTitle = false; break;
	}
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::setHead( int nHasHead ){
	qDebug() << "Start" << Q_FUNC_INFO;

	switch (nHasHead) {
	case Qt::Checked: m_bHasHead = true; break;
	case Qt::Unchecked: m_bHasHead = false; break;
	}
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::newTable(){
	qDebug() << "Start" << Q_FUNC_INFO;

	m_sTableString = "";
	this->show();
	this->exec();
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::preview(){
	qDebug() << "Start" << Q_FUNC_INFO;

	this->generateTable();
    m_pTextDocument->setPlainText(m_sTableString);

	m_pParser->genOutput("");
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::tableParseFinished(QString sHtmlFilePath){
	qDebug() << "Start" << Q_FUNC_INFO;

    m_pUi->previewBox->load( QUrl::fromLocalFile(sHtmlFilePath) );
}

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

void CTableTemplate::generateTable(){
	qDebug() << "Start" << Q_FUNC_INFO;

	int colsNum = m_pUi->colsNum->value();
	int rowsNum = m_pUi->rowsNum->value();

    m_sTableString = "{{{#!vorlage Tabelle\n";

	// Create title if set
	if (m_bHasTitle)
		m_sTableString += QString("<rowclass=\"titel\"-%1> %2\n+++\n").arg(colsNum).arg(tr("Title"));

	// Create head if set
	if (m_bHasHead){
		m_sTableString += "<rowclass=\"kopf\"> ";
		for (int i=0;i<colsNum;i++)
			m_sTableString += QString(tr("Description %1\n")).arg(i+1);
		m_sTableString += "+++\n";
	}

	// Create body
	for (int i=0;i<rowsNum;i++){
		for(int j=0;j<colsNum;j++)
			m_sTableString += "\n";
		if(i != rowsNum-1)
				m_sTableString += "+++\n";
	}

	m_sTableString += "}}}";
}

void CTableTemplate::accept(){
	qDebug() << "Start" << Q_FUNC_INFO;

	this->generateTable();
	m_pEditor->insertPlainText(m_sTableString);
	done(Accepted);
}
