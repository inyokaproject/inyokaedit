/**
 * \file CInyokaEdit.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2014 The InyokaEdit developers
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

#include <QComboBox>
#include <QtGui>
#include <QScrollBar>
#include <QWebFrame>

#if QT_VERSION >= 0x050000
    #include <QtWebKitWidgets/QWebView>
#else
    #include <QWebView>
#endif

#include "./CInyokaEdit.h"
#include "ui_CInyokaEdit.h"

extern bool bDEBUG;

CInyokaEdit::CInyokaEdit(QDir userDataDir,
                         QWidget *parent)
    : QMainWindow(parent),
      m_pUi(new Ui::CInyokaEdit),
      m_UserDataDir(userDataDir),
      m_sPreviewFile(m_UserDataDir.absolutePath() + "/tmpinyoka.html") {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_bOpenFileAfterStart = false;

    m_pUi->setupUi(this);

    // Check for command line arguments (except version/debug)
    if ((!bDEBUG && qApp->arguments().size() >= 2)
            || (bDEBUG && qApp->arguments().size() >= 3)) {
            m_bOpenFileAfterStart = true;
    }

    // Create folder for downloaded article images
    m_tmpPreviewImgDir = m_UserDataDir.absolutePath() + "/tmpImages";
    if (!m_tmpPreviewImgDir.exists()) {
        // Create folder including possible parent directories (mkPATH)!
        m_tmpPreviewImgDir.mkpath(m_tmpPreviewImgDir.absolutePath());
    }

    // After definition of StylesAndImagesDir AND m_tmpPreviewImgDir!
    this->createObjects();

    // Setup gui, menus, actions, toolbar...
    this->setupEditor();
    this->createActions();
    this->createMenus();
    this->createToolBars();

    // Download style files if preview/styles/imgages folders doesn't
    // exist (see QDesktopServices::DataLocation)
    if (!m_UserDataDir.exists()
            || !QDir(m_UserDataDir.absolutePath() + "/img").exists()
            || !QDir(m_UserDataDir.absolutePath() + "/styles").exists()
            || !QDir(m_UserDataDir.absolutePath() + "/Wiki").exists()) {
        // Create folder because user may not start download.
        // Folder is needed for preview.
        m_UserDataDir.mkdir(m_UserDataDir.absolutePath());
#if !defined _WIN32
        m_pDownloadModule->loadInyokaStyles();
#endif
    }

    if (true == m_bOpenFileAfterStart) {
        if (bDEBUG) {
            m_pFileOperations->loadFile(qApp->arguments()[2], true);
        } else {
            m_pFileOperations->loadFile(qApp->arguments()[1], true);
        }
    }

    m_bReloadPreviewBlocked = false;

    if (CUtils::getOnlineState() && m_pSettings->getWindowsCheckUpdate()) {
        m_pUtils->checkWindowsUpdate();
    }

    this->loadPlugins();
}

CInyokaEdit::~CInyokaEdit() {
    if (NULL != m_pUi) {
        delete m_pUi;
    }
    m_pUi = NULL;
    qDebug() << "Closing " << qApp->applicationName();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::createObjects() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pSettings = new CSettings(this);

    m_pFindReplace = new CFindReplace(m_pSettings);

    // Has to be created before parser
    m_pTemplates = new CTemplates(m_pSettings->getTemplateLanguage());

    m_pDownloadModule = new CDownload(this,
                                      m_UserDataDir.absolutePath(),
                                      m_tmpPreviewImgDir.absolutePath());

    m_pEditor = new CTextEditor(m_pTemplates->getListTplMacrosALL(),
                                m_UserDataDir.absolutePath(),
                                this);

//  if (true == m_pSettings->getPreviewAlongside()) {
    m_pEditor->installEventFilter(this);
//  }

    m_pSettings->init(m_pTemplates, m_pEditor->document());

    /*
    m_pUploadModule = new CUpload(this, m_pSettings->getInyokaUrl(),
                                  m_pEditor->document());
    */

    m_pFileOperations = new CFileOperations(this,
                                            m_pEditor,
                                            m_pSettings,
                                            m_sPreviewFile);

    m_pParser = new CParser(m_UserDataDir,
                            m_tmpPreviewImgDir,
                            m_pSettings->getInyokaUrl(),
                            m_pSettings->getCheckLinks(),
                            m_pTemplates);

    /**
     * \todo Add tabs for editing multiple documents.
     */
    // m_pTabwidgetDocuments = new QTabWidget;
    m_pTabwidgetRawPreview = new QTabWidget;

    m_pWebview = new QWebView(this);
    m_pWebview->installEventFilter(this);

    connect(m_pWebview->page(), SIGNAL(scrollRequested(int, int, QRect)),
            this, SLOT(syncScrollbarsWebview()));
    connect(m_pEditor->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(syncScrollbarsEditor()));
    m_bEditorScrolling = false;
    m_bWebviewScrolling = false;

    m_pUtils = new CUtils(this);
    connect(m_pUtils, SIGNAL(setWindowsUpdateCheck(bool)),
            m_pSettings, SLOT(setWindowsCheckUpdate(bool)));

    m_pPreviewTimer = new QTimer(this);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::setupEditor() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    // Application icon
    this->setWindowIcon(QIcon(":/images/"
                              + qApp->applicationName().toLower()
                              + "_64x64.png"));

    // Timed preview
    connect(m_pPreviewTimer, SIGNAL(timeout()),
            this, SLOT(previewInyokaPage()));

    this->updateEditorSettings();
    connect(m_pSettings, SIGNAL(updateEditorSettings()),
            this, SLOT(updateEditorSettings()));

    // Find/replace dialogs
    m_pFindReplace->setEditor(m_pEditor);

    connect(m_pFileOperations, SIGNAL(setMenuLastOpenedEnabled(bool)),
            m_pUi->fileMenuLastOpened, SLOT(setEnabled(bool)));

    /**
     * \todo Add tabs for editing multiple documents.
     */
    /*
    m_pTabwidgetDocuments->setTabPosition(QTabWidget::North);
    m_pTabwidgetDocuments->setTabsClosable(true);
    m_pTabwidgetDocuments->setMovable(true);
    m_pTabwidgetDocuments->setDocumentMode(true);
    */

    m_pFrameLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_pFrameLayout->addWidget(m_pWebview);

    connect(m_pWebview, SIGNAL(loadFinished(bool)),
            this, SLOT(loadPreviewFinished(bool)));

    if (true == m_pSettings->getPreviewAlongside()
            && true == m_pSettings->getPreviewInEditor()) {
        m_pWidgetSplitter = new QSplitter;

        /**
         * \todo Add tabs for editing multiple documents.
         */
        /*
        m_pWidgetSplitter->addWidget(m_pTabwidgetDocuments);
        m_pTabwidgetDocuments->addTab(m_pEditor, trUtf8("Untitled"));
        */

        m_pWidgetSplitter->addWidget(m_pEditor);
        m_pWidgetSplitter->addWidget(m_pWebview);

        connect(m_pFileOperations, SIGNAL(loadedFile()),
                this, SLOT(previewInyokaPage()));

        setCentralWidget(m_pWidgetSplitter);
        m_pWidgetSplitter->restoreState(m_pSettings->getSplitterState());

        if (!m_bOpenFileAfterStart) {
            // Show an empty website after start
            this->previewInyokaPage();
        }
    } else {
        /**
         * \todo Add tabs for editing multiple documents.
         */
        /*
        setCentralWidget(m_pTabwidgetDocuments);
        m_pTabwidgetDocuments->addTab(m_pTabwidgetRawPreview, trUtf8("Untitled"));
        */

        setCentralWidget(m_pTabwidgetRawPreview);
        m_pTabwidgetRawPreview->setTabPosition(QTabWidget::West);
        m_pTabwidgetRawPreview->addTab(m_pEditor, trUtf8("Raw format"));

        m_pTabwidgetRawPreview->addTab(m_pWebview, trUtf8("Preview"));
        if (false == m_pSettings->getPreviewInEditor()) {
            m_pTabwidgetRawPreview->setTabEnabled(
                        m_pTabwidgetRawPreview->indexOf(
                            m_pWebview), false);
        }
    }

    m_pFileOperations->setCurrentFile("");
    this->setUnifiedTitleAndToolBarOnMac(true);

    connect(m_pDownloadModule, SIGNAL(sendArticleText(QString, QString)),
            this, SLOT(displayArticleText(QString, QString)));

    // Restore window and toolbar settings
    // Settings have to be restored after toolbars are created!
    this->restoreGeometry(m_pSettings->getWindowGeometry());
    // Restore toolbar position etc.
    this->restoreState(m_pSettings->getWindowState());

    if (false == m_pSettings->getPreviewAlongside()) {
        this->removeToolBar(m_pUi->browserBar);
    }

    // Setting proxy if available
    CUtils::setProxy(m_pSettings->getProxyHostName(), m_pSettings->getProxyPort(),
                     m_pSettings->getProxyUserName(), m_pSettings->getProxyPassword());

    m_pUi->aboutAct->setText(m_pUi->aboutAct->text() + " " + qApp->applicationName());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void CInyokaEdit::createActions() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    // File menu
    // New file
    m_pUi->newAct->setShortcuts(QKeySequence::New);
    connect(m_pUi->newAct, SIGNAL(triggered()),
            m_pFileOperations, SLOT(newFile()));

    // Open file
    m_pUi->openAct->setShortcuts(QKeySequence::Open);
    connect(m_pUi->openAct, SIGNAL(triggered()),
            this, SLOT(openFile()));

    // Clear recent files list
    m_pClearRecentFilesAct = new QAction(trUtf8("Clear list"), this);
    connect(m_pClearRecentFilesAct, SIGNAL(triggered()),
            m_pFileOperations, SLOT(clearRecentFiles()));

    // Save file
    m_pUi->saveAct->setShortcuts(QKeySequence::Save);
    connect(m_pUi->saveAct, SIGNAL(triggered()),
            m_pFileOperations, SLOT(save()));

    // Save file as...
    m_pUi->saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(m_pUi->saveAsAct, SIGNAL(triggered()),
            m_pFileOperations, SLOT(saveAs()));

    // Print preview
    m_pUi->printPreviewAct->setShortcut(QKeySequence::Print);
    connect(m_pUi->printPreviewAct, SIGNAL(triggered()),
            m_pFileOperations, SLOT(printPreview()));
    if (false == m_pSettings->getPreviewAlongside()) {
        m_pUi->printPreviewAct->setEnabled(false);
    }

    // Exit application
    m_pUi->exitAct->setShortcuts(QKeySequence::Quit);
    connect(m_pUi->exitAct, SIGNAL(triggered()),
            this, SLOT(close()));

    // ------------------------------------------------------------------------
    // EDIT MENU

    // Find
    m_pUi->searchAct->setShortcuts(QKeySequence::Find);
    connect(m_pUi->searchAct, SIGNAL(triggered()),
            this, SLOT(callSearch()));

    // Replace
    m_pUi->replaceAct->setShortcuts(QKeySequence::Replace);
    connect(m_pUi->replaceAct, SIGNAL(triggered()),
            this, SLOT(callReplace()));
