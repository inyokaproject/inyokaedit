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

#define sVERSION "0.0.5"

#include <QtGui>
#include <QtWebKit/QWebView>

#include "CInyokaEdit.h"

CInyokaEdit::CInyokaEdit(const QString &name, const int argc, char **argv)
    : myCompleter(0), sAppName(name)
{
    QStringList wordlist;
    wordlist << "Inhaltsverzeichnis(1)]]" << "Vorlage(Getestet, Ubuntuversion)]]" << "Vorlage(Baustelle, Datum, \"Bearbeiter\")]]"
             << "Vorlage(Fortgeschritten)]]" << "Vorlage(Pakete, \"foo bar\")]]" << trUtf8("Vorlage(Ausbaufähig, \"Begründung\")]]")
             << trUtf8("Vorlage(Fehlerhaft, \"Begründung\")]]") << trUtf8("Vorlage(Verlassen, \"Begründung\")]]") << "Vorlage(Archiviert, \"Text\")]]"
             << "Vorlage(Kopie, Seite, Autor)]]" << trUtf8("Vorlage(Überarbeitung, Datum, Seite, Autor)]]") << "Vorlage(Fremd, Paket, \"Kommentar\")]]"
             << "Vorlage(Fremd, Quelle, \"Kommentar\")]]" << "Vorlage(Fremd, Software, \"Kommentar\")]]" << trUtf8("Vorlage(Award, \"Preis\", Link, Preiskategorie, \"Preisträger\")]]")
             << "Vorlage(PPA, PPA-Besitzer, PPA-Name)]]" << "Vorlage(Fremdquelle-auth, URL zum PGP-Key)]]" << trUtf8("Vorlage(Fremdquelle-auth, key PGP-Schlüsselnummer)]]")
             << "Vorlage(Fremdquelle, URL, Ubuntuversion(en), Komponente(n) )]]" << "Vorlage(Fremdpaket, Projekthoster, Projektname, Ubuntuversion(en))]]"
             << trUtf8("Vorlage(Fremdpaket, \"Anbieter\", URL zu einer Downloadübersicht, Ubuntuversion(en))]]") << "Vorlage(Fremdpaket, \"Anbieter\", dl, URL zu EINEM Download, Ubuntuversion(en))]]"
             << "Vorlage(Tasten, TASTE)]]" << trUtf8("Bild(name.png, Größe, Ausrichtung)]]") << "Anker(Name)]]" << "[[Vorlage(Bildunterschrift, BILDLINK, BILDBREITE, \"Beschreibung\", left|right)]]"
             << trUtf8("Vorlage(Bildersammlung, BILDHÖHE\nBild1.jpg, \"Beschreibung 1\"\nBild2.png, \"Beschreibung 2\"\n)]]");

    // Font settings for editor
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10.5);

    // Editor object (objects have to be create before find/replace)
    try
    {
        myEditor = new CTextEditor;
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myEditor - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: TextEditor"));
      exit (-1);
    }
    myEditor->setFont(font);
    myEditor->setAcceptRichText(false); // Paste plain text only
    myEditor->setCompleter(myCompleter);

    // Find / replace (objects have to be create before readSettings!)
    try
    {
        m_findDialog = new FindDialog(this);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: m_findDialog - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: FindDialog"));
      exit (-1);
    }
    m_findDialog->setModal(false);
    m_findDialog->setTextEdit(myEditor);

    try
    {
        m_findReplaceDialog = new FindReplaceDialog(this);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: m_findReplaceDialog - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: FindReplaceDialog"));
      exit (-1);
    }
    m_findReplaceDialog->setModal(false);
    m_findReplaceDialog->setTextEdit(myEditor);

    // Set config and styles/images path
    StylesAndImagesDir = QDir::homePath() + "/." + sAppName;
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, StylesAndImagesDir.absolutePath());
    try
    {
        mySettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, sAppName);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: mySettings - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: Settings"));
      exit (-1);
    }

    // Load settings from config file
    readSettings();

    try
    {
        myCompleter = new QCompleter(wordlist, this);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myCompleter - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: Completer"));
      exit (-1);
    }
    myCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    myCompleter->setWrapAround(false);

    // Setup gui, menus, actions, toolbar...
    setupEditor();
    createActions();
    createMenus();
    createToolBars();

    setCentralWidget(myTabwidget);
    myTabwidget->setTabPosition(QTabWidget::West);
    myTabwidget->addTab(myEditor, trUtf8("Rohformat"));

    myTabwidget->addTab(myWebview, trUtf8("Vorschau"));
    if (bPreviewInEditor == false)
        myTabwidget->setTabEnabled(myTabwidget->indexOf(myWebview), false);

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);

    // Application icon
    setWindowIcon(QIcon(":/images/inyokaedit_64x64.png"));

    // Download style files if preview/styles/imgages folders doesn't exist (/home/user/.InyokaEdit)
    if (!StylesAndImagesDir.exists() || !QDir(StylesAndImagesDir.absolutePath() + "/img").exists() ||
        !QDir(StylesAndImagesDir.absolutePath() + "/styles").exists() || !QDir(StylesAndImagesDir.absolutePath() + "/Wiki").exists()){
        DownloadStyles(StylesAndImagesDir);
    }

    // Open file if command line argument parsed
    if (argc >= 2) {
        QString tmpstr = argv[1];

        // Download inyoka styles
        if (tmpstr == "--version"){
            std::cout << argv[0] << "\t Version: " << sVERSION << std::endl;
            exit(0);
        }
        else if (tmpstr == "--dlstyles"){
            DownloadStyles(StylesAndImagesDir);
        }
        else {
            loadFile(argv[1]);
        }
    }

    // Restore window and toolbar settings
    // Settings have to be restored after toolbars are created!
    mySettings->beginGroup("Window");
    restoreGeometry(mySettings->value("Geometry").toByteArray());
    restoreState(mySettings->value("WindowState").toByteArray());  // Restore toolbar position etc.
    mySettings->endGroup();

    statusBar()->showMessage(trUtf8("Bereit"));
}

