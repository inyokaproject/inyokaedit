/**
 * \file CInyokaEdit.cpp
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
 * Main application generation (gui, object creation etc.).
 */

#include <QtGui>
#include <QtWebKit/QWebView>

#include "CInyokaEdit.h"
#include "ui_CInyokaEdit.h"

CInyokaEdit::CInyokaEdit( QApplication *ptrApp, QDir userAppDir, QWidget *parent ) :
    QMainWindow(parent),
    m_pUi( new Ui::CInyokaEdit ),
    m_pApp( ptrApp ),
    m_UserAppDir (userAppDir)
{
    qDebug() << "Start" << Q_FUNC_INFO;

    bool bOpenFileAfterStart = false;

    m_pUi->setupUi(this);

    // Check for command line arguments
    if ( m_pApp->argc() >= 2 )
    {
        QString sTmp = m_pApp->argv()[1];

        if ( "-v"== sTmp || "--version"== sTmp )
        {
            std::cout << m_pApp->argv()[0] << "\t v" << m_pApp->applicationVersion().toStdString() << std::endl;
            exit(0);
        }
        else
        {
            bOpenFileAfterStart = true;
        }
    }

    // Create folder for downloaded article images
    m_tmpPreviewImgDir = m_UserAppDir.absolutePath() + "/tmpImages";
    if ( !m_tmpPreviewImgDir.exists() )
    {
        m_tmpPreviewImgDir.mkpath(m_tmpPreviewImgDir.absolutePath());  // Create folder including possible parent directories (mkPATH)!
    }

    // Create all objects (after definition of StylesAndImagesDir AND m_tmpPreviewImgDir)
    this->createObjects();

    // Setup gui, menus, actions, toolbar...
    this->setupEditor();
    this->createActions();
    this->createMenus();
    this->createToolBars();

    // Download style files if preview/styles/imgages folders doesn't exist (/home/user/.InyokaEdit)
    if ( !m_UserAppDir.exists() ||
         !QDir(m_UserAppDir.absolutePath() + "/img").exists() ||
         !QDir(m_UserAppDir.absolutePath() + "/styles").exists() ||
         !QDir(m_UserAppDir.absolutePath() + "/Wiki").exists() )
    {
        m_UserAppDir.mkdir( m_UserAppDir.absolutePath() );  // Create folder because user may not start download. Folder is needed for preview.
#if !defined _WIN32
        m_pDownloadModule->loadInyokaStyles();
#endif
    }

    if ( true == bOpenFileAfterStart )
    {
        m_pFileOperations->loadFile( m_pApp->argv()[1], true );
    }

    qDebug() << "End" << Q_FUNC_INFO;
}

