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

#define sVERSION "0.4.0"

#include <QtGui>
#ifndef DISABLE_WEBVIEW
#include <QtWebKit/QWebView>
#endif

#include "CInyokaEdit.h"
#include "ui_CInyokaEdit.h"

CInyokaEdit::CInyokaEdit( QApplication *ptrApp, QWidget *parent ) :
    QMainWindow(parent),
    m_pUi( new Ui::CInyokaEdit ),
    m_pApp( ptrApp ),
    myCompleter( 0 )
{
    m_pUi->setupUi(this);

    m_bLogging = false;

    // Check for command line arguments
    if ( m_pApp->argc() >= 2 )
    {
        QString sTmp = m_pApp->argv()[1];

        if ( "-v"== sTmp || "--version"== sTmp )
        {
            std::cout << m_pApp->argv()[0] << "\t v" << sVERSION << std::endl;
            exit(0);
        }
        else if ("--log" == sTmp)
        {
            m_bLogging = true;
        }
    }

    m_sListCompleter << "Inhaltsverzeichnis(1)]]" << "Vorlage(Getestet, Ubuntuversion)]]" << "Vorlage(Baustelle, Datum, \"Bearbeiter\")]]"
                     << "Vorlage(Fortgeschritten)]]" << "Vorlage(Pakete, \"foo bar\")]]" << trUtf8("Vorlage(Ausbaufähig, \"Begründung\")]]")
                     << trUtf8("Vorlage(Fehlerhaft, \"Begründung\")]]") << trUtf8("Vorlage(Verlassen, \"Begründung\")]]") << "Vorlage(Archiviert, \"Text\")]]"
                     << "Vorlage(Kopie, Seite, Autor)]]" << trUtf8("Vorlage(Überarbeitung, Datum, Seite, Autor)]]") << "Vorlage(Fremd, Paket, \"Kommentar\")]]"
                     << "Vorlage(Fremd, Quelle, \"Kommentar\")]]" << "Vorlage(Fremd, Software, \"Kommentar\")]]" << trUtf8("Vorlage(Award, \"Preis\", Link, Preiskategorie, \"Preisträger\")]]")
                     << "Vorlage(PPA, PPA-Besitzer, PPA-Name)]]" << "Vorlage(Fremdquelle-auth, URL zum PGP-Key)]]" << trUtf8("Vorlage(Fremdquelle-auth, key PGP-Schlüsselnummer)]]")
                     << "Vorlage(Fremdquelle, URL, Ubuntuversion(en), Komponente(n) )]]" << "Vorlage(Fremdpaket, Projekthoster, Projektname, Ubuntuversion(en))]]"
                     << trUtf8("Vorlage(Fremdpaket, \"Anbieter\", URL zu einer Downloadübersicht, Ubuntuversion(en))]]") << "Vorlage(Fremdpaket, \"Anbieter\", dl, URL zu EINEM Download, Ubuntuversion(en))]]"
                     << "Vorlage(Tasten, TASTE)]]" << trUtf8("Bild(name.png, Größe, Ausrichtung)]]") << "Anker(Name)]]" << "[[Vorlage(Bildunterschrift, BILDLINK, BILDBREITE, \"Beschreibung\", left|right)]]"
                     << trUtf8("Vorlage(Bildersammlung, BILDHÖHE\nBild1.jpg, \"Beschreibung 1\"\nBild2.png, \"Beschreibung 2\"\n)]]");

    // Set config and styles/images path
    m_StylesAndImagesDir = QDir::homePath() + "/." + m_pApp->applicationName();

    // Create folder for downloaded article images
    m_tmpPreviewImgDir = m_StylesAndImagesDir.absolutePath() + "/tmpImages";
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
    if ( !m_StylesAndImagesDir.exists() ||
         !QDir(m_StylesAndImagesDir.absolutePath() + "/img").exists() ||
         !QDir(m_StylesAndImagesDir.absolutePath() + "/styles").exists() ||
         !QDir(m_StylesAndImagesDir.absolutePath() + "/Wiki").exists() )
    {
        m_StylesAndImagesDir.mkdir( m_StylesAndImagesDir.absolutePath() );  // Create folder because user may not start download. Folder is needed for preview.
#if !defined _WIN32
        myDownloadModule->loadInyokaStyles();
#endif
    }

    // Check for command line arguments
    // Calling loadFile() only possible AFTER creating objects...
    if ( m_pApp->argc() >= 2 )
    {
        QString sTmp = m_pApp->argv()[1];

        if ("--log" != sTmp)
        {
            myFileOperations->loadFile( m_pApp->argv()[1] );
            this->previewInyokaPage();
        }
        else if ("--log" == sTmp && m_pApp->argc() >= 3)
        {
            myFileOperations->loadFile(m_pApp->argv()[2]);
            this->previewInyokaPage();
        }
    }

    if ( mySettings->getShowStatusbar() )
    {
        this->statusBar()->showMessage(tr("Ready"));
    }

    if ( m_bLogging ) { std::clog << "Created CInyokaEdit" << std::endl; }
}