/*
    // Find next
    m_pUi->findNextAct->setShortcuts(QKeySequence::FindNext);
    connect(m_pUi->findNextAct, SIGNAL(triggered()),
            m_findDialog, SLOT(findNext()));

    // Find previous
    m_pUi->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
    connect(m_pUi->findPreviousAct, SIGNAL(triggered()),
            m_findDialog, SLOT(findPrev()));
*/

    // Cut
    m_pUi->cutAct->setShortcuts(QKeySequence::Cut);
    connect(m_pUi->cutAct, SIGNAL(triggered()),
            m_pEditor, SLOT(cut()));
    connect(m_pEditor, SIGNAL(copyAvailable(bool)),
            m_pUi->cutAct, SLOT(setEnabled(bool)));
    // Copy
    m_pUi->copyAct->setShortcuts(QKeySequence::Copy);
    connect(m_pUi->copyAct, SIGNAL(triggered()),
            m_pEditor, SLOT(copy()));
    connect(m_pEditor, SIGNAL(copyAvailable(bool)),
            m_pUi->copyAct, SLOT(setEnabled(bool)));
    // Paste
    m_pUi->pasteAct->setShortcuts(QKeySequence::Paste);
    connect(m_pUi->pasteAct, SIGNAL(triggered()),
            m_pEditor, SLOT(paste()));
    // Undo
    m_pUi->undoAct->setShortcuts(QKeySequence::Undo);
    connect(m_pUi->undoAct, SIGNAL(triggered()),
            m_pEditor, SLOT(undo()));
    connect(m_pEditor, SIGNAL(undoAvailable(bool)),
            m_pUi->undoAct, SLOT(setEnabled(bool)));
    // Redo
    m_pUi->redoAct->setShortcuts(QKeySequence::Redo);
    connect(m_pUi->redoAct, SIGNAL(triggered()),
            m_pEditor, SLOT(redo()));
    connect(m_pEditor, SIGNAL(redoAvailable(bool)),
            m_pUi->redoAct, SLOT(setEnabled(bool)));

    m_pUi->cutAct->setEnabled(false);
    m_pUi->copyAct->setEnabled(false);
    m_pUi->undoAct->setEnabled(false);
    m_pUi->redoAct->setEnabled(false);

    // ------------------------------------------------------------------------
    // TOOLS MENU

    // Download styles and IWLs
    connect(m_pUi->downloadInyokaStylesAct, SIGNAL(triggered()),
            m_pDownloadModule, SLOT(loadInyokaStyles()));
    connect(m_pUi->updateIWLsAct, SIGNAL(triggered()),
            m_pDownloadModule, SLOT(updateIWLs()));
#if defined _WIN32
    m_pUi->downloadInyokaStylesAct->setDisabled(true);
    m_pUi->updateIWLsAct->setDisabled(true);
