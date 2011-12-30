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
* File Name:  CInyokaEdit.cpp
* Purpose:    Main application, gui definition, editor functions
***************************************************************************/

#define sVERSION "0.0.9"

#include <QtGui>
#include <QtWebKit/QWebView>

#include "CInyokaEdit.h"
#include "ui_CInyokaEdit.h"

CInyokaEdit::CInyokaEdit(QApplication *ptrApp, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CInyokaEdit),
    pApp(ptrApp),
    myCompleter(0)
{
    ui->setupUi(this);

    bLogging = false;

    // Check for command line arguments
    if (pApp->argc() >= 2) {
        QString sTmp = pApp->argv()[1];

        if ("--version"== sTmp) {
            std::cout << pApp->argv()[0] << "\t Version: " << sVERSION << std::endl;
            exit(0);
        }
        else if ("--log" == sTmp) {
            bLogging = true;
        }
    }

    sListCompleter << "Inhaltsverzeichnis(1)]]" << "Vorlage(Getestet, Ubuntuversion)]]" << "Vorlage(Baustelle, Datum, \"Bearbeiter\")]]"
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
    StylesAndImagesDir = QDir::homePath() + "/." + pApp->applicationName();

    // Create all objects (after definition of StylesAndImagesDir)
    createObjects();

    // Setup gui, menus, actions, toolbar...
    setupEditor();
    createActions();
    createMenus();
    createToolBars();

    // Download style files if preview/styles/imgages folders doesn't exist (/home/user/.InyokaEdit)
    if (!StylesAndImagesDir.exists() || !QDir(StylesAndImagesDir.absolutePath() + "/img").exists() ||
        !QDir(StylesAndImagesDir.absolutePath() + "/styles").exists() || !QDir(StylesAndImagesDir.absolutePath() + "/Wiki").exists()){
        DownloadStyles(StylesAndImagesDir);
    }

    // Download styles or open file from command line argument
    if (pApp->argc() >= 2) {
        QString sTmp = pApp->argv()[1];

        // Download inyoka styles
        if ("--dlstyles" == sTmp){
            DownloadStyles(StylesAndImagesDir);
        }
        else {
            if ("--log" != sTmp) {
                loadFile(pApp->argv()[1]);
            }
            else if ("--log" == sTmp && pApp->argc() >= 3) {
                loadFile(pApp->argv()[2]);
            }
        }
    }

    // In config file an older version was found
    if (sVERSION != mySettings->getConfVersion()) {
        DownloadStyles(StylesAndImagesDir);
    }

    if (mySettings->getShowStatusbar()) {
        statusBar()->showMessage(tr("Ready"));
    }
    if (bLogging) { std::clog << "Created CInyokaEdit" << std::endl; }

}