CInyokaEdit::~CInyokaEdit()
{
    if ( m_pUi != NULL )
    {
        delete m_pUi;
    }
    m_pUi = NULL;
    qDebug() << "Closing " << m_pApp->applicationName();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::createObjects()
{
    qDebug() << "Start" << Q_FUNC_INFO;

    m_findDialog = new FindDialog(this);  // Has to be create before readSettings
    m_findReplaceDialog = new FindReplaceDialog(this);

    m_pSettings = new CSettings( m_UserAppDir,
                                 m_pApp->applicationName(),
                                 *m_findDialog, *m_findReplaceDialog );
    // Load settings from config file
    m_pSettings->readSettings();

    m_pDownloadModule = new CDownload( this,
                                       m_pApp->applicationName(),
                                       m_pApp->applicationDirPath(),
                                       m_UserAppDir );

    m_pEditor = new CTextEditor( m_pUi, m_pSettings->getCodeCompletion(), this );  // Has to be create before find/replace
//  if ( true == m_pSettings->getPreviewAlongside() )
//  {
    m_pEditor->installEventFilter(this);
//  }

    m_pFileOperations = new CFileOperations( this,
                                             m_pEditor,
                                             m_pSettings,
                                             m_pApp->applicationName() );

    m_pInterWikiLinks = new CInterWiki( m_pApp );  // Has to be created before parser

    m_pParser = new CParser( m_pEditor->document(),
                             m_UserAppDir,
                             m_tmpPreviewImgDir,
                             m_pInterWikiLinks->getInterwikiLinks(),
                             m_pInterWikiLinks->getInterwikiLinksUrls(),
                             m_pApp->applicationName(),
                             m_pApp->applicationDirPath(),
                             m_pSettings );

    QStringList tmpsListMacro;
    tmpsListMacro << m_pParser->getTransTemplate() << m_pParser->getTransTOC() <<
                     m_pParser->getTransImage() << m_pParser->getTransAnchor() <<
                     m_pParser->getTransAttachment() << m_pParser->getTransDate();

    QStringList tmpsListParser;
    tmpsListParser << m_pParser->getTransTemplate().toLower() <<
                      m_pParser->getTransCodeBlock().toLower();

    m_pHighlighter = new CHighlighter( m_pInterWikiLinks->getInterwikiLinks(),
                                       m_pParser->getFlaglist(),
                                       m_pParser->getTransOverview(),
                                       tmpsListMacro,
                                       tmpsListParser,
                                       m_pEditor->document() );

    /**
     * \todo Add tabs for editing multiple documents.
     */
    //myTabwidgetDocuments = new QTabWidget;
    //if ( m_bLogging ) { std::clog << "Created myTabwidgetDocuments" << std::endl; }
    m_pTabwidgetRawPreview = new QTabWidget;

    m_pWebview = new QWebView( this );

    m_pInsertSyntaxElement = new CInsertSyntaxElement( m_pParser->getTransTemplate(),
                                                       m_pParser->getTransImage(),
                                                       m_pParser->getTransTOC() );

    m_pTableTemplate = new CTableTemplate( m_pEditor,
										   m_UserAppDir,
										   m_tmpPreviewImgDir,
										   m_pInterWikiLinks->getInterwikiLinks(),
										   m_pInterWikiLinks->getInterwikiLinksUrls(),
										   m_pApp->applicationName(),
										   m_pApp->applicationDirPath(),
										   m_pSettings );

    qDebug() << "End" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::setupEditor()
{
    qDebug() << "Start" << Q_FUNC_INFO;

    // Application icon
    this->setWindowIcon( QIcon(":/images/" + m_pApp->applicationName().toLower() + "_64x64.png") );

    m_pEditor->setFont( m_pSettings->getEditorFont() );

    // Find/replace dialogs
    m_findDialog->setTextEdit(m_pEditor);
    m_findReplaceDialog->setTextEdit(m_pEditor);

    // Connect signals from parser with functions
    connect( m_pParser, SIGNAL(callShowPreview(QString)),
             this, SLOT(showHtmlPreview(QString)) );

    connect( m_pFileOperations, SIGNAL(setMenuLastOpenedEnabled(bool)),
             m_pUi->fileMenuLastOpened, SLOT(setEnabled(bool)) );

    /*
    setCentralWidget(myTabwidgetDocuments);
    myTabwidgetDocuments->setTabPosition(QTabWidget::North);
    myTabwidgetDocuments->setTabsClosable(true);
    myTabwidgetDocuments->setMovable(true);
    myTabwidgetDocuments->setDocumentMode(true);

    myTabwidgetDocuments->addTab(m_pTabwidgetRawPreview, tr("Untitled"));
    */

    m_pFrameLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_pFrameLayout->addWidget( m_pWebview );
    m_pWebviewFrame = new QFrame;
    m_pWebviewFrame->setLayout( m_pFrameLayout );
    m_pWebviewFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    if ( true == m_pSettings->getPreviewAlongside() && true == m_pSettings->getPreviewInEditor() )
    {
        m_pWidgetSplitter = new QSplitter;
        m_pWidgetSplitter->addWidget( m_pEditor );
        //m_pWidgetSplitter->addWidget( m_pWebview );
        m_pWidgetSplitter->addWidget( m_pWebviewFrame );

        connect( m_pFileOperations, SIGNAL(loadedFile()),
                 this, SLOT(previewInyokaPage()) );

        setCentralWidget( m_pWidgetSplitter );
        m_pWidgetSplitter->restoreState( m_pSettings->getSplitterState() );

        // Show an empty website after start
        this->previewInyokaPage();
    }
    else
    {
        setCentralWidget( m_pTabwidgetRawPreview );
        m_pTabwidgetRawPreview->setTabPosition(QTabWidget::West);
        m_pTabwidgetRawPreview->addTab(m_pEditor, tr("Raw format"));

        m_pTabwidgetRawPreview->addTab(m_pWebviewFrame, tr("Preview"));
        if ( false == m_pSettings->getPreviewInEditor() )
        {
            m_pTabwidgetRawPreview->setTabEnabled(m_pTabwidgetRawPreview->indexOf(m_pWebviewFrame), false);
        }
    }

    connect( m_pWebview, SIGNAL(loadFinished(bool)),
             this, SLOT(loadPreviewFinished(bool)) );

    m_pFileOperations->setCurrentFile("");
    this->setUnifiedTitleAndToolBarOnMac(true);

    connect( m_pDownloadModule, SIGNAL(sendArticleText(QString, QString)),
             this, SLOT(displayArticleText(QString, QString)) );

    // Restore window and toolbar settings
    // Settings have to be restored after toolbars are created!
    this->restoreGeometry(m_pSettings->getWindowGeometry());
    this->restoreState(m_pSettings->getWindowState());  // Restore toolbar position etc.

    if ( false == m_pSettings->getPreviewAlongside() )
    {
        this->removeToolBar(m_pUi->browserBar);
    }

    m_pUi->aboutAct->setText( m_pUi->aboutAct->text() + " " + m_pApp->applicationName() );

    qDebug() << "End" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void CInyokaEdit::createActions()
{
    qDebug() << "Start" << Q_FUNC_INFO;

    // File menu
    // New file

    m_pUi->newAct->setShortcuts(QKeySequence::New);
    connect( m_pUi->newAct, SIGNAL(triggered()),
             m_pFileOperations, SLOT(newFile()) );

    // Open file
    m_pUi->openAct->setShortcuts(QKeySequence::Open);
    connect( m_pUi->openAct, SIGNAL(triggered()),
             m_pFileOperations, SLOT(open()) );

    // Clear recent files list
    m_pClearRecentFilesAct = new QAction(tr("Clear list"), this);
    connect( m_pClearRecentFilesAct, SIGNAL(triggered()),
             m_pFileOperations, SLOT(clearRecentFiles()) );

    // Save file
    m_pUi->saveAct->setShortcuts(QKeySequence::Save);
    connect( m_pUi->saveAct, SIGNAL(triggered()),
             m_pFileOperations, SLOT(save()) );

    // Save file as...
    m_pUi->saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect( m_pUi->saveAsAct, SIGNAL(triggered()),
             m_pFileOperations, SLOT(saveAs()) );

    // Print preview
    m_pUi->printPreviewAct->setShortcut(QKeySequence::Print);
    connect( m_pUi->printPreviewAct, SIGNAL(triggered()),
             m_pFileOperations, SLOT(printPreview()) );
    if ( false == m_pSettings->getPreviewAlongside() )
    {
        m_pUi->printPreviewAct->setEnabled(false);
    }

    // Exit application
    m_pUi->exitAct->setShortcuts(QKeySequence::Quit);
    connect( m_pUi->exitAct, SIGNAL(triggered()),
             this, SLOT(close()) );

    // ---------------------------------------------------------------------------------------------
    // EDIT MENU

    // Find
    m_pUi->searchAct->setShortcuts(QKeySequence::Find);
    connect( m_pUi->searchAct, SIGNAL(triggered()),
             m_findDialog, SLOT(show()) );

    // Replace
    m_pUi->replaceAct->setShortcuts(QKeySequence::Replace);
    connect( m_pUi->replaceAct, SIGNAL(triggered()),
             m_findReplaceDialog, SLOT(show()) );

    // Find next
    m_pUi->findNextAct->setShortcuts(QKeySequence::FindNext);
    connect( m_pUi->findNextAct, SIGNAL(triggered()),
             m_findDialog, SLOT(findNext()) );

    // Find previous
    m_pUi->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
    connect( m_pUi->findPreviousAct, SIGNAL(triggered()),
             m_findDialog, SLOT(findPrev()) );


    // Initialize cut / copy / redo / undo
    m_pUi->cutAct->setEnabled(false);
    m_pUi->copyAct->setEnabled(false);
    m_pUi->undoAct->setEnabled(false);
    m_pUi->redoAct->setEnabled(false);

    // ---------------------------------------------------------------------------------------------
    // TOOLS MENU

    // Spell checker
    m_pUi->spellCheckerAct->setShortcut(Qt::Key_F7);
    connect( m_pUi->spellCheckerAct, SIGNAL(triggered()),
             this, SLOT(checkSpelling()) );
#ifdef DISABLE_SPELLCHECKER
    m_pUi->spellCheckerAct->setVisible(false);
#endif

    // Download styles
    connect( m_pUi->DownloadInyokaStylesAct, SIGNAL(triggered()),
             m_pDownloadModule, SLOT(loadInyokaStyles()) );
#if defined _WIN32
    m_pUi->DownloadInyokaStylesAct->setDisabled( true );
#endif

    // Clear temp. image download folder
    connect( m_pUi->deleteTempImagesAct, SIGNAL(triggered()),
             this, SLOT(deleteTempImages()) );

    // ---------------------------------------------------------------------------------------------

    // Show html preview
    m_pUi->previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
    connect( m_pUi->previewAct, SIGNAL(triggered()),
             this, SLOT(previewInyokaPage()) );

    // Click on tabs of widget - int = index of tab
    connect( m_pTabwidgetRawPreview, SIGNAL(currentChanged(int)),
             this, SLOT(previewInyokaPage(int)) );

    // Download Inyoka article
    connect( m_pUi->downloadArticleAct, SIGNAL(triggered()),
             this, SLOT(downloadArticle()) );

    // ---------------------------------------------------------------------------------------------

    mySigMapTextSamples = new QSignalMapper(this);

    // Insert headline
    m_pHeadlineBox = new QComboBox();
    m_pHeadlineBox->setStatusTip(tr("Insert a headline - 5 headline steps are supported"));

    // Insert sample
    m_pTextmacrosBox = new QComboBox();
    m_pTextmacrosBox->setStatusTip(tr("Insert text sample"));

    // Insert text format
    m_pTextformatBox = new QComboBox();
    m_pTextformatBox->setStatusTip(tr("Insert text format"));

    // ---------------------------------------------------------------------------------------------
    // INSERT SYNTAX ELEMENTS

    // Insert bold element
    m_pUi->boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    mySigMapTextSamples->setMapping(m_pUi->boldAct, "boldAct");
    connect( m_pUi->boldAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    // Insert italic element
    m_pUi->italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    mySigMapTextSamples->setMapping(m_pUi->italicAct, "italicAct");
    connect( m_pUi->italicAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    // Insert monotype element
    mySigMapTextSamples->setMapping(m_pUi->monotypeAct, "monotypeAct");
    connect( m_pUi->monotypeAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    // Insert wiki link
    mySigMapTextSamples->setMapping(m_pUi->wikilinkAct, "wikilinkAct");
    connect( m_pUi->wikilinkAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    // Insert extern link
    mySigMapTextSamples->setMapping(m_pUi->externalLinkAct, "externalLinkAct");
    connect( m_pUi->externalLinkAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    // Insert image
    mySigMapTextSamples->setMapping(m_pUi->imageAct, "imageAct");
    connect( m_pUi->imageAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    // Code block + syntax highlighting
    mySigMapCodeHighlight = new QSignalMapper(this);
    QStringList sListHighlightText, sListHighlightLang;
    sListHighlightText << tr("Raw text") << tr("Code without highlighting") << "Bash" << "C" <<
                          "C#" << "C++" << "CSS" << "D" << "Django / Jinja Templates" <<
                          "HTML" << "IRC Logs" << "Java" << "JavaScript" << "Perl" << "PHP" <<
                          "Python" << "Python Console Sessions" << "Python Tracebacks" <<
                          "Ruby" << "sources.list" << "SQL" << "XML";

    sListHighlightLang << ""  << "text" << "bash" << "c" <<
                          "csharp" << "cpp" << "css" << "d" << "html+django" <<
                          "html" << "irc" << "java" << "js" << "perl" << "html+php" <<
                          "python" << "pycon" << "pytb" <<
                          "ruby" << "sourceslist" << "sql" << "xml";

    QString sCodeTag("#!" + m_pParser->getTransCodeBlock().toLower() + " ");
    m_pCodePopup = new QToolButton;
    m_pCodePopup->setIcon(QIcon(":/images/code.png"));
    m_pCodePopup->setPopupMode( QToolButton::InstantPopup );
    m_pCodeStyles = new QMenu( m_pCodePopup );

    for ( int i = 0; i < sListHighlightText.size(); i++ ) {
        if (0 != i) { sListHighlightLang[i] = sCodeTag + sListHighlightLang[i]; }
        m_CodeHighlightActions << new QAction( sListHighlightText[i], this );
        mySigMapCodeHighlight->setMapping( m_CodeHighlightActions[i], sListHighlightLang[i] );
        connect( m_CodeHighlightActions[i], SIGNAL(triggered()),
                 mySigMapCodeHighlight, SLOT(map()) );
    }

    m_pCodeStyles->addActions( m_CodeHighlightActions );
    m_pCodePopup->setMenu(m_pCodeStyles);
    m_pUi->inyokaeditorBar->addWidget(m_pCodePopup);

    connect( mySigMapCodeHighlight, SIGNAL(mapped(QString)),
             this, SLOT(insertCodeblock(QString)) );

    // ---------------------------------------------------------------------------------------------
    // TEXT SAMPLES

    mySigMapTextSamples->setMapping(m_pUi->insertUnderConstructionAct, "insertUnderConstructionAct");
    connect( m_pUi->insertUnderConstructionAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertTestedForAct, "insertTestedForAct");
    connect( m_pUi->insertTestedForAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertKnowledgeAct, "insertKnowledgeAct");
    connect( m_pUi->insertKnowledgeAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertTableOfContentsAct, "insertTableOfContentsAct");
    connect( m_pUi->insertTableOfContentsAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertAdvancedAct, "insertAdvancedAct");
    connect( m_pUi->insertAdvancedAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertAwardAct, "insertAwardAct");
    connect( m_pUi->insertAwardAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertImageAct, "imageAct");  // insertImageAct = imageAct !
    connect( m_pUi->insertImageAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertImageUnderlineAct, "insertImageUnderlineAct");
    connect( m_pUi->insertImageUnderlineAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertImageCollectionAct, "insertImageCollectionAct");
    connect( m_pUi->insertImageCollectionAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertImageCollectionInTextAct, "insertImageCollectionInTextAct");
    connect( m_pUi->insertImageCollectionInTextAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertBashCommandAct, "insertBashCommandAct");
    connect( m_pUi->insertBashCommandAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertNoticeAct, "insertNoticeAct");
    connect( m_pUi->insertNoticeAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertWarningAct, "insertWarningAct");
    connect( m_pUi->insertWarningAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertExpertsAct, "insertExpertsAct");
    connect( m_pUi->insertExpertsAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertPackageListAct, "insertPackageListAct");
    connect( m_pUi->insertPackageListAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertPackageInstallAct, "insertPackageInstallAct");
    connect( m_pUi->insertPackageInstallAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertPPAAct, "insertPPAAct");
    connect( m_pUi->insertPPAAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyRepoAct, "insertThirdPartyRepoAct");
    connect( m_pUi->insertThirdPartyRepoAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyRepoAuthAct, "insertThirdPartyRepoAuthAct");
    connect( m_pUi->insertThirdPartyRepoAuthAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyPackageAct, "insertThirdPartyPackageAct");
    connect( m_pUi->insertThirdPartyPackageAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertImprovableAct, "insertImprovableAct");
    connect( m_pUi->insertImprovableAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertFixMeAct, "insertFixMeAct");
    connect( m_pUi->insertFixMeAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertLeftAct, "insertLeftAct");
    connect( m_pUi->insertLeftAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyPackageWarningAct, "insertThirdPartyPackageWarningAct");
    connect( m_pUi->insertThirdPartyPackageWarningAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyRepoWarningAct, "insertThirdPartyRepoWarningAct");
    connect( m_pUi->insertThirdPartyRepoWarningAct, SIGNAL(triggered()),
             mySigMapTextSamples, SLOT(map()) );

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartySoftwareWarningAct, "insertThirdPartySoftwareWarningAct");
    connect( m_pUi->insertThirdPartySoftwareWarningAct, SIGNAL(triggered()),
            mySigMapTextSamples, SLOT(map()) );

    connect( mySigMapTextSamples, SIGNAL(mapped(QString)),
            this, SLOT(insertTextSample(QString)) );

    connect( m_pUi->insertTableAct, SIGNAL(triggered()),
    		m_pTableTemplate, SLOT(newTable()) );

    // ---------------------------------------------------------------------------------------------
    // INTERWIKI LINKS MENU

    m_pSigMapInterWikiLinks = new QSignalMapper(this);
    QList <QAction *> emptyActionList;
    emptyActionList.clear();

    for ( int i = 0; i < m_pInterWikiLinks->getInterwikiLinksGroups().size(); i++ )
    {
        m_iWikiLinksActions << emptyActionList;
        for ( int j = 0; j < m_pInterWikiLinks->getInterwikiLinksNames()[i].size(); j++ )
        {
            // Path from normal installation
            if ( QFile::exists("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks") )
            {
                m_iWikiLinksActions[i] << new QAction(QIcon("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks/" + m_pInterWikiLinks->getInterwikiLinksIcons()[i][j]), m_pInterWikiLinks->getInterwikiLinksNames()[i][j], this);
            }
            // No installation: Use app path
            else
            {
                m_iWikiLinksActions[i] << new QAction(QIcon(m_pApp->applicationDirPath() + "/iWikiLinks/" + m_pInterWikiLinks->getInterwikiLinksIcons()[i][j]), m_pInterWikiLinks->getInterwikiLinksNames()[i][j], this);
            }

            m_pSigMapInterWikiLinks->setMapping( m_iWikiLinksActions[i][j], QString::number(i) + "," + QString::number(j) );
            connect( m_iWikiLinksActions[i][j], SIGNAL(triggered()),
                     m_pSigMapInterWikiLinks, SLOT(map()) );
        }
    }

    connect( m_pSigMapInterWikiLinks, SIGNAL(mapped(QString)),
             this, SLOT(insertInterwikiLink(QString)) );

    // ---------------------------------------------------------------------------------------------
    // ABOUT MENU

    // Report a bug using apport
    connect( m_pUi->reportBugAct, SIGNAL(triggered()),
             this, SLOT(reportBug()) );

    // Open about windwow
    connect( m_pUi->aboutAct, SIGNAL(triggered()),
             this, SLOT(about()) );

    qDebug() << "End" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate menus
void CInyokaEdit::createMenus()
{
    qDebug() << "Start" << Q_FUNC_INFO;

    QDir articleTemplateDir("");

    // File menu (new from template)
    if ( QFile::exists("/usr/share/" + m_pApp->applicationName().toLower() + "/templates/" + m_pSettings->getTemplateLanguage() + "/articles"))
    {
        articleTemplateDir.setPath("/usr/share/" + m_pApp->applicationName().toLower() + "/templates/" + m_pSettings->getTemplateLanguage() + "/articles");
    }
    // No installation: Use app path
    else
    {
        articleTemplateDir.setPath( m_pApp->applicationDirPath() + "/templates/" + m_pSettings->getTemplateLanguage() + "/articles" );
    }

    unsigned short nTplFileCount = 0;
    if ( articleTemplateDir.exists() )
    {
        m_pSigMapOpenTemplate = new QSignalMapper(this);
        QFileInfoList fiListFiles = articleTemplateDir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files );
        for ( int nFile = 0; nFile < fiListFiles.count(); nFile++ )
        {
            if ( "tpl" == fiListFiles.at(nFile).suffix()) {
                m_OpenTemplateFilesActions << new QAction( fiListFiles.at(nFile).baseName().replace("_", " "), this );
                m_pSigMapOpenTemplate->setMapping (m_OpenTemplateFilesActions.last(), fiListFiles.at(nFile).absoluteFilePath() );
                connect( m_OpenTemplateFilesActions.last(), SIGNAL(triggered()),
                         m_pSigMapOpenTemplate, SLOT(map()) );
                nTplFileCount++;
            }
        }
        m_pUi->fileMenuFromTemplate->addActions( m_OpenTemplateFilesActions );
        connect( m_pSigMapOpenTemplate, SIGNAL(mapped(QString)),
                 m_pFileOperations, SLOT(loadFile(QString)) );

        if ( 0 == nTplFileCount ){
            m_pUi->fileMenuFromTemplate->setDisabled(true);
        }
    }
    else
    {
        m_pUi->fileMenuFromTemplate->setDisabled(true);
    }

    // File menu (recent opened files)
    m_pUi->fileMenuLastOpened->addActions( m_pFileOperations->getLastOpenedFiles() );
    m_pUi->fileMenuLastOpened->addSeparator();
    m_pUi->fileMenuLastOpened->addAction( m_pClearRecentFilesAct );
    if ( 0 == m_pSettings->getRecentFiles().size() )
    {
        m_pUi->fileMenuLastOpened->setEnabled(false);
    }

    // Insert interwiki-links menu
    for ( int i = 0; i < m_pInterWikiLinks->getInterwikiLinksGroups().size(); i++ )
    {
        // Path from normal installation
        if ( QFile::exists("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks") )
        {
            m_iWikiGroups.append( m_pUi->iWikiMenu->addMenu(QIcon("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks/" + m_pInterWikiLinks->getInterwikiLinksGroupIcons()[i]), m_pInterWikiLinks->getInterwikiLinksGroups()[i]) );
        }
        // No installation: Use app path
        else
        {
            m_iWikiGroups.append( m_pUi->iWikiMenu->addMenu(QIcon(m_pApp->applicationDirPath() + "/iWikiLinks/" + m_pInterWikiLinks->getInterwikiLinksGroupIcons()[i]), m_pInterWikiLinks->getInterwikiLinksGroups()[i]) );
        }
        m_iWikiGroups[i]->addActions( m_iWikiLinksActions[i] );
    }

    qDebug() << "End" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate tool bars
void CInyokaEdit::createToolBars()
{
    qDebug() << "Start" << Q_FUNC_INFO;

    // Tool bar for combo boxes (samples and macros)
    m_pUi->samplesmacrosBar->addWidget( m_pHeadlineBox );

    // Headline combo box
    QString sHeadline = tr("Headline");
    QString sHeadlineStep = tr("Step", "GUI: Headline combo box");
    m_pHeadlineBox->addItem(sHeadline);
    m_pHeadlineBox->insertSeparator(1);
    m_pHeadlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 1");
    m_pHeadlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 2");
    m_pHeadlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 3");
    m_pHeadlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 4");
    m_pHeadlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 5");
    connect(m_pHeadlineBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownHeadline(int)));

    // Macros combo box
    m_pUi->samplesmacrosBar->addWidget( m_pTextmacrosBox );
    m_pTextmacrosBox->addItem(tr("Text macros", "GUI: Text macro combo box"));
    m_pTextmacrosBox->insertSeparator(1);
    m_pTextmacrosBox->addItem(tr("Under construction", "GUI: Text macro combo box - Work in progress"));
    m_pTextmacrosBox->addItem(tr("Table of contents", "GUI: Text macro combo box - Table of contents"));
    m_pTextmacrosBox->addItem(tr("Tested for", "GUI: Text macro combo box - Tested for"));
    m_pTextmacrosBox->addItem(tr("Package installation", "GUI: Text macro combo box - Package installation"));
    m_pTextmacrosBox->addItem(tr("Bash command", "GUI: Text macro combo box - Bash command"));
    m_pTextmacrosBox->addItem(tr("PPA sample", "GUI: Text macro combo box - PPA sample"));
    m_pTextmacrosBox->addItem(tr("Notice box", "GUI: Text macro combo box - Notice box"));
    m_pTextmacrosBox->addItem(tr("Third-party repo warning", "GUI: Text macro combo box - Third-party repo warning"));
    m_pTextmacrosBox->addItem(tr("Warning box", "GUI: Text macro combo box - Warning box"));
    m_pTextmacrosBox->addItem(tr("Experts information", "GUI: Text macro combo box - Experts info box"));
    m_pTextmacrosBox->addItem(tr("Keys", "GUI: Text macro combo box - Keys"));
    m_pTextmacrosBox->addItem(tr("Table", "GUI: Text macro combo box - Table"));
    m_pTextmacrosBox->addItem(tr("Game info box", "GUI: Game info box"));
    connect(m_pTextmacrosBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextmacro(int)));

    // Text format combo box
    m_pUi->samplesmacrosBar->addWidget( m_pTextformatBox );
    m_pTextformatBox->addItem(tr("Text format", "GUI: Text format combo box"));
    m_pTextformatBox->insertSeparator(1);
    m_pTextformatBox->addItem(tr("Folders", "GUI: Text format folders"));
    m_pTextformatBox->addItem(tr("Menu entries", "GUI: Text format menu entries"));
    m_pTextformatBox->addItem(tr("Files", "GUI: Text format files"));
    m_pTextformatBox->addItem(tr("Command", "GUI: Text format command"));
    connect(m_pTextformatBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextformat(int)));

    // Browser buttons
    connect( m_pUi->goBackBrowserAct, SIGNAL(triggered()),
             m_pWebview, SLOT(back()) );
    connect( m_pUi->goForwardBrowserAct, SIGNAL(triggered()),
            m_pWebview, SLOT(forward()) );
    connect( m_pUi->reloadBrowserAct, SIGNAL(triggered()),
             m_pWebview, SLOT(reload()) );
    connect( m_pWebview, SIGNAL(urlChanged(QUrl)),
             this, SLOT(clickedLink()) );

    qDebug() << "End" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Call parser
void CInyokaEdit::previewInyokaPage( const int nIndex )
{
    // Call parser if iIndex == index of m_pWebview -> Click on tab preview
    // or if iIndex == 999 -> Default parameter value when calling the function (e.g.) by clicking on button preview
    if ( m_pTabwidgetRawPreview->indexOf(m_pWebviewFrame) == nIndex || 999 == nIndex )
    {
        // Only disable buttons if preview is not shown alongside editor
        if ( false == m_pSettings->getPreviewAlongside() )
        {
            // Disable editor and insert samples/macros toolbars
            m_pUi->editMenu->setDisabled(true);
            m_pUi->insertTextSampleMenu->setDisabled(true);
            m_pUi->iWikiMenu->setDisabled(true);
            m_pUi->editToolBar->setDisabled(true);
            m_pUi->inyokaeditorBar->setDisabled(true);
            //this->removeToolBar(m_pUi->inyokaeditorBar);
            m_pUi->samplesmacrosBar->setDisabled(true);
            //this->removeToolBar(m_pUi->samplesmacrosBar);
            m_pUi->previewAct->setDisabled(true);
            this->addToolBar(m_pUi->browserBar);
            m_pUi->browserBar->show();

            m_pUi->printPreviewAct->setEnabled(true);
        }

        if ( "" == m_pFileOperations->getCurrentFile() || tr("Untitled") == m_pFileOperations->getCurrentFile() )
        {
            m_pParser->genOutput("");
        }
        else
        {
            QFileInfo fi(m_pFileOperations->getCurrentFile());
            m_pParser->genOutput(fi.fileName());
        }
    }
    else
    {
        // Enable editor and insert samples/macros toolbars again
        m_pUi->editMenu->setEnabled(true);
        m_pUi->insertTextSampleMenu->setEnabled(true);
        m_pUi->iWikiMenu->setEnabled(true);
        m_pUi->editToolBar->setEnabled(true);
        m_pUi->inyokaeditorBar->setEnabled(true);
        //this->addToolBar(m_pUi->inyokaeditorBar);
        //m_pUi->inyokaeditorBar->show();
        m_pUi->samplesmacrosBar->setEnabled(true);
        //this->addToolBar(m_pUi->samplesmacrosBar);
        //m_pUi->samplesmacrosBar->show();
        m_pUi->previewAct->setEnabled(true);
        this->removeToolBar(m_pUi->browserBar);

        m_pUi->printPreviewAct->setEnabled(false);
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// INSERT INYOKA ELEMENTS

// Headline (combobox in toolbar)
void CInyokaEdit::insertDropDownHeadline( const int nSelection )
{
    if ( nSelection > 1 )
    {
        QString sHeadline = tr("Headline");
        QString sHeadTag = "";

        // Generate headline tag
        for ( int i = 1; i < nSelection; i++ )
        {
            sHeadTag.append("=");
        }

        // Some text was selected
        if ( m_pEditor->textCursor().selectedText() != "" )
        {
            m_pEditor->insertPlainText( sHeadTag + " " + m_pEditor->textCursor().selectedText() + " " + sHeadTag );
        }
        // Select text sHeadline if no text was selected
        else
        {
            m_pEditor->insertPlainText( sHeadTag + " " + sHeadline + " " + sHeadTag );

            QTextCursor myTextCursor = m_pEditor->textCursor();
            myTextCursor.setPosition( m_pEditor->textCursor().position() - sHeadline.length() - nSelection );
            myTextCursor.setPosition( m_pEditor->textCursor().position() - nSelection, QTextCursor::KeepAnchor );
            m_pEditor->setTextCursor( myTextCursor );
        }
    }

    // Reset selection
    m_pHeadlineBox->setCurrentIndex(0);

    m_pEditor->setFocus();
}

// Macro (combobox in toolbar)
void CInyokaEdit::insertDropDownTextmacro( const int nSelection )
{

    if ( nSelection != 0 && nSelection != 1 )
    {
        // -1 because of separator (considered as "item")
        switch ( nSelection-1 )
        {
            default:
            case 1:  // Under construction (Baustelle)
                insertTextSample("insertUnderConstructionAct");
                break;
            case 2:  // Table of contents (Inhaltsverzeichnis)
                insertTextSample("insertTableOfContentsAct");
                break;
            case 3:  // Tested for (Getestet)
                insertTextSample("insertTestedForAct");
                break;
            case 4:  // Package installation (Paketinstallation)
                insertTextSample("insertPackageInstallAct");
                break;
            case 5:  // Bash command (Befehl)
                insertTextSample("insertBashCommandAct");
                break;
            case 6:  // PPA sample (PPA-Vorlage)
                insertTextSample("insertPPAAct");
                break;
            case 7:  // Notice (Hinweis)
                insertTextSample("insertNoticeAct");
                break;
            case 8:  // Third-party repo warning (Fremdquelle-Warnung)
                insertTextSample("insertThirdPartyRepoWarningAct");
                break;
            case 9:  // Warning (Warnung)
                insertTextSample("insertWarningAct");
                break;
            case 10:  // Expert information (Experten-Info)
                insertTextSample("insertExpertsAct");
                break;
            case 11:  // Keys (Tasten)
                insertTextSample("Keys");
                break;
            case 12:
                // Table (Tabelle)
                insertTextSample("Table");
                break;
            case 13:
                // Game info box
                insertTextSample("GameInfoBox");
                break;
        }
        // Reset selection
        m_pTextmacrosBox->setCurrentIndex(0);

        m_pEditor->setFocus();
    }
}

// Text format (combobox in toolbar)
void CInyokaEdit::insertDropDownTextformat( const int nSelection )
{
    bool bSelected = false;
    QString sInsertedText = "";
    unsigned short iFormatLength = 0;

    // Some text was selected
    if ( m_pEditor->textCursor().selectedText() != "" )
    {
        bSelected = true;
    }

    if ( nSelection != 0 && nSelection != 1 )
    {
        // -1 because of separator (considered as "item")
        switch ( nSelection-1 )
        {
            default:
            case 1:  // Folders
                if ( bSelected )
                {
                    m_pEditor->insertPlainText( "'''" + m_pEditor->textCursor().selectedText() + "'''" );
                }
                else
                {
                    sInsertedText = tr("Folders", "Text format: Folders");
                    iFormatLength = 3;
                    m_pEditor->insertPlainText( "'''" + sInsertedText + "'''" );
                }
                break;
            case 2:  // Menus
                if ( bSelected )
                {
                    m_pEditor->insertPlainText( "''\"" + m_pEditor->textCursor().selectedText() + "\"''" );
                }
                else
                {
                    sInsertedText = tr("Menu -> sub menu -> menu entry", "Text format: Menu entries example");
                    iFormatLength = 3;
                    m_pEditor->insertPlainText( "''\"" + sInsertedText + "\"''" );
                }
                break;
            case 3:  // Files
                if ( bSelected )
                {
                    m_pEditor->insertPlainText( "'''" + m_pEditor->textCursor().selectedText() + "'''" );
                }
                else
                {
                    sInsertedText = tr("Files", "GUI: Text format: Files");
                    iFormatLength = 3;
                    m_pEditor->insertPlainText( "'''" + sInsertedText + "'''" );
                }
                break;
            case 4:  // Commands
                if ( bSelected )
                {
                    m_pEditor->insertPlainText( "`" + m_pEditor->textCursor().selectedText() + "`" );
                }
                else
                {
                    sInsertedText = tr("Command", "Text format: Command");
                    iFormatLength = 1;
                    m_pEditor->insertPlainText( "`" + sInsertedText + "`" );
                }
                break;
        }

        // Reset selection
        m_pTextformatBox->setCurrentIndex(0);

        if ( !bSelected )
        {
            QTextCursor myTextCursor = m_pEditor->textCursor();
            myTextCursor.setPosition( m_pEditor->textCursor().position() - sInsertedText.length() - iFormatLength );
            myTextCursor.setPosition( m_pEditor->textCursor().position() - iFormatLength, QTextCursor::KeepAnchor );
            m_pEditor->setTextCursor( myTextCursor );
        }

        m_pEditor->setFocus();
    }
}

// Insert text sample / syntax element
void CInyokaEdit::insertTextSample( const QString &sMenuEntry )
{
    m_pEditor->insertPlainText( QString::fromUtf8(m_pInsertSyntaxElement->getElementInyokaCode(sMenuEntry.toStdString(), m_pEditor->textCursor().selectedText().toStdString()).c_str()) );
    m_pEditor->setFocus();
}

// Insert interwiki-link
void CInyokaEdit::insertInterwikiLink( const QString &sMenuEntry )
{
    // Get indices for links
    QStringList sTmp = sMenuEntry.split(",");

    // Check if right number of indices found
    if ( sTmp.size() == 2 )
    {
        // No text selected
        if ( m_pEditor->textCursor().selectedText() == "" )
        {
            QString sSitename = tr("Sitename", "InterWiki links: Common sitename");
            QString sText = tr("Text", "Interwiki links: Common text");

            // Insert InterWiki-Link
            m_pEditor->insertPlainText( "[" + m_pInterWikiLinks->getInterwikiLinks()[sTmp[0].toInt()][sTmp[1].toInt()] + ":" + sSitename + ":" + sText + "]" );

            // Select site name in InterWiki-Link
            QTextCursor textCursor = m_pEditor->textCursor();
            textCursor.setPosition( m_pEditor->textCursor().position() - sSitename.length() - sText.length() - 2);
            textCursor.setPosition( m_pEditor->textCursor().position() - sText.length() - 2, QTextCursor::KeepAnchor );
            m_pEditor->setTextCursor( textCursor );
        }
        // Some text is selected
        else
        {
            // Insert InterWiki-Link with selected text
            m_pEditor->insertPlainText( "[" + m_pInterWikiLinks->getInterwikiLinks()[sTmp[0].toInt()][sTmp[1].toInt()] + ":" + m_pEditor->textCursor().selectedText() + ":]" );
        }
    }
    // Problem with indices
    else
    {
        QMessageBox::warning( this, m_pApp->applicationName(), "Error while inserting InterWiki link: InterWiki indice" );
    }

    m_pEditor->setFocus();
}

// Insert code block
void CInyokaEdit::insertCodeblock( const QString &sCodeStyle )
{
    // No text selected
    if ( m_pEditor->textCursor().selectedText() == "" )
    {
        QString sCode("Code");

        // Insert code block
        m_pEditor->insertPlainText( "{{{" + sCodeStyle + "\n" + sCode + "\n}}}\n" );

        // Select the word "code"
        QTextCursor textCursor = m_pEditor->textCursor();
        textCursor.setPosition( m_pEditor->textCursor().position() - sCode.length() - 5);
        textCursor.setPosition( m_pEditor->textCursor().position() - 5, QTextCursor::KeepAnchor );
        m_pEditor->setTextCursor( textCursor );
    }
    // Some text is selected
    else
    {
        // Insert code block with selected text
        m_pEditor->insertPlainText( "{{{" + sCodeStyle + "\n" + m_pEditor->textCursor().selectedText() + "\n}}}\n" );
    }

    m_pEditor->setFocus();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::downloadArticle()
{
    if ( m_pFileOperations->maybeSave() )
    {
        bool bSuccess = m_pDownloadModule->downloadArticle( m_tmpPreviewImgDir, m_pSettings->getInyokaUrl(), m_pSettings->getAutomaticImageDownload() );
        if ( bSuccess )
        {
            this->previewInyokaPage();
        }
    }
}

void CInyokaEdit::displayArticleText( const QString &sArticleText, const QString &sSitename )
{
    m_pEditor->setPlainText(sArticleText);
    m_pFileOperations->setCurrentFile(sSitename);
    m_pEditor->document()->setModified(true);
    this->documentWasModified();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
void CInyokaEdit::showHtmlPreview( const QString &sFilename )
{
    m_pWebview->history()->clear();  // Clear history (clicked links)

    if ( false == m_pSettings->getPreviewInEditor() )
    {
        // Open html-file in system web browser
        QDesktopServices::openUrl( QUrl::fromLocalFile(sFilename) );
    }
    else
    {
        m_pWebview->load( QUrl::fromLocalFile(sFilename) );
    }
}

// Wait until loading has finished
void CInyokaEdit::loadPreviewFinished( bool bSuccess )
{
    if ( bSuccess )
    {
        m_pTabwidgetRawPreview->setCurrentIndex( m_pTabwidgetRawPreview->indexOf(m_pWebviewFrame) );
        // Enable / disbale back button
        if ( m_pWebview->history()->canGoBack() )
        {
            m_pUi->goBackBrowserAct->setEnabled(true);
        }
        else
        {
            m_pUi->goBackBrowserAct->setEnabled(false);
        }

        // Enable / disable forward button
        if ( m_pWebview->history()->canGoForward() )
        {
            m_pUi->goForwardBrowserAct->setEnabled(true);
        }
        else
        {
            m_pUi->goForwardBrowserAct->setEnabled(false);
        }
    }
    else
    {
        QMessageBox::warning( this, m_pApp->applicationName(), tr("Error while loading preview.") );
    }
}

// A link on preview page was clicked
void CInyokaEdit::clickedLink()
{
    // Disable forward / backward button
    m_pUi->goForwardBrowserAct->setEnabled(false);
    m_pUi->goBackBrowserAct->setEnabled(false);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

bool CInyokaEdit::eventFilter( QObject *obj, QEvent *event )
{
    if ( obj == m_pEditor )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // ---------------------------------------------------------------------------
        // Bug fix for LP: #922808
        Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
        bool isSHIFT = keyMod.testFlag(Qt::ShiftModifier);
        bool isCTRL = keyMod.testFlag(Qt::ControlModifier);

        // CTRL + SHIFT + arrow right
        if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Right &&
             isSHIFT && isCTRL )
        {
            QTextCursor cursor(m_pEditor->textCursor());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(cursor);
            return true;
        }
        // CTRL + arrow right
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Right &&
             !isSHIFT && isCTRL )
        {
            m_pEditor->moveCursor(QTextCursor::Right);
            m_pEditor->moveCursor(QTextCursor::EndOfWord);
            return true;
        }

        // ----------------------------------------------
        // Bug fix for LP: #889321

        // CTRL + SHIFT arrow down
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Up &&
             isSHIFT && isCTRL )
        {
            QTextCursor cursor(m_pEditor->textCursor());
            cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(cursor);
            return true;
        }

        // CTRL + SHIFT arrow down
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Down &&
             isSHIFT && isCTRL )
        {
            QTextCursor cursor(m_pEditor->textCursor());
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(cursor);
            return true;
        }
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // CTRL + SHIFT + T (only for debugging)
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_T &&
             isSHIFT && isCTRL )
        {
            static bool bToggle = false;
            static QTextDocument docBackup("");

            if ( !bToggle ) {
                docBackup.setPlainText( m_pEditor->document()->toPlainText() );
                m_pParser->replaceTemplates( m_pEditor->document() );
            }
            else {
                m_pEditor->setText( docBackup.toPlainText() );
            }
            bToggle = !bToggle;
            return true;
        }
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        // Reload preview at RETURN if preview alongside
        if ( event->type() == QEvent::KeyPress )
        {

            switch( keyEvent->key() )
            {
                case Qt::Key_Enter:
                case Qt::Key_Return:
                case Qt::Key_F5:
                    if ( true == m_pSettings->getPreviewAlongside() )
                    {
                        previewInyokaPage();
                    }
                break;
            }
            return false;
        }
        else
        {
            return QObject::eventFilter( obj, event );
        }
    }
    else
    {
        return QObject::eventFilter( obj, event );
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::checkSpelling()
{
  #ifndef DISABLE_SPELLCHECKER
    QString sDictPath("");

    // Standard path for Hunspell (Linux only)
    if ( QDir("/usr/share/hunspell").exists() )
    {
        sDictPath = "/usr/share/hunspell/" + m_pSettings->getSpellCheckerLanguage();
    }
    // Otherwise look for MySpell dictionary (Linx only)
    else if ( QDir("/usr/share/myspell/dicts").exists() )
    {
        sDictPath = "/usr/share/myspell/dicts/" + m_pSettings->getSpellCheckerLanguage();
    }
    // Fallback and for Windows look in app dir
    else
    {
        sDictPath = m_pApp->applicationDirPath() + "/dicts/" + m_pSettings->getSpellCheckerLanguage();
    }

    if ( !QFile::exists(sDictPath + ".dic") || !QFile::exists(sDictPath + ".aff") )
    {
        QMessageBox::critical( this, m_pApp->applicationName(), "Error: Spell checker dictionary file does not exist!" );
        return;
    }

    QString sUserDict= m_UserAppDir.absolutePath() + "/userDict_" + m_pSettings->getSpellCheckerLanguage() + ".txt";
    if ( !QFile::exists(sUserDict) )
    {
        QFile userDictFile(sUserDict);
        if( userDictFile.open(QIODevice::WriteOnly) )
        {
            userDictFile.close();
        }
        else {
            QMessageBox::warning( 0, m_pApp->applicationName(), "User dictionary file could not be created." );
        }
    }
    CSpellChecker *spellChecker = new CSpellChecker( sDictPath, sUserDict, this );
    spellChecker->start(m_pEditor);

    if ( spellChecker != NULL )
    {
        delete spellChecker;
    }
    spellChecker = NULL;

    QMessageBox::information( this, m_pApp->applicationName(), tr("Spell check has finished.") );
  #endif
}


// -----------------------------------------------------------------------------------------------

// Delete images in temp. download folder (images downloaded with articles)
void CInyokaEdit::deleteTempImages()
{
    int nRet = QMessageBox::question( this, m_pApp->applicationName(), tr("Do you really want to delete all images downloaded with articles?"), QMessageBox::Yes | QMessageBox::No);

    if ( QMessageBox::Yes== nRet )
    {
        // Remove all files in current folder
        QFileInfoList fiListFiles = m_tmpPreviewImgDir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files );
        for ( int nFile = 0; nFile < fiListFiles.count(); nFile++ )
        {
            if ( !m_tmpPreviewImgDir.remove( fiListFiles.at(nFile).fileName() ) )
            {
                // Problem while removing
                QMessageBox::warning( this, m_pApp->applicationName(), tr("Could not delete file: ") + fiListFiles.at(nFile).fileName() );
                return;
            }
        }
        QMessageBox::information( this, m_pApp->applicationName(), tr("Images successfully deleted.") );
    }
    else
    {
        return;
    }
}

// -----------------------------------------------------------------------------------------------

// Report a bug
void CInyokaEdit::reportBug()
{
    // Ubuntu: Using Apport, if needed files exist
    if ( QFile::exists( "/usr/bin/ubuntu-bug" )
         && QFile::exists( "/etc/apport/crashdb.conf.d/inyokaedit-crashdb.conf" )
         && QFile::exists( "/usr/share/apport/package-hooks/source_inyokaedit.py" ) )
    {
        // Start apport
        QProcess procApport;
        procApport.start( "ubuntu-bug", QStringList() << m_pApp->applicationName().toLower() );

        if ( !procApport.waitForStarted() )
        {
            QMessageBox::critical( this, m_pApp->applicationName(), tr("Error while starting Apport.") );
            return;
        }
        if ( !procApport.waitForFinished() )
        {
            QMessageBox::critical( this, m_pApp->applicationName(), tr("Error while executing Apport.") );
            return;
        }
    }
    // Not Ubuntu or apport files not found: Load Launchpad bug tracker
    else
    {
        QDesktopServices::openUrl( QUrl("https://bugs.launchpad.net/inyokaedit") );
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Set modified flag for window
void CInyokaEdit::documentWasModified()
{
    m_pTabwidgetRawPreview->setCurrentIndex( m_pTabwidgetRawPreview->indexOf(m_pEditor) );
    this->setWindowModified( m_pEditor->document()->isModified() );
}

// -----------------------------------------------------------------------------------------------

// About info box
void CInyokaEdit::about()
{
    QMessageBox::about(this, tr("About %1", "About dialog <sAppName>").arg( m_pApp->applicationName() ),
                       tr("<b>%1</b> - Editor for Inyoka-based portals<br />"
                          "Version: %2<br /><br />"
                          "&copy; 2011-2012, The %3 developers<br />"
                          "Licence: <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License Version 3</a><br /><br />"
                          "This application uses icons from <a href=\"http://tango.freedesktop.org\">Tango project</a>.",
                          "About dialog text, <sAppName>, <sAppVersion>, <sAppName>")
                       .arg(m_pApp->applicationName())
                       .arg(m_pApp->applicationVersion())
                       .arg(m_pApp->applicationName()));
}

// -----------------------------------------------------------------------------------------------

// Close event (File -> Close or X)
void CInyokaEdit::closeEvent( QCloseEvent *event )
{
    if ( m_pFileOperations->maybeSave() )
    {
        if ( true == m_pSettings->getPreviewAlongside() && true == m_pSettings->getPreviewInEditor() )
        {
            m_pSettings->writeSettings( saveGeometry(), saveState(), m_pWidgetSplitter->saveState() );
        }
        else
        {
            m_pSettings->writeSettings( saveGeometry(), saveState() );
        }
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