#endif

    // Clear temp. image download folder
    connect(m_pUi->deleteTempImagesAct, SIGNAL(triggered()),
            this, SLOT(deleteTempImages()));

    // Show settings dialog
    connect(m_pUi->preferencesAct, SIGNAL(triggered()),
            m_pSettings, SIGNAL(showSettingsDialog()));

    // ------------------------------------------------------------------------

    // Show html preview
    m_pUi->previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(m_pUi->previewAct, SIGNAL(triggered()),
            this, SLOT(previewInyokaPage()));

    // Click on tabs of widget - int = index of tab
    connect(m_pTabwidgetRawPreview, SIGNAL(currentChanged(int)),
            this, SLOT(previewInyokaPage(int)));

    // Download Inyoka article
    connect(m_pUi->downloadArticleAct, SIGNAL(triggered()),
            this, SLOT(downloadArticle()));

    /*
    // Upload Inyoka article
    connect(m_pUi->uploadArticleAct, SIGNAL(triggered()),
            m_pUploadModule, SLOT(uploadArticle()));
    */

    // ------------------------------------------------------------------------

    // Insert headline
    m_pHeadlineBox = new QComboBox();
    // Insert sample
    m_pTextmacrosBox = new QComboBox();
    // Insert text format
    m_pTextformatBox = new QComboBox();

    // ------------------------------------------------------------------------
    // INSERT SYNTAX ELEMENTS

    m_pSigMapMainEditorToolbar = new QSignalMapper(this);

    // Insert bold element
    m_pUi->boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    m_pSigMapMainEditorToolbar->setMapping(m_pUi->boldAct, "boldAct");
    connect(m_pUi->boldAct, SIGNAL(triggered()),
            m_pSigMapMainEditorToolbar, SLOT(map()));

    // Insert italic element
    m_pUi->italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    m_pSigMapMainEditorToolbar->setMapping(m_pUi->italicAct, "italicAct");
    connect(m_pUi->italicAct, SIGNAL(triggered()),
            m_pSigMapMainEditorToolbar, SLOT(map()));

    // Insert monotype element
    m_pSigMapMainEditorToolbar->setMapping(m_pUi->monotypeAct, "monotypeAct");
    connect(m_pUi->monotypeAct, SIGNAL(triggered()),
            m_pSigMapMainEditorToolbar, SLOT(map()));

    // Insert wiki link
    m_pSigMapMainEditorToolbar->setMapping(m_pUi->wikilinkAct, "wikilinkAct");
    connect(m_pUi->wikilinkAct, SIGNAL(triggered()),
            m_pSigMapMainEditorToolbar, SLOT(map()));

    // Insert extern link
    m_pSigMapMainEditorToolbar->setMapping(m_pUi->externalLinkAct,
                                      "externalLinkAct");
    connect(m_pUi->externalLinkAct, SIGNAL(triggered()),
            m_pSigMapMainEditorToolbar, SLOT(map()));

    // Insert image
    m_pSigMapMainEditorToolbar->setMapping(m_pUi->imageAct, "imageAct");
    connect(m_pUi->imageAct, SIGNAL(triggered()),
            m_pSigMapMainEditorToolbar, SLOT(map()));

    connect(m_pSigMapMainEditorToolbar, SIGNAL(mapped(QString)),
            this, SLOT(insertMainEditorButtons(QString)));

    // Code block + syntax highlighting
    m_pSigMapCodeHighlight = new QSignalMapper(this);
    QStringList sListHighlightText, sListHighlightLang;
    sListHighlightText << trUtf8("Raw text") << trUtf8("Code without highlighting")
                       << "Bash" << "C" << "C#" << "C++" << "CSS" << "D"
                       << "Django / Jinja Templates" << "HTML" << "IRC Logs"
                       << "Java" << "JavaScript" << "Perl" << "PHP" << "Python"
                       << "Python Console Sessions" << "Python Tracebacks"
                       << "Ruby" << "sources.list" << "SQL" << "XML";

    sListHighlightLang << "" << "text" << "bash" << "c" << "csharp" << "cpp"
                       << "css" << "d" << "html+django" << "html" << "irc"
                       << "java" << "js" << "perl" << "html+php" << "python"
                       << "pycon" << "pytb" << "ruby" << "sourceslist"
                       << "sql" << "xml";

    QString sCodeTag("#!" + m_pTemplates->getTransCodeBlock().toLower() + " ");
    m_pCodePopup = new QToolButton;
    m_pCodePopup->setIcon(QIcon(":/images/code.png"));
    m_pCodePopup->setPopupMode(QToolButton::InstantPopup);
    m_pCodeStyles = new QMenu(m_pCodePopup);

    for (int i = 0; i < sListHighlightText.size(); i++) {
        if (0 != i) {
            sListHighlightLang[i] = sCodeTag + sListHighlightLang[i];
        }
        m_CodeHighlightActions << new QAction(sListHighlightText[i], this);
        m_pSigMapCodeHighlight->setMapping(m_CodeHighlightActions[i],
                                          sListHighlightLang[i]);
        connect(m_CodeHighlightActions[i], SIGNAL(triggered()),
                m_pSigMapCodeHighlight, SLOT(map()));
    }

    m_pCodeStyles->addActions(m_CodeHighlightActions);
    m_pCodePopup->setMenu(m_pCodeStyles);
    m_pUi->inyokaeditorBar->addWidget(m_pCodePopup);

    connect(m_pSigMapCodeHighlight, SIGNAL(mapped(QString)),
            this, SLOT(insertCodeblock(QString)));

    // ------------------------------------------------------------------------
    // MARKUP TEMPLATES MENU

    m_pSigMapTemplates = new QSignalMapper(this);

    this->createXmlActions(m_pSigMapTemplates,
                           "/templates/"
                           + m_pSettings->getTemplateLanguage()
                           + "/",
                            m_TplActions, m_pTemplates->getTPLs());

    connect(m_pSigMapTemplates, SIGNAL(mapped(QString)),
            this, SLOT(insertMacro(QString)));

    // ------------------------------------------------------------------------
    // INTERWIKI LINKS MENU

    m_pSigMapInterWikiLinks = new QSignalMapper(this);

    this->createXmlActions(m_pSigMapInterWikiLinks, "/iWikiLinks/",
                           m_iWikiLinksActions, m_pTemplates->getIWLs());

    connect(m_pSigMapInterWikiLinks, SIGNAL(mapped(QString)),
            this, SLOT(insertInterwikiLink(QString)));

    // ------------------------------------------------------------------------
    // ABOUT MENU

    // Show syntax overview
    connect(m_pUi->showSyntaxOverviewAct, SIGNAL(triggered()),
            this, SLOT(showSyntaxOverview()));

    // Report a bug using apport
    connect(m_pUi->reportBugAct, SIGNAL(triggered()),
            m_pUtils, SLOT(reportBug()));

    // Open about windwow
    connect(m_pUi->aboutAct, SIGNAL(triggered()),
            m_pUtils, SLOT(showAbout()));

}

// ----------------------------------------------------------------------------