CInyokaEdit::~CInyokaEdit()
{
    delete ui;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::createObjects() {

    try
    {
        m_findDialog = new FindDialog(this);  // Has to be create before readSettings
        m_findReplaceDialog = new FindReplaceDialog(this);
        if (bLogging) { std::clog << "Created find/replace dialogs" << std::endl; }

        mySettings = new CSettings(StylesAndImagesDir, pApp->applicationName(), *m_findDialog, *m_findReplaceDialog);
        if (bLogging) { std::clog << "Created mySettings" << std::endl; }
        // Load settings from config file
        mySettings->readSettings();
        if (bLogging) { std::clog << "Read settings" << std::endl; }

        myEditor = new CTextEditor(mySettings->getCodeCompletion());  // Has to be create before find/replace
        if (bLogging) { std::clog << "Created myEditor" << std::endl; }

        myCompleter = new QCompleter(sListCompleter, this);
        if (bLogging) { std::clog << "Created myCompleter" << std::endl; }

        myInterWikiLinks = new CInterWiki(pApp);  // Has to be created before parser
        if (bLogging) { std::clog << "Created myInterWikiLinks" << std::endl; }

        myHighlighter = new CHighlighter(myEditor->document());
        if (bLogging) { std::clog << "Created myHighlighter" << std::endl; }

        myParser = new CParser(myEditor->document(), mySettings->getInyokaUrl(), StylesAndImagesDir, myInterWikiLinks->getInterwikiLinks(), myInterWikiLinks->getInterwikiLinksUrls());
        if (bLogging) { std::clog << "Created myParser" << std::endl; }

        //myTabwidgetDocuments = new QTabWidget;
        //if (bLogging) { std::clog << "Created myTabwidgetDocuments" << std::endl; }
        myTabwidgetRawPreview = new QTabWidget;
        if (bLogging) { std::clog << "Created myTabwidgetRawPreview" << std::endl; }

        myWebview = new QWebView(this);
        if (bLogging) { std::clog << "Created myWebview" << std::endl; }

        myInsertSyntaxElement = new CInsertSyntaxElement;
        if (bLogging) { std::clog << "Created myInsertSyntaxElement" << std::endl; }
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createObjects()\": " << ba.what() << std::endl;
        QMessageBox::critical(this, pApp->applicationName(), "Error while memory allocation: bad_alloc - createObjects()");
        exit (-1);
    }

    if (bLogging) { std::clog << "Created objects" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::setupEditor()
{
    // Application icon
    setWindowIcon(QIcon(":/images/" + pApp->applicationName().toLower() + "_64x64.png"));

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

    /*
    setCentralWidget(myTabwidgetDocuments);
    myTabwidgetDocuments->setTabPosition(QTabWidget::North);
    myTabwidgetDocuments->setTabsClosable(true);
    myTabwidgetDocuments->setMovable(true);
    myTabwidgetDocuments->setDocumentMode(true);

    myTabwidgetDocuments->addTab(myTabwidgetRawPreview, tr("Untitled"));
    */

    setCentralWidget(myTabwidgetRawPreview);
    myTabwidgetRawPreview->setTabPosition(QTabWidget::West);
    myTabwidgetRawPreview->addTab(myEditor, tr("Raw format"));

    myTabwidgetRawPreview->addTab(myWebview, tr("Preview"));
    if (false == mySettings->getPreviewInEditor())
        myTabwidgetRawPreview->setTabEnabled(myTabwidgetRawPreview->indexOf(myWebview), false);

    connect(myWebview, SIGNAL(loadFinished(bool)),
            this, SLOT(loadPreviewFinished(bool)));

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);

    // Hide statusbar, if option is false
    if (false == mySettings->getShowStatusbar()) {
        setStatusBar(0);
    }

    // Restore window and toolbar settings
    // Settings have to be restored after toolbars are created!
    restoreGeometry(mySettings->getWindowGeometry());
    restoreState(mySettings->getWindowState());  // Restore toolbar position etc.

    ui->aboutAct->setText(ui->aboutAct->text() + " " + pApp->applicationName());

    if (bLogging) { std::clog << "Editor setup completed" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void CInyokaEdit::createActions()
{
    // File menu
    try {
        // New file
        ui->newAct->setShortcuts(QKeySequence::New);
        connect(ui->newAct, SIGNAL(triggered()), this, SLOT(newFile()));

        // Open file
        ui->openAct->setShortcuts(QKeySequence::Open);
        connect(ui->openAct, SIGNAL(triggered()), this, SLOT(open()));

        // Open recent files
        mySigMapLastOpenedFiles = new QSignalMapper(this);
        for (int i = 0; i < mySettings->getMaxNumOfRecentFiles(); i++) {
            if (i < mySettings->getRecentFiles().size()) {
                LastOpenedFilesAct << new QAction(mySettings->getRecentFiles()[i], this);
            }
            else {
                LastOpenedFilesAct << new QAction("EMPTY", this);
                LastOpenedFilesAct[i]->setVisible(false);
            }
            mySigMapLastOpenedFiles->setMapping(LastOpenedFilesAct[i], i);
            connect(LastOpenedFilesAct[i], SIGNAL(triggered()), mySigMapLastOpenedFiles, SLOT(map()));
        }
        connect(mySigMapLastOpenedFiles, SIGNAL(mapped(int)), this, SLOT(openRecentFile(int)));

        // Clear recent files list
        clearRecentFilesAct = new QAction(tr("Clear list"), this);
        connect(clearRecentFilesAct, SIGNAL(triggered()), this, SLOT(clearRecentFiles()));

        // Save file
        ui->saveAct->setShortcuts(QKeySequence::Save);
        connect(ui->saveAct, SIGNAL(triggered()), this, SLOT(save()));

        // Save file as...
        ui->saveAsAct->setShortcuts(QKeySequence::SaveAs);
        connect(ui->saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

        // Exit application
        ui->exitAct->setShortcuts(QKeySequence::Quit);
        connect(ui->exitAct, SIGNAL(triggered()), this, SLOT(close()));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / file menu: " << ba.what() << std::endl;
        QMessageBox::critical(this, pApp->applicationName(), "Error while memory allocation: bad_alloc - \"createActions()\" / file menu");
        exit (-2);
    }

    // ---------------------------------------------------------------------------------------------
    // EDIT MENU

    // Cut
    ui->cutAct->setShortcuts(QKeySequence::Cut);
    connect(ui->cutAct, SIGNAL(triggered()), myEditor, SLOT(cut()));

    // Copy
    ui->copyAct->setShortcuts(QKeySequence::Copy);
    connect(ui->copyAct, SIGNAL(triggered()), myEditor, SLOT(copy()));

    // Paste
    ui->pasteAct->setShortcuts(QKeySequence::Paste);
    connect(ui->pasteAct, SIGNAL(triggered()), myEditor, SLOT(paste()));

    // Undo
    ui->undoAct->setShortcuts(QKeySequence::Undo);
    connect(ui->undoAct, SIGNAL(triggered()), myEditor, SLOT(undo()));

    // Redo
    ui->redoAct->setShortcuts(QKeySequence::Redo);
    connect(ui->redoAct, SIGNAL(triggered()), myEditor, SLOT(redo()));

    // Find
    ui->searchAct->setShortcuts(QKeySequence::Find);
    connect(ui->searchAct, SIGNAL(triggered()), m_findDialog, SLOT(show()));

    // Replace
    ui->replaceAct->setShortcuts(QKeySequence::Replace);
    connect(ui->replaceAct, SIGNAL(triggered()), m_findReplaceDialog, SLOT(show()));

    // Find next
    ui->findNextAct->setShortcuts(QKeySequence::FindNext);
    connect(ui->findNextAct, SIGNAL(triggered()), m_findDialog, SLOT(findNext()));

    // Find previous
    ui->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
    connect(ui->findPreviousAct, SIGNAL(triggered()), m_findDialog, SLOT(findPrev()));


    // Set / initialize / connect cut / copy / redo / undo
    ui->cutAct->setEnabled(false);
    ui->copyAct->setEnabled(false);
    connect(myEditor, SIGNAL(copyAvailable(bool)),
            ui->cutAct, SLOT(setEnabled(bool)));
    connect(myEditor, SIGNAL(copyAvailable(bool)),
            ui->copyAct, SLOT(setEnabled(bool)));

    ui->undoAct->setEnabled(false);
    connect(myEditor, SIGNAL(undoAvailable(bool)),
            ui->undoAct, SLOT(setEnabled(bool)));
    ui->redoAct->setEnabled(false);
    connect(myEditor, SIGNAL(redoAvailable(bool)),
            ui->redoAct, SLOT(setEnabled(bool)));

    // Spell checker
    connect(ui->spellCheckerAct, SIGNAL(triggered()), this, SLOT(checkSpelling()));

    // ---------------------------------------------------------------------------------------------

    // Show html preview
    ui->previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(ui->previewAct, SIGNAL(triggered()), this, SLOT(previewInyokaPage()));

    // Click on tabs of widget - int = index of tab
    connect(myTabwidgetRawPreview, SIGNAL(currentChanged(int)), this, SLOT(previewInyokaPage(int)));

    // Download Inyoka article
    connect(ui->downloadArticleAct, SIGNAL(triggered()), this, SLOT(downloadArticle()));

    // ---------------------------------------------------------------------------------------------

    try {
        mySigMapTextSamples = new QSignalMapper(this);

        // Insert headline
        headlineBox = new QComboBox();
        headlineBox->setStatusTip(tr("Insert a headline - 5 headline steps are supported"));

        // Insert sample
        textmacrosBox = new QComboBox();
        textmacrosBox->setStatusTip(tr("Insert text sample"));

        // Insert text format
        textformatBox = new QComboBox();
        textformatBox->setStatusTip(tr("Insert text format"));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / text samples: " << ba.what() << std::endl;
        QMessageBox::critical(this, pApp->applicationName(), "Error while memory allocation: bad_alloc - \"createActions()\" / combo boxes");
        exit (-6);
    }

    // ---------------------------------------------------------------------------------------------
    // INSERT SYNTAX ELEMENTS

    // Insert bold element
    ui->boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    mySigMapTextSamples->setMapping(ui->boldAct, "boldAct");
    connect(ui->boldAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert italic element
    ui->italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    mySigMapTextSamples->setMapping(ui->italicAct, "italicAct");
    connect(ui->italicAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert monotype element
    mySigMapTextSamples->setMapping(ui->monotypeAct, "monotypeAct");
    connect(ui->monotypeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert wiki link
    mySigMapTextSamples->setMapping(ui->wikilinkAct, "wikilinkAct");
    connect(ui->wikilinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert extern link
    mySigMapTextSamples->setMapping(ui->externalLinkAct, "externalLinkAct");
    connect(ui->externalLinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert image
    mySigMapTextSamples->setMapping(ui->imageAct, "imageAct");
    connect(ui->imageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert code block
    mySigMapTextSamples->setMapping(ui->codeblockAct, "codeblockAct");
    connect(ui->codeblockAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // ---------------------------------------------------------------------------------------------
    // TEXT SAMPLES

    mySigMapTextSamples->setMapping(ui->insertUnderConstructionAct, "insertUnderConstructionAct");
    connect(ui->insertUnderConstructionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertTestedForAct, "insertTestedForAct");
    connect(ui->insertTestedForAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertKnowledgeAct, "insertKnowledgeAct");
    connect(ui->insertKnowledgeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertTableOfContentsAct, "insertTableOfContentsAct");
    connect(ui->insertTableOfContentsAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertAdvancedAct, "insertAdvancedAct");
    connect(ui->insertAdvancedAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertAwardAct, "insertAwardAct");
    connect(ui->insertAwardAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertImageAct, "imageAct");  // insertImageAct = imageAct !
    connect(ui->insertImageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertImageUnderlineAct, "insertImageUnderlineAct");
    connect(ui->insertImageUnderlineAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertImageCollectionAct, "insertImageCollectionAct");
    connect(ui->insertImageCollectionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertImageCollectionInTextAct, "insertImageCollectionInTextAct");
    connect(ui->insertImageCollectionInTextAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertBashCommandAct, "insertBashCommandAct");
    connect(ui->insertBashCommandAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertNoticeAct, "insertNoticeAct");
    connect(ui->insertNoticeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertWarningAct, "insertWarningAct");
    connect(ui->insertWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertExpertsAct, "insertExpertsAct");
    connect(ui->insertExpertsAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertPackageListAct, "insertPackageListAct");
    connect(ui->insertPackageListAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertPackageInstallAct, "insertPackageInstallAct");
    connect(ui->insertPackageInstallAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertPPAAct, "insertPPAAct");
    connect(ui->insertPPAAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertThirdPartyRepoAct, "insertThirdPartyRepoAct");
    connect(ui->insertThirdPartyRepoAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertThirdPartyRepoAuthAct, "insertThirdPartyRepoAuthAct");
    connect(ui->insertThirdPartyRepoAuthAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertThirdPartyPackageAct, "insertThirdPartyPackageAct");
    connect(ui->insertThirdPartyPackageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertImprovableAct, "insertImprovableAct");
    connect(ui->insertImprovableAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertFixMeAct, "insertFixMeAct");
    connect(ui->insertFixMeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertLeftAct, "insertLeftAct");
    connect(ui->insertLeftAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertThirdPartyPackageWarningAct, "insertThirdPartyPackageWarningAct");
    connect(ui->insertThirdPartyPackageWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertThirdPartyRepoWarningAct, "insertThirdPartyRepoWarningAct");
    connect(ui->insertThirdPartyRepoWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    mySigMapTextSamples->setMapping(ui->insertThirdPartySoftwareWarningAct, "insertThirdPartySoftwareWarningAct");
    connect(ui->insertThirdPartySoftwareWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    connect(mySigMapTextSamples, SIGNAL(mapped(QString)), this, SLOT(insertTextSample(QString)));

    // ---------------------------------------------------------------------------------------------
    // INTERWIKI LINKS MENU

    try {
        mySigMapInterWikiLinks = new QSignalMapper(this);
        QList <QAction *> emptyActionList;
        emptyActionList.clear();

        for (int i = 0; i < myInterWikiLinks->getInterwikiLinksGroups().size(); i++) {
            iWikiLinksActions << emptyActionList;
            for (int j = 0; j < myInterWikiLinks->getInterwikiLinksNames()[i].size(); j++) {
                // Path from normal installation
                if (QFile::exists("/usr/share/" + pApp->applicationName().toLower() + "/iWikiLinks")) {
                    iWikiLinksActions[i] << new QAction(QIcon("/usr/share/" + pApp->applicationName().toLower() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksIcons()[i][j]), myInterWikiLinks->getInterwikiLinksNames()[i][j], this);
                }
                // No installation: Use app path
                else {
                    iWikiLinksActions[i] << new QAction(QIcon(pApp->applicationDirPath() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksIcons()[i][j]), myInterWikiLinks->getInterwikiLinksNames()[i][j], this);
                }

                mySigMapInterWikiLinks->setMapping(iWikiLinksActions[i][j], QString::number(i) + "," + QString::number(j));
                connect(iWikiLinksActions[i][j], SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
            }
        }

        connect(mySigMapInterWikiLinks, SIGNAL(mapped(QString)), this, SLOT(insertInterwikiLink(QString)));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / interwiki actions: " << ba.what() << std::endl;
        QMessageBox::critical(this, pApp->applicationName(), "Error while memory allocation: bad_alloc - \"createActions()\" / interwiki actions");
        exit (-7);
    }

    // ---------------------------------------------------------------------------------------------
    // ABOUT MENU

    // Report a bug using apport
    connect(ui->reportBugAct, SIGNAL(triggered()), this, SLOT(reportBug()));

    // Open about windwow
    connect(ui->aboutAct, SIGNAL(triggered()), this, SLOT(about()));


    if (bLogging) { std::clog << "Created actions" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate menus
void CInyokaEdit::createMenus()
{
    // File menu
    ui->fileMenuLastOpened->addActions(LastOpenedFilesAct);
    ui->fileMenuLastOpened->addSeparator();
    ui->fileMenuLastOpened->addAction(clearRecentFilesAct);
    if (0 == mySettings->getRecentFiles().size()) {
        ui->fileMenuLastOpened->setEnabled(false);
    }

    // Insert interwiki-links menu
    for (int i = 0; i < myInterWikiLinks->getInterwikiLinksGroups().size(); i++) {        
        // Path from normal installation
        if (QFile::exists("/usr/share/" + pApp->applicationName().toLower() + "/iWikiLinks")) {
            iWikiGroups.append(ui->iWikiMenu->addMenu(QIcon("/usr/share/" + pApp->applicationName().toLower() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksGroupIcons()[i]), myInterWikiLinks->getInterwikiLinksGroups()[i]));
        }
        // No installation: Use app path
        else {
            iWikiGroups.append(ui->iWikiMenu->addMenu(QIcon(pApp->applicationDirPath() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksGroupIcons()[i]), myInterWikiLinks->getInterwikiLinksGroups()[i]));
        }
        iWikiGroups[i]->addActions(iWikiLinksActions[i]);
    }

    if (bLogging) { std::clog << "Created menus" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate tool bars
void CInyokaEdit::createToolBars()
{
    // Tool bar for combo boxes (samples and macros)
    ui->samplesmacrosBar->addWidget(headlineBox);

    // Headline combo box
    QString sHeadline = tr("Headline");
    QString sHeadlineStep = tr("Step", "GUI: Headline combo box");
    headlineBox->addItem(sHeadline);
    headlineBox->insertSeparator(1);
    headlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 1");
    headlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 2");
    headlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 3");
    headlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 4");
    headlineBox->addItem(sHeadline + ": " + sHeadlineStep + " 5");
    connect(headlineBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownHeadline(int)));

    // Macros combo box
    ui->samplesmacrosBar->addWidget(textmacrosBox);
    textmacrosBox->addItem(tr("Text macros", "GUI: Text macro combo box"));
    textmacrosBox->insertSeparator(1);
    textmacrosBox->addItem(tr("Under construction", "GUI: Text macro combo box - Work in progress"));
    textmacrosBox->addItem(tr("Table of contents", "GUI: Text macro combo box - Table of contents"));
    textmacrosBox->addItem(tr("Tested for", "GUI: Text macro combo box - Tested for"));
    textmacrosBox->addItem(tr("Package installation", "GUI: Text macro combo box - Package installation"));
    textmacrosBox->addItem(tr("Bash command", "GUI: Text macro combo box - Bash command"));
    textmacrosBox->addItem(tr("PPA sample", "GUI: Text macro combo box - PPA sample"));
    textmacrosBox->addItem(tr("Notice box", "GUI: Text macro combo box - Notice box"));
    textmacrosBox->addItem(tr("Third-party repo warning", "GUI: Text macro combo box - Third-party repo warning"));
    textmacrosBox->addItem(tr("Warning box", "GUI: Text macro combo box - Warning box"));
    textmacrosBox->addItem(tr("Experts information", "GUI: Text macro combo box - Experts info box"));
    textmacrosBox->addItem(tr("Keys", "GUI: Text macro combo box - Keys"));
    textmacrosBox->addItem(tr("Table", "GUI: Text macro combo box - Table"));
    connect(textmacrosBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextmacro(int)));

    // Text format combo box
    ui->samplesmacrosBar->addWidget(textformatBox);
    textformatBox->addItem(tr("Text format", "GUI: Text format combo box"));
    textformatBox->insertSeparator(1);
    textformatBox->addItem(tr("Folders", "GUI: Text format folders"));
    textformatBox->addItem(tr("Menu entries", "GUI: Text format menu entries"));
    textformatBox->addItem(tr("Files", "GUI: Text format files"));
    textformatBox->addItem(tr("Command", "GUI: Text format command"));
    connect(textformatBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextformat(int)));

    if (bLogging) { std::clog << "Created toolbars" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::DownloadStyles(const QDir myDirectory)
{
    int iRet = QMessageBox::question(this, tr("Download styles"), tr("In order that articles could be previewed correctly, you have to download some Inyoka ressources. This process may take a few minutes.\n\nDo you want to download these files now?"), QMessageBox::Yes | QMessageBox::No);
    if (QMessageBox::Yes== iRet){
        try
        {
            // Path from normal installation
            if (QFile::exists("/usr/share/" + pApp->applicationName().toLower() + "/GetInyokaStyles")) {
                myArticleDownloadProgress = new CProgressDialog("/usr/share/" + pApp->applicationName().toLower() + "/GetInyokaStyles", pApp->applicationName(), this, myDirectory.absolutePath());
            }
            // No installation: Use app path
            else {
                myArticleDownloadProgress = new CProgressDialog(pApp->applicationDirPath() + "/GetInyokaStyles", pApp->applicationName(), this, myDirectory.absolutePath());
            }
        }
        catch (std::bad_alloc& ba)
        {
            std::cerr << "ERROR: myArticleDownloadProgress - bad_alloc caught: " << ba.what() << std::endl;
            QMessageBox::critical(this, pApp->applicationName(), "Error while memory allocation: bad_alloc - myArticleDownloadProgress");
            exit (-8);
        }
        myArticleDownloadProgress->open();

        mySettings->setConfVersion(sVERSION);

        if (bLogging) { std::clog << "Downloaded styles" << std::endl; }
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Call parser
void CInyokaEdit::previewInyokaPage(const int iIndex){

    // Call parser if iIndex == index of myWebview -> Click on tab preview
    // or if iIndex == 999 -> Default parameter value when calling the function (e.g.) by clicking on button preview
    if (myTabwidgetRawPreview->indexOf(myWebview) == iIndex || 999 == iIndex) {

        // Disable editor and insert samples/macros toolbars
        ui->editMenu->setDisabled(true);
        ui->insertTextSampleMenu->setDisabled(true);
        ui->iWikiMenu->setDisabled(true);
        ui->editToolBar->setDisabled(true);
        ui->inyokaeditorBar->setDisabled(true);
        ui->samplesmacrosBar->setDisabled(true);
        ui->previewAct->setDisabled(true);

        if ("" == sCurFile || tr("Untitled") == sCurFile){
            myParser->genOutput("");
        }
        else{
            QFileInfo fi(sCurFile);
            myParser->genOutput(fi.fileName());
        }
    }
    else {
        // Enable editor and insert samples/macros toolbars again
        ui->editMenu->setEnabled(true);
        ui->insertTextSampleMenu->setEnabled(true);
        ui->iWikiMenu->setEnabled(true);
        ui->editToolBar->setEnabled(true);
        ui->inyokaeditorBar->setEnabled(true);
        ui->samplesmacrosBar->setEnabled(true);
        ui->previewAct->setEnabled(true);
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// INSERT INYOKA ELEMENTS

// Headline (combobox in toolbar)
void CInyokaEdit::insertDropDownHeadline(const int iSelection){

    if (iSelection > 1) {
        QString sHeadline = tr("Headline");
        QString sHeadTag = "";

        // Generate headline tag
        for (int i = 1; i < iSelection; i++) {
            sHeadTag.append("=");
        }

        // Some text was selected
        if (myEditor->textCursor().selectedText() != "") {
            myEditor->insertPlainText(sHeadTag + " " + myEditor->textCursor().selectedText() + " " + sHeadTag);
        }
        // Select text sHeadline if no text was selected
        else {
            myEditor->insertPlainText(sHeadTag + " " + sHeadline + " " + sHeadTag);

            QTextCursor myTextCursor = myEditor->textCursor();
            myTextCursor.setPosition( myEditor->textCursor().position() - sHeadline.length() - iSelection);
            myTextCursor.setPosition( myEditor->textCursor().position() - iSelection, QTextCursor::KeepAnchor );
            myEditor->setTextCursor( myTextCursor );
        }
    }

    // Reset selection
    headlineBox->setCurrentIndex(0);

    myEditor->setFocus();
}

// Macro (combobox in toolbar)
void CInyokaEdit::insertDropDownTextmacro(const int iSelection){

    if (iSelection != 0 && iSelection != 1) {
        // -1 because of separator (considered as "item")
        switch (iSelection-1) {
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
        textmacrosBox->setCurrentIndex(0);

        myEditor->setFocus();
    }
}

// Text format (combobox in toolbar)
void CInyokaEdit::insertDropDownTextformat(const int iSelection){

    bool bSelected = false;
    QString sInsertedText = "";
    unsigned short iFormatLength;

    // Some text was selected
    if (myEditor->textCursor().selectedText() != "") {
        bSelected = true;
    }

    if (iSelection != 0 && iSelection != 1) {
        // -1 because of separator (considered as "item")
        switch (iSelection-1) {
        default:
        case 1:  // Folders
            if (bSelected) {
                myEditor->insertPlainText("'''" + myEditor->textCursor().selectedText() + "'''");
            }
            else {
                sInsertedText = tr("Folders", "Text format: Folders");
                iFormatLength = 3;
                myEditor->insertPlainText("'''" + sInsertedText + "'''");
            }
            break;
        case 2:  // Menus
            if (bSelected) {
                myEditor->insertPlainText("''\"" + myEditor->textCursor().selectedText() + "\"''");
            }
            else {
                sInsertedText = tr("Menu -> sub menu -> menu entry", "Text format: Menu entries example");
                iFormatLength = 3;
                myEditor->insertPlainText("''\"" + sInsertedText + "\"''");
            }
            break;
        case 3:  // Files
            if (bSelected) {
                myEditor->insertPlainText("'''" + myEditor->textCursor().selectedText() + "'''");
            }
            else {
                sInsertedText = tr("Files", "GUI: Text format: Files");
                iFormatLength = 3;
                myEditor->insertPlainText("'''" + sInsertedText + "'''");
            }
            break;
        case 4:  // Commands
            if (bSelected) {
                myEditor->insertPlainText("`" + myEditor->textCursor().selectedText() + "`");
            }
            else {
                sInsertedText = tr("Command", "Text format: Command");
                iFormatLength = 1;
                myEditor->insertPlainText("`" + sInsertedText + "`");
            }
            break;
        }

        // Reset selection
        textformatBox->setCurrentIndex(0);

        if (!bSelected) {
            QTextCursor myTextCursor = myEditor->textCursor();
            myTextCursor.setPosition( myEditor->textCursor().position() - sInsertedText.length() - iFormatLength);
            myTextCursor.setPosition( myEditor->textCursor().position() - iFormatLength, QTextCursor::KeepAnchor );
            myEditor->setTextCursor( myTextCursor );
        }

        myEditor->setFocus();
    }
}

// Insert text sample / syntax element
void CInyokaEdit::insertTextSample(const QString &sMenuEntry){
    myEditor->insertPlainText(QString::fromUtf8(myInsertSyntaxElement->GetElementInyokaCode(sMenuEntry.toStdString(), myEditor->textCursor().selectedText().toStdString()).c_str()));
    myEditor->setFocus();
}

// Insert interwiki-link
void CInyokaEdit::insertInterwikiLink(const QString &sMenuEntry){

    // Get indices for links
    QStringList sTmp = sMenuEntry.split(",");

    // Check if right number of indices found
    if (sTmp.size() == 2) {
        // No text selected
        if (myEditor->textCursor().selectedText() == "") {
            QString sSitename = tr("Sitename", "InterWiki links: Common sitename");
            QString sText = tr("Text", "Interwiki links: Common text");

            // Insert InterWiki-Link
            myEditor->insertPlainText("[" + myInterWikiLinks->getInterwikiLinks()[sTmp[0].toInt()][sTmp[1].toInt()] + ":" + sSitename + ":" + sText + "]");

            // Select site name in InterWiki-Link
            QTextCursor textCursor = myEditor->textCursor();
            textCursor.setPosition( myEditor->textCursor().position() - sSitename.length() - sText.length() - 2);
            textCursor.setPosition( myEditor->textCursor().position() - sText.length() - 2, QTextCursor::KeepAnchor );
            myEditor->setTextCursor( textCursor );
        }
        // Some text is selected
        else {
            // Insert InterWiki-Link with selected text
            myEditor->insertPlainText("[" + myInterWikiLinks->getInterwikiLinks()[sTmp[0].toInt()][sTmp[1].toInt()] + ":" + myEditor->textCursor().selectedText() + ":]");
        }
    }
    // Problem with indices
    else {
        QMessageBox::warning(this, pApp->applicationName(), "Error while inserting InterWiki link: InterWiki indice");
    }

    myEditor->setFocus();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// DOWNLOAD EXISTING INYOKA WIKI ARTICLE

void CInyokaEdit::downloadArticle()
{
    QString sTmpArticle("");
    QString sSitename("");
    QByteArray tempResult;

    if (!maybeSave()) {
        return;
    }
    else{
        bool ok; // Buttons of input dialog (click on "OK" -> ok = true, click on "Cancel" -> ok = false)

        // Show input dialog
        sSitename = QInputDialog::getText(this, pApp->applicationName(),
                                          tr("Please insert name of the article which should be downloaded:"), QLineEdit::Normal,
                                          tr("Category/Article", "Msg: Input dialog DL article example text"), &ok);

        // Click on "cancel" or string is empty
        if (false == ok || sSitename.isEmpty())
            return;

        // Replace non valid characters
        sSitename.replace(QString::fromUtf8("ä"), "a", Qt::CaseInsensitive);
        sSitename.replace(QString::fromUtf8("ö"), "o", Qt::CaseInsensitive);
        sSitename.replace(QString::fromUtf8("ü"), "u", Qt::CaseInsensitive);
        sSitename.replace(" ", "_");

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        // Start article download
        QProcess procDownloadRawtext;
        procDownloadRawtext.start("wget -O - " + mySettings->getInyokaUrl() + "/" + sSitename + "?action=export&format=raw");

        if (!procDownloadRawtext.waitForStarted()) {
            #ifndef QT_NO_CURSOR
                QApplication::restoreOverrideCursor();
            #endif
            QMessageBox::critical(this, pApp->applicationName(), tr("Could not start the download of the raw format of article."));
            procDownloadRawtext.kill();
            return;
        }
        if (!procDownloadRawtext.waitForFinished()) {
            #ifndef QT_NO_CURSOR
                QApplication::restoreOverrideCursor();
            #endif
            QMessageBox::critical(this, pApp->applicationName(), tr("Error while downloading raw format of article."));
            procDownloadRawtext.kill();
            return;
        }

        tempResult = procDownloadRawtext.readAll();
        sTmpArticle = QString::fromUtf8(tempResult);

        // Site does not exist etc.
        if ("" == sTmpArticle) {
            #ifndef QT_NO_CURSOR
                QApplication::restoreOverrideCursor();
            #endif
            QMessageBox::information(this, pApp->applicationName(), tr("Could not download the article."));
            return;
        }

        myEditor->setPlainText(sTmpArticle);
        myEditor->document()->setModified(true);
        documentWasModified();


#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif

        downloadImages(sSitename);
        myTabwidgetRawPreview->setCurrentIndex(myTabwidgetRawPreview->indexOf(myEditor));
    }
}

// -----------------------------------------------------------------------------------------------
// DOWNLOAD IN ARTICLES INCLUDED IMAGES

void CInyokaEdit::downloadImages(const QString &sArticlename)
{
    int iRet = 0;
    QByteArray tempResult;
    QString sMetadata("");
    QStringList sListTmp, sListMetadata;
    const QString sScriptName("tmpDlScript");

    // Start metadata download
    QProcess procDownloadMetadata;
    procDownloadMetadata.start("wget -O - " + mySettings->getInyokaUrl() + "/" + sArticlename + "?action=metaexport");

    if (!procDownloadMetadata.waitForStarted()) {
        QMessageBox::critical(this, pApp->applicationName(), tr("Could not start download of the meta data."));
        procDownloadMetadata.kill();
        return;
    }
    if (!procDownloadMetadata.waitForFinished()) {
        QMessageBox::critical(this, pApp->applicationName(), tr("Error while downloading meta data."));
        procDownloadMetadata.kill();
        return;
    }

    tempResult = procDownloadMetadata.readAll();
    sMetadata = QString::fromLocal8Bit(tempResult);

    // Site does not exist etc.
    if ("" == sMetadata) {
        QMessageBox::information(this, pApp->applicationName(), tr("Could not find meta data."));
        return;
    }

    // Copy metadata line by line in list
    sListTmp << sMetadata.split("\n");

    // Get only attachments article metadata
    for (int i = 0; i < sListTmp.size(); i++) {
        if (sListTmp[i].startsWith("X-Attach: " + sArticlename + "/", Qt::CaseInsensitive)) {
            sListMetadata << sListTmp[i];
            //qDebug() << sListTmp[i];
        }
    }

    // If attachments exist
    if (sListMetadata.size() > 0) {

        // Ask if images should be downloaded (if not enabled by default in settings)
        if (false == mySettings->getAutomaticImageDownload()) {
            iRet = QMessageBox::question(this, pApp->applicationName(), tr("Do you want to download the images which are attached to the article?"), QMessageBox::Yes, QMessageBox::No);
        }
        else {
            iRet = QMessageBox::Yes;
        }

        if (QMessageBox::Yes == iRet) {

            // File for download script
            QFile tmpScriptfile(StylesAndImagesDir.absolutePath() + "/" + sScriptName);

            // No write permission
            if (!tmpScriptfile.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, pApp->applicationName(), tr("Could not create temporary download file!"));
                return;
            }

            // Stream for output in file
            QTextStream scriptOutputstream(&tmpScriptfile);
            scriptOutputstream << "#!/bin/bash\n"
                                  "# Temporary script for downloading images from an article\n"
                                  "#\n\necho \"Downloading images...\"\n"
                                  "cd " << StylesAndImagesDir.absolutePath() << endl;

            // Write wget download lines
            QString sTmp("");
            for (int j = 0; j < sListMetadata.size(); j++) {
                // Trim image infos lines
                sListMetadata[j].remove(0, 10);  // Remove "X-Attach: "

                // http://wiki.ubuntuusers.de/_image?target=Kontact/uebersicht.png
                sTmp = sListMetadata[j];
                sTmp = sTmp.remove(sArticlename + "/", Qt::CaseInsensitive);
                scriptOutputstream << "wget -nv " << mySettings->getInyokaUrl() << "/_image?target=" << sListMetadata[j].toLower() << " -O " << sTmp << endl;
            }
            scriptOutputstream << "sleep 2\n"
                                  "echo \"Finished image download.\"\n" << endl;

            tmpScriptfile.close();

            // Make script executable
            tmpScriptfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);

            // Start download script
            try
            {
                QString sTmpFilePath = StylesAndImagesDir.absolutePath() + "/" + sScriptName;
                myImageDownloadProgress = new CProgressDialog(sTmpFilePath, pApp->applicationName(), this, StylesAndImagesDir.absolutePath());
                }
            catch (std::bad_alloc& ba)
            {
                std::cerr << "ERROR: Caught bad_alloc in \"downloadImages()\": " << ba.what() << std::endl;
                QMessageBox::critical(this, pApp->applicationName(), "Error while memory allocation: bad_alloc - myImageDownloadProgress");
                exit (-9);
            }

            myImageDownloadProgress->open();
        }
    }

}

// -----------------------------------------------------------------------------------------------
void CInyokaEdit::showHtmlPreview(const QString &filename){

    if (mySettings->getShowStatusbar()) {
        statusBar()->showMessage(tr("Opening preview", "GUI: Status bar"));
    }
    myWebview->history()->clear();  // Clear history (clicked links)

    if (false == mySettings->getPreviewInEditor()){
        // Open html-file in system web browser
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    }
    else{
        myWebview->load(QUrl::fromLocalFile(filename));
    }
}

// Wait until loading has finished
void CInyokaEdit::loadPreviewFinished(bool bSuccess) {
    if (bSuccess) {
        myTabwidgetRawPreview->setCurrentIndex(myTabwidgetRawPreview->indexOf(myWebview));
    }
    else {
        QMessageBox::warning(this, pApp->applicationName(), tr("Error while loading preview."));
    }
}

// -----------------------------------------------------------------------------------------------
// Open recent opened file

void CInyokaEdit::openRecentFile(int iEntry) {
    if (maybeSave()) {
        loadFile(mySettings->getRecentFiles()[iEntry]);
    }
}

void CInyokaEdit::updateRecentFiles(const QString &sFileName) {

    QStringList sListTmp;

    if (sFileName != "_-CL3AR#R3C3NT#F!L35-_") {
        sListTmp = mySettings->getRecentFiles();

        // Remove entry if exists
        if (sListTmp.contains(sFileName)) {
            sListTmp.removeAll(sFileName);
        }
        // Add file name to list
        sListTmp.push_front(sFileName);

        // Remove all entries from end, if list is too long
        while (sListTmp.size() > mySettings->getMaxNumOfRecentFiles() || sListTmp.size() > mySettings->getNumOfRecentFiles()) {
            sListTmp.removeLast();
        }

        for (int i = 0; i < mySettings->getMaxNumOfRecentFiles(); i++) {
            // Set list menu entries
            if (i < sListTmp.size()) {
                LastOpenedFilesAct[i]->setText(sListTmp[i]);
                LastOpenedFilesAct[i]->setVisible(true);
            }
            else {
                LastOpenedFilesAct[i]->setVisible(false);
            }

        }
        if (sListTmp.size() > 0) {
            ui->fileMenuLastOpened->setEnabled(true);
        }
    }

    // Clear list
    else {
        sListTmp.clear();
        ui->fileMenuLastOpened->setEnabled(false);
    }

    mySettings->setRecentFiles(sListTmp);
}

void CInyokaEdit::clearRecentFiles(){
    updateRecentFiles("_-CL3AR#R3C3NT#F!L35-_");
}

// -----------------------------------------------------------------------------------------------
// Spell checker (original code form http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell)

void CInyokaEdit::checkSpelling()
{

#if not defined _WIN32
    QString dictPath = "/usr/share/hunspell/" + mySettings->getSpellCheckerLanguage();
    if (!QFile::exists(dictPath + ".dic") || !QFile::exists(dictPath + ".aff")) {
        QMessageBox::critical(this, pApp->applicationName(), "Error: Spell checker dictionary file does not exist!");
        return;
    }

    QString userDict= StylesAndImagesDir.absolutePath() + "/userDict_" + mySettings->getSpellCheckerLanguage() + ".txt";
    if (!QFile::exists(userDict)) {
        QFile userDictFile(userDict);
        if( userDictFile.open(QIODevice::WriteOnly) ) {
            userDictFile.close();
        }
        else {
            QMessageBox::warning(0, pApp->applicationName(), "User dictionary file could not be created.");
        }
    }
    CSpellChecker *spellChecker = new CSpellChecker(dictPath, userDict);
    CSpellCheckDialog *checkDialog = new CSpellCheckDialog(spellChecker, this);

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(QColor("#ff6060")));
    highlightFormat.setForeground(QBrush(QColor("#000000")));
    // Alternative format
    //highlightFormat.setUnderlineColor(QColor("red"));
    //highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    // Save the position of the current cursor
    QTextCursor oldCursor = myEditor->textCursor();

    // Create a new cursor to walk through the text
    QTextCursor cursor(myEditor->document());

    // Don't call cursor.beginEditBlock(), as this prevents the redraw after changes to the content
    // cursor.beginEditBlock();
    while(!cursor.atEnd()) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

        // Workaround for better recognition of words punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        if(!word.isEmpty() && !spellChecker->spell(word)) {
            QTextCursor tmpCursor(cursor);
            tmpCursor.setPosition(cursor.anchor());
            myEditor->setTextCursor(tmpCursor);
            myEditor->ensureCursorVisible();

            // Highlight the unknown word
            QTextEdit::ExtraSelection es;
            es.cursor = cursor;
            es.format = highlightFormat;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            myEditor->setExtraSelections(esList);
            QCoreApplication::processEvents();

            // Ask user what to do
            CSpellCheckDialog::SpellCheckAction spellResult = checkDialog->checkWord(word);

            // Reset the word highlight
            esList.clear();
            myEditor->setExtraSelections(esList);
            QCoreApplication::processEvents();

            if(spellResult == CSpellCheckDialog::AbortCheck)
                break;

            switch(spellResult) {
                case CSpellCheckDialog::ReplaceOnce:
                    cursor.insertText(checkDialog->replacement());
                    break;

                default:
                    break;
            }
            QCoreApplication::processEvents();
        }
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }

    //cursor.endEditBlock();
    myEditor->setTextCursor(oldCursor);

    if (spellChecker != NULL) { delete spellChecker; }
    spellChecker = NULL;
    if (checkDialog != NULL) { delete checkDialog; }
    checkDialog = NULL;

    QMessageBox::information(this, pApp->applicationName(), tr("Spell check has finished."));

// Windows
#else
    QMessageBox::information(this, pApp->applicationName(), tr("Spell checker is currently not supported for windows."));
#endif
}

// -----------------------------------------------------------------------------------------------
// Report a bug

void CInyokaEdit::reportBug(){
    // Ubuntu: Using Apport
    if (QFile::exists("/usr/bin/ubuntu-bug")) {
        // Start apport
        QProcess procApport;
        procApport.start("ubuntu-bug " + pApp->applicationName().toLower());

        if (!procApport.waitForStarted()) {
            QMessageBox::critical(this, pApp->applicationName(), tr("Error while starting Apport."));
            return;
        }
        if (!procApport.waitForFinished()) {
            QMessageBox::critical(this, pApp->applicationName(), tr("Error while executing Apport."));
            return;
        }
    }
    // Not Ubuntu: Load Launchpad bug tracker
    else {
        QDesktopServices::openUrl(QUrl("https://bugs.launchpad.net/inyokaedit"));
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

/****************************************************************************
 ****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This code is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this code under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

// FILE FUNCTIONS (open, save, load...)

void CInyokaEdit::newFile()
{
    if (maybeSave()) {
        myEditor->clear();
        setCurrentFile("");
        myTabwidgetRawPreview->setCurrentIndex(myTabwidgetRawPreview->indexOf(myEditor));
    }
}

void CInyokaEdit::open()
{
    if (maybeSave()) {
        QString sFileName = QFileDialog::getOpenFileName(this, tr("Open file", "GUI: Open file dialog"), mySettings->getLastOpenedDir().absolutePath());  // File dialog opens last used folder
        if (!sFileName.isEmpty()){
            QFileInfo tmpFI(sFileName);
            mySettings->setLastOpenedDir(tmpFI.absoluteDir());
            loadFile(sFileName);
            myTabwidgetRawPreview->setCurrentIndex(myTabwidgetRawPreview->indexOf(myEditor));
        }
    }
}

bool CInyokaEdit::save()
{
    if (sCurFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(sCurFile);
    }
}

bool CInyokaEdit::saveAs()
{
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save file", "GUI: Save file dialog"), mySettings->getLastOpenedDir().absolutePath());  // File dialog opens last used folder
    if (sFileName.isEmpty())
        return false;

    QFileInfo tmpFI(sFileName);
    mySettings->setLastOpenedDir(tmpFI.absoluteDir());

    return saveFile(sFileName);
}

void CInyokaEdit::about()
{
    QMessageBox::about(this, tr("About %1", "About dialog <sAppName>").arg(pApp->applicationName()),
                       tr("<b>%1</b> - Editor for Inyoka-based portals<br />"
                          "Version: %2<br /><br />"
                          "&copy; 2011, the %3 authors<br />"
                          "Licence: <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License Version 3</a><br /><br />"
                          "This application uses icons from <a href=\"http://tango.freedesktop.org\">Tango project</a>.", "About dialog text, <sAppName>, <sVERSION>, <sAppName>").arg(pApp->applicationName()).arg(sVERSION).arg(pApp->applicationName()));
}

void CInyokaEdit::documentWasModified()
{
    setWindowModified(myEditor->document()->isModified());
}

// Handle unsaved files
bool CInyokaEdit::maybeSave()
{
    if (myEditor->document()->isModified()) {
        QMessageBox::StandardButton ret;
        QString sTempCurFileName;
        if ("" == sCurFile){
            sTempCurFileName = tr("Untitled", "No file name set");
        }
        else {
            QFileInfo tempCurFile(sCurFile);
            sTempCurFileName = tempCurFile.fileName();
        }

        ret = QMessageBox::warning(this, pApp->applicationName(),
                                   tr("The document \"%1\" has been modified.\n"
                                      "Do you want to save your changes or discard them?", "Msg: Unsaved <sTempCurFileName>").arg(sTempCurFileName),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (QMessageBox::Save == ret)
            return save();
        else if (QMessageBox::Cancel == ret)
            return false;
    }
    return true;
}

void CInyokaEdit::loadFile(const QString &sFileName)
{
    QFile file(sFileName);
    // No permission to read
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, pApp->applicationName(),
                             tr("The file \"%1\" could not be opened:\n%2.", "Msg: Can not open file, <sFileName>, <ErrorString>")
                             .arg(sFileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    in.setAutoDetectUnicode(true);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    myEditor->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    updateRecentFiles(sFileName);
    setCurrentFile(sFileName);
    if (mySettings->getShowStatusbar()) {
        statusBar()->showMessage(tr("File loaded", "GUI: Status bar"), 2000);
    }
}

bool CInyokaEdit::saveFile(const QString &sFileName)
{
    QFile file(sFileName);
    // No write permission
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, pApp->applicationName(),
                             tr("The file \"%1\" could not be saved:\n%2.", "Msg: Can not save file, <sFileName>, <ErrorString>")
                             .arg(sFileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out.setAutoDetectUnicode(true);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << myEditor->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    updateRecentFiles(sFileName);
    setCurrentFile(sFileName);
    if (mySettings->getShowStatusbar()) {
        statusBar()->showMessage(tr("File saved"), 2000);
    }
    return true;
}

void CInyokaEdit::setCurrentFile(const QString &sFileName)
{
    sCurFile = sFileName;
    myEditor->document()->setModified(false);
    setWindowModified(false);

    QString sShownName = sCurFile;
    if (sCurFile.isEmpty())
        sShownName = tr("Untitled");
    setWindowFilePath(sShownName);
}

QString CInyokaEdit::strippedName(const QString &sFullFileName)
{
    return QFileInfo(sFullFileName).fileName();
}

// Close event (File -> Close or X)
void CInyokaEdit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        mySettings->writeSettings(saveGeometry(), saveState());
        event->accept();
    } else {
        event->ignore();
    }
}
