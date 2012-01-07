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

#define sVERSION "0.1.0"

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

    // Create folder for downloaded article images
    m_tmpPreviewImgDir = StylesAndImagesDir.absolutePath() + "/tmpImages";
    if (!m_tmpPreviewImgDir.exists()) {
        m_tmpPreviewImgDir.mkdir(m_tmpPreviewImgDir.absolutePath());
    }

    // Create all objects (after definition of StylesAndImagesDir AND m_tmpPreviewImgDir)
    this->createObjects();

    // Setup gui, menus, actions, toolbar...
    this->setupEditor();
    this->createActions();
    this->createMenus();
    this->createToolBars();

    bool bDialogShowed = false;
    // Download style files if preview/styles/imgages folders doesn't exist (/home/user/.InyokaEdit)
    if (!StylesAndImagesDir.exists() || !QDir(StylesAndImagesDir.absolutePath() + "/img").exists() ||
        !QDir(StylesAndImagesDir.absolutePath() + "/styles").exists() || !QDir(StylesAndImagesDir.absolutePath() + "/Wiki").exists()){
        StylesAndImagesDir.mkdir(StylesAndImagesDir.absolutePath());  // Create folder because user may not start download. Folder is needed for preview.
        if(myDownloadModule->loadInyokaStyles()) {
            mySettings->setConfVersion(sVERSION);
        }
        bDialogShowed = true;
    }

    // Download styles or open file from command line argument
    if (pApp->argc() >= 2) {
        QString sTmp = pApp->argv()[1];

        // Download inyoka styles
        if ("--dlstyles" == sTmp){
            if(myDownloadModule->loadInyokaStyles()) {
                mySettings->setConfVersion(sVERSION);
            }
        }
        else {
            if ("--log" != sTmp) {
                myFileOperations->loadFile(pApp->argv()[1]);
            }
            else if ("--log" == sTmp && pApp->argc() >= 3) {
                myFileOperations->loadFile(pApp->argv()[2]);
            }
        }
    }

    // In config file an older version was found
    if (sVERSION != mySettings->getConfVersion() && !bDialogShowed) {
        if(myDownloadModule->loadInyokaStyles()) {
            mySettings->setConfVersion(sVERSION);
        }
    }

    if (mySettings->getShowStatusbar()) {
        this->statusBar()->showMessage(tr("Ready"));
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

        myDownloadModule = new CDownload(this, pApp->applicationName(), pApp->applicationDirPath(), StylesAndImagesDir);
        if (bLogging) { std::clog << "Created myDownloadModule" << std::endl; }

        myEditor = new CTextEditor(mySettings->getCodeCompletion());  // Has to be create before find/replace
        if (bLogging) { std::clog << "Created myEditor" << std::endl; }

        myFileOperations = new CFileOperations(this, myEditor, mySettings, pApp->applicationName());
        if (bLogging) { std::clog << "Created myFileOperations" << std::endl; }

        myCompleter = new QCompleter(sListCompleter, this);
        if (bLogging) { std::clog << "Created myCompleter" << std::endl; }

        myInterWikiLinks = new CInterWiki(pApp);  // Has to be created before parser
        if (bLogging) { std::clog << "Created myInterWikiLinks" << std::endl; }

        myHighlighter = new CHighlighter(myEditor->document());
        if (bLogging) { std::clog << "Created myHighlighter" << std::endl; }

        myParser = new CParser(myEditor->document(), mySettings->getInyokaUrl(), StylesAndImagesDir, m_tmpPreviewImgDir, myInterWikiLinks->getInterwikiLinks(), myInterWikiLinks->getInterwikiLinksUrls());
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
    this->setWindowIcon(QIcon(":/images/" + pApp->applicationName().toLower() + "_64x64.png"));

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

    setCentralWidget(myTabwidgetRawPreview);
    myTabwidgetRawPreview->setTabPosition(QTabWidget::West);
    myTabwidgetRawPreview->addTab(myEditor, tr("Raw format"));

    myTabwidgetRawPreview->addTab(myWebview, tr("Preview"));
    if (false == mySettings->getPreviewInEditor())
        myTabwidgetRawPreview->setTabEnabled(myTabwidgetRawPreview->indexOf(myWebview), false);

    connect(myWebview, SIGNAL(loadFinished(bool)),
            this, SLOT(loadPreviewFinished(bool)));

    myFileOperations->setCurrentFile("");
    this->setUnifiedTitleAndToolBarOnMac(true);

    connect(myDownloadModule, SIGNAL(sendArticleText(QString)),
            this, SLOT(displayArticleText(QString)));

    // Hide statusbar, if option is false
    if (false == mySettings->getShowStatusbar()) {
        this->setStatusBar(0);
    }

    // Restore window and toolbar settings
    // Settings have to be restored after toolbars are created!
    this->restoreGeometry(mySettings->getWindowGeometry());
    this->restoreState(mySettings->getWindowState());  // Restore toolbar position etc.

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
        connect(ui->newAct, SIGNAL(triggered()), myFileOperations, SLOT(newFile()));

        // Open file
        ui->openAct->setShortcuts(QKeySequence::Open);
        connect(ui->openAct, SIGNAL(triggered()), myFileOperations, SLOT(open()));
/*
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
        connect(mySigMapLastOpenedFiles, SIGNAL(mapped(int)), myFileOperations, SLOT(openRecentFile(int)));
*/
        // Clear recent files list
        clearRecentFilesAct = new QAction(tr("Clear list"), this);
        connect(clearRecentFilesAct, SIGNAL(triggered()), myFileOperations, SLOT(clearRecentFiles()));

        // Save file
        ui->saveAct->setShortcuts(QKeySequence::Save);
        connect(ui->saveAct, SIGNAL(triggered()), myFileOperations, SLOT(save()));

        // Save file as...
        ui->saveAsAct->setShortcuts(QKeySequence::SaveAs);
        connect(ui->saveAsAct, SIGNAL(triggered()), myFileOperations, SLOT(saveAs()));

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
    ui->fileMenuLastOpened->addActions(myFileOperations->getLastOpenedFiles());
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

        if ("" == myFileOperations->getCurrentFile() || tr("Untitled") == myFileOperations->getCurrentFile()){
            myParser->genOutput("");
        }
        else{
            QFileInfo fi(myFileOperations->getCurrentFile());
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
    myEditor->insertPlainText(QString::fromUtf8(myInsertSyntaxElement->getElementInyokaCode(sMenuEntry.toStdString(), myEditor->textCursor().selectedText().toStdString()).c_str()));
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

void CInyokaEdit::receiveMenuLastOpenedState(bool bEnabled) {
    ui->fileMenuLastOpened->setEnabled(bEnabled);
}

void CInyokaEdit::receiveStatusbarMessage(const QString &sMessage) {
    this->statusBar()->showMessage(sMessage, 2000);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::downloadArticle() {
    if (!myFileOperations->maybeSave()) {
        return;
    }
    else {
        myDownloadModule->downloadArticle(m_tmpPreviewImgDir, mySettings->getInyokaUrl(), mySettings->getAutomaticImageDownload());
    }
}

void CInyokaEdit::displayArticleText(const QString &sArticleText) {
    myEditor->setPlainText(sArticleText);
    myEditor->document()->setModified(true);
    this->documentWasModified();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
void CInyokaEdit::showHtmlPreview(const QString &filename){

    if (mySettings->getShowStatusbar()) {
        this->statusBar()->showMessage(tr("Opening preview", "GUI: Status bar"));
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
// -----------------------------------------------------------------------------------------------

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
    CSpellChecker *spellChecker = new CSpellChecker(dictPath, userDict, this);
    spellChecker->start(myEditor);

    if (spellChecker != NULL) { delete spellChecker; }
    spellChecker = NULL;

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
        procApport.start("ubuntu-bug", QStringList() << pApp->applicationName().toLower());

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

// Set modified flag for window
void CInyokaEdit::documentWasModified()
{
    myTabwidgetRawPreview->setCurrentIndex(myTabwidgetRawPreview->indexOf(myEditor));
    this->setWindowModified(myEditor->document()->isModified());
}

// -----------------------------------------------------------------------------------------------

// About info box
void CInyokaEdit::about()
{
    QMessageBox::about(this, tr("About %1", "About dialog <sAppName>").arg(pApp->applicationName()),
                       tr("<b>%1</b> - Editor for Inyoka-based portals<br />"
                          "Version: %2<br /><br />"
                          "&copy; 2011, the %3 authors<br />"
                          "Licence: <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License Version 3</a><br /><br />"
                          "This application uses icons from <a href=\"http://tango.freedesktop.org\">Tango project</a>.", "About dialog text, <sAppName>, <sVERSION>, <sAppName>").arg(pApp->applicationName()).arg(sVERSION).arg(pApp->applicationName()));
}

// -----------------------------------------------------------------------------------------------

// Close event (File -> Close or X)
void CInyokaEdit::closeEvent(QCloseEvent *event)
{
    if (myFileOperations->maybeSave()) {
        mySettings->writeSettings(saveGeometry(), saveState());
        event->accept();
    } else {
        event->ignore();
    }
}