void CInyokaEdit::createXmlActions(QSignalMapper *SigMap,
                                   const QString &sIconPath,
                                   QList<QList<QAction *> >& listActions,
                                   CXmlParser* pXmlMenu) {
    QList <QAction *> emptyActionList;
    emptyActionList.clear();

    // No installation: Use app path
    QString sTmpPath = qApp->applicationDirPath() + sIconPath;
    // Path from normal installation
    if (QFile::exists("/usr/share/" + qApp->applicationName().toLower()
                      + sIconPath) && !bDEBUG) {
        sTmpPath = "/usr/share/" + qApp->applicationName().toLower()
                + sIconPath;
    }

    for (int i = 0; i < pXmlMenu->getGrouplist().size(); i++) {
        listActions.append(emptyActionList);
        for (int j = 0; j < pXmlMenu->getElementNames()[i].size(); j++) {
            listActions[i].append(
                        new QAction(QIcon(sTmpPath +
                                          pXmlMenu->getElementIcons()[i][j]),
                                    pXmlMenu->getElementNames()[i][j], this));

            SigMap->setMapping(listActions[i][j],
                               QString::number(i) + "," + QString::number(j));
            connect(listActions[i][j], SIGNAL(triggered()),
                    SigMap, SLOT(map()));
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Generate menus
void CInyokaEdit::createMenus() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QDir articleTemplateDir("");
    QDir userArticleTemplateDir(m_UserDataDir.absolutePath() +
                                "/templates/articles");

    // File menu (new from template)
    if (QFile::exists("/usr/share/" + qApp->applicationName().toLower()
                      + "/templates/" + m_pSettings->getTemplateLanguage()
                      + "/articles") && !bDEBUG) {
        articleTemplateDir.setPath("/usr/share/"
                                   + qApp->applicationName().toLower()
                                   + "/templates/"
                                   + m_pSettings->getTemplateLanguage()
                                   + "/articles");
    } else {
        // No installation: Use app path
        articleTemplateDir.setPath(qApp->applicationDirPath()
                                   + "/templates/"
                                   + m_pSettings->getTemplateLanguage()
                                   + "/articles");
    }

    QList<QDir> listTplDirs;
    if (articleTemplateDir.exists()) {
        listTplDirs << articleTemplateDir;
    }
    if (userArticleTemplateDir.exists()) {
        listTplDirs << userArticleTemplateDir;
    }

    m_pSigMapOpenTemplate = new QSignalMapper(this);

    bool bFirstLoop(true);
    foreach (QDir tplDir, listTplDirs) {
        QFileInfoList fiListFiles = tplDir.entryInfoList(
                    QDir::NoDotAndDotDot | QDir::Files);
        foreach (QFileInfo fi, fiListFiles) {
            if ("tpl" == fi.suffix()) {
                m_OpenTemplateFilesActions << new QAction(
                                                  fi.baseName().replace("_", " "),
                                                  this);
                m_pSigMapOpenTemplate->setMapping(m_OpenTemplateFilesActions.last(),
                                                  fi.absoluteFilePath());
                connect(m_OpenTemplateFilesActions.last(), SIGNAL(triggered()),
                        m_pSigMapOpenTemplate, SLOT(map()));
            }
        }

        // Add separator between templates and user templates
        if (bFirstLoop) {
            bFirstLoop = false;
            m_OpenTemplateFilesActions << new QAction(this);
            m_OpenTemplateFilesActions.last()->setSeparator(true);
        }
    }
    m_pUi->fileMenuFromTemplate->addActions(m_OpenTemplateFilesActions);
    connect(m_pSigMapOpenTemplate, SIGNAL(mapped(QString)),
            m_pFileOperations, SLOT(loadFile(QString)));

    if (0 == m_OpenTemplateFilesActions.size()) {
        m_pUi->fileMenuFromTemplate->setDisabled(true);
    }

    // File menu (recent opened files)
    m_pUi->fileMenuLastOpened->addActions(
                m_pFileOperations->getLastOpenedFiles());
    m_pUi->fileMenuLastOpened->addSeparator();
    m_pUi->fileMenuLastOpened->addAction(m_pClearRecentFilesAct);
    if (0 == m_pSettings->getRecentFiles().size()) {
        m_pUi->fileMenuLastOpened->setEnabled(false);
    }

    // Insert TPL menu
    m_pTplMenu = new QMenu(m_pTemplates->getTPLs()->getMenuName(), this);
    this->insertXmlMenu(m_pTplMenu, m_TplGroups,
                        "/templates/" + m_pSettings->getTemplateLanguage() +"/",
                        m_TplActions, m_pTemplates->getTPLs(),
                        m_pUi->toolsMenu->menuAction());

    // Insert IWL menu
    m_piWikiMenu = new QMenu(m_pTemplates->getIWLs()->getMenuName(), this);
    this->insertXmlMenu(m_piWikiMenu, m_iWikiGroups, "/iWikiLinks/",
                        m_iWikiLinksActions, m_pTemplates->getIWLs(),
                        m_pUi->toolsMenu->menuAction());
}

// ----------------------------------------------------------------------------

void CInyokaEdit::insertXmlMenu(QMenu* pMenu, QList<QMenu *> pMenuGroup,
                                const QString &sIconPath,
                                QList<QList<QAction *> > listActions,
                                CXmlParser* pXmlMenu, QAction* pPosition) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi->menuBar->insertMenu(pPosition, pMenu);

    // No installation: Use app path
    QString sTmpPath(qApp->applicationDirPath() + sIconPath);
    // Path from normal installation
    if (QFile::exists("/usr/share/" + qApp->applicationName().toLower()
                      + sIconPath) && !bDEBUG) {
        sTmpPath = "/usr/share/" + qApp->applicationName().toLower() + sIconPath;
    }

    for (int i = 0; i < pXmlMenu->getGrouplist().size(); i++) {
        pMenuGroup.append(pMenu->addMenu(QIcon(sTmpPath
                                               + pXmlMenu->getGroupIcons()[i]),
                                         pXmlMenu->getGrouplist()[i]));

        pMenuGroup[i]->addActions(listActions[i]);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Generate tool bars
void CInyokaEdit::createToolBars() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    // Tool bar for combo boxes (samples and macros)
    m_pUi->samplesmacrosBar->addWidget(m_pHeadlineBox);

    // Headline combo box
    QString sHeadline(trUtf8("Headline"));
    QString sHeadlineStep(trUtf8("Step"));
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
    m_pUi->samplesmacrosBar->addWidget(m_pTextmacrosBox);
    m_pTextmacrosBox->addItem(m_pTemplates->getDropTPLs()->getMenuName());
    m_pTextmacrosBox->insertSeparator(1);
    if (m_pTemplates->getDropTPLs()->getElementNames().size() > 0) {
        foreach (QString s, m_pTemplates->getDropTPLs()->getElementNames()[0]) {
            m_pTextmacrosBox->addItem(s);
        }
    }
    connect(m_pTextmacrosBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextmacro(int)));

    // Text format combo box
    m_pUi->samplesmacrosBar->addWidget(m_pTextformatBox);
    m_pTextformatBox->addItem(trUtf8("Text format"));
    m_pTextformatBox->insertSeparator(1);
    m_pTextformatBox->addItem(trUtf8("Folders"));
    m_pTextformatBox->addItem(trUtf8("Menu entries"));
    m_pTextformatBox->addItem(trUtf8("Files"));
    m_pTextformatBox->addItem(trUtf8("Command"));
    connect(m_pTextformatBox, SIGNAL(activated(int)),
            this, SLOT(insertDropDownTextformat(int)));

    // Browser buttons
    connect(m_pUi->goBackBrowserAct, SIGNAL(triggered()),
            m_pWebview, SLOT(back()));
    connect(m_pUi->goForwardBrowserAct, SIGNAL(triggered()),
            m_pWebview, SLOT(forward()));
    connect(m_pUi->reloadBrowserAct, SIGNAL(triggered()),
            m_pWebview, SLOT(reload()));
    connect(m_pWebview, SIGNAL(urlChanged(QUrl)),
            this, SLOT(changedUrl()));
    m_pWebview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_pWebview, SIGNAL(linkClicked(QUrl)),
            this, SLOT(clickedLink(QUrl)));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::openFile() {
    // Reset scroll position
    m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
    m_pFileOperations->open();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::callSearch() {
    m_pFindReplace->toggleSearchReplace(false);
    m_pFindReplace->show();
}

void CInyokaEdit::callReplace() {
    m_pFindReplace->toggleSearchReplace(true);
    m_pFindReplace->show();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Call parser
void CInyokaEdit::previewInyokaPage(const int nIndex) {
    // Call parser if iIndex == index of m_pWebview -> Click on tab preview
    // or if iIndex == 999 -> Default parameter value when calling the function
    // e.g. by clicking on button preview
    if (m_pTabwidgetRawPreview->indexOf(m_pWebview) == nIndex
            || 999 == nIndex) {
        // Only disable buttons if preview is not shown alongside editor
        if (false == m_pSettings->getPreviewAlongside()
                && true == m_pSettings->getPreviewInEditor()) {
            // Disable editor and insert samples/macros toolbars
            m_pUi->editMenu->setDisabled(true);
            m_pTplMenu->setDisabled(true);
            m_piWikiMenu->setDisabled(true);
            m_pUi->editToolBar->setDisabled(true);
            m_pUi->inyokaeditorBar->setDisabled(true);
            // this->removeToolBar(m_pUi->inyokaeditorBar);
            m_pUi->samplesmacrosBar->setDisabled(true);
            // this->removeToolBar(m_pUi->samplesmacrosBar);
            m_pUi->previewAct->setDisabled(true);
            this->addToolBar(m_pUi->browserBar);
            m_pUi->browserBar->show();

            m_pUi->printPreviewAct->setEnabled(true);
        }

        m_pWebview->history()->clear();  // Clear history (clicked links)

        QString sRetHTML("");
        if (m_pFileOperations->getCurrentFile().isEmpty()
                || trUtf8("Untitled") == m_pFileOperations->getCurrentFile()) {
            sRetHTML = m_pParser->genOutput("", m_pEditor->document());
        } else {
            sRetHTML = m_pParser->genOutput(m_pFileOperations->getCurrentFile(),
                                            m_pEditor->document());
        }

        // File for temporary html output
        QFile tmphtmlfile(m_sPreviewFile);

        // No write permission
        if (!tmphtmlfile.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, qApp->applicationName(),
                                 trUtf8("Could not create temporary HTML file!"));
            qWarning() << "Could not create temporary HTML file:"
                       << m_sPreviewFile;
            return;
        }

        // Stream for output in file
        QTextStream tmpoutputstream(&tmphtmlfile);
        tmpoutputstream.setCodec("UTF-8");
        tmpoutputstream.setAutoDetectUnicode(true);

        // Write HTML code into output file
        tmpoutputstream << sRetHTML;
        tmphtmlfile.close();

        if (false == m_pSettings->getPreviewInEditor()) {
            // Open html-file in system web browser
            QDesktopServices::openUrl(
                        QUrl::fromLocalFile(
                            QFileInfo(tmphtmlfile).absoluteFilePath()) );
        } else {
            // Store scroll position
            m_WebviewScrollPosition = m_pWebview->page()->mainFrame()->scrollPosition();
            m_pWebview->load(QUrl::fromLocalFile(QFileInfo(tmphtmlfile).absoluteFilePath()));
        }
    } else {
        // Enable editor and insert samples/macros toolbars again
        m_pUi->editMenu->setEnabled(true);
        m_pTplMenu->setEnabled(true);
        m_piWikiMenu->setEnabled(true);
        m_pUi->editToolBar->setEnabled(true);
        m_pUi->inyokaeditorBar->setEnabled(true);
        // this->addToolBar(m_pUi->inyokaeditorBar);
        // m_pUi->inyokaeditorBar->show();
        m_pUi->samplesmacrosBar->setEnabled(true);
        // this->addToolBar(m_pUi->samplesmacrosBar);
        // m_pUi->samplesmacrosBar->show();
        m_pUi->previewAct->setEnabled(true);
        this->removeToolBar(m_pUi->browserBar);

        m_pUi->printPreviewAct->setEnabled(false);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// INSERT INYOKA ELEMENTS

// Headline (combobox in toolbar)
void CInyokaEdit::insertDropDownHeadline(const int nSelection) {
    if (nSelection > 1) {
        QString sHeadline(trUtf8("Headline"));
        QString sHeadTag("");

        // Generate headline tag
        for (int i = 1; i < nSelection; i++) {
            sHeadTag.append("=");
        }

        // Some text was selected
        if (!m_pEditor->textCursor().selectedText().isEmpty()) {
            m_pEditor->insertPlainText(sHeadTag + " "
                                       + m_pEditor->textCursor().selectedText()
                                       + " " + sHeadTag);
        } else {
            // Select text sHeadline if no text was selected
            m_pEditor->insertPlainText(sHeadTag + " "
                                       + sHeadline + " "
                                       + sHeadTag);

            QTextCursor textCursor(m_pEditor->textCursor());
            textCursor.setPosition(m_pEditor->textCursor().position() - sHeadline.length() - nSelection);
            textCursor.setPosition(m_pEditor->textCursor().position() - nSelection, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(textCursor);
        }
    }

    // Reset selection
    m_pHeadlineBox->setCurrentIndex(0);

    m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Macro (combobox in toolbar)
void CInyokaEdit::insertDropDownTextmacro(const int nSelection) {
    if (nSelection != 0 && nSelection != 1) {
        QString sTmp("");
        QString sName(m_pTemplates->getDropTPLs()->getElementUrls()[0][nSelection -2]);
        sName.remove(".tpl");
        sName.remove(".macro");

        int nIndex = m_pTemplates->getListTplNamesALL().indexOf(sName);
        if (nIndex >= 0) {
            QString sMacro(m_pTemplates->getListTplMacrosALL()[nIndex]);
            sMacro.replace("\\n", "\n");
            int nPlaceholder1 = sMacro.indexOf("%%");
            int nPlaceholder2 = sMacro.lastIndexOf("%%");

            // No text selected
            if (m_pEditor->textCursor().selectedText().isEmpty()) {
                int nCurrentPos = m_pEditor->textCursor().position();

                // Insert macro
                sMacro.remove("%%");  // Remove placeholder
                m_pEditor->insertPlainText(sMacro);

                // Select placeholder
                if ((nPlaceholder1 != nPlaceholder2)
                        && nPlaceholder1 >= 0
                        && nPlaceholder2 >= 0) {
                    QTextCursor textCursor(m_pEditor->textCursor());
                    textCursor.setPosition(nCurrentPos + nPlaceholder1);
                    textCursor.setPosition(nCurrentPos + nPlaceholder2 -2, QTextCursor::KeepAnchor);
                    m_pEditor->setTextCursor(textCursor);
                }
            } else {
                // Some text is selected
                sTmp = sMacro;
                if ((nPlaceholder1 != nPlaceholder2)
                        && nPlaceholder1 >= 0
                        && nPlaceholder2 >= 0) {
                    sTmp.replace(nPlaceholder1, nPlaceholder2 - nPlaceholder1,
                                 m_pEditor->textCursor().selectedText());
                    m_pEditor->insertPlainText(sTmp.remove("%%"));
                } else {
                    // Problem with placeholder
                    m_pEditor->insertPlainText(sMacro.remove("%%"));
                }
            }
        } else {
            qWarning()  << "Unknown macro chosen:" << sName;
        }
    }

    m_pTextmacrosBox->setCurrentIndex(0);
    m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Text format (combobox in toolbar)
void CInyokaEdit::insertDropDownTextformat(const int nSelection) {
    bool bSelected(false);
    QString sInsertedText("");

    // Some text was selected
    if (!m_pEditor->textCursor().selectedText().isEmpty()) {
        bSelected = true;
    }

    if (nSelection != 0 && nSelection != 1) {
        quint16 nFormatLength(0);

        // -1 because of separator (considered as "item")
        switch (nSelection-1) {
            default:
            case 1:  // Folders
                if (bSelected) {
                    m_pEditor->insertPlainText("'''"
                                               + m_pEditor->textCursor().selectedText()
                                               + "'''");
                } else {
                    sInsertedText = trUtf8("Folders");
                    nFormatLength = 3;
                    m_pEditor->insertPlainText("'''"
                                               + sInsertedText
                                               + "'''");
                }
                break;
            case 2:  // Menus
                if (bSelected) {
                    m_pEditor->insertPlainText("''\""
                                               + m_pEditor->textCursor().selectedText()
                                               + "\"''");
                } else {
                    sInsertedText = trUtf8("Menu -> sub menu -> menu entry");
                    nFormatLength = 3;
                    m_pEditor->insertPlainText("''\""
                                               + sInsertedText
                                               + "\"''");
                }
                break;
            case 3:  // Files
                if (bSelected) {
                    m_pEditor->insertPlainText("'''"
                                               + m_pEditor->textCursor().selectedText()
                                               + "'''");
                } else {
                    sInsertedText = trUtf8("Files");
                    nFormatLength = 3;
                    m_pEditor->insertPlainText("'''"
                                               + sInsertedText
                                               + "'''");
                }
                break;
            case 4:  // Commands
                if (bSelected) {
                    m_pEditor->insertPlainText("`"
                                               + m_pEditor->textCursor().selectedText()
                                               + "`");
                } else {
                    sInsertedText = trUtf8("Command");
                    nFormatLength = 1;
                    m_pEditor->insertPlainText("`"
                                               + sInsertedText
                                               + "`");
                }
                break;
        }

        // Reset selection
        m_pTextformatBox->setCurrentIndex(0);

        if (!bSelected) {
            QTextCursor textCursor(m_pEditor->textCursor());
            textCursor.setPosition(m_pEditor->textCursor().position() -
                                     sInsertedText.length() - nFormatLength);
            textCursor.setPosition(m_pEditor->textCursor().position() -
                                     nFormatLength, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(textCursor);
        }

        m_pEditor->setFocus();
    }
}

// ----------------------------------------------------------------------------

// Insert main syntax element from toolbar (bold, italic, ...)
void CInyokaEdit::insertMainEditorButtons(const QString &sAction) {
    bool bSelected(false);
    QString sInsertedText("");
    quint16 nFormatLength(0);

    // Some text was selected
    if (!m_pEditor->textCursor().selectedText().isEmpty()) {
        bSelected = true;
    }

    if ("boldAct" == sAction) {
        if (bSelected) {
            m_pEditor->insertPlainText("'''"
                                       + m_pEditor->textCursor().selectedText()
                                       + "'''");
        } else {
            sInsertedText = trUtf8("Bold");
            nFormatLength = 3;
            m_pEditor->insertPlainText("'''"
                                       + sInsertedText
                                       + "'''");
        }
    } else if ("italicAct" == sAction) {
        if (bSelected) {
            m_pEditor->insertPlainText("''"
                                       + m_pEditor->textCursor().selectedText()
                                       + "''");
        } else {
            sInsertedText = trUtf8("Italic");
            nFormatLength = 2;
            m_pEditor->insertPlainText("''"
                                       + sInsertedText
                                       + "''");
        }
    } else if ("monotypeAct" == sAction) {
        if (bSelected) {
            m_pEditor->insertPlainText("`"
                                       + m_pEditor->textCursor().selectedText()
                                       + "`");
        } else {
            sInsertedText = trUtf8("Monotype");
            nFormatLength = 1;
            m_pEditor->insertPlainText("`"
                                       + sInsertedText
                                       + "`");
        }
    } else if ("wikilinkAct" == sAction) {
        if (bSelected) {
            m_pEditor->insertPlainText("[:"
                                       + m_pEditor->textCursor().selectedText()
                                       + ":]");
        } else {
            sInsertedText = trUtf8("Site name");
            nFormatLength = 2;
            m_pEditor->insertPlainText("[:"
                                       + sInsertedText
                                       + ":]");
        }
    } else if ("externalLinkAct" == sAction) {
        if (bSelected) {
            m_pEditor->insertPlainText("["
                                       + m_pEditor->textCursor().selectedText()
                                       + "]");
        } else {
            sInsertedText = "http://www.example.org/";
            nFormatLength = 1;
            m_pEditor->insertPlainText("["
                                       + sInsertedText
                                       + "]");
        }
    } else if ("imageAct" == sAction) {
        if (bSelected) {
            m_pEditor->insertPlainText("[[" + m_pTemplates->getTransImage() + "("
                                       + m_pEditor->textCursor().selectedText()
                                       + ")]]");
        } else {
            sInsertedText = trUtf8("Image.png");
            nFormatLength = 3;
            m_pEditor->insertPlainText("[[" + m_pTemplates->getTransImage() + "("
                                       + sInsertedText
                                       + ")]]");
        }
    } else {
        QMessageBox::warning(this, qApp->applicationName(),
                             "Error while inserting syntax element: "
                             "Unknown action");
        qWarning() << "Error while inserting syntax element:"
                   << "UNKNOWN ACTION" << sAction;
    }

    if (!bSelected) {
        QTextCursor textCursor(m_pEditor->textCursor());
        textCursor.setPosition(m_pEditor->textCursor().position() -
                                 sInsertedText.length() - nFormatLength);
        textCursor.setPosition(m_pEditor->textCursor().position() -
                                 nFormatLength, QTextCursor::KeepAnchor);
        m_pEditor->setTextCursor(textCursor);
    }

    m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Insert macro
void CInyokaEdit::insertMacro(const QString &sMenuEntry) {
    // Get indices for links
    QStringList slistTmp = sMenuEntry.split(",");
    QString sTmp("");

    // Check if right number of indices found
    if (slistTmp.size() == 2) {
        QString sName(m_pTemplates->getTPLs()->getElementUrls()[slistTmp[0].toInt()][slistTmp[1].toInt()]);
        sName.remove(".tpl");
        sName.remove(".macro");

        int nIndex = m_pTemplates->getListTplNamesALL().indexOf(sName);
        if (nIndex >= 0) {
            QString sMacro(m_pTemplates->getListTplMacrosALL()[nIndex]);
            sMacro.replace("\\n", "\n");
            int nPlaceholder1(sMacro.indexOf("%%"));
            int nPlaceholder2(sMacro.lastIndexOf("%%"));

            // No text selected
            if (m_pEditor->textCursor().selectedText().isEmpty()) {
                int nCurrentPos =  m_pEditor->textCursor().position();

                // Insert macro
                sMacro.remove("%%");  // Remove placeholder
                m_pEditor->insertPlainText(sMacro);

                // Select placeholder
                if ((nPlaceholder1 != nPlaceholder2)
                        && nPlaceholder1 >= 0
                        && nPlaceholder2 >= 0) {
                    QTextCursor textCursor(m_pEditor->textCursor());
                    textCursor.setPosition(nCurrentPos + nPlaceholder1);
                    textCursor.setPosition(nCurrentPos + nPlaceholder2 -2, QTextCursor::KeepAnchor);
                    m_pEditor->setTextCursor(textCursor);
                }
            } else {
                // Some text is selected
                sTmp = sMacro;
                if ((nPlaceholder1 != nPlaceholder2)
                        && nPlaceholder1 >= 0
                        && nPlaceholder2 >= 0) {
                    sTmp.replace(nPlaceholder1, nPlaceholder2 - nPlaceholder1,
                                 m_pEditor->textCursor().selectedText());
                    m_pEditor->insertPlainText(sTmp.remove("%%"));
                } else {
                    // Problem with placeholder
                    m_pEditor->insertPlainText(sMacro.remove("%%"));
                }
            }
        } else {
            qWarning() << "Unknown macro chosen:" << sName;
        }
    } else {
        // Problem with indices
        qWarning() << "Error while inserting template macro - TPL indice:"
                   << sMenuEntry;
        QMessageBox::warning(this, qApp->applicationName(),
                             "Error while inserting template macro: "
                             "Template indice");
    }

    m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Insert interwiki-link
void CInyokaEdit::insertInterwikiLink(const QString &sMenuEntry) {
    // Get indices for links
    QStringList sTmp = sMenuEntry.split(",");

    // Check if right number of indices found
    if (sTmp.size() == 2) {
        // No text selected
        if (m_pEditor->textCursor().selectedText().isEmpty()) {
            QString sSitename(trUtf8("Sitename"));
            QString sText(trUtf8("Text"));

            // Insert InterWiki-Link
            m_pEditor->insertPlainText("[" + m_pTemplates->getIWLs()->getElementTypes()[sTmp[0].toInt()][sTmp[1].toInt()]
                                       + ":" + sSitename + ":" + sText + "]");

            // Select site name in InterWiki-Link
            QTextCursor textCursor(m_pEditor->textCursor());
            textCursor.setPosition(m_pEditor->textCursor().position() - sSitename.length() - sText.length() - 2);
            textCursor.setPosition(m_pEditor->textCursor().position() - sText.length() - 2, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(textCursor);
        } else {
            // Some text is selected
            // Insert InterWiki-Link with selected text
            m_pEditor->insertPlainText("[" + m_pTemplates->getIWLs()->getElementTypes()[sTmp[0].toInt()][sTmp[1].toInt()]
                                       + ":" + m_pEditor->textCursor().selectedText() + ":]");
        }
    } else {
        // Problem with indices
        QMessageBox::warning(this, qApp->applicationName(),
                             "Error while inserting InterWiki link: "
                             "InterWiki indice");
        qWarning() << "Error while inserting InterWiki link - IWL indice:"
                   << sMenuEntry;
    }

    m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Insert code block
void CInyokaEdit::insertCodeblock(const QString &sCodeStyle) {
    // No text selected
    if (m_pEditor->textCursor().selectedText().isEmpty()) {
        QString sCode("Code");

        // Insert code block
        m_pEditor->insertPlainText("{{{"
                                   + sCodeStyle + "\n"
                                   + sCode
                                   + "\n}}}\n");

        // Select the word "code"
        QTextCursor textCursor(m_pEditor->textCursor());
        textCursor.setPosition(m_pEditor->textCursor().position() -
                               sCode.length() - 5);
        textCursor.setPosition(m_pEditor->textCursor().position() -
                               5, QTextCursor::KeepAnchor);
        m_pEditor->setTextCursor(textCursor);
    } else {
        // Some text is selected
        // Insert code block with selected text
        m_pEditor->insertPlainText("{{{" + sCodeStyle + "\n"
                                   + m_pEditor->textCursor().selectedText()
                                   + "\n}}}\n");
    }

    m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::downloadArticle() {
    if (m_pFileOperations->maybeSave()) {
        m_pDownloadModule->downloadArticle();
    }
}

void CInyokaEdit::displayArticleText(const QString &sArticleText,
                                     const QString &sSitename) {
    m_pEditor->setPlainText(sArticleText);
    m_pFileOperations->setCurrentFile(sSitename);
    m_pEditor->document()->setModified(true);
    this->documentWasModified();

    // Reset scroll position
    m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(0, 0));

    this->previewInyokaPage();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Wait until loading has finished
void CInyokaEdit::loadPreviewFinished(const bool bSuccess) {
    if (bSuccess) {
        m_pTabwidgetRawPreview->setCurrentIndex(
                    m_pTabwidgetRawPreview->indexOf(m_pWebview));
        // Enable / disbale back button
        if (m_pWebview->history()->canGoBack()) {
            m_pUi->goBackBrowserAct->setEnabled(true);
        } else {
            m_pUi->goBackBrowserAct->setEnabled(false);
        }

        // Enable / disable forward button
        if (m_pWebview->history()->canGoForward()) {
            m_pUi->goForwardBrowserAct->setEnabled(true);
        } else {
            m_pUi->goForwardBrowserAct->setEnabled(false);
        }

        // Restore scroll position
        m_pWebview->page()->mainFrame()->setScrollPosition(m_WebviewScrollPosition);
        m_bReloadPreviewBlocked = false;
    } else {
        QMessageBox::warning(this, qApp->applicationName(),
                             trUtf8("Error while loading preview."));
        qWarning() << "Error while loading preview:" << m_sPreviewFile;
    }
}

void CInyokaEdit::changedUrl() {
    // Disable forward / backward button
    m_pUi->goForwardBrowserAct->setEnabled(false);
    m_pUi->goBackBrowserAct->setEnabled(false);
}

void CInyokaEdit::clickedLink(QUrl newUrl) {
    if (!newUrl.toString().contains(m_sPreviewFile)
            && newUrl.isLocalFile()) {
        qDebug() << "Trying to open file:" << newUrl;
        QDesktopServices::openUrl(newUrl);
    } else {
        m_pWebview->load(newUrl);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::updateEditorSettings() {
    QPalette pal;
    pal.setColor(QPalette::Base, m_pSettings->getHighlightBG());
    pal.setColor(QPalette::Text, m_pSettings->getHighlightFG());
    m_pEditor->setPalette(pal);
    m_pEditor->setFont(m_pSettings->getEditorFont());

    m_pParser->updateSettings(m_pSettings->getInyokaUrl(),
                              m_pSettings->getCheckLinks());

    m_pPreviewTimer->stop();
    if (m_pSettings->getPreviewInEditor()
            && m_pSettings->getPreviewAlongside()
            && m_pSettings->getTimedPreview() != 0) {
        m_pPreviewTimer->start(m_pSettings->getTimedPreview() * 1000);
    }

    m_pEditor->updateTextEditorSettings(m_pSettings->getCodeCompletion(),
                                        m_pSettings->getAutoSave());

    m_pDownloadModule->updateSettings(m_pSettings->getAutomaticImageDownload(),
                                      m_pSettings->getInyokaUrl());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CInyokaEdit::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_pEditor && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        // Bug fix for LP: #922808
        Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
        bool isSHIFT(keyMod.testFlag(Qt::ShiftModifier));
        bool isCTRL(keyMod.testFlag(Qt::ControlModifier));

        if (keyEvent->key() == Qt::Key_Right
                && isSHIFT && isCTRL) {
            // CTRL + SHIFT + arrow right
            QTextCursor cursor(m_pEditor->textCursor());
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(cursor);
            return true;
        } else if (keyEvent->key() == Qt::Key_Right
                   && !isSHIFT && isCTRL) {
            // CTRL + arrow right
            m_pEditor->moveCursor(QTextCursor::Right);
            m_pEditor->moveCursor(QTextCursor::EndOfWord);
            return true;
        } else if (keyEvent->key() == Qt::Key_Up
                   && isSHIFT && isCTRL) {
            // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
            QTextCursor cursor(m_pEditor->textCursor());
            cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(cursor);
            return true;
        } else if (keyEvent->key() == Qt::Key_Down
                   && isSHIFT && isCTRL) {
            // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
            QTextCursor cursor(m_pEditor->textCursor());
            cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
            m_pEditor->setTextCursor(cursor);
            return true;
        }
        // --------------------------------------------------------------------
        // --------------------------------------------------------------------
        // CTRL + SHIFT + T (only for debugging)
        else if (keyEvent->key() == Qt::Key_T
                 && isSHIFT && isCTRL) {
            static bool bToggle(false);
            static QTextDocument docBackup("");

            if (!bToggle) {
                docBackup.setPlainText(m_pEditor->document()->toPlainText());
                m_pParser->replaceTemplates(m_pEditor->document());
            } else {
                m_pEditor->setText(docBackup.toPlainText());
            }
            bToggle = !bToggle;
            return true;
        }
        // --------------------------------------------------------------------
        // --------------------------------------------------------------------

        // Reload preview at F5 or defined button if preview alongside
        else if ((Qt::Key_F5 == keyEvent->key()
                  || m_pSettings->getReloadPreviewKey() == keyEvent->key())
                 && (true == m_pSettings->getPreviewAlongside()
                     && true == m_pSettings->getPreviewInEditor())
                 && !m_bReloadPreviewBlocked) {
            m_bReloadPreviewBlocked = true;
            previewInyokaPage();
        }
    }

    // Forward / backward mouse button
    else if (obj == m_pWebview && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::XButton1) {
            m_pWebview->back();
        } else if (mouseEvent->button() == Qt::XButton2) {
            m_pWebview->forward();
        }
    }

    // Else
    return QObject::eventFilter(obj, event);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::loadPlugins() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_PluginMenuEntries.clear();
    m_PluginToolbarEntries.clear();

    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    qDebug() << "Plugins folder:" << pluginsDir.absolutePath();

    foreach (QString sFile, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(sFile));
        QObject *plugin = loader.instance();
        if (plugin) {
            IEditorPlugin *piPlugin = qobject_cast<IEditorPlugin *>(plugin);

            if (piPlugin) {
                qDebug() << "Loaded plugin:" << sFile;
                piPlugin->initPlugin(this, m_pEditor, m_UserDataDir);
                qApp->installTranslator(piPlugin->getPluginTranslator(
                                            m_pSettings->getGuiLanguage()));

                QIcon icon(piPlugin->getMenuIcon());
                QString sMenu(piPlugin->getMenuEntry());

                if (!sMenu.isEmpty()) {  // Add to menue if entry available
                    m_PluginMenuEntries << new QAction(piPlugin->getMenuIcon(),
                                                       piPlugin->getMenuEntry(),
                                                       this);
                    connect(m_PluginMenuEntries.last(), SIGNAL(triggered()),
                            plugin, SLOT(executePlugin()));
                }
                if (!icon.isNull()) {  // Add to toolbar if icon available
                    m_PluginToolbarEntries << new QAction(piPlugin->getMenuIcon(),
                                                          piPlugin->getMenuEntry(),
                                                          this);
                    connect(m_PluginToolbarEntries.last(), SIGNAL(triggered()),
                            plugin, SLOT(executePlugin()));
                }
            }
        }
    }

    // Add plugins to plugin toolbar and tools menu
    m_pUi->pluginsBar->addActions(m_PluginToolbarEntries);
    if (m_pUi->toolsMenu->actions().size() > 0) {
        QAction *separator = new QAction(this);
        separator->setSeparator(true);
        m_PluginMenuEntries << separator;
        m_pUi->toolsMenu->insertActions(m_pUi->toolsMenu->actions().first(),
                                        m_PluginMenuEntries);
    } else {
        m_pUi->toolsMenu->addActions(m_PluginMenuEntries);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Delete images in temp. download folder (images downloaded with articles)
void CInyokaEdit::deleteTempImages() {
    int nRet = QMessageBox::question(this, qApp->applicationName(),
                                     trUtf8("Do you really want to delete all "
                                            "images downloaded with articles?"),
                                     QMessageBox::Yes | QMessageBox::No);

    if (QMessageBox::Yes== nRet) {
        // Remove all files in current folder
        QFileInfoList fiListFiles = m_tmpPreviewImgDir.entryInfoList(
                    QDir::NoDotAndDotDot | QDir::Files);
        foreach (QFileInfo fi, fiListFiles) {
            if (!m_tmpPreviewImgDir.remove(fi.fileName())) {
                // Problem while removing
                QMessageBox::warning(this, qApp->applicationName(),
                                     trUtf8("Could not delete file: ")
                                     + fi.fileName());
                qWarning() << "Could not delete files:" <<
                              fi.fileName();
                return;
            }
        }
        QMessageBox::information(this, qApp->applicationName(),
                                 trUtf8("Images successfully deleted."));
    } else {
        return;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Set modified flag for window
void CInyokaEdit::documentWasModified() {
    m_pTabwidgetRawPreview->setCurrentIndex(
                m_pTabwidgetRawPreview->indexOf(m_pEditor));
    this->setWindowModified(m_pEditor->document()->isModified());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::syncScrollbarsEditor() {
    if (!m_bWebviewScrolling
            && true == m_pSettings->getSyncScrollbars()
            &&true == m_pSettings->getPreviewAlongside()
            && true == m_pSettings->getPreviewInEditor()) {
        int nSizeEditorBar = m_pEditor->verticalScrollBar()->maximum();
        int nSizeWebviewBar = m_pWebview->page()->mainFrame()->scrollBarMaximum(
                    Qt::Vertical);
        float nRatio = static_cast<float>(nSizeWebviewBar) / nSizeEditorBar;

        m_bEditorScrolling = true;
        m_pWebview->page()->mainFrame()->setScrollPosition(
                    QPoint(0, m_pEditor->verticalScrollBar()->sliderPosition() * nRatio));
        m_bEditorScrolling = false;
    }
}

// ----------------------------------------------------------------------------

void CInyokaEdit::syncScrollbarsWebview() {
    if (!m_bEditorScrolling
            && true == m_pSettings->getSyncScrollbars()
            && true == m_pSettings->getPreviewAlongside()
            && true == m_pSettings->getPreviewInEditor()) {
        int nSizeEditorBar = m_pEditor->verticalScrollBar()->maximum();
        int nSizeWebviewBar = m_pWebview->page()->mainFrame()->scrollBarMaximum(
                    Qt::Vertical);
        float nRatio = static_cast<float>(nSizeEditorBar) / nSizeWebviewBar;

        m_bWebviewScrolling = true;
        m_pEditor->verticalScrollBar()->setSliderPosition(
                    m_pWebview->page()->mainFrame()->scrollPosition().y() * nRatio);
        m_bWebviewScrolling = false;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::showSyntaxOverview() {
    QDialog* dialog = new QDialog(this, this->windowFlags()
                                  & ~Qt::WindowContextHelpButtonHint);
    QGridLayout* layout = new QGridLayout(dialog);
    QWebView* webview = new QWebView();
    QTextDocument* pTextDocument = new QTextDocument(this);

    QFile OverviewFile("");
    // Path from normal installation
    if (QFile::exists("/usr/share/"
                      + qApp->applicationName().toLower()
                      + "/templates/"
                      + m_pSettings->getTemplateLanguage()
                      + "/SyntaxOverview") && !bDEBUG) {
        OverviewFile.setFileName("/usr/share/"
                                 + qApp->applicationName().toLower()
                                 + "/templates/" +
                                 m_pSettings->getTemplateLanguage()
                                 + "/SyntaxOverview");
    } else {
        // No installation: Use app path
        OverviewFile.setFileName(qApp->applicationDirPath()
                                 + "/templates/" +
                                 m_pSettings->getTemplateLanguage()
                                 + "/SyntaxOverview");
    }

    QTextStream in(&OverviewFile);
    if (!OverviewFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(0, "Warning",
                             trUtf8("Could not open syntax overview file!"));
        qWarning() << "Could not open syntax overview file:"
                   << OverviewFile.fileName();
        return;
    }
    pTextDocument->setPlainText(in.readAll());
    OverviewFile.close();

    QString sRet(m_pParser->genOutput("", pTextDocument));
    sRet.remove(QRegExp("<h1 class=\"pagetitle\">.*</h1>"));
    sRet.remove(QRegExp("<p class=\"meta\">.*</p>"));
    sRet.replace("</style>", "#page table{margin:0px;}</style>");
    pTextDocument->setPlainText(sRet);

    layout->setMargin(2);
    layout->setSpacing(0);
    layout->addWidget(webview);
    dialog->setWindowTitle(trUtf8("Syntax overview"));

    webview->setHtml(pTextDocument->toPlainText(),
                     QUrl::fromLocalFile(m_UserDataDir.absolutePath() + "/"));
    dialog->show();
}

// ----------------------------------------------------------------------------

// Close event (File -> Close or X)
void CInyokaEdit::closeEvent(QCloseEvent *event) {
    if (m_pFileOperations->maybeSave()) {
        if (true == m_pSettings->getPreviewAlongside()
                && true == m_pSettings->getPreviewInEditor()) {
            m_pSettings->writeSettings(saveGeometry(), saveState(),
                                       m_pWidgetSplitter->saveState());
        } else {
            m_pSettings->writeSettings(saveGeometry(), saveState());
        }
        event->accept();
    } else {
        event->ignore();
    }
}