CInyokaEdit::~CInyokaEdit(){
    delete myInsertSyntaxElement;
    myInsertSyntaxElement = NULL;
    delete myWebview;
    myWebview = NULL;
    delete myTabwidget;
    myTabwidget = NULL;
    delete myParser;
    myParser = NULL;
    delete myCompleter;
    myCompleter = NULL;
    delete m_findReplaceDialog;
    m_findReplaceDialog = NULL;
    delete m_findDialog;
    m_findDialog = NULL;

//    delete myEditor;
//    myEditor = NULL;
//    delete myHighlighter;
//    myHighlighter = NULL;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::DownloadStyles(const QDir myDirectory)
{
    int iRet = QMessageBox::question(this, trUtf8("Styles herunterladen"), trUtf8("Damit die Vorschaufunktion korrekt funktioniert, müssen einige Ressourcen von Inyoka heruntergeladen werden. Dieser Vorgang kann einige Minuten dauern.\n\nMöchten Sie die Ressourcen jetzt herunterladen?"), QMessageBox::Yes | QMessageBox::No);
    if (iRet == QMessageBox::Yes){
        try
        {
            myArticleDownloadProgress = new CProgressDialog("/usr/lib/inyokaedit/GetInyokaStyles", sAppName, this, myDirectory.absolutePath());
        }
        catch (std::bad_alloc& ba)
        {
          std::cerr << "ERROR: myArticleDownloadProgress - bad_alloc caught: " << ba.what() << std::endl;
          QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: ArticleDownloadProgress"));
          exit (-1);
        }
        myArticleDownloadProgress->open();
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CInyokaEdit::setupEditor()
{
    // Text changed
    connect(myEditor->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    // Send code completion state (in-/active) to myeditor
    connect(this, SIGNAL(sendCodeCompState(bool)),
            myEditor, SLOT(getcodecomplState(bool)));

    // Activate syntax highlighting
    try
    {
        myHighlighter = new CHighlighter(myEditor->document());
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myHighlighter - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: Highlighter"));
      exit (-1);
    }

    // Parser object
    try
    {
        myParser = new CParser(myEditor->document(), sInyokaUrl, StylesAndImagesDir);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myParser - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: Parser"));
      exit (-1);
    }

    // Connect signals from parser with functions
    connect(myParser, SIGNAL(callShowPreview(QString)),
            this, SLOT(showHtmlPreview(QString)));
    connect(myParser, SIGNAL(callShowMessageBox(QString,QString)),
            this, SLOT(showMessageBox(QString,QString)));

    try
    {
        myTabwidget = new QTabWidget;
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myTabwidget - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: TabWidget"));
      exit (-1);
    }

    try
    {
        myWebview = new QWebView(this);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myWebview - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: Webview"));
      exit (-1);
    }

    try
    {
        myInsertSyntaxElement = new CInsertSyntaxElement;
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: myInsertSyntaxElement - bad_alloc caught: " << ba.what() << std::endl;
      QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: InsertSyntaxElement"));
      exit (-1);
    }

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Load settings from config file
void CInyokaEdit::readSettings()
{
    bCodeCompletion = mySettings->value("CodeCompletion", true).toBool();
    emit sendCodeCompState(bCodeCompletion);

    bPreviewInEditor = mySettings->value("PreviewInEditor", true).toBool();

    sInyokaUrl = mySettings->value("InyokaUrl", "http://wiki.ubuntuusers.de").toString();
    if (sInyokaUrl.endsWith("/")) {
        sInyokaUrl.remove(sInyokaUrl.length()-1, 1);
    }

    LastOpenedDir = mySettings->value("LastOpenedDir", QDir::homePath()).toString();

    bAutomaticImageDownload = mySettings->value("AutomaticImageDownload", false).toBool();

    m_findDialog->readSettings(*mySettings);
    m_findReplaceDialog->readSettings(*mySettings);
}

// Save settings (close event)
void CInyokaEdit::writeSettings()
{
    mySettings->setValue("CodeCompletion", bCodeCompletion);
    mySettings->setValue("PreviewInEditor", bPreviewInEditor);
    mySettings->setValue("InyokaUrl", sInyokaUrl);
    mySettings->setValue("LastOpenedDir", LastOpenedDir.absolutePath());
    mySettings->setValue("AutomaticImageDownload", bAutomaticImageDownload);

    m_findDialog->writeSettings(*mySettings);
    m_findReplaceDialog->writeSettings(*mySettings);

    // Save toolbar position etc.
    mySettings->beginGroup("Window");
    mySettings->setValue("Geometry", saveGeometry());
    mySettings->setValue("WindowState", saveState());
    mySettings->endGroup();
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void CInyokaEdit::createActions()
{
    // New file
    newAct = new QAction(QIcon(":/images/document-new.png"), trUtf8("&Neu"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(trUtf8("Erstellt eine neue Datei"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    // Open file
    openAct = new QAction(QIcon(":/images/document-open.png"), trUtf8("&Öffnen..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(trUtf8("Öffnet eine Datei"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    // Save file
    saveAct = new QAction(QIcon(":/images/document-save.png"), trUtf8("&Speichern"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(trUtf8("Speichert das Dokument"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    // Save file as...
    saveAsAct = new QAction(trUtf8("Speichern unter..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(trUtf8("Speichert das Dokument unter neuem Namen"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    // Exit application
    exitAct = new QAction(trUtf8("Beenden"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(trUtf8("Beendet die Anwendung"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // Edit: Cut
    cutAct = new QAction(QIcon(":/images/edit-cut.png"), trUtf8("Ausschneiden"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(trUtf8("Schneidet den markierten Text aus"));
    connect(cutAct, SIGNAL(triggered()), myEditor, SLOT(cut()));

    // Edit: Copy
    copyAct = new QAction(QIcon(":/images/edit-copy.png"), trUtf8("Kopieren"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(trUtf8("Kopiert den markierten Inhalt in die Zwischenablage"));
    connect(copyAct, SIGNAL(triggered()), myEditor, SLOT(copy()));

    // Edit: Paste
    pasteAct = new QAction(QIcon(":/images/edit-paste.png"), trUtf8("Einfügen"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(trUtf8("Fügt den Inhalt der Zwischenablage ein"));
    connect(pasteAct, SIGNAL(triggered()), myEditor, SLOT(paste()));

    // Edit: Undo
    undoAct = new QAction(QIcon(":/images/edit-undo.png"), trUtf8("Rückgängig"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(trUtf8("Änderungen rückgängig machen"));
    connect(undoAct, SIGNAL(triggered()), myEditor, SLOT(undo()));

    // Edit: Redo
    redoAct = new QAction(QIcon(":/images/edit-redo.png"), trUtf8("Wiederherstellen"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(trUtf8("Rückgängig gemachte Änderungen wiederherstellen"));
    connect(redoAct, SIGNAL(triggered()), myEditor, SLOT(redo()));

    // Edit: Search
    searchAct = new QAction(QIcon(":/images/edit-find.png"), trUtf8("Suchen..."), this);
    searchAct->setShortcuts(QKeySequence::Find);
    searchAct->setStatusTip(trUtf8("Suchen"));
    connect(searchAct, SIGNAL(triggered()), this, SLOT(findDialog()));

    // Edit: Replace
    replaceAct = new QAction(QIcon(":/images/edit-find-replace.png"), trUtf8("Ersetzen..."), this);
    replaceAct->setShortcuts(QKeySequence::Replace);
    replaceAct->setStatusTip(trUtf8("Ersetzen"));
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(findReplaceDialog()));

    // Edit: Find next
    findNextAct = new QAction(QIcon(":/images/go-down.png"), trUtf8("Weitersuchen"), this);
    findNextAct->setShortcuts(QKeySequence::FindNext);
    findNextAct->setStatusTip(trUtf8("Weitersuchen (vorwärts)"));
    connect(findNextAct, SIGNAL(triggered()), m_findDialog, SLOT(findNext()));

    // Edit: Find previous
    findPreviousAct = new QAction(QIcon(":/images/go-up.png"), trUtf8("Rückwärts weitersuchen"), this);
    findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
    findPreviousAct->setStatusTip(trUtf8("Weitersuchen (rückwärts)"));
    connect(findPreviousAct, SIGNAL(triggered()), m_findDialog, SLOT(findPrev()));

    // Open about windwow
    aboutAct = new QAction(QIcon(":images/question.png"), trUtf8("Über") + " " + sAppName, this);
    aboutAct->setStatusTip(trUtf8("Zeigt die Infobox dieser Anwendung"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    // Report a bug using apport
    reportBugAct = new QAction(QIcon(":images/interwiki/bug.png"), trUtf8("Fehler melden"), this);
    reportBugAct->setStatusTip(trUtf8("Fehler melden - Hierzu wird ein Launchpad-Account benötigt!"));
    connect(reportBugAct, SIGNAL(triggered()), this, SLOT(reportBug()));

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

    // Show html preview
    previewAct = new QAction(QIcon(":/images/preview.png"), trUtf8("Vorschau"), this);
    previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
    previewAct->setStatusTip(trUtf8("Vorschau der Inyokaseite öffnen"));
    connect(previewAct, SIGNAL(triggered()), this, SLOT(previewInyokaPage()));

    // Click on tab generates a new preview, too
    connect(myTabwidget, SIGNAL(currentChanged(int)), this, SLOT(previewInyokaPage(int)));

    // Download Inyoka article
    downloadArticleAct = new QAction(QIcon(":/images/network-receive.png"), trUtf8("Inyokaartikel herunterladen"), this);
    downloadArticleAct->setStatusTip(trUtf8("Lädt den Rohtext eines bestehenden Inyoka Wikiartikels herunter"));
    downloadArticleAct->setPriority(QAction::LowPriority);
    connect(downloadArticleAct, SIGNAL(triggered()), this, SLOT(downloadArticle()));

    // Insert syntax elements
    mySigMapTextSamples = new QSignalMapper(this);

    // Insert bold element
    boldAct = new QAction(QIcon(":/images/format-text-bold.png"), trUtf8("Fett"), this);
    boldAct->setStatusTip(trUtf8("Fett - Dateinamen, Verzeichnisse, Paketnamen, Formatnamen"));
    boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    boldAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(boldAct, "Fett");
    connect(boldAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert italic element
    italicAct = new QAction(QIcon(":/images/format-text-italic.png"), trUtf8("Kursiv"), this);
    italicAct->setStatusTip(trUtf8("Kursiv - Menüelemente, Schaltflächen, G-Conf-Schlüssel, immer in Anführungszeichen!"));
    italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    italicAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(italicAct, "Kursiv");
    connect(italicAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert monotype element
    monotypeAct = new QAction(QIcon(":/images/monotype.png"), trUtf8("Monotype"), this);
    monotypeAct->setStatusTip(trUtf8("Befehle und ihre Optionen, Terminalausgaben im Fließtext, Module, Benutzer, Gruppen"));
    monotypeAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(monotypeAct, "Monotype");
    connect(monotypeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert wiki link
    wikilinkAct = new QAction(QIcon(":/images/go-next.png"), trUtf8("Link Wikiseite"), this);
    wikilinkAct->setStatusTip(trUtf8("Link auf eine Wikiseite"));
    wikilinkAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(wikilinkAct, "Wikilink");
    connect(wikilinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert extern link
    externLinkAct = new QAction(QIcon(":/images/internet-web-browser.png"), trUtf8("Link extern"), this);
    externLinkAct->setStatusTip(trUtf8("Link auf eine externe Webseite, immer mit Fahne z.B. {de}, {en}!"));
    externLinkAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(externLinkAct, "ExternerLink");
    connect(externLinkAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert image
    imageAct = new QAction(QIcon(":/images/image-x-generic.png"), trUtf8("Bild einfügen"), this);
    imageAct->setStatusTip(trUtf8("Ein Bild einfügen"));
    imageAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(imageAct, "EinfachesBild");
    connect(imageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert code block
    codeblockAct = new QAction(QIcon(":/images/code.png"), trUtf8("Codeblock"), this);
    codeblockAct->setStatusTip(trUtf8("Codeblock - Terminalausgaben, Auszüge aus Konfigurationsdateien"));
    codeblockAct->setPriority(QAction::LowPriority);
    mySigMapTextSamples->setMapping(codeblockAct, "Codeblock");
    connect(codeblockAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    // Insert heading
    headingsBox = new QComboBox();
    headingsBox->setStatusTip(trUtf8("Überschrift einfügen"));
    headingsBox->setStatusTip(trUtf8("Überschrift einfügen - Es werden bis zu 5 Stufen unterstützt"));

    // Insert sample
    textmacrosBox = new QComboBox();
    textmacrosBox->setStatusTip(trUtf8("Textbaustein einfügen"));
    textmacrosBox->setStatusTip(trUtf8("Textbaustein einfügen"));

    insertUnderConstructionAct = new QAction(trUtf8("Baustelle"), this);
    insertUnderConstructionAct->setStatusTip(trUtf8("Baustelle Textbaustein einfügen - Nur in Artikeln zu verwenden, die gerade in der Baustelle erstellt werden"));
    mySigMapTextSamples->setMapping(insertUnderConstructionAct, "Baustelle");
    connect(insertUnderConstructionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertTestedAct = new QAction(trUtf8("Getestet"), this);
    insertTestedAct->setStatusTip(trUtf8("Getestet Textbaustein einfügen - Kennzeichnet für welche Ubuntuversionen der Artikel Gültigkeit hat"));
    mySigMapTextSamples->setMapping(insertTestedAct, "Getestet");
    connect(insertTestedAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertKnowledgeAct = new QAction(trUtf8("Wissensblock"), this);
    insertKnowledgeAct->setStatusTip(trUtf8("Wissensblock einfügen - Verweise auf grundlegende Artikel die zum Verständnis des Artikels hilfreich sind"));
    mySigMapTextSamples->setMapping(insertKnowledgeAct, "Wissensblock");
    connect(insertKnowledgeAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertTableOfContentAct = new QAction(trUtf8("Inhaltsverzeichnis"), this);
    insertTableOfContentAct->setStatusTip(trUtf8("Inhaltsverzeichnis einfügen"));
    mySigMapTextSamples->setMapping(insertTableOfContentAct, "Inhaltsverzeichnis");
    connect(insertTableOfContentAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertExpertboxAct = new QAction(trUtf8("Experten-Markierung"), this);
    insertExpertboxAct->setStatusTip(trUtf8("Experten-Markierung einfügen - Zeigt an, das der Artikel eher für fortgeschrittene Nutzer gedacht ist"));
    mySigMapTextSamples->setMapping(insertExpertboxAct, "Expertenmarkierung");
    connect(insertExpertboxAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertAwardboxAct = new QAction(trUtf8("Preis-Markierung"), this);
    insertAwardboxAct->setStatusTip(trUtf8("Preis-Markierung einfügen - Für preisgekrönte Programme"));
    mySigMapTextSamples->setMapping(insertAwardboxAct, "Preis");
    connect(insertAwardboxAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertCommandAct = new QAction(trUtf8("Shell Befehl"), this);
    insertCommandAct->setStatusTip(trUtf8("Shell Befehl einfügen"));
    mySigMapTextSamples->setMapping(insertCommandAct, "Shellbefehl");
    connect(insertCommandAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertHintAct = new QAction(trUtf8("Hinweis"), this);
    insertHintAct->setStatusTip(trUtf8("Hinweisbox einfügen - Besondere Hervorhebung eines Sachverhalts"));
    mySigMapTextSamples->setMapping(insertHintAct, "Hinweisbox");
    connect(insertHintAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertWarningAct = new QAction(trUtf8("Warnung"), this);
    insertWarningAct->setStatusTip(trUtf8("Warnbox einfügen - Hinweis auf potenzielle Gefahren"));
    mySigMapTextSamples->setMapping(insertWarningAct, "Warnbox");
    connect(insertWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertExpertinfoAct = new QAction(trUtf8("Experten-Info"), this);
    insertExpertinfoAct->setStatusTip(trUtf8("Experten-Info einfügen - Bereitstellung von Hintergrundinformationen"));
    mySigMapTextSamples->setMapping(insertExpertinfoAct, "Experteninfo");
    connect(insertExpertinfoAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertImageAct = new QAction(trUtf8("Einfaches Bild"), this);
    insertImageAct->setStatusTip(trUtf8("Einfaches Bild einfügen"));
    mySigMapTextSamples->setMapping(insertImageAct, "EinfachesBild");
    connect(insertImageAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertImageSubscriptionAct = new QAction(trUtf8("Bild mit Bildunterschrit"), this);
    insertImageSubscriptionAct->setStatusTip(trUtf8("Bild mit Bildunterschrift einfügen"));
    mySigMapTextSamples->setMapping(insertImageSubscriptionAct, "Bildunterschrift");
    connect(insertImageSubscriptionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertImageCollectionAct = new QAction(trUtf8("Bildersammlung (mit Umbruch)"), this);
    insertImageCollectionAct->setStatusTip(trUtf8("Bildersammlung (mit Umbruch) einfügen - Sammlung wird abhängig von der Browserbreite automatisch umgebrochen"));
    mySigMapTextSamples->setMapping(insertImageCollectionAct, "BildersammlungUmbruch");
    connect(insertImageCollectionAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertImageCollectionInTextAct = new QAction(trUtf8("Bildersammlung (im Fließtext)"), this);
    insertImageCollectionInTextAct->setStatusTip(trUtf8("Bildersammlung (im Fließtext) einfügen - Text umfließt die Sammlung (max. 3 Bilder, Breite pro Bild: 200px)"));
    mySigMapTextSamples->setMapping(insertImageCollectionInTextAct, "BildersammlungFliesstext");
    connect(insertImageCollectionInTextAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertPackageMacroAct = new QAction(trUtf8("Pakete-Makro"), this);
    insertPackageMacroAct->setStatusTip(trUtf8("Pakete-Makro einfügen - Veraltet und sollte nur in Ausnahmefällen verwendet werden"));
    mySigMapTextSamples->setMapping(insertPackageMacroAct, "Paketemakro");
    connect(insertPackageMacroAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertPackageInstallAct = new QAction(trUtf8("Paketinstallation"), this);
    insertPackageInstallAct->setStatusTip(trUtf8("Paketinstallation-Makro einfügen - Erstellt Paketliste mit Installationsknopf"));
    mySigMapTextSamples->setMapping(insertPackageInstallAct, "Paketinstallation");
    connect(insertPackageInstallAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertPpaSampleAct = new QAction(trUtf8("PPA-Vorlage"), this);
    insertPpaSampleAct->setStatusTip(trUtf8("PPA-Vorlage einfügen - Erstellt einen Abschnitt zur Verwendung von Launchpad-PPAs ink. Warnung und Link zur PPA-Quelle"));
    mySigMapTextSamples->setMapping(insertPpaSampleAct, "Ppavorlage");
    connect(insertPpaSampleAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertForeignSourceSampleAct = new QAction(trUtf8("Fremdquellen-Vorlage"), this);
    insertForeignSourceSampleAct->setStatusTip(trUtf8("Fremdquellen-Vorlage einfügen - Erstellt einen Abschnitt zur Verwendung von Qullen außerhalb von Ubuntu oder Launchpad"));
    mySigMapTextSamples->setMapping(insertForeignSourceSampleAct, "Fremdquellenvorlage");
    connect(insertForeignSourceSampleAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertForeignAuthSampleAct = new QAction(trUtf8("Fremdquellen authentifizieren Vorlage"), this);
    insertForeignAuthSampleAct->setStatusTip(trUtf8("Fremdquellen authentifizieren Vorlage einfügen - Erstellt einen Abschnitt zur Authentifizierung von Fremdquellen"));
    mySigMapTextSamples->setMapping(insertForeignAuthSampleAct, "Fremdquellenauth");
    connect(insertForeignAuthSampleAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertForeignPackageSampleAct = new QAction(trUtf8("Fremdpaket-Vorlage"), this);
    insertForeignPackageSampleAct->setStatusTip(trUtf8("Fremdpaket-Vorlage einfügen - Erstellt einen Abschnitt zur Installation von DEB-Paketen aus Fremdquellen"));
    mySigMapTextSamples->setMapping(insertForeignPackageSampleAct, "Fremdpaket");
    connect(insertForeignPackageSampleAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertExpandableAct = new QAction(trUtf8("Ausbaufähig-Markierung"), this);
    insertExpandableAct->setStatusTip(trUtf8("Ausbaufähig-Markierung einfügen - Der Artikel ist unvollständig oder kann noch erweitert werden"));
    mySigMapTextSamples->setMapping(insertExpandableAct, "Ausbaufaehig");
    connect(insertExpandableAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertFaultyAct = new QAction(trUtf8("Fehlerhaft-Markierung"), this);
    insertFaultyAct->setStatusTip(trUtf8("Fehlerhaft-Markierung einfügen - Der Artikel enthält Angaben oder Abschnitte die nicht (mehr) korrekt sind"));
    mySigMapTextSamples->setMapping(insertFaultyAct, "Fehlerhaft");
    connect(insertFaultyAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertLeftAct = new QAction(trUtf8("Verlassen-Markierung"), this);
    insertLeftAct->setStatusTip(trUtf8("Verlassen-Markierung einfügen - Nur für Baustellenartikel, die der Originalautor nicht mehr zu Ende bringt"));
    mySigMapTextSamples->setMapping(insertLeftAct, "Verlassen");
    connect(insertLeftAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertPackageWarningAct = new QAction(trUtf8("Fremdpakete-Warnung"), this);
    insertPackageWarningAct->setStatusTip(trUtf8("Fremdpakete-Warnung einfügen - Für Pakete aus einer externen Quelle"));
    mySigMapTextSamples->setMapping(insertPackageWarningAct, "Fremdpaketewarnung");
    connect(insertPackageWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertSourceWaringAct = new QAction(trUtf8("Fremdquelle-Warnung"), this);
    insertSourceWaringAct->setStatusTip(trUtf8("Fremdquelle-Warnung einfügen - Für Pakete, die aus externen Quellen installiert werden"));
    mySigMapTextSamples->setMapping(insertSourceWaringAct, "Fremdquellewarnung");
    connect(insertSourceWaringAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    insertSoftwareWarningAct = new QAction(trUtf8("Fremdsoftware-Warnung"), this);
    insertSoftwareWarningAct->setStatusTip(trUtf8("Fremdsoftware-Warnung einfügen - Nur für Software, die komplett ohne Paketquelle installiert wird"));
    mySigMapTextSamples->setMapping(insertSoftwareWarningAct, "Fremdsoftwarewarnung");
    connect(insertSoftwareWarningAct, SIGNAL(triggered()), mySigMapTextSamples, SLOT(map()));

    connect(mySigMapTextSamples, SIGNAL(mapped(QString)), this, SLOT(insertTextSample(QString)));

    // ---------------------------------------------------------------------------------------------
    // INTERWIKI-LINKS ACTIONS

    mySigMapInterWikiLinks = new QSignalMapper(this);

    // Inyoka
    iWikiForum = new QAction(trUtf8("Forum"), this);
    mySigMapInterWikiLinks->setMapping(iWikiForum, "iWikiForum");
    connect(iWikiForum, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiTopic = new QAction(trUtf8("Topic"), this);
    mySigMapInterWikiLinks->setMapping(iWikiTopic, "iWikiTopic");
    connect(iWikiTopic, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiPost = new QAction(trUtf8("Post"), this);
    mySigMapInterWikiLinks->setMapping(iWikiPost, "iWikiPost");
    connect(iWikiPost, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiUser = new QAction(QIcon(":/images/interwiki/user.png"), trUtf8("Benutzer"), this);
    mySigMapInterWikiLinks->setMapping(iWikiUser, "iWikiUser");
    connect(iWikiUser, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiIkhaya = new QAction(trUtf8("Ikhaya"), this);
    mySigMapInterWikiLinks->setMapping(iWikiIkhaya, "iWikiIkhaya");
    connect(iWikiIkhaya, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiCalendar = new QAction(trUtf8("Kalender"), this);
    mySigMapInterWikiLinks->setMapping(iWikiCalendar, "iWikiCalendar");
    connect(iWikiCalendar, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiTicket = new QAction(QIcon(":/images/interwiki/ticket.png"), trUtf8("Inyoka-Ticket"), this);
    mySigMapInterWikiLinks->setMapping(iWikiTicket, "iWikiTicket");
    connect(iWikiTicket, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiBehind = new QAction(trUtf8("Behind Ubuntuusers"), this);
    mySigMapInterWikiLinks->setMapping(iWikiBehind, "iWikiBehind");
    connect(iWikiBehind, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiPaste = new QAction(trUtf8("Paste Service"), this);
    mySigMapInterWikiLinks->setMapping(iWikiPaste, "iWikiPaste");
    connect(iWikiPaste, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiPlanet = new QAction(trUtf8("Planet"), this);
    mySigMapInterWikiLinks->setMapping(iWikiPlanet, "iWikiPlanet");
    connect(iWikiPlanet, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));

    // Canonical, Ubuntu, Derivate
    iWikiCanonical = new QAction(QIcon(":/images/interwiki/canonical.png"), trUtf8("Canonical"), this);
    mySigMapInterWikiLinks->setMapping(iWikiCanonical, "iWikiCanonical");
    connect(iWikiCanonical, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiCanonicalblog = new QAction(QIcon(":/images/interwiki/canonicalblog.png"), trUtf8("Canonical Blog"), this);
    mySigMapInterWikiLinks->setMapping(iWikiCanonicalblog, "iWikiCanonicalblog");
    connect(iWikiCanonicalblog, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiPackages = new QAction(QIcon(":/images/interwiki/packages.png"), trUtf8("Packages"), this);
    mySigMapInterWikiLinks->setMapping(iWikiPackages, "iWikiPackages");
    connect(iWikiPackages, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiApt = new QAction(trUtf8("Apt"), this);
    mySigMapInterWikiLinks->setMapping(iWikiApt, "iWikiApt");
    connect(iWikiApt, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiUbuntu = new QAction(QIcon(":/images/interwiki/ubuntu.png"), trUtf8("Ubuntu"), this);
    mySigMapInterWikiLinks->setMapping(iWikiUbuntu, "iWikiUbuntu");
    connect(iWikiUbuntu, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiUbuntu_doc = new QAction(QIcon(":/images/interwiki/ubuntu_doc.png"), trUtf8("Ubuntu Help"), this);
    mySigMapInterWikiLinks->setMapping(iWikiUbuntu_doc, "iWikiUbuntu_doc");
    connect(iWikiUbuntu_doc, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiAskubuntu = new QAction(QIcon(":/images/interwiki/askubuntu.png"), trUtf8("Ask Ubuntu"), this);
    mySigMapInterWikiLinks->setMapping(iWikiAskubuntu, "iWikiAskubuntu");
    connect(iWikiAskubuntu, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiUbuntu_fr = new QAction(QIcon(":/images/interwiki/ubuntu_fr.png"), trUtf8("Franz. Ubuntuwiki"), this);
    mySigMapInterWikiLinks->setMapping(iWikiUbuntu_fr, "iWikiUbuntu_fr");
    connect(iWikiUbuntu_fr, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiUbuntuone = new QAction(QIcon(":/images/interwiki/ubuntuone.png"), trUtf8("Ubuntu One"), this);
    mySigMapInterWikiLinks->setMapping(iWikiUbuntuone, "iWikiUbuntuone");
    connect(iWikiUbuntuone, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiKubuntu = new QAction(QIcon(":/images/interwiki/kubuntu.png"), trUtf8("Kubuntu"), this);
    mySigMapInterWikiLinks->setMapping(iWikiKubuntu, "iWikiKubuntu");
    connect(iWikiKubuntu, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiKubuntuDe = new QAction(QIcon(":/images/interwiki/kubuntu-de.png"), trUtf8("Dt. Kubuntuwiki"), this);
    mySigMapInterWikiLinks->setMapping(iWikiKubuntuDe, "iWikiKubuntuDe");
    connect(iWikiKubuntuDe, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiKubuntuDoc = new QAction(QIcon(":/images/interwiki/kubuntu_doc.png"), trUtf8("Kubuntuwiki"), this);
    mySigMapInterWikiLinks->setMapping(iWikiKubuntuDoc, "iWikiKubuntuDoc");
    connect(iWikiKubuntuDoc, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiXubuntu = new QAction(QIcon(":/images/interwiki/xubuntu.png"), trUtf8("Xubuntu"), this);
    mySigMapInterWikiLinks->setMapping(iWikiXubuntu, "iWikiXubuntu");
    connect(iWikiXubuntu, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiLubuntu = new QAction(QIcon(":/images/interwiki/lubuntu.png"), trUtf8("Lubuntu"), this);
    mySigMapInterWikiLinks->setMapping(iWikiLubuntu, "iWikiLubuntu");
    connect(iWikiLubuntu, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiUbuntustudio = new QAction(QIcon(":/images/interwiki/ubuntustudio.png"), trUtf8("Ubuntu Studio"), this);
    mySigMapInterWikiLinks->setMapping(iWikiUbuntustudio, "iWikiUbuntustudio");
    connect(iWikiUbuntustudio, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiEdubuntu = new QAction(QIcon(":/images/interwiki/edubuntu.png"), trUtf8("Edubuntu"), this);
    mySigMapInterWikiLinks->setMapping(iWikiEdubuntu, "iWikiEdubuntu");
    connect(iWikiEdubuntu, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiDebian = new QAction(QIcon(":/images/interwiki/debian.png"), trUtf8("Debianwiki"), this);
    mySigMapInterWikiLinks->setMapping(iWikiDebian, "iWikiDebian");
    connect(iWikiDebian, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiDebian_de = new QAction(QIcon(":/images/interwiki/debian_de.png"), trUtf8("Dt. Debianwiki"), this);
    mySigMapInterWikiLinks->setMapping(iWikiDebian_de, "iWikiDebian_de");
    connect(iWikiDebian_de, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));

    // Wikimedia
    iWikiWikipedia = new QAction(QIcon(":/images/interwiki/wikipedia.png"), trUtf8("Dt. Wikipedia"), this);
    mySigMapInterWikiLinks->setMapping(iWikiWikipedia, "iWikiWikipedia");
    connect(iWikiWikipedia, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiWikipedia_en = new QAction(QIcon(":/images/interwiki/wikipedia_en.png"), trUtf8("Engl. Wikipedia"), this);
    mySigMapInterWikiLinks->setMapping(iWikiWikipedia_en, "iWikiWikipedia_en");
    connect(iWikiWikipedia_en, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiWikibooks = new QAction(QIcon(":/images/interwiki/wikibooks.png"), trUtf8("Wikibooks"), this);
    mySigMapInterWikiLinks->setMapping(iWikiWikibooks, "iWikiWikibooks");
    connect(iWikiWikibooks, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiWikimedia = new QAction(QIcon(":/images/interwiki/wikimedia.png"), trUtf8("Wikimedia"), this);
    mySigMapInterWikiLinks->setMapping(iWikiWikimedia, "iWikiWikimedia");
    connect(iWikiWikimedia, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));

    // Source & project hoster, bugtracker
    iWikiFreshmeat = new QAction(QIcon(":/images/interwiki/freshmeat.png"), trUtf8("Freshmeat"), this);
    mySigMapInterWikiLinks->setMapping(iWikiFreshmeat, "iWikiFreshmeat");
    connect(iWikiFreshmeat, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiGetdeb = new QAction(QIcon(":/images/interwiki/getdeb.png"), trUtf8("GetDeb"), this);
    mySigMapInterWikiLinks->setMapping(iWikiGetdeb, "iWikiGetdeb");
    connect(iWikiGetdeb, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiGooglecode = new QAction(QIcon(":/images/interwiki/googlecode.png"), trUtf8("Google Code"), this);
    mySigMapInterWikiLinks->setMapping(iWikiGooglecode, "iWikiGooglecode");
    connect(iWikiGooglecode, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiLaunchpad = new QAction(QIcon(":/images/interwiki/launchpad.png"), trUtf8("Launchpad"), this);
    mySigMapInterWikiLinks->setMapping(iWikiLaunchpad, "iWikiLaunchpad");
    connect(iWikiLaunchpad, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiBug = new QAction(QIcon(":/images/interwiki/bug.png"), trUtf8("Bug (Launchpad)"), this);
    mySigMapInterWikiLinks->setMapping(iWikiBug, "iWikiBug");
    connect(iWikiBug, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiLpuser = new QAction(QIcon(":/images/interwiki/lpuser.png"), trUtf8("Launchpad User"), this);
    mySigMapInterWikiLinks->setMapping(iWikiLpuser, "iWikiLpuser");
    connect(iWikiLpuser, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiPlaydeb = new QAction(QIcon(":/images/interwiki/playdeb.png"), trUtf8("PlayDeb"), this);
    mySigMapInterWikiLinks->setMapping(iWikiPlaydeb, "iWikiPlaydeb");
    connect(iWikiPlaydeb, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiSourceforge = new QAction(QIcon(":/images/interwiki/sourceforge.png"), trUtf8("Sourceforge"), this);
    mySigMapInterWikiLinks->setMapping(iWikiSourceforge, "iWikiSourceforge");
    connect(iWikiSourceforge, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));

    // Misc
    iWikiDropbox = new QAction(QIcon(":/images/interwiki/dropbox.png"), trUtf8("Dropbox"), this);
    mySigMapInterWikiLinks->setMapping(iWikiDropbox, "iWikiDropbox");
    connect(iWikiDropbox, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiEan = new QAction(QIcon(":/images/interwiki/ean.png"), trUtf8("EAN"), this);
    mySigMapInterWikiLinks->setMapping(iWikiEan, "iWikiEan");
    connect(iWikiEan, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiFb = new QAction(QIcon(":/images/interwiki/fb.png"), trUtf8("Facebook"), this);
    mySigMapInterWikiLinks->setMapping(iWikiFb, "iWikiFb");
    connect(iWikiFb, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiGoogle = new QAction(trUtf8("Google"), this);
    mySigMapInterWikiLinks->setMapping(iWikiGoogle, "iWikiGoogle");
    connect(iWikiGoogle, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiGplus = new QAction(QIcon(":/images/interwiki/gplus.png"), trUtf8("Google+"), this);
    mySigMapInterWikiLinks->setMapping(iWikiGplus, "iWikiGplus");
    connect(iWikiGplus, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiHolarse = new QAction(QIcon(":/images/interwiki/holarse.png"), trUtf8("Holarse"), this);
    mySigMapInterWikiLinks->setMapping(iWikiHolarse, "iWikiHolarse");
    connect(iWikiHolarse, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiIdentica = new QAction(QIcon(":/images/interwiki/identica.png"), trUtf8("Identica"), this);
    mySigMapInterWikiLinks->setMapping(iWikiIdentica, "iWikiIdentica");
    connect(iWikiIdentica, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiInternetWayback = new QAction(QIcon(":/images/interwiki/iawm.png"), trUtf8("Internet Wayback Machine"), this);
    mySigMapInterWikiLinks->setMapping(iWikiInternetWayback, "iWikiInternetWayback");
    connect(iWikiInternetWayback, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiIsbn = new QAction(QIcon(":/images/interwiki/isbn.png"), trUtf8("ISBN"), this);
    mySigMapInterWikiLinks->setMapping(iWikiIsbn, "iWikiIsbn");
    connect(iWikiIsbn, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiLastfm = new QAction(QIcon(":/images/interwiki/lastfm.png"), trUtf8("Last.fm"), this);
    mySigMapInterWikiLinks->setMapping(iWikiLastfm, "iWikiLastfm");
    connect(iWikiLastfm, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiLiflg = new QAction(QIcon(":/images/interwiki/liflg.jpg"), trUtf8("Linux Installers"), this);
    mySigMapInterWikiLinks->setMapping(iWikiLiflg, "iWikiLiflg");
    connect(iWikiLiflg, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiLinuxgaming = new QAction(QIcon(":/images/interwiki/linuxgaming.png"), trUtf8("Linux Gaming"), this);
    mySigMapInterWikiLinks->setMapping(iWikiLinuxgaming, "iWikiLinuxgaming");
    connect(iWikiLinuxgaming, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiOsm = new QAction(QIcon(":/images/interwiki/osm.png"), trUtf8("OpenStreetMap"), this);
    mySigMapInterWikiLinks->setMapping(iWikiOsm, "iWikiOsm");
    connect(iWikiOsm, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiTwitter = new QAction(QIcon(":/images/interwiki/twitter.png"), trUtf8("Twitter"), this);
    mySigMapInterWikiLinks->setMapping(iWikiTwitter, "iWikiTwitter");
    connect(iWikiTwitter, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));
    iWikiYoutube = new QAction(QIcon(":/images/interwiki/youtube.png"), trUtf8("Youtube"), this);
    mySigMapInterWikiLinks->setMapping(iWikiYoutube, "iWikiYoutube");
    connect(iWikiYoutube, SIGNAL(triggered()), mySigMapInterWikiLinks, SLOT(map()));

    connect(mySigMapInterWikiLinks, SIGNAL(mapped(QString)), this, SLOT(insertInterwikiLink(QString)));

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate menus
void CInyokaEdit::createMenus()
{
    // File menu
    fileMenu = menuBar()->addMenu(trUtf8("&Datei"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // Edit menu
    editMenu = menuBar()->addMenu(trUtf8("&Bearbeiten"));
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
    insertTextSampleMenu = menuBar()->addMenu(trUtf8("&Textbausteine"));
    insertBeginningMenu = insertTextSampleMenu->addMenu(trUtf8("Am Anfang eines Artikels"));
    insertBeginningMenu->addAction(insertUnderConstructionAct);
    insertBeginningMenu->addAction(insertTestedAct);
    insertBeginningMenu->addAction(insertKnowledgeAct);
    insertBeginningMenu->addAction(insertTableOfContentAct);
    insertBeginningMenu->addAction(insertExpertboxAct);
    insertBeginningMenu->addAction(insertAwardboxAct);

    insertImageMenu = insertTextSampleMenu->addMenu(trUtf8("Bilder"));
    insertImageMenu->addAction(insertImageAct);
    insertImageMenu->addAction(insertImageSubscriptionAct);
    insertImageMenu->addAction(insertImageCollectionAct);
    insertImageMenu->addAction(insertImageCollectionInTextAct);

    insertFormatingHelpMenu = insertTextSampleMenu->addMenu(trUtf8("Allg. Formatierungshilfen"));
    insertFormatingHelpMenu->addAction(insertCommandAct);
    insertFormatingHelpMenu->addAction(insertHintAct);
    insertFormatingHelpMenu->addAction(insertWarningAct);
    insertFormatingHelpMenu->addAction(insertExpertinfoAct);

    insertPacketinstallationMenu = insertTextSampleMenu->addMenu(trUtf8("Paketinstallation"));
    insertPacketinstallationMenu->addAction(insertPackageInstallAct);
    insertPacketinstallationMenu->addAction(insertPpaSampleAct);
    insertPacketinstallationMenu->addAction(insertForeignSourceSampleAct);
    insertPacketinstallationMenu->addAction(insertForeignAuthSampleAct);
    insertPacketinstallationMenu->addAction(insertForeignPackageSampleAct);
    insertPacketinstallationMenu->addAction(insertPackageMacroAct);

    insertMiscelementMenu = insertTextSampleMenu->addMenu(trUtf8("Sonstige Bausteine"));
    insertMiscelementMenu->addAction(insertExpandableAct);
    insertMiscelementMenu->addAction(insertFaultyAct);
    insertMiscelementMenu->addAction(insertLeftAct);
    insertMiscelementMenu->addAction(insertPackageWarningAct);
    insertMiscelementMenu->addAction(insertSourceWaringAct);
    insertMiscelementMenu->addAction(insertSoftwareWarningAct);

    // Insert interwiki-links menu
    iWikiMenu = menuBar()->addMenu(trUtf8("&InterWiki-Links"));

    // Inyoka
    iWikiMenuInyoka = iWikiMenu->addMenu(QIcon(":images/interwiki/user.png"), trUtf8("Inyoka"));
    iWikiMenuInyoka->addAction(iWikiForum);
    iWikiMenuInyoka->addAction(iWikiTopic);
    iWikiMenuInyoka->addAction(iWikiPost);
    iWikiMenuInyoka->addAction(iWikiUser);
    iWikiMenuInyoka->addAction(iWikiIkhaya);
    iWikiMenuInyoka->addAction(iWikiCalendar);
    iWikiMenuInyoka->addAction(iWikiTicket);
    iWikiMenuInyoka->addAction(iWikiBehind);
    iWikiMenuInyoka->addAction(iWikiPaste);
    iWikiMenuInyoka->addAction(iWikiPlanet);

    // Canonical, Ubuntu, Derivate
    iWikiMenuCanonical = iWikiMenu->addMenu(QIcon(":images/interwiki/canonical.png"), trUtf8("Canonical, Ubuntu, Derivate"));
    iWikiMenuCanonical->addAction(iWikiCanonical);
    iWikiMenuCanonical->addAction(iWikiCanonicalblog);
    iWikiMenuCanonical->addAction(iWikiPackages);
    iWikiMenuCanonical->addAction(iWikiApt);
    iWikiMenuCanonical->addAction(iWikiUbuntu);
    iWikiMenuCanonical->addAction(iWikiUbuntu_doc);
    iWikiMenuCanonical->addAction(iWikiAskubuntu);
    iWikiMenuCanonical->addAction(iWikiUbuntu_fr);
    iWikiMenuCanonical->addAction(iWikiUbuntuone);
    iWikiMenuCanonical->addAction(iWikiKubuntu);
    iWikiMenuCanonical->addAction(iWikiKubuntuDe);
    iWikiMenuCanonical->addAction(iWikiKubuntuDoc);
    iWikiMenuCanonical->addAction(iWikiXubuntu);
    iWikiMenuCanonical->addAction(iWikiLubuntu);
    iWikiMenuCanonical->addAction(iWikiUbuntustudio);
    iWikiMenuCanonical->addAction(iWikiEdubuntu);
    iWikiMenuCanonical->addAction(iWikiDebian);
    iWikiMenuCanonical->addAction(iWikiDebian_de);

    // Wikimedia
    iWikiMenuWikimedia = iWikiMenu->addMenu(QIcon(":images/interwiki/wikimedia.png"), trUtf8("Wikimedia"));
    iWikiMenuWikimedia->addAction(iWikiWikipedia);
    iWikiMenuWikimedia->addAction(iWikiWikipedia_en);
    iWikiMenuWikimedia->addAction(iWikiWikibooks);
    iWikiMenuWikimedia->addAction(iWikiWikimedia);

    // Source & project hoster, bugtracker
    iWikiMenuHoster = iWikiMenu->addMenu(QIcon(":images/interwiki/launchpad.png"), trUtf8("Bugtracker, Projekthoster"));
    iWikiMenuHoster->addAction(iWikiFreshmeat);
    iWikiMenuHoster->addAction(iWikiGetdeb);
    iWikiMenuHoster->addAction(iWikiGooglecode);
    iWikiMenuHoster->addAction(iWikiLaunchpad);
    iWikiMenuHoster->addAction(iWikiBug);
    iWikiMenuHoster->addAction(iWikiLpuser);
    iWikiMenuHoster->addAction(iWikiPlaydeb);
    iWikiMenuHoster->addAction(iWikiSourceforge);

    // Misc
    iWikiMenuMisc = iWikiMenu->addMenu(QIcon(":images/interwiki/youtube.png"), trUtf8("Sonstige"));
    iWikiMenuMisc->addAction(iWikiDropbox);
    iWikiMenuMisc->addAction(iWikiEan);
    iWikiMenuMisc->addAction(iWikiFb);
    iWikiMenuMisc->addAction(iWikiGoogle);
    iWikiMenuMisc->addAction(iWikiGplus);
    iWikiMenuMisc->addAction(iWikiHolarse);
    iWikiMenuMisc->addAction(iWikiIdentica);
    iWikiMenuMisc->addAction(iWikiInternetWayback);
    iWikiMenuMisc->addAction(iWikiIsbn);
    iWikiMenuMisc->addAction(iWikiLastfm);
    iWikiMenuMisc->addAction(iWikiLiflg);
    iWikiMenuMisc->addAction(iWikiLinuxgaming);
    iWikiMenuMisc->addAction(iWikiOsm);
    iWikiMenuMisc->addAction(iWikiTwitter);
    iWikiMenuMisc->addAction(iWikiYoutube);

    // Help menu
    helpMenu = menuBar()->addMenu(trUtf8("&Hilfe"));
    helpMenu->addAction(reportBugAct);
    helpMenu->addAction(aboutAct);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Generate tool bars
void CInyokaEdit::createToolBars()
{
    // File tool bar
    fileToolBar = addToolBar(trUtf8("Datei"));
    fileToolBar->setObjectName("fileToolBar");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    // Edit tool bar
    editToolBar = addToolBar(trUtf8("Bearbeiten"));
    editToolBar->setObjectName("editToolBar");
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addSeparator();
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);

    // Preview DL bar
    previewDlBar = addToolBar(trUtf8("Vorschau / Download"));
    previewDlBar->setObjectName("previewDlBar");
    previewDlBar->addAction(previewAct);
    previewDlBar->addAction(downloadArticleAct);

    // Inyoka tool bar
    addToolBarBreak(Qt::TopToolBarArea);  // second tool bar area under first one
    inyokaeditorBar = addToolBar(trUtf8("Editor"));
    inyokaeditorBar->setObjectName("inyokaeditorBar");
    inyokaeditorBar->addAction(boldAct);
    inyokaeditorBar->addAction(italicAct);
    inyokaeditorBar->addAction(monotypeAct);
    inyokaeditorBar->addAction(wikilinkAct);
    inyokaeditorBar->addAction(externLinkAct);
    inyokaeditorBar->addAction(imageAct);
    inyokaeditorBar->addAction(codeblockAct);

    // Tool bar for combo boxes (samples and macros)
    comboboxBar = addToolBar(trUtf8("Vorlagen und Makros"));
    comboboxBar->setObjectName("comboboxBar");
    comboboxBar->addWidget(headingsBox);
    // Headings combo box
    headingsBox->addItem(trUtf8("Überschrift"));
    headingsBox->insertSeparator(1);
    headingsBox->addItem(trUtf8("Überschrift: Stufe 1"));
    headingsBox->addItem(trUtf8("Überschrift: Stufe 2"));
    headingsBox->addItem(trUtf8("Überschrift: Stufe 3"));
    headingsBox->addItem(trUtf8("Überschrift: Stufe 4"));
    headingsBox->addItem(trUtf8("Überschrift: Stufe 5"));
    connect(headingsBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownHeading(int)));

    // Macros combo box
    comboboxBar->addWidget(textmacrosBox);
    textmacrosBox->addItem(trUtf8("Textbausteine"));
    textmacrosBox->insertSeparator(1);
    textmacrosBox->addItem(trUtf8("Baustelle"));
    textmacrosBox->addItem(trUtf8("Inhaltsverzeichnis"));
    textmacrosBox->addItem(trUtf8("Getestet"));
    textmacrosBox->addItem(trUtf8("Paketinstallation"));
    textmacrosBox->addItem(trUtf8("Befehl"));
    textmacrosBox->addItem(trUtf8("PPA-Vorlage"));
    textmacrosBox->addItem(trUtf8("Hinweis"));
    textmacrosBox->addItem(trUtf8("Fremdquelle-Warnung"));
    textmacrosBox->addItem(trUtf8("Warnung"));
    textmacrosBox->addItem(trUtf8("Experten-Info"));
    textmacrosBox->addItem(trUtf8("Tasten"));
    textmacrosBox->addItem(trUtf8("Tabelle"));
    connect(textmacrosBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextmacro(int)));

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Call parser
void CInyokaEdit::previewInyokaPage(const int iIndex){

    // Call parser if iIndex == index of myWebview -> Click on tab preview
    // or if iIndex == 999 -> Default parameter value when calling the function (e.g. )by clicking on button preview
    if (iIndex == myTabwidget->indexOf(myWebview) || iIndex == 999) {

        if (sCurFile == "" || sCurFile == trUtf8("Unbenannt")){
            myParser->genOutput("");
        }
        else{
            QFileInfo fi(sCurFile);
            myParser->genOutput(fi.fileName());
        }

    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// INSERT INYOKA ELEMENTS

// Headings (combobox in toolbar)
void CInyokaEdit::insertDropDownHeading(const int iSelection){

    bool bSelected = false;
    if (myEditor->textCursor().selectedText() != ""){
        bSelected = true;
    }

    myEditor->insertPlainText(QString::fromUtf8(myInsertSyntaxElement->GetInyokaHeading(iSelection, myEditor->textCursor().selectedText().toStdString()).c_str()));

    // Select text "Überschrift" if no text was selected
    if (bSelected == false && iSelection > 1) {
        QTextCursor textCursor = myEditor->textCursor();
        textCursor.setPosition( myEditor->textCursor().position() - QString(trUtf8("Überschrift")).length() - iSelection);
        textCursor.setPosition( myEditor->textCursor().position() - iSelection, QTextCursor::KeepAnchor );
        myEditor->setTextCursor( textCursor );
    }

    // Reset selection
    headingsBox->setCurrentIndex(0);

    myEditor->setFocus();
}

// Macro (combobox in toolbar)
void CInyokaEdit::insertDropDownTextmacro(const int iSelection){

    if (iSelection != 0 && iSelection != 1) {
        // -1 because of separator (considered as "item")
        switch (iSelection-1) {
        default:
        case 1:  // Lot (Baustelle)
            insertTextSample("Baustelle");
            break;
        case 2:  // Table of contents (Inhaltsverzeichnis)
            insertTextSample("Inhaltsverzeichnis");
            break;
        case 3:  // Tested (Getestet)
            insertTextSample("Getestet");
            break;
        case 4:  // Packet installation (Paketinstallation)
            insertTextSample("Paketinstallation");
            break;
        case 5:  // Command (Befehl)
            insertTextSample("Shellbefehl");
            break;
        case 6:  // PPA sample (PPA-Vorlage)
            insertTextSample("Ppavorlage");
            break;
        case 7:  // Hint (Hinweis)
            insertTextSample("Hinweisbox");
            break;
        case 8:  // Foreign source warning (Fremdquelle-Warnung)
            insertTextSample("Fremdquellewarnung");
            break;
        case 9:  // Warning (Warnung)
            insertTextSample("Warnbox");
            break;
        case 10:  // Expert information (Experten-Info)
            insertTextSample("Experteninfo");
            break;
        case 11:  // Keys (Tasten)
            insertTextSample("Tasten");
            break;
        case 12:
            // Table (Tabelle)
            insertTextSample("Tabelle");
            break;
        }
        // Reset selection
        textmacrosBox->setCurrentIndex(0);

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
    myEditor->insertPlainText(QString::fromUtf8(myInsertSyntaxElement->GetInterwikiLink(sMenuEntry.toStdString(), myEditor->textCursor().selectedText().toStdString()).c_str()));
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
                                         trUtf8("Bitte Namen des Artikel eingeben, der heruntergeladen werden soll:"), QLineEdit::Normal,
                                         trUtf8("Baustelle/MeinArtikel"), &ok);

        // Click on "cancel" or string is empty
        if (ok == false || sSitename.isEmpty())
            return;

        // Replace non valid characters
        sSitename.replace(trUtf8("ä"), "a", Qt::CaseInsensitive);
        sSitename.replace(trUtf8("ö"), "o", Qt::CaseInsensitive);
        sSitename.replace(trUtf8("ü"), "u", Qt::CaseInsensitive);
        sSitename.replace(" ", "_");

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        // Start article download
        QProcess procDownloadRawtext;
        procDownloadRawtext.start("wget -O - " + sInyokaUrl + "/" + sSitename + "?action=export&format=raw");

        if (!procDownloadRawtext.waitForStarted()) {
            QMessageBox::critical(this, sAppName, trUtf8("Kann Download des Rohformats des Artikels nicht starten."));
            procDownloadRawtext.kill();
            #ifndef QT_NO_CURSOR
                QApplication::restoreOverrideCursor();
            #endif
            return;
        }
        if (!procDownloadRawtext.waitForFinished()) {
            QMessageBox::critical(this, sAppName, trUtf8("Fehler beim Download des Rohtexts des Artikels."));
            procDownloadRawtext.kill();
            #ifndef QT_NO_CURSOR
                QApplication::restoreOverrideCursor();
            #endif
            return;
        }

        tempResult = procDownloadRawtext.readAll();
        sTmpArticle = QString::fromLocal8Bit(tempResult);

        // Site does not exist etc.
        if (sTmpArticle == "") {
            QMessageBox::information(this, sAppName, trUtf8("Der Artikel konnte nicht heruntergeladen werden."));
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
    procDownloadMetadata.start("wget -O - " + sInyokaUrl + "/" + sArticlename + "?action=metaexport");

    if (!procDownloadMetadata.waitForStarted()) {
        QMessageBox::critical(this, sAppName, trUtf8("Kann Download der Metadaten nicht starten."));
        procDownloadMetadata.kill();
        return;
    }
    if (!procDownloadMetadata.waitForFinished()) {
        QMessageBox::critical(this, sAppName, trUtf8("Fehler beim Download der Metadaten."));
        procDownloadMetadata.kill();
        return;
    }

    tempResult = procDownloadMetadata.readAll();
    sMetadata = QString::fromLocal8Bit(tempResult);

    // Site does not exist etc.
    if (sMetadata == "") {
        QMessageBox::information(this, sAppName, trUtf8("Es konnten keine Metadaten gefunden werden."));
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
        if (bAutomaticImageDownload == false) {
            iRet = QMessageBox::question(this, sAppName, trUtf8("Am Artikel angehängte Bilder ebenfalls herunterladen?"), QMessageBox::Yes, QMessageBox::No);
        }
        else {
            iRet = QMessageBox::Yes;
        }

        if (iRet == QMessageBox::Yes) {

            // File for download script
            QFile tmpScriptfile(StylesAndImagesDir.absolutePath() + "/" + sScriptName);

            // No write permission
            if (!tmpScriptfile.open(QFile::WriteOnly | QFile::Text)) {
                QMessageBox::warning(this, sAppName, trUtf8("Es konnte keine temporäre Download-Datei erstellt werden!"));
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
                scriptOutputstream << "wget -nv " << sInyokaUrl << "/_image?target=" << sListMetadata[j].toLower() << " -O " << sTmp << endl;
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
                std::cerr << "ERROR: myImageDownloadProgress - bad_alloc caught: " << ba.what() << std::endl;
                QMessageBox::critical(this, sAppName, trUtf8("Fehler bei der Speicherallokierung: ImageDownloadProgress"));
                exit (-1);
            }

            myImageDownloadProgress->open();
        }
    }

}

// -----------------------------------------------------------------------------------------------
void CInyokaEdit::showHtmlPreview(const QString &filename){

    statusBar()->showMessage(trUtf8("Öffne Vorschau"));

    if (bPreviewInEditor == false){
        // Open html-file in system web browser
        QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    }
    else{
        myWebview->load(QUrl::fromLocalFile(filename));
        myTabwidget->setCurrentIndex(myTabwidget->indexOf(myWebview));
        myWebview->reload();
    }
}


void CInyokaEdit::showMessageBox(const QString &sMessagetext, const QString &sType){

    if (sType == "warning"){
        QMessageBox::warning(this, sAppName, sMessagetext);
    }
    else if (sType == "critical"){
        QMessageBox::critical(this, sAppName, sMessagetext);
    }
    else {
        QMessageBox::information(this, sAppName, sMessagetext);
    }
}


void CInyokaEdit::findDialog(){
    m_findDialog->show();
}

void CInyokaEdit::findReplaceDialog(){
    m_findReplaceDialog->show();
}

// -----------------------------------------------------------------------------------------------
// Report a bug via Apport to Launchpad

void CInyokaEdit::reportBug(){

    // Start apport
    QProcess procApport;
    procApport.start("ubuntu-bug inyokaedit");

    if (!procApport.waitForStarted()) {
        QMessageBox::critical(this, sAppName, trUtf8("Problem beim Aufruf von Apport festgestellt."));
        return;
    }
    if (!procApport.waitForFinished()) {
        QMessageBox::critical(this, sAppName, trUtf8("Problem beim Ausführen von Apport."));
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
        QString sFileName = QFileDialog::getOpenFileName(this, trUtf8("Datei öffnen"), LastOpenedDir.absolutePath());  // File dialog opens last used folder
        if (!sFileName.isEmpty()){
            QFileInfo tmpFI(sFileName);
            LastOpenedDir = tmpFI.absoluteDir();
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
    QString sFileName = QFileDialog::getSaveFileName(this, trUtf8("Datei speichern"), LastOpenedDir.absolutePath());  // File dialog opens last used folder
    if (sFileName.isEmpty())
        return false;

    QFileInfo tmpFI(sFileName);
    LastOpenedDir = tmpFI.absoluteDir();

    return saveFile(sFileName);
}

void CInyokaEdit::about()
{
    QMessageBox::about(this, trUtf8("Über %1").arg(sAppName),
                       trUtf8("<b>%1</b> - Editor für das uu.de Wiki<br />"
                              "Version: %2<br /><br />"
                              "&copy; 2011, die Autoren von %3<br />"
                              "Lizenz: <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License Version 3</a><br /><br />"
                              "Die Anwendung verwendet Icons aus dem <a href=\"http://tango.freedesktop.org\">Tango-Projekt</a>.").arg(sAppName).arg(sVERSION).arg(sAppName));
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
        ret = QMessageBox::warning(this, sAppName,
                                   trUtf8("Die Datei wurde geändert.\n"
                                          "Sollen die Änderungen gespeichert werden?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
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
                             trUtf8("Kann Datei %1 nicht lesen:\n%2.")
                             .arg(sFileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    myEditor->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(sFileName);
    statusBar()->showMessage(trUtf8("Datei geladen"), 2000);
}

bool CInyokaEdit::saveFile(const QString &sFileName)
{
    QFile file(sFileName);
    // No write permission
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, sAppName,
                             trUtf8("Kann Datei %1 nicht speichern:\n%2.")
                             .arg(sFileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << myEditor->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(sFileName);
    statusBar()->showMessage(trUtf8("Datei gespeichert"), 2000);
    return true;
}

void CInyokaEdit::setCurrentFile(const QString &sFileName)
{
    sCurFile = sFileName;
    myEditor->document()->setModified(false);
    setWindowModified(false);

    QString sShownName = sCurFile;
    if (sCurFile.isEmpty())
        sShownName = trUtf8("Unbenannt");
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
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}
