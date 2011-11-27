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

#define sVERSION "0.0.8"

#include <QtGui>
#include <QtWebKit/QWebView>

#include "CInyokaEdit.h"

CInyokaEdit::CInyokaEdit(const QString &name, const int argc, char **argv)
    : myCompleter(0), sAppName(name)
{
    bLogging = false;

    // Check for command line arguments
    if (argc >= 2) {
        QString sTmp = argv[1];

        if ("--version"== sTmp) {
            std::cout << argv[0] << "\t Version: " << sVERSION << std::endl;
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
    StylesAndImagesDir = QDir::homePath() + "/." + sAppName;

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
    if (argc >= 2) {
        QString sTmp = argv[1];

        // Download inyoka styles
        if ("--dlstyles" == sTmp){
            DownloadStyles(StylesAndImagesDir);
        }
        else {
            if ("--log" != sTmp) {
                loadFile(argv[1]);
            }
            else if ("--log" == sTmp && argc >= 3) {
                loadFile(argv[2]);
            }
        }
    }

    // In config file an older version was found
    if (sVERSION != mySettings->getConfVersion()) {
        DownloadStyles(StylesAndImagesDir);
    }

    if (mySettings->getShowStatusbar()) {
        statusBar()->showMessage(tr("Ready", "GUI: Statusbar"));
    }
    if (bLogging) { std::clog << "Created CInyokaEdit" << std::endl; }
}

// -----------------------------------------------------------------------------------------------

CInyokaEdit::~CInyokaEdit(){

    if (myInsertSyntaxElement != NULL) { delete myInsertSyntaxElement; }
    myInsertSyntaxElement = NULL;
    if (bLogging) { std::clog << "Deleted myInsertSyntaxElement" << std::endl; }

    if (myWebview != NULL) { delete myWebview; }
    myWebview = NULL;
    if (bLogging) { std::clog << "Deleted myWebview" << std::endl; }

    if (myTabwidget != NULL) { delete myTabwidget; }
    myTabwidget = NULL;
    if (bLogging) { std::clog << "Deleted myTabwidget" << std::endl; }

    if (myParser != NULL) { delete myParser; }
    myParser = NULL;
    if (bLogging) { std::clog << "Deleted myParser" << std::endl; }

    if (myCompleter != NULL) { delete myCompleter; }
    myCompleter = NULL;
    if (bLogging) { std::clog << "Deleted myCompleter" << std::endl; }

    if (mySettings != NULL) { delete mySettings; }
    mySettings = NULL;
    if (bLogging) { std::clog << "Deleted mySettings" << std::endl; }

    if (m_findReplaceDialog != NULL) { delete m_findReplaceDialog; }
    m_findReplaceDialog = NULL;
    if (m_findDialog != NULL) { delete m_findDialog; }
    m_findDialog = NULL;
    if (bLogging) { std::clog << "Deleted find/replace dialogs" << std::endl; }

//    delete myEditor;
//    myEditor = NULL;
//    delete myHighlighter;
//    myHighlighter = NULL;
    if (bLogging) { std::clog << "Deleted CInyokaEdit" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::createObjects() {

    try
    {
        m_findDialog = new FindDialog(this);  // Has to be create before readSettings
        m_findReplaceDialog = new FindReplaceDialog(this);
        if (bLogging) { std::clog << "Created find/replace dialogs" << std::endl; }

        mySettings = new CSettings(StylesAndImagesDir, sAppName, *m_findDialog, *m_findReplaceDialog);
        if (bLogging) { std::clog << "Created mySettings" << std::endl; }
        // Load settings from config file
        mySettings->readSettings();
        if (bLogging) { std::clog << "Read settings" << std::endl; }

        myEditor = new CTextEditor(mySettings->getCodeCompletion());  // Has to be create before find/replace
        if (bLogging) { std::clog << "Created myEditor" << std::endl; }

        myCompleter = new QCompleter(sListCompleter, this);
        if (bLogging) { std::clog << "Created myCompleter" << std::endl; }

        myInterWikiLinks = new CInterWiki(sAppName);  // Has to be created before parser
        if (bLogging) { std::clog << "Created myInterWikiLinks" << std::endl; }

        myHighlighter = new CHighlighter(myEditor->document());
        if (bLogging) { std::clog << "Created myHighlighter" << std::endl; }

        myParser = new CParser(myEditor->document(), mySettings->getInyokaUrl(), StylesAndImagesDir, myInterWikiLinks->getInterwikiLinks(), myInterWikiLinks->getInterwikiLinksUrls());
        if (bLogging) { std::clog << "Created myParser" << std::endl; }

        myTabwidget = new QTabWidget;
        if (bLogging) { std::clog << "Created myTabwidget" << std::endl; }
        myWebview = new QWebView(this);
        if (bLogging) { std::clog << "Created myWebview" << std::endl; }

        myInsertSyntaxElement = new CInsertSyntaxElement;
        if (bLogging) { std::clog << "Created myInsertSyntaxElement" << std::endl; }
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createObjects()\": " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "createActions()");
        exit (-1);
    }

    if (bLogging) { std::clog << "Created objects" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::setupEditor()
{
    // Application icon
    setWindowIcon(QIcon(":/images/" + sAppName.toLower() + "_64x64.png"));

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

    setCentralWidget(myTabwidget);
    myTabwidget->setTabPosition(QTabWidget::West);
    myTabwidget->addTab(myEditor, tr("Raw format", "GUI: Vertical tab"));

    myTabwidget->addTab(myWebview, tr("Preview", "GUI: Vertical tab"));
    if (false == mySettings->getPreviewInEditor())
        myTabwidget->setTabEnabled(myTabwidget->indexOf(myWebview), false);

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
        newAct = new QAction(QIcon(":/images/document-new.png"), tr("&New", "GUI: File menu"), this);
        newAct->setShortcuts(QKeySequence::New);
        newAct->setStatusTip(tr("Create a new file", "GUI: Status tip"));
        connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

        // Open file
        openAct = new QAction(QIcon(":/images/document-open.png"), tr("&Open...", "GUI: File menu"), this);
        openAct->setShortcuts(QKeySequence::Open);
        openAct->setStatusTip(tr("Open a file", "GUI: Status tip"));
        connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

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
        clearRecentFilesAct = new QAction(tr("Clear list", "GUI: File menu"), this);
        connect(clearRecentFilesAct, SIGNAL(triggered()), this, SLOT(clearRecentFiles()));

        // Save file
        saveAct = new QAction(QIcon(":/images/document-save.png"), tr("&Save", "GUI: File menu"), this);
        saveAct->setShortcuts(QKeySequence::Save);
        saveAct->setStatusTip(tr("Save current document", "GUI: Status tip"));
        connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

        // Save file as...
        saveAsAct = new QAction(tr("Save as...", "GUI: File menu"), this);
        saveAsAct->setShortcuts(QKeySequence::SaveAs);
        saveAsAct->setStatusTip(tr("Save current document under a new name", "GUI: Status tip"));
        connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

        // Exit application
        exitAct = new QAction(tr("Quit", "GUI: File menu"), this);
        exitAct->setShortcuts(QKeySequence::Quit);
        exitAct->setStatusTip(tr("Quit the application", "GUI: File menu"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / file menu: " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "\"createActions()\" / file menu");
        exit (-2);
    }

    // Edit menu
    try {
        // Edit: Cut
        cutAct = new QAction(QIcon(":/images/edit-cut.png"), tr("Cut", "GUI: Edit menu"), this);
        cutAct->setShortcuts(QKeySequence::Cut);
        cutAct->setStatusTip(tr("Cut highlighted text", "GUI: Status tip"));
        connect(cutAct, SIGNAL(triggered()), myEditor, SLOT(cut()));

        // Edit: Copy
        copyAct = new QAction(QIcon(":/images/edit-copy.png"), tr("Copy", "GUI: Edit menu"), this);
        copyAct->setShortcuts(QKeySequence::Copy);
        copyAct->setStatusTip(tr("Copy highlighted text to clipboard", "GUI: Status tip"));
        connect(copyAct, SIGNAL(triggered()), myEditor, SLOT(copy()));

        // Edit: Paste
        pasteAct = new QAction(QIcon(":/images/edit-paste.png"), tr("Paste", "GUI: Edit menu"), this);
        pasteAct->setShortcuts(QKeySequence::Paste);
        pasteAct->setStatusTip(tr("Paste text from clipboard", "GUI: Status tip"));
        connect(pasteAct, SIGNAL(triggered()), myEditor, SLOT(paste()));

        // Edit: Undo
        undoAct = new QAction(QIcon(":/images/edit-undo.png"), tr("Undo", "GUI: Edit menu"), this);
        undoAct->setShortcuts(QKeySequence::Undo);
        undoAct->setStatusTip(tr("Undo changes", "GUI: Status tip"));
        connect(undoAct, SIGNAL(triggered()), myEditor, SLOT(undo()));

        // Edit: Redo
        redoAct = new QAction(QIcon(":/images/edit-redo.png"), tr("Redo", "GUI: Edit menu"), this);
        redoAct->setShortcuts(QKeySequence::Redo);
        redoAct->setStatusTip(tr("Redo changes", "GUI: Status tip"));
        connect(redoAct, SIGNAL(triggered()), myEditor, SLOT(redo()));

        // Edit: Find
        searchAct = new QAction(QIcon(":/images/edit-find.png"), tr("Find...", "GUI: Edit menu"), this);
        searchAct->setShortcuts(QKeySequence::Find);
        searchAct->setStatusTip(tr("Find text", "GUI: Status tip"));
        connect(searchAct, SIGNAL(triggered()), m_findDialog, SLOT(show()));

        // Edit: Replace
        replaceAct = new QAction(QIcon(":/images/edit-find-replace.png"), tr("Replace...", "GUI: Edit menu"), this);
        replaceAct->setShortcuts(QKeySequence::Replace);
        replaceAct->setStatusTip(tr("Replace text", "GUI: Status tip"));
        connect(replaceAct, SIGNAL(triggered()), m_findReplaceDialog, SLOT(show()));

        // Edit: Find next
        findNextAct = new QAction(QIcon(":/images/go-down.png"), tr("Find next", "GUI: Edit menu"), this);
        findNextAct->setShortcuts(QKeySequence::FindNext);
        findNextAct->setStatusTip(tr("Find next (search forward)", "GUI: Status tip"));
        connect(findNextAct, SIGNAL(triggered()), m_findDialog, SLOT(findNext()));

        // Edit: Find previous
        findPreviousAct = new QAction(QIcon(":/images/go-up.png"), tr("Find previous", "GUI: Edit menu"), this);
        findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
        findPreviousAct->setStatusTip(tr("Find previous (search backward)", "GUI: Status tip"));
        connect(findPreviousAct, SIGNAL(triggered()), m_findDialog, SLOT(findPrev()));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / edit menu: " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "\"createActions()\" / edit menu");
        exit (-3);
    }

    // About menu
    try {
        // Report a bug using apport
        reportBugAct = new QAction(QIcon(":images/bug.png"), tr("Report bug", "GUI: About menu"), this);
        reportBugAct->setStatusTip(tr("Report a bug - For this a launchpad account is needed!", "GUI: Status tip"));
        connect(reportBugAct, SIGNAL(triggered()), this, SLOT(reportBug()));

        // Open about windwow
        aboutAct = new QAction(QIcon(":images/question.png"), tr("About", "GUI: About menu") + " " + sAppName, this);
        aboutAct->setStatusTip(tr("Shows the about box of this application", "GUI: Status tip"));
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / about menu: " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "\"createActions()\" / about menu");
        exit (-4);
    }

    // Set / initialize / connect cut / copy / redo / undo
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(myEditor, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(myEditor, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    undoAct->setEnabled(false);
    connect(myEditor, SIGNAL(undoAvailable(bool)),
            undoAct, SLOT(setEnabled(bool)));
    redoAct->setEnabled(false);
    connect(myEditor, SIGNAL(redoAvailable(bool)),
            redoAct, SLOT(setEnabled(bool)));

    try {
        // Show html preview
        previewAct = new QAction(QIcon(":/images/preview.png"), tr("Open preview", "GUI: Inyoka toolbar"), this);
        previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
        previewAct->setStatusTip(tr("Open preview of current article", "GUI: Status tip"));
        connect(previewAct, SIGNAL(triggered()), this, SLOT(previewInyokaPage()));

        // Click on tabs of widget - int = index of tab
        connect(myTabwidget, SIGNAL(currentChanged(int)), this, SLOT(previewInyokaPage(int)));

        // Download Inyoka article
        downloadArticleAct = new QAction(QIcon(":/images/network-receive.png"), tr("Download raw format", "GUI: Inyoka toolbar"), this);
        downloadArticleAct->setStatusTip(tr("Downloads raw format of an existing Inyoka article", "GUI: Status tip"));
        downloadArticleAct->setPriority(QAction::LowPriority);
        connect(downloadArticleAct, SIGNAL(triggered()), this, SLOT(downloadArticle()));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / inyoka toolbar: " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "\"createActions()\" / inyoka toolbar");
        exit (-5);
    }

    // Insert syntax elements
    try {
        mySigMapTextSamples = new QSignalMapper(this);

        // Insert bold element
        boldAct = new QAction(QIcon(":/images/format-text-bold.png"), tr("Bold", "GUI: Editor toolbar"), this);
        boldAct->setStatusTip(tr("Bold - File names, folders, packages, file extensions", "GUI: Status tip"));
        boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
        boldAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(boldAct, "boldAct");
        connect(boldAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert italic element
        italicAct = new QAction(QIcon(":/images/format-text-italic.png"), tr("Italic", "GUI: Editor toolbar"), this);
        italicAct->setStatusTip(tr("Italic - Menu entries, buttons, G-Conf keys - Always enclosed by quotation marks!", "GUI: Status tip"));
        italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
        italicAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(italicAct, "italicAct");
        connect(italicAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert monotype element
        monotypeAct = new QAction(QIcon(":/images/monotype.png"), tr("Monotype", "GUI: Editor toolbar"), this);
        monotypeAct->setStatusTip(tr("Monotype - Commands and their options, console outputs in continuous text, modules, users, groups", "GUI: Status tip"));
        monotypeAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(monotypeAct, "monotypeAct");
        connect(monotypeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert wiki link
        wikilinkAct = new QAction(QIcon(":/images/go-next.png"), tr("Link to wiki page", "GUI: Editor toolbar"), this);
        wikilinkAct->setStatusTip(tr("Link to an Inyoka wiki page", "GUI: Status tip"));
        wikilinkAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(wikilinkAct, "wikilinkAct");
        connect(wikilinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert extern link
        externalLinkAct = new QAction(QIcon(":/images/internet-web-browser.png"), tr("External link", "GUI: Editor toolbar"), this);
        externalLinkAct->setStatusTip(tr("Link to an external website - always with flag e.g. {en}, {de}!", "GUI: Status tip"));
        externalLinkAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(externalLinkAct, "externalLinkAct");
        connect(externalLinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert image
        imageAct = new QAction(QIcon(":/images/image-x-generic.png"), tr("Insert image", "GUI: Editor toolbar"), this);
        imageAct->setStatusTip(tr("Insert an image", "GUI: Status tip"));
        imageAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(imageAct, "imageAct");
        connect(imageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert code block
        codeblockAct = new QAction(QIcon(":/images/code.png"), tr("Code block", "GUI: Editor toolbar"), this);
        codeblockAct->setStatusTip(tr("Code block - Console output, extract of config files", "GUI: Status tip"));
        codeblockAct->setPriority(QAction::LowPriority);
        mySigMapTextSamples->setMapping(codeblockAct, "codeblockAct");
        connect(codeblockAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        // Insert headline
        headlineBox = new QComboBox();
        headlineBox->setStatusTip(tr("Insert a headline - 5 headline steps are supported", "GUI: Status tip"));

        // Insert sample
        textmacrosBox = new QComboBox();
        textmacrosBox->setStatusTip(tr("Insert text sample", "GUI: Status tip"));

        // Insert text format
        textformatBox = new QComboBox();
        textformatBox->setStatusTip(tr("Insert text format", "GUI: Status tip"));

        insertUnderConstructionAct = new QAction(tr("Under construction", "GUI: Text samples menu - Under construction"), this);
        insertUnderConstructionAct->setStatusTip(tr("Insert \"Under construction\" sample - Only for articles which are currently under construction", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertUnderConstructionAct, "insertUnderConstructionAct");
        connect(insertUnderConstructionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertTestedForAct = new QAction(tr("Tested for", "GUI: Text samples menu - Tested for"), this);
        insertTestedForAct->setStatusTip(tr("Insert \"Tested for\" sample - Indicates if an article was tested", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertTestedForAct, "insertTestedForAct");
        connect(insertTestedForAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertKnowledgeAct = new QAction(tr("Knowledge block", "GUI: Text samples menu - Knowledge block"), this);
        insertKnowledgeAct->setStatusTip(tr("Insert \"Knowledge block\" sample - Reference to elementary articles which are helpful for the understanding of an article", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertKnowledgeAct, "insertKnowledgeAct");
        connect(insertKnowledgeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertTableOfContentsAct = new QAction(tr("Table of contents", "GUI: Text samples menu - Table of contents"), this);
        insertTableOfContentsAct->setStatusTip(tr("Insert \"Table of contents\" sample", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertTableOfContentsAct, "insertTableOfContentsAct");
        connect(insertTableOfContentsAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertAdvancedAct = new QAction(tr("Advanced label", "GUI: Text samples menu - Advanced label"), this);
        insertAdvancedAct->setStatusTip(tr("Insert \"Advanced label\" sample - Indicates that an article is recommended for advanced users only", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertAdvancedAct, "insertAdvancedAct");
        connect(insertAdvancedAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertAwardAct = new QAction(tr("Award label", "GUI: Text samples menu - Award label"), this);
        insertAwardAct->setStatusTip(tr("Insert \"Award label\" sample - For awarded applications", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertAwardAct, "insertAwardAct");
        connect(insertAwardAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertBashCommandAct = new QAction(tr("Bash command", "GUI: Text samples menu - Bash command"), this);
        insertBashCommandAct->setStatusTip(tr("Insert \"Bash command\" sample", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertBashCommandAct, "insertBashCommandAct");
        connect(insertBashCommandAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertNoticeAct = new QAction(tr("Notice box", "GUI: Text samples menu - Notice box"), this);
        insertNoticeAct->setStatusTip(tr("Insert \"Notice box\" sample - Accentuate a special circumstance", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertNoticeAct, "insertNoticeAct");
        connect(insertNoticeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertWarningAct = new QAction(tr("Warning box", "GUI: Text samples menu - Warning box"), this);
        insertWarningAct->setStatusTip(tr("Insert \"Warning box\" sample - Notice for potential risks", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertWarningAct, "insertWarningAct");
        connect(insertWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertExpertsAct = new QAction(tr("Experts information", "GUI: Text samples menu - Experts info box"), this);
        insertExpertsAct->setStatusTip(tr("Insert \"Experts info\" sample - Providing background information", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertExpertsAct, "insertExpertsAct");
        connect(insertExpertsAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertImageAct = new QAction(tr("Image", "GUI: Text samples menu - Simple image"), this);
        insertImageAct->setStatusTip(tr("Insert an image sample", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertImageAct, "imageAct");  // insertImageAct = imageAct !
        connect(insertImageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertImageUnderlineAct = new QAction(tr("Image with underline", "GUI: Text samples menu - Image with underline"), this);
        insertImageUnderlineAct->setStatusTip(tr("Insert an image with underline", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertImageUnderlineAct, "insertImageUnderlineAct");
        connect(insertImageUnderlineAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertImageCollectionAct = new QAction(tr("Image collection (with wordwrap)", "GUI: Text samples menu - Image collection (with wordwrap)"), this);
        insertImageCollectionAct->setStatusTip(tr("Insert an image collection (with wordwrap) sample - Depending on the browser width, automatic word wrapping will be used to align the images", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertImageCollectionAct, "insertImageCollectionAct");
        connect(insertImageCollectionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertImageCollectionInTextAct = new QAction(tr("Image collection (continuous text)", "GUI: Text samples menu - Image collection (continuous text)"), this);
        insertImageCollectionInTextAct->setStatusTip(tr("Insert an image collection (continuous text) sample - Text flows round the collection (max. 3 images, width per image: 200px)", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertImageCollectionInTextAct, "insertImageCollectionInTextAct");
        connect(insertImageCollectionInTextAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertPackageListAct = new QAction(tr("Package macro (obsolete!)", "GUI: Text samples menu - Package macro"), this);
        insertPackageListAct->setStatusTip(tr("Insert \"Package macro\" - This macro is obsolete and shall be used only in exceptional cases", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertPackageListAct, "insertPackageListAct");
        connect(insertPackageListAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertPackageInstallAct = new QAction(tr("Package installation", "GUI: Text samples menu - Package installation"), this);
        insertPackageInstallAct->setStatusTip(tr("Insert \"Package installation\" sample - Create a package list with installation button", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertPackageInstallAct, "insertPackageInstallAct");
        connect(insertPackageInstallAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertPPAAct = new QAction(tr("PPA sample", "GUI: Text samples menu - PPA sample"), this);
        insertPPAAct->setStatusTip(tr("Insert PPA sample - Create a sample for using a Launchpad-PPA, including warning and link to PPA source"));
        mySigMapTextSamples->setMapping(insertPPAAct, "insertPPAAct");
        connect(insertPPAAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertThirdPartyRepoAct = new QAction(tr("Third-party repository", "GUI: Text samples menu - Third-party repo"), this);
        insertThirdPartyRepoAct->setStatusTip(tr("Insert \"Third-party repo\" sample - Create a sample for using third-party repositories (not Ubuntu or Launchpad)", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertThirdPartyRepoAct, "insertThirdPartyRepoAct");
        connect(insertThirdPartyRepoAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertThirdPartyRepoAuthAct = new QAction(tr("Authenticate third-party repo", "GUI: Text samples menu - Authenticate third-party repo"), this);
        insertThirdPartyRepoAuthAct->setStatusTip(tr("Insert a sample for authenticating third-party repositories", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertThirdPartyRepoAuthAct, "insertThirdPartyRepoAuthAct");
        connect(insertThirdPartyRepoAuthAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertThirdPartyPackageAct = new QAction(tr("Third-party package", "GUI: Text samples menu - Third-party package"), this);
        insertThirdPartyPackageAct->setStatusTip(tr("Insert \"Third-party package\" sample - Create a sample for installing DEB-Packages from third-party repos", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertThirdPartyPackageAct, "insertThirdPartyPackageAct");
        connect(insertThirdPartyPackageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertImprovableAct = new QAction(tr("Improvable label", "GUI: Text samples menu - Improvable"), this);
        insertImprovableAct->setStatusTip(tr("Insert \"Improvable\" sample - Article is incomplete or could be extended", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertImprovableAct, "insertImprovableAct");
        connect(insertImprovableAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertFixMeAct = new QAction(tr("Fix me label", "GUI: Text samples menu - Fix me"), this);
        insertFixMeAct->setStatusTip(tr("Insert \"Fix Me\" sample - Article contains descriptions or paragraphs which are not correct (any more)", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertFixMeAct, "insertFixMeAct");
        connect(insertFixMeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertLeftAct = new QAction(tr("Left label", "GUI: Text samples menu - Article is left"), this);
        insertLeftAct->setStatusTip(tr("Insert \"Left\" sample - Only for articles under construction which are left by primal author", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertLeftAct, "insertLeftAct");
        connect(insertLeftAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertThirdPartyPackageWarningAct = new QAction(tr("Third-party package warning", "GUI: Text samples menu - Third-party package warning"), this);
        insertThirdPartyPackageWarningAct->setStatusTip(tr("Insert \"Third-party package warning\" sample - For packages from third-party repositories", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertThirdPartyPackageWarningAct, "insertThirdPartyPackageWarningAct");
        connect(insertThirdPartyPackageWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertThirdPartyRepoWarningAct = new QAction(tr("Third-party repository warning", "GUI: Text samples menu - Third-party repo warning"), this);
        insertThirdPartyRepoWarningAct->setStatusTip(tr("Insert \"Third-party repo warning\" sample - For packages which are installed from third-party repositories", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertThirdPartyRepoWarningAct, "insertThirdPartyRepoWarningAct");
        connect(insertThirdPartyRepoWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        insertThirdPartySoftwareWarningAct = new QAction(tr("Third-party software warning", "GUI: Text samples menu - Third-party software warning"), this);
        insertThirdPartySoftwareWarningAct->setStatusTip(tr("Insert \"Third-party software warning\" sample - Only for software which is installed completly without package repositories", "GUI: Status tip"));
        mySigMapTextSamples->setMapping(insertThirdPartySoftwareWarningAct, "insertThirdPartySoftwareWarningAct");
        connect(insertThirdPartySoftwareWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

        connect(mySigMapTextSamples, SIGNAL(mapped(QString)), this, SLOT(insertTextSample(QString)));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / text samples: " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "\"createActions()\" / text samples");
        exit (-6);
    }

    // ---------------------------------------------------------------------------------------------
    // INTERWIKI-LINKS ACTIONS

    try {
        mySigMapInterWikiLinks = new QSignalMapper(this);
        QList <QAction *> emptyActionList;
        emptyActionList.clear();

        for (int i = 0; i < myInterWikiLinks->getInterwikiLinksGroups().size(); i++) {
            iWikiLinksActions << emptyActionList;
            for (int j = 0; j < myInterWikiLinks->getInterwikiLinksNames()[i].size(); j++) {
                iWikiLinksActions[i] << new QAction(QIcon("/usr/share/" + sAppName.toLower() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksIcons()[i][j]), myInterWikiLinks->getInterwikiLinksNames()[i][j], this);
                mySigMapInterWikiLinks->setMapping(iWikiLinksActions[i][j], QString::number(i) + "," + QString::number(j));
                connect(iWikiLinksActions[i][j], SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
            }
        }

        connect(mySigMapInterWikiLinks, SIGNAL(mapped(QString)), this, SLOT(insertInterwikiLink(QString)));
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: Caught bad_alloc in \"createActions()\" / interwiki actions: " << ba.what() << std::endl;
        QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "\"createActions()\" / interwiki actions");
        exit (-7);
    }

    if (bLogging) { std::clog << "Created actions" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate menus
void CInyokaEdit::createMenus()
{
    // File menu
    fileMenu = menuBar()->addMenu(tr("&File", "GUI: File menu"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenuLastOpened = fileMenu->addMenu(tr("Open &recent", "GUI: File menu"));
    fileMenuLastOpened->addActions(LastOpenedFilesAct);
    fileMenuLastOpened->addSeparator();
    fileMenuLastOpened->addAction(clearRecentFilesAct);
    if (0 == mySettings->getRecentFiles().size()) {
        fileMenuLastOpened->setEnabled(false);
    }
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Edit menu
    editMenu = menuBar()->addMenu(tr("&Edit", "GUI: Edit menu"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(searchAct);
    editMenu->addAction(replaceAct);
    editMenu->addAction(findNextAct);
    editMenu->addAction(findPreviousAct);

    // Insert text sample menu
    insertTextSampleMenu = menuBar()->addMenu(tr("&Text samples", "GUI: Text samples menu"));
    insertBeginningMenu = insertTextSampleMenu->addMenu(tr("Beginning of an article", "Text samples menu: Beginning"));
    insertBeginningMenu->addAction(insertUnderConstructionAct);
    insertBeginningMenu->addAction(insertTestedForAct);
    insertBeginningMenu->addAction(insertKnowledgeAct);
    insertBeginningMenu->addAction(insertTableOfContentsAct);
    insertBeginningMenu->addAction(insertAdvancedAct);
    insertBeginningMenu->addAction(insertAwardAct);

    insertImageMenu = insertTextSampleMenu->addMenu(tr("Images", "Text samples menu: Images"));
    insertImageMenu->addAction(insertImageAct);
    insertImageMenu->addAction(insertImageUnderlineAct);
    insertImageMenu->addAction(insertImageCollectionAct);
    insertImageMenu->addAction(insertImageCollectionInTextAct);

    insertFormatingHelpMenu = insertTextSampleMenu->addMenu(tr("General formating samples", "Text samples menu: General formats"));
    insertFormatingHelpMenu->addAction(insertBashCommandAct);
    insertFormatingHelpMenu->addAction(insertNoticeAct);
    insertFormatingHelpMenu->addAction(insertWarningAct);
    insertFormatingHelpMenu->addAction(insertExpertsAct);

    insertPacketinstallationMenu = insertTextSampleMenu->addMenu(tr("Package installation", "Text samples menu: Packages"));
    insertPacketinstallationMenu->addAction(insertPackageInstallAct);
    insertPacketinstallationMenu->addAction(insertPPAAct);
    insertPacketinstallationMenu->addAction(insertThirdPartyRepoAct);
    insertPacketinstallationMenu->addAction(insertThirdPartyRepoAuthAct);
    insertPacketinstallationMenu->addAction(insertThirdPartyPackageAct);
    insertPacketinstallationMenu->addAction(insertPackageListAct);

    insertMiscelementMenu = insertTextSampleMenu->addMenu(tr("Misc samples", "Text samples menu: Misc samples"));
    insertMiscelementMenu->addAction(insertImprovableAct);
    insertMiscelementMenu->addAction(insertFixMeAct);
    insertMiscelementMenu->addAction(insertLeftAct);
    insertMiscelementMenu->addAction(insertThirdPartyPackageWarningAct);
    insertMiscelementMenu->addAction(insertThirdPartyRepoWarningAct);
    insertMiscelementMenu->addAction(insertThirdPartySoftwareWarningAct);

    // Insert interwiki-links menu
    iWikiMenu = menuBar()->addMenu(tr("&InterWiki-Links", "GUI: InterWiki links menu"));
    for (int i = 0; i < myInterWikiLinks->getInterwikiLinksGroups().size(); i++) {
        iWikiGroups.append(iWikiMenu->addMenu(QIcon("/usr/share/" + sAppName.toLower() + "/iWikiLinks/" + myInterWikiLinks->getInterwikiLinksGroupIcons()[i]), myInterWikiLinks->getInterwikiLinksGroups()[i]));
        iWikiGroups[i]->addActions(iWikiLinksActions[i]);
    }

    // Help menu
    helpMenu = menuBar()->addMenu(tr("&Help", "GUI: About menu"));
    helpMenu->addAction(reportBugAct);
    helpMenu->addAction(aboutAct);

    if (bLogging) { std::clog << "Created menus" << std::endl; }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate tool bars
void CInyokaEdit::createToolBars()
{
    // File tool bar
    fileToolBar = addToolBar(tr("File", "GUI: File toolbar"));
    fileToolBar->setObjectName("fileToolBar");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    // Edit tool bar
    editToolBar = addToolBar(tr("Edit", "GUI: Edit toolbar"));
    editToolBar->setObjectName("editToolBar");
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addSeparator();
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);

    // Preview DL bar
    previewDlBar = addToolBar(tr("Preview / Download", "GUI: Preview / download toolbar"));
    previewDlBar->setObjectName("previewDlBar");
    previewDlBar->addAction(previewAct);
    previewDlBar->addAction(downloadArticleAct);

    // Inyoka tool bar
    addToolBarBreak(Qt::TopToolBarArea);  // second tool bar area under first one
    inyokaeditorBar = addToolBar(tr("Inyoka editor", "GUI: Inyoka editor toolbar"));
    inyokaeditorBar->setObjectName("inyokaeditorBar");
    inyokaeditorBar->addAction(boldAct);
    inyokaeditorBar->addAction(italicAct);
    inyokaeditorBar->addAction(monotypeAct);
    inyokaeditorBar->addAction(wikilinkAct);
    inyokaeditorBar->addAction(externalLinkAct);
    inyokaeditorBar->addAction(imageAct);
    inyokaeditorBar->addAction(codeblockAct);

    // Tool bar for combo boxes (samples and macros)
    samplesmacrosBar = addToolBar(tr("Samples and macros", "GUI: Samples and macros toolbar"));
    samplesmacrosBar->setObjectName("samplesmacrosBar");
    samplesmacrosBar->addWidget(headlineBox);

    // Headline combo box
    QString sHeadline = tr("Headline", "GUI: Headline combo box");
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
    samplesmacrosBar->addWidget(textmacrosBox);
    textmacrosBox->addItem(tr("Text macros", "GUI: Text macros combo box"));
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
    samplesmacrosBar->addWidget(textformatBox);
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
    int iRet = QMessageBox::question(this, tr("Download styles", "Msg: Title DL styles"), tr("In order that articles could be previewed correctly, you have to download some Inyoka ressources. This process may take a few minutes.\n\nDo you want to download these files now?", "Msg: DL styles"), QMessageBox::Yes | QMessageBox::No);
    if (QMessageBox::Yes== iRet){
        try
        {
            myArticleDownloadProgress = new CProgressDialog("/usr/share/" + sAppName.toLower() + "/GetInyokaStyles", sAppName, this, myDirectory.absolutePath());
        }
        catch (std::bad_alloc& ba)
        {
            std::cerr << "ERROR: myArticleDownloadProgress - bad_alloc caught: " << ba.what() << std::endl;
            QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "ArticleDownloadProgress");
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
    if (myTabwidget->indexOf(myWebview) == iIndex || 999 == iIndex) {

        // Disable editor and insert samples/macros toolbars
        editMenu->setDisabled(true);
        insertTextSampleMenu->setDisabled(true);
        iWikiMenu->setDisabled(true);
        editToolBar->setDisabled(true);
        inyokaeditorBar->setDisabled(true);
        samplesmacrosBar->setDisabled(true);
        previewAct->setDisabled(true);

        if ("" == sCurFile || tr("Untitled", "No file name set") == sCurFile){
            myParser->genOutput("");
        }
        else{
            QFileInfo fi(sCurFile);
            myParser->genOutput(fi.fileName());
        }
    }
    else {
        // Enable editor and insert samples/macros toolbars again
        editMenu->setEnabled(true);
        insertTextSampleMenu->setEnabled(true);
        iWikiMenu->setEnabled(true);
        editToolBar->setEnabled(true);
        inyokaeditorBar->setEnabled(true);
        samplesmacrosBar->setEnabled(true);
        previewAct->setEnabled(true);
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
        QMessageBox::warning(this, sAppName, tr("Error while inserting InterWiki link.", "Msg: Error interwiki indice"));
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
        sSitename = QInputDialog::getText(this, sAppName,
                                          tr("Please insert name of the article which should be downloaded:", "Msg: Input dialog DL article"), QLineEdit::Normal,
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
            QMessageBox::critical(this, sAppName, tr("Could not start the download of the raw format of article.", "Msg: Problem while downloading article"));
            procDownloadRawtext.kill();
            return;
        }
        if (!procDownloadRawtext.waitForFinished()) {
            #ifndef QT_NO_CURSOR
                QApplication::restoreOverrideCursor();
            #endif
            QMessageBox::critical(this, sAppName, tr("Error while downloading raw format of article.", "Msg: Problem while downloading article"));
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
            QMessageBox::information(this, sAppName, tr("Could not download the article.", "Msg: Can not download raw format"));
            return;
        }

        myEditor->setPlainText(sTmpArticle);
        myEditor->document()->setModified(true);
        documentWasModified();


#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif

        downloadImages(sSitename);
        myTabwidget->setCurrentIndex(myTabwidget->indexOf(myEditor));
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
        QMessageBox::critical(this, sAppName, tr("Could not start download of the meta data.", "Msg: Problem starting meta data download"));
        procDownloadMetadata.kill();
        return;
    }
    if (!procDownloadMetadata.waitForFinished()) {
        QMessageBox::critical(this, sAppName, tr("Error while downloading meta data.", "Msg: Problem while downloading meta data"));
        procDownloadMetadata.kill();
        return;
    }

    tempResult = procDownloadMetadata.readAll();
    sMetadata = QString::fromLocal8Bit(tempResult);

    // Site does not exist etc.
    if ("" == sMetadata) {
        QMessageBox::information(this, sAppName, tr("Could not find meta data.", "Msg: No meta data found"));
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
            iRet = QMessageBox::question(this, sAppName, tr("Do you want to download the images which are attached to the article?", "Msg: DL aricle images"), QMessageBox::Yes, QMessageBox::No);
        }
        else {
            iRet = QMessageBox::Yes;
        }

        if (QMessageBox::Yes == iRet) {

            // File for download script
            QFile tmpScriptfile(StylesAndImagesDir.absolutePath() + "/" + sScriptName);

            // No write permission
            if (!tmpScriptfile.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, sAppName, tr("Could not create temporary download file!", "Msg: Problem creating DL script"));
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
                myImageDownloadProgress = new CProgressDialog(sTmpFilePath, sAppName, this, StylesAndImagesDir.absolutePath());
                }
            catch (std::bad_alloc& ba)
            {
                std::cerr << "ERROR: Caught bad_alloc in \"downloadImages()\": " << ba.what() << std::endl;
                QMessageBox::critical(this, sAppName, tr("Error while memory allocation: ", "Msg: Bad alloc") + "ImageDownloadProgress");
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
        myTabwidget->setCurrentIndex(myTabwidget->indexOf(myWebview));
    }
    else {
        QMessageBox::warning(this, sAppName, tr("Error while loading preview.", "Msg box"));
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
            fileMenuLastOpened->setEnabled(true);
        }
    }

    // Clear list
    else {
        sListTmp.clear();
        fileMenuLastOpened->setEnabled(false);
    }

    mySettings->setRecentFiles(sListTmp);
}

void CInyokaEdit::clearRecentFiles(){
    updateRecentFiles("_-CL3AR#R3C3NT#F!L35-_");
}

// -----------------------------------------------------------------------------------------------
// Report a bug via Apport to Launchpad

void CInyokaEdit::reportBug(){

    // Start apport
    QProcess procApport;
    procApport.start("ubuntu-bug " + sAppName.toLower());

    if (!procApport.waitForStarted()) {
        QMessageBox::critical(this, sAppName, tr("Error while starting Apport.", "Msg box"));
        return;
    }
    if (!procApport.waitForFinished()) {
        QMessageBox::critical(this, sAppName, tr("Error while executing Apport.", "Msg box"));
        return;
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
        myTabwidget->setCurrentIndex(myTabwidget->indexOf(myEditor));
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
            myTabwidget->setCurrentIndex(myTabwidget->indexOf(myEditor));
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
    QMessageBox::about(this, tr("About %1", "About dialog <sAppName>").arg(sAppName),
                       tr("<b>%1</b> - Editor for Inyoka-based portals<br />"
                          "Version: %2<br /><br />"
                          "&copy; 2011, the %3 authors<br />"
                          "Licence: <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License Version 3</a><br /><br />"
                          "This application uses icons from <a href=\"http://tango.freedesktop.org\">Tango project</a>.", "About dialog text, <sAppName>, <sVERSION>, <sAppName>").arg(sAppName).arg(sVERSION).arg(sAppName));
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

        ret = QMessageBox::warning(this, sAppName,
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
        QMessageBox::warning(this, sAppName,
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
        QMessageBox::warning(this, sAppName,
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
        statusBar()->showMessage(tr("File saved", "GUI: Status bar"), 2000);
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
        sShownName = tr("Untitled", "No file name set");
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