CInyokaEdit::~CInyokaEdit()
{
    if ( m_pUi != NULL )
    {
        delete m_pUi;
    }
    m_pUi = NULL;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::createObjects()
{
    try
    {
        m_findDialog = new FindDialog(this);  // Has to be create before readSettings
        m_findReplaceDialog = new FindReplaceDialog(this);
        if ( m_bLogging ) { std::clog << "Created find/replace dialogs" << std::endl; }

        mySettings = new CSettings(m_StylesAndImagesDir, m_pApp->applicationName(), *m_findDialog, *m_findReplaceDialog);
        if ( m_bLogging ) { std::clog << "Created mySettings" << std::endl; }
        // Load settings from config file
        mySettings->readSettings();
        if ( m_bLogging ) { std::clog << "Read settings" << std::endl; }

        myDownloadModule = new CDownload(this, m_pApp->applicationName(), m_pApp->applicationDirPath(), m_StylesAndImagesDir);
        if ( m_bLogging ) { std::clog << "Created myDownloadModule" << std::endl; }

        myEditor = new CTextEditor(mySettings->getCodeCompletion());  // Has to be create before find/replace
        if ( m_bLogging ) { std::clog << "Created myEditor" << std::endl; }
//        if ( true == mySettings->getPreviewAlongside() )
//        {
            myEditor->installEventFilter(this);
//        }

        myFileOperations = new CFileOperations(this, myEditor, mySettings, m_pApp->applicationName());
        if ( m_bLogging ) { std::clog << "Created myFileOperations" << std::endl; }

        myCompleter = new QCompleter(m_sListCompleter, this);
        if ( m_bLogging ) { std::clog << "Created myCompleter" << std::endl; }

        myInterWikiLinks = new CInterWiki(m_pApp);  // Has to be created before parser
        if ( m_bLogging ) { std::clog << "Created myInterWikiLinks" << std::endl; }

        myHighlighter = new CHighlighter(myEditor->document());
        if ( m_bLogging ) { std::clog << "Created myHighlighter" << std::endl; }

        myParser = new CParser(myEditor->document(), mySettings->getInyokaUrl(), m_StylesAndImagesDir, m_tmpPreviewImgDir, myInterWikiLinks->getInterwikiLinks(), myInterWikiLinks->getInterwikiLinksUrls());
        if ( m_bLogging ) { std::clog << "Created myParser" << std::endl; }

        /**
         * \todo Add tabs for editing multiple documents.
         */
        //myTabwidgetDocuments = new QTabWidget;
        //if ( m_bLogging ) { std::clog << "Created myTabwidgetDocuments" << std::endl; }
        myTabwidgetRawPreview = new QTabWidget;
        if ( m_bLogging ) { std::clog << "Created myTabwidgetRawPreview" << std::endl; }

        #ifndef DISABLE_WEBVIEW
        myWebview = new QWebView(this);
        if ( m_bLogging ) { std::clog << "Created myWebview" << std::endl; }
        #endif

        myInsertSyntaxElement = new CInsertSyntaxElement;
        if ( m_bLogging ) { std::clog << "Created myInsertSyntaxElement" << std::endl; }
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createObjects()\": " << ba.what() << std::endl;
        QMessageBox::critical(this, m_pApp->applicationName(), "Error while memory allocation: bad_alloc - createObjects()");
        exit (-1);
    }

    if ( m_bLogging ) { std::clog << "Created objects" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::setupEditor()
{
    // Application icon
    this->setWindowIcon( QIcon(":/images/" + m_pApp->applicationName().toLower() + "_64x64.png") );

    // Font settings for editor
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(mySettings->getFontsize());

    myEditor->setFont(font);
    myEditor->setAcceptRichText(false); // Paste plain text only
    myEditor->setCompleter(myCompleter);

    // Text changed
    connect(myEditor->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    // Find/replace dialogs
    m_findDialog->setModal(false);
    m_findDialog->setTextEdit(myEditor);
    m_findReplaceDialog->setModal(false);
    m_findReplaceDialog->setTextEdit(myEditor);

    myCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    myCompleter->setWrapAround(false);

    // Connect signals from parser with functions
    connect(myParser, SIGNAL(callShowPreview(QString)),
            this, SLOT(showHtmlPreview(QString)));

    connect(myFileOperations, SIGNAL(setMenuLastOpenedEnabled(bool)),
            this, SLOT(receiveMenuLastOpenedState(bool)));

    connect(myFileOperations, SIGNAL(setStatusbarMessage(QString)),
            this, SLOT(receiveStatusbarMessage(QString)));

    /*
    setCentralWidget(myTabwidgetDocuments);
    myTabwidgetDocuments->setTabPosition(QTabWidget::North);
    myTabwidgetDocuments->setTabsClosable(true);
    myTabwidgetDocuments->setMovable(true);
    myTabwidgetDocuments->setDocumentMode(true);

    myTabwidgetDocuments->addTab(myTabwidgetRawPreview, tr("Untitled"));
    */

    #ifndef DISABLE_WEBVIEW
    if ( true == mySettings->getPreviewAlongside() && true == mySettings->getPreviewInEditor() )
    {
        try
        {
            myWidgetSplitter = new QSplitter;
            myFrameLayout = new QBoxLayout(QBoxLayout::LeftToRight);
            myWebviewFrame = new QFrame;
        }
        catch ( std::bad_alloc& ba )
        {
            std::cerr << "ERROR: Caught bad_alloc in \"create QSplitter()\": " << ba.what() << std::endl;
            QMessageBox::critical(this, m_pApp->applicationName(), "Error while memory allocation: bad_alloc - create QSplitter");
            exit (-1);
        }

        myWidgetSplitter->addWidget( myEditor );
        myFrameLayout->addWidget( myWebview );
        myWebviewFrame->setLayout( myFrameLayout );
        myWebviewFrame->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
        //myWidgetSplitter->addWidget( myWebview );
        myWidgetSplitter->addWidget( myWebviewFrame );

        connect(myFileOperations, SIGNAL(loadedFile()),
                this, SLOT(previewInyokaPage()));

        setCentralWidget( myWidgetSplitter );
        myWidgetSplitter->restoreState( mySettings->getSplitterState() );

        // Show an empty website after start
        this->previewInyokaPage();
    }
    else
    {
    #endif
        setCentralWidget( myTabwidgetRawPreview );
        myTabwidgetRawPreview->setTabPosition(QTabWidget::West);
        myTabwidgetRawPreview->addTab(myEditor, tr("Raw format"));

    #ifndef DISABLE_WEBVIEW
        myTabwidgetRawPreview->addTab(myWebview, tr("Preview"));
        if ( false == mySettings->getPreviewInEditor() )
        {
            myTabwidgetRawPreview->setTabEnabled(myTabwidgetRawPreview->indexOf(myWebview), false);
        }
    }

    connect(myWebview, SIGNAL(loadFinished(bool)),
            this, SLOT(loadPreviewFinished(bool)));

    // Browser buttons
    connect(m_pUi->goBackBrowserAct, SIGNAL(triggered()),
            myWebview, SLOT(back()));
    connect(m_pUi->goForwardBrowserAct, SIGNAL(triggered()),
            myWebview, SLOT(forward()));
    connect(m_pUi->reloadBrowserAct, SIGNAL(triggered()),
            myWebview, SLOT(reload()));
    connect(myWebview, SIGNAL(urlChanged(QUrl)),
            this, SLOT(clickedLink()));
    #endif

    myFileOperations->setCurrentFile("");
    this->setUnifiedTitleAndToolBarOnMac(true);

    connect(myDownloadModule, SIGNAL(sendArticleText(QString, QString)),
            this, SLOT(displayArticleText(QString, QString)));

    // Hide statusbar, if option is false
    if ( false == mySettings->getShowStatusbar() )
    {
        this->setStatusBar(0);
    }

    // Restore window and toolbar settings
    // Settings have to be restored after toolbars are created!
    this->restoreGeometry(mySettings->getWindowGeometry());
    this->restoreState(mySettings->getWindowState());  // Restore toolbar position etc.

    if ( false == mySettings->getPreviewAlongside() )
    {
        this->removeToolBar(m_pUi->browserBar);
    }

    m_pUi->aboutAct->setText( m_pUi->aboutAct->text() + " " + m_pApp->applicationName() );

    if ( m_bLogging ) { std::clog << "Editor setup completed" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void CInyokaEdit::createActions()
{
    // File menu
    try
    {
        // New file
        m_pUi->newAct->setShortcuts(QKeySequence::New);
        connect(m_pUi->newAct, SIGNAL(triggered()), myFileOperations, SLOT(newFile()));

        // Open file
        m_pUi->openAct->setShortcuts(QKeySequence::Open);
        connect(m_pUi->openAct, SIGNAL(triggered()), myFileOperations, SLOT(open()));

        // Clear recent files list
        m_pClearRecentFilesAct = new QAction(tr("Clear list"), this);
        connect(m_pClearRecentFilesAct, SIGNAL(triggered()), myFileOperations, SLOT(clearRecentFiles()));

        // Save file
        m_pUi->saveAct->setShortcuts(QKeySequence::Save);
        connect(m_pUi->saveAct, SIGNAL(triggered()), myFileOperations, SLOT(save()));

        // Save file as...
        m_pUi->saveAsAct->setShortcuts(QKeySequence::SaveAs);
        connect(m_pUi->saveAsAct, SIGNAL(triggered()), myFileOperations, SLOT(saveAs()));

        // Print preview
        m_pUi->printPreviewAct->setShortcut(QKeySequence::Print);
        connect(m_pUi->printPreviewAct, SIGNAL(triggered()), myFileOperations, SLOT(printPreview()));
        if ( false == mySettings->getPreviewAlongside() )
        {
            m_pUi->printPreviewAct->setEnabled(false);
        }

        // Exit application
        m_pUi->exitAct->setShortcuts(QKeySequence::Quit);
        connect(m_pUi->exitAct, SIGNAL(triggered()), this, SLOT(close()));
    }
    catch ( std::bad_alloc& ba )
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / file menu: " << ba.what() << std::endl;
        QMessageBox::critical(this, m_pApp->applicationName(), "Error while memory allocation: bad_alloc - \"createActions()\" / file menu");
        exit (-2);
    }

    // ---------------------------------------------------------------------------------------------
    // EDIT MENU

    // Cut
    m_pUi->cutAct->setShortcuts(QKeySequence::Cut);
    connect(m_pUi->cutAct, SIGNAL(triggered()), myEditor, SLOT(cut()));

    // Copy
    m_pUi->copyAct->setShortcuts(QKeySequence::Copy);
    connect(m_pUi->copyAct, SIGNAL(triggered()), myEditor, SLOT(copy()));

    // Paste
    m_pUi->pasteAct->setShortcuts(QKeySequence::Paste);
    connect(m_pUi->pasteAct, SIGNAL(triggered()), myEditor, SLOT(paste()));

    // Undo
    m_pUi->undoAct->setShortcuts(QKeySequence::Undo);
    connect(m_pUi->undoAct, SIGNAL(triggered()), myEditor, SLOT(undo()));

    // Redo
    m_pUi->redoAct->setShortcuts(QKeySequence::Redo);
    connect(m_pUi->redoAct, SIGNAL(triggered()), myEditor, SLOT(redo()));

    // Find
    m_pUi->searchAct->setShortcuts(QKeySequence::Find);
    connect(m_pUi->searchAct, SIGNAL(triggered()), m_findDialog, SLOT(show()));

    // Replace
    m_pUi->replaceAct->setShortcuts(QKeySequence::Replace);
    connect(m_pUi->replaceAct, SIGNAL(triggered()), m_findReplaceDialog, SLOT(show()));

    // Find next
    m_pUi->findNextAct->setShortcuts(QKeySequence::FindNext);
    connect(m_pUi->findNextAct, SIGNAL(triggered()), m_findDialog, SLOT(findNext()));

    // Find previous
    m_pUi->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
    connect(m_pUi->findPreviousAct, SIGNAL(triggered()), m_findDialog, SLOT(findPrev()));


    // Set / initialize / connect cut / copy / redo / undo
    m_pUi->cutAct->setEnabled(false);
    m_pUi->copyAct->setEnabled(false);
    connect(myEditor, SIGNAL(copyAvailable(bool)),
            m_pUi->cutAct, SLOT(setEnabled(bool)));
    connect(myEditor, SIGNAL(copyAvailable(bool)),
            m_pUi->copyAct, SLOT(setEnabled(bool)));

    m_pUi->undoAct->setEnabled(false);
    connect(myEditor, SIGNAL(undoAvailable(bool)),
            m_pUi->undoAct, SLOT(setEnabled(bool)));
    m_pUi->redoAct->setEnabled(false);
    connect(myEditor, SIGNAL(redoAvailable(bool)),
            m_pUi->redoAct, SLOT(setEnabled(bool)));

    // ---------------------------------------------------------------------------------------------
    // TOOLS MENU

    // Spell checker
    m_pUi->spellCheckerAct->setShortcut(Qt::Key_F7);
    connect(m_pUi->spellCheckerAct, SIGNAL(triggered()), this, SLOT(checkSpelling()));
#ifdef DISABLE_SPELLCHECKER
    m_pUi->spellCheckerAct->setVisible(false);
#endif

    // Download styles
    connect(m_pUi->DownloadInyokaStylesAct, SIGNAL(triggered()), myDownloadModule, SLOT(loadInyokaStyles()));
#if defined _WIN32
    m_pUi->DownloadInyokaStylesAct->setDisabled( true );
#endif

    // Clear temp. image download folder
    connect(m_pUi->deleteTempImagesAct, SIGNAL(triggered()), this, SLOT(deleteTempImages()));

    // ---------------------------------------------------------------------------------------------

    // Show html preview
    m_pUi->previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(m_pUi->previewAct, SIGNAL(triggered()), this, SLOT(previewInyokaPage()));

    // Click on tabs of widget - int = index of tab
    connect(myTabwidgetRawPreview, SIGNAL(currentChanged(int)), this, SLOT(previewInyokaPage(int)));

    // Download Inyoka article
    connect(m_pUi->downloadArticleAct, SIGNAL(triggered()), this, SLOT(downloadArticle()));

    // ---------------------------------------------------------------------------------------------

    try
    {
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
    }
    catch ( std::bad_alloc& ba )
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / text samples: " << ba.what() << std::endl;
        QMessageBox::critical(this, m_pApp->applicationName(), "Error while memory allocation: bad_alloc - \"createActions()\" / combo boxes");
        exit (-6);
    }

    // ---------------------------------------------------------------------------------------------
    // INSERT SYNTAX ELEMENTS

    // Insert bold element
    m_pUi->boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    mySigMapTextSamples->setMapping(m_pUi->boldAct, "boldAct");
    connect(m_pUi->boldAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert italic element
    m_pUi->italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    mySigMapTextSamples->setMapping(m_pUi->italicAct, "italicAct");
    connect(m_pUi->italicAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert monotype element
    mySigMapTextSamples->setMapping(m_pUi->monotypeAct, "monotypeAct");
    connect(m_pUi->monotypeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert wiki link
    mySigMapTextSamples->setMapping(m_pUi->wikilinkAct, "wikilinkAct");
    connect(m_pUi->wikilinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert extern link
    mySigMapTextSamples->setMapping(m_pUi->externalLinkAct, "externalLinkAct");
    connect(m_pUi->externalLinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert image
    mySigMapTextSamples->setMapping(m_pUi->imageAct, "imageAct");
    connect(m_pUi->imageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert code block
    mySigMapTextSamples->setMapping(m_pUi->codeblockAct, "codeblockAct");
    connect(m_pUi->codeblockAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // ---------------------------------------------------------------------------------------------
    // TEXT SAMPLES

    mySigMapTextSamples->setMapping(m_pUi->insertUnderConstructionAct, "insertUnderConstructionAct");
    connect(m_pUi->insertUnderConstructionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertTestedForAct, "insertTestedForAct");
    connect(m_pUi->insertTestedForAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertKnowledgeAct, "insertKnowledgeAct");
    connect(m_pUi->insertKnowledgeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertTableOfContentsAct, "insertTableOfContentsAct");
    connect(m_pUi->insertTableOfContentsAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertAdvancedAct, "insertAdvancedAct");
    connect(m_pUi->insertAdvancedAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertAwardAct, "insertAwardAct");
    connect(m_pUi->insertAwardAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertImageAct, "imageAct");  // insertImageAct = imageAct !
    connect(m_pUi->insertImageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertImageUnderlineAct, "insertImageUnderlineAct");
    connect(m_pUi->insertImageUnderlineAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertImageCollectionAct, "insertImageCollectionAct");
    connect(m_pUi->insertImageCollectionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertImageCollectionInTextAct, "insertImageCollectionInTextAct");
    connect(m_pUi->insertImageCollectionInTextAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertBashCommandAct, "insertBashCommandAct");
    connect(m_pUi->insertBashCommandAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertNoticeAct, "insertNoticeAct");
    connect(m_pUi->insertNoticeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertWarningAct, "insertWarningAct");
    connect(m_pUi->insertWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertExpertsAct, "insertExpertsAct");
    connect(m_pUi->insertExpertsAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertPackageListAct, "insertPackageListAct");
    connect(m_pUi->insertPackageListAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertPackageInstallAct, "insertPackageInstallAct");
    connect(m_pUi->insertPackageInstallAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertPPAAct, "insertPPAAct");
    connect(m_pUi->insertPPAAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyRepoAct, "insertThirdPartyRepoAct");
    connect(m_pUi->insertThirdPartyRepoAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyRepoAuthAct, "insertThirdPartyRepoAuthAct");
    connect(m_pUi->insertThirdPartyRepoAuthAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyPackageAct, "insertThirdPartyPackageAct");
    connect(m_pUi->insertThirdPartyPackageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertImprovableAct, "insertImprovableAct");
    connect(m_pUi->insertImprovableAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertFixMeAct, "insertFixMeAct");
    connect(m_pUi->insertFixMeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertLeftAct, "insertLeftAct");
    connect(m_pUi->insertLeftAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyPackageWarningAct, "insertThirdPartyPackageWarningAct");
    connect(m_pUi->insertThirdPartyPackageWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartyRepoWarningAct, "insertThirdPartyRepoWarningAct");
    connect(m_pUi->insertThirdPartyRepoWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(m_pUi->insertThirdPartySoftwareWarningAct, "insertThirdPartySoftwareWarningAct");
    connect(m_pUi->insertThirdPartySoftwareWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    connect(mySigMapTextSamples, SIGNAL(mapped(QString)), this, SLOT(insertTextSample(QString)));

    // ---------------------------------------------------------------------------------------------
    // INTERWIKI LINKS MENU

    try {
        mySigMapInterWikiLinks = new QSignalMapper(this);
        QList <QAction *> emptyActionList;
        emptyActionList.clear();

        for ( int i = 0; i < myInterWikiLinks->getInterwikiLinksGroups().size(); i++ )
        {
            m_iWikiLinksActions << emptyActionList;
            for ( int j = 0; j < myInterWikiLinks->getInterwikiLinksNames()[i].size(); j++ )
            {
                // Path from normal installation
                if ( QFile::exists("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks") )
                {
                    m_iWikiLinksActions[i] << new QAction(QIcon("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksIcons()[i][j]), myInterWikiLinks->getInterwikiLinksNames()[i][j], this);
                }
                // No installation: Use app path
                else
                {
                    m_iWikiLinksActions[i] << new QAction(QIcon(m_pApp->applicationDirPath() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksIcons()[i][j]), myInterWikiLinks->getInterwikiLinksNames()[i][j], this);
                }

                mySigMapInterWikiLinks->setMapping( m_iWikiLinksActions[i][j], QString::number(i) + "," + QString::number(j) );
                connect(m_iWikiLinksActions[i][j], SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
            }
        }

        connect(mySigMapInterWikiLinks, SIGNAL(mapped(QString)), this, SLOT(insertInterwikiLink(QString)));
    }
    catch ( std::bad_alloc& ba )
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / interwiki actions: " << ba.what() << std::endl;
        QMessageBox::critical(this, m_pApp->applicationName(), "Error while memory allocation: bad_alloc - \"createActions()\" / interwiki actions");
        exit (-7);
    }

    // ---------------------------------------------------------------------------------------------
    // ABOUT MENU

    // Report a bug using apport
    connect(m_pUi->reportBugAct, SIGNAL(triggered()), this, SLOT(reportBug()));

    // Open about windwow
    connect(m_pUi->aboutAct, SIGNAL(triggered()), this, SLOT(about()));


    if ( m_bLogging ) { std::clog << "Created actions" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate menus
void CInyokaEdit::createMenus()
{
    // File menu
    m_pUi->fileMenuLastOpened->addActions( myFileOperations->getLastOpenedFiles() );
    m_pUi->fileMenuLastOpened->addSeparator();
    m_pUi->fileMenuLastOpened->addAction( m_pClearRecentFilesAct );
    if ( 0 == mySettings->getRecentFiles().size() )
    {
        m_pUi->fileMenuLastOpened->setEnabled(false);
    }

    // Insert interwiki-links menu
    for ( int i = 0; i < myInterWikiLinks->getInterwikiLinksGroups().size(); i++ )
    {
        // Path from normal installation
        if ( QFile::exists("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks") )
        {
            m_iWikiGroups.append( m_pUi->iWikiMenu->addMenu(QIcon("/usr/share/" + m_pApp->applicationName().toLower() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksGroupIcons()[i]), myInterWikiLinks->getInterwikiLinksGroups()[i]) );
        }
        // No installation: Use app path
        else
        {
            m_iWikiGroups.append( m_pUi->iWikiMenu->addMenu(QIcon(m_pApp->applicationDirPath() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksGroupIcons()[i]), myInterWikiLinks->getInterwikiLinksGroups()[i]) );
        }
        m_iWikiGroups[i]->addActions( m_iWikiLinksActions[i] );
    }

    if ( m_bLogging ) { std::clog << "Created menus" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate tool bars
void CInyokaEdit::createToolBars()
{
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

    if ( m_bLogging ) { std::clog << "Created toolbars" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Call parser
void CInyokaEdit::previewInyokaPage( const int nIndex )
{
    #ifndef DISABLE_WEBVIEW
    // Call parser if iIndex == index of myWebview -> Click on tab preview
    // or if iIndex == 999 -> Default parameter value when calling the function (e.g.) by clicking on button preview
    if ( myTabwidgetRawPreview->indexOf(myWebview) == nIndex || 999 == nIndex )
    {
        // Only disable buttons if preview is not shown alongside editor
        if ( false == mySettings->getPreviewAlongside() )
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
    #endif
        if ( "" == myFileOperations->getCurrentFile() || tr("Untitled") == myFileOperations->getCurrentFile() )
        {
            myParser->genOutput("");
        }
        else
        {
            QFileInfo fi(myFileOperations->getCurrentFile());
            myParser->genOutput(fi.fileName());
        }
    #ifndef DISABLE_WEBVIEW
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
    #endif
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
        if ( myEditor->textCursor().selectedText() != "" )
        {
            myEditor->insertPlainText( sHeadTag + " " + myEditor->textCursor().selectedText() + " " + sHeadTag );
        }
        // Select text sHeadline if no text was selected
        else
        {
            myEditor->insertPlainText( sHeadTag + " " + sHeadline + " " + sHeadTag );

            QTextCursor myTextCursor = myEditor->textCursor();
            myTextCursor.setPosition( myEditor->textCursor().position() - sHeadline.length() - nSelection );
            myTextCursor.setPosition( myEditor->textCursor().position() - nSelection, QTextCursor::KeepAnchor );
            myEditor->setTextCursor( myTextCursor );
        }
    }

    // Reset selection
    m_pHeadlineBox->setCurrentIndex(0);

    myEditor->setFocus();
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
        }
        // Reset selection
        m_pTextmacrosBox->setCurrentIndex(0);

        myEditor->setFocus();
    }
}

// Text format (combobox in toolbar)
void CInyokaEdit::insertDropDownTextformat( const int nSelection )
{
    bool bSelected = false;
    QString sInsertedText = "";
    unsigned short iFormatLength = 0;

    // Some text was selected
    if ( myEditor->textCursor().selectedText() != "" )
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
                    myEditor->insertPlainText( "'''" + myEditor->textCursor().selectedText() + "'''" );
                }
                else
                {
                    sInsertedText = tr("Folders", "Text format: Folders");
                    iFormatLength = 3;
                    myEditor->insertPlainText( "'''" + sInsertedText + "'''" );
                }
                break;
            case 2:  // Menus
                if ( bSelected )
                {
                    myEditor->insertPlainText( "''\"" + myEditor->textCursor().selectedText() + "\"''" );
                }
                else
                {
                    sInsertedText = tr("Menu -> sub menu -> menu entry", "Text format: Menu entries example");
                    iFormatLength = 3;
                    myEditor->insertPlainText( "''\"" + sInsertedText + "\"''" );
                }
                break;
            case 3:  // Files
                if ( bSelected )
                {
                    myEditor->insertPlainText( "'''" + myEditor->textCursor().selectedText() + "'''" );
                }
                else
                {
                    sInsertedText = tr("Files", "GUI: Text format: Files");
                    iFormatLength = 3;
                    myEditor->insertPlainText( "'''" + sInsertedText + "'''" );
                }
                break;
            case 4:  // Commands
                if ( bSelected )
                {
                    myEditor->insertPlainText( "`" + myEditor->textCursor().selectedText() + "`" );
                }
                else
                {
                    sInsertedText = tr("Command", "Text format: Command");
                    iFormatLength = 1;
                    myEditor->insertPlainText( "`" + sInsertedText + "`" );
                }
                break;
        }

        // Reset selection
        m_pTextformatBox->setCurrentIndex(0);

        if ( !bSelected )
        {
            QTextCursor myTextCursor = myEditor->textCursor();
            myTextCursor.setPosition( myEditor->textCursor().position() - sInsertedText.length() - iFormatLength );
            myTextCursor.setPosition( myEditor->textCursor().position() - iFormatLength, QTextCursor::KeepAnchor );
            myEditor->setTextCursor( myTextCursor );
        }

        myEditor->setFocus();
    }
}

// Insert text sample / syntax element
void CInyokaEdit::insertTextSample( const QString &sMenuEntry )
{
    myEditor->insertPlainText( QString::fromUtf8(myInsertSyntaxElement->getElementInyokaCode(sMenuEntry.toStdString(), myEditor->textCursor().selectedText().toStdString()).c_str()) );
    myEditor->setFocus();
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
        if ( myEditor->textCursor().selectedText() == "" )
        {
            QString sSitename = tr("Sitename", "InterWiki links: Common sitename");
            QString sText = tr("Text", "Interwiki links: Common text");

            // Insert InterWiki-Link
            myEditor->insertPlainText( "[" + myInterWikiLinks->getInterwikiLinks()[sTmp[0].toInt()][sTmp[1].toInt()] + ":" + sSitename + ":" + sText + "]" );

            // Select site name in InterWiki-Link
            QTextCursor textCursor = myEditor->textCursor();
            textCursor.setPosition( myEditor->textCursor().position() - sSitename.length() - sText.length() - 2);
            textCursor.setPosition( myEditor->textCursor().position() - sText.length() - 2, QTextCursor::KeepAnchor );
            myEditor->setTextCursor( textCursor );
        }
        // Some text is selected
        else
        {
            // Insert InterWiki-Link with selected text
            myEditor->insertPlainText( "[" + myInterWikiLinks->getInterwikiLinks()[sTmp[0].toInt()][sTmp[1].toInt()] + ":" + myEditor->textCursor().selectedText() + ":]" );
        }
    }
    // Problem with indices
    else
    {
        QMessageBox::warning( this, m_pApp->applicationName(), "Error while inserting InterWiki link: InterWiki indice" );
    }

    myEditor->setFocus();
}

// -----------------------------------------------------------------------------------------------

void CInyokaEdit::receiveMenuLastOpenedState( bool bEnabled )
{
    m_pUi->fileMenuLastOpened->setEnabled(bEnabled);
}

void CInyokaEdit::receiveStatusbarMessage( const QString &sMessage )
{
    this->statusBar()->showMessage(sMessage, 2000);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::downloadArticle()
{
    if ( !myFileOperations->maybeSave() )
    {
        return;
    }
    else
    {
        myDownloadModule->downloadArticle( m_tmpPreviewImgDir, mySettings->getInyokaUrl(), mySettings->getAutomaticImageDownload() );
        this->previewInyokaPage();
    }
}

void CInyokaEdit::displayArticleText( const QString &sArticleText, const QString &sSitename )
{
    myEditor->setPlainText(sArticleText);
    myFileOperations->setCurrentFile(sSitename);
    myEditor->document()->setModified(true);
    this->documentWasModified();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
void CInyokaEdit::showHtmlPreview( const QString &sFilename )
{
    if ( mySettings->getShowStatusbar() )
    {
        this->statusBar()->showMessage( tr("Opening preview", "GUI: Status bar") );
    }

    if ( false == mySettings->getPreviewInEditor() )
    {
        // Open html-file in system web browser
        QDesktopServices::openUrl( QUrl::fromLocalFile(sFilename) );
    }
    #ifndef DISABLE_WEBVIEW
    else
    {
        myWebview->load( QUrl::fromLocalFile(sFilename) );
    }
    myWebview->history()->clear();  // Clear history (clicked links)
    #endif
}

// Wait until loading has finished
void CInyokaEdit::loadPreviewFinished( bool bSuccess )
{
    #ifndef DISABLE_WEBVIEW
    if ( bSuccess )
    {
        myTabwidgetRawPreview->setCurrentIndex( myTabwidgetRawPreview->indexOf(myWebview) );
        // Enable / disbale back button
        if ( myWebview->history()->canGoBack() )
        {
            m_pUi->goBackBrowserAct->setEnabled(true);
        }
        else
        {
            m_pUi->goBackBrowserAct->setEnabled(false);
        }

        // Enable / disable forward button
        if ( myWebview->history()->canGoForward() )
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
    #endif
}

// A link on preview page was clicked
void CInyokaEdit::clickedLink()
{
    #ifndef DISABLE_WEBVIEW
    // Disable forward / backward button
    m_pUi->goForwardBrowserAct->setEnabled(false);
    m_pUi->goBackBrowserAct->setEnabled(false);
    #endif
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

bool CInyokaEdit::eventFilter( QObject *obj, QEvent *event )
{
    if ( obj == myEditor )
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
            QTextCursor cursor(myEditor->textCursor());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            myEditor->setTextCursor(cursor);
            return true;
        }
        // CTRL + arrow right
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Right &&
             !isSHIFT && isCTRL )
        {
                myEditor->moveCursor(QTextCursor::Right);
                myEditor->moveCursor(QTextCursor::EndOfWord);
                return true;
        }

        // ----------------------------------------------
        // Bug fix for LP: #889321

        // CTRL + SHIFT arrow down
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Up &&
             isSHIFT && isCTRL )
        {
                QTextCursor cursor(myEditor->textCursor());
                cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
                myEditor->setTextCursor(cursor);
                return true;
        }

        // CTRL + SHIFT arrow down
        else if ( event->type() == QEvent::KeyPress &&
             keyEvent->key() == Qt::Key_Down &&
             isSHIFT && isCTRL )
        {
                QTextCursor cursor(myEditor->textCursor());
                cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
                myEditor->setTextCursor(cursor);
                return true;
        }
        // ---------------------------------------------------------------------------

        // Reload preview at RETURN if preview alongside
        if ( event->type() == QEvent::KeyPress )
        {

            switch( keyEvent->key() )
            {
                case Qt::Key_Enter:
                case Qt::Key_Return:
                case Qt::Key_F5:
                    if ( true == mySettings->getPreviewAlongside() )
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
        sDictPath = "/usr/share/hunspell/" + mySettings->getSpellCheckerLanguage();
    }
    // Otherwise look for MySpell dictionary (Linx only)
    else if ( QDir("/usr/share/myspell/dicts").exists() )
    {
        sDictPath = "/usr/share/myspell/dicts/" + mySettings->getSpellCheckerLanguage();
    }
    // Fallback and for Windows look in app dir
    else
    {
        sDictPath = m_pApp->applicationDirPath() + "/dicts/" + mySettings->getSpellCheckerLanguage();
    }

    if ( !QFile::exists(sDictPath + ".dic") || !QFile::exists(sDictPath + ".aff") )
    {
        QMessageBox::critical( this, m_pApp->applicationName(), "Error: Spell checker dictionary file does not exist!" );
        return;
    }

    QString sUserDict= m_StylesAndImagesDir.absolutePath() + "/userDict_" + mySettings->getSpellCheckerLanguage() + ".txt";
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
    spellChecker->start(myEditor);

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
    myTabwidgetRawPreview->setCurrentIndex( myTabwidgetRawPreview->indexOf(myEditor) );
    this->setWindowModified( myEditor->document()->isModified() );
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
                          "This application uses icons from <a href=\"http://tango.freedesktop.org\">Tango project</a>.", "About dialog text, <sAppName>, <sVERSION>, <sAppName>").arg(m_pApp->applicationName()).arg(sVERSION).arg(m_pApp->applicationName()));
}

// -----------------------------------------------------------------------------------------------

// Close event (File -> Close or X)
void CInyokaEdit::closeEvent( QCloseEvent *event )
{
    if ( myFileOperations->maybeSave() )
    {
        if ( true == mySettings->getPreviewAlongside() && true == mySettings->getPreviewInEditor() )
        {
            mySettings->writeSettings( saveGeometry(), saveState(), myWidgetSplitter->saveState() );
        }
        else
        {
            mySettings->writeSettings( saveGeometry(), saveState() );
        }
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
