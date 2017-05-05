/**
 * \file CInyokaEdit.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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

#ifdef USEQTWEBKIT
#include <QtWebKitWidgets/QWebView>
#include <QWebFrame>
#include <QWebHistory>
#else
#include <QWebEngineView>
#include <QWebEngineHistory>
#endif

#include "./CInyokaEdit.h"
#include "ui_CInyokaEdit.h"

CInyokaEdit::CInyokaEdit(const QDir &userDataDir, const QDir &sharePath,
                         QWidget *parent)
  : QMainWindow(parent),
    m_pUi(new Ui::CInyokaEdit),
    m_UserDataDir(userDataDir),
    m_tmpPreviewImgDir(m_UserDataDir.absolutePath() + "/tmpImages"),
    m_sPreviewFile(m_UserDataDir.absolutePath() + "/tmpinyoka.html"),
    m_sSharePath(sharePath.absolutePath()),
    m_bOpenFileAfterStart(false),
    m_bReloadPreviewBlocked(false){
  qDebug() << "Calling" << Q_FUNC_INFO;
  if (qApp->arguments().contains("--debug")) {
    qWarning() << "DEBUG is enabled!";
  }

  m_pUi->setupUi(this);

  if (!sharePath.exists()) {
    QMessageBox::warning(0, "Warning", "App share folder not found!");
    qWarning() << "Share folder does not exist:" << m_sSharePath;
    exit(-1);
  }

  // Create folder for downloaded article images
  if (!m_tmpPreviewImgDir.exists()) {
    // Create folder including possible parent directories (mkPATH)!
    m_tmpPreviewImgDir.mkpath(m_tmpPreviewImgDir.absolutePath());
  }

  QString sFile("");
  if (qApp->arguments().size() > 1) {
    for (int i = 1; i < qApp->arguments().size(); i++) {
      if (!qApp->arguments()[i].startsWith('-')) {
        m_bOpenFileAfterStart = true;  // Checked in setupEditor()
        sFile = qApp->arguments()[i];
        break;
      }
    }
  }

  // After definition of StylesAndImagesDir AND m_tmpPreviewImgDir!
  this->createObjects();
  this->setupEditor();
  this->createActions();
  this->createMenus();
  this->createToolBars();

  // Download style files if preview/styles/imgages folders doesn't
  // exist (see QDesktopServices::DataLocation)
  if (!m_UserDataDir.exists() ||
      !QDir(m_UserDataDir.absolutePath() + "/community/" +
            m_pSettings->getInyokaCommunity() + "/img").exists() ||
      !QDir(m_UserDataDir.absolutePath() + "/community/" +
            m_pSettings->getInyokaCommunity() + "/styles").exists() ||
      !QDir(m_UserDataDir.absolutePath() + "/community/" +
            m_pSettings->getInyokaCommunity() + "/Wiki").exists()) {
    // Create folder because user may not start download.
    // Folder is needed for preview.
    m_UserDataDir.mkpath(m_UserDataDir.absolutePath() + "/community");
#if !defined _WIN32
    m_pDownloadModule->loadInyokaStyles();
#endif
  }

  if (CUtils::getOnlineState() && m_pSettings->getWindowsCheckUpdate()) {
    m_pUtils->checkWindowsUpdate();
  }

  // Load file via command line
  if (m_bOpenFileAfterStart) {
    m_pFileOperations->loadFile(sFile, true);
  }

  m_pPlugins->loadPlugins();
  this->updateEditorSettings();
  this->deleteAutoSaveBackups();
  m_pCurrentEditor->setFocus();
}

CInyokaEdit::~CInyokaEdit() {
  if (NULL != m_pUi) {
    delete m_pUi;
  }
  m_pUi = NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::createObjects() {
  qDebug() << "Calling" << Q_FUNC_INFO;

  m_pSettings = new CSettings(this, m_sSharePath);
  qDebug() << "Inyoka Community:" << m_pSettings->getInyokaCommunity();
  if (m_pSettings->getInyokaCommunity().isEmpty() ||
      !QDir(m_sSharePath + "/community/" + m_pSettings->getInyokaCommunity()).exists()) {
    qCritical() << "No Inyoka community files found / installed!";
    qCritical() << "Community path:" << m_sSharePath + "/community/" +
                   m_pSettings->getInyokaCommunity();
    QMessageBox::critical(this, qApp->applicationName(),
                          trUtf8("No Inyoka community files found / installed!\n"
                                 "Please check your installation and restart "
                                 "the application."));
    exit(-2);
  }

  // Has to be created before parser
  m_pTemplates = new CTemplates(m_pSettings->getInyokaCommunity(),
                                m_sSharePath, m_UserDataDir.absolutePath());

  m_pDownloadModule = new CDownload(this, m_UserDataDir.absolutePath(),
                                    m_tmpPreviewImgDir.absolutePath(),
                                    m_sSharePath);

  m_pParser = new CParser(m_tmpPreviewImgDir,
                          m_pSettings->getInyokaUrl(),
                          m_pSettings->getCheckLinks(),
                          m_pTemplates);

  m_pDocumentTabs = new QTabWidget;
  m_pDocumentTabs->setTabPosition(QTabWidget::North);
  m_pDocumentTabs->setTabsClosable(true);
  m_pDocumentTabs->setDocumentMode(true);
  // Attention: Currently tab order is fixed (same as m_pListEditors)
  m_pDocumentTabs->setMovable(false);

  m_pFileOperations = new CFileOperations(this, m_pDocumentTabs, m_pSettings,
                                          m_sPreviewFile,
                                          m_UserDataDir.absolutePath(),
                                          m_pTemplates->getListTplMacrosALL());
  m_pCurrentEditor = m_pFileOperations->getCurrentEditor();

  connect(m_pFileOperations, SIGNAL(callPreview()),
          this, SLOT(previewInyokaPage()));
  connect(m_pFileOperations, SIGNAL(modifiedDoc(bool)),
          this, SLOT(setWindowModified(bool)));
  connect(m_pFileOperations, SIGNAL(changedCurrentEditor()),
          this, SLOT(setCurrentEditor()));

  m_pPlugins = new CPlugins(this, m_pCurrentEditor, m_pSettings->getGuiLanguage(),
                            m_pSettings->getDisabledPlugins(), m_UserDataDir,
                            m_sSharePath);
  connect(m_pPlugins,
          SIGNAL(addMenuToolbarEntries(QList<QAction*>, QList<QAction*>)),
          this,
          SLOT(addPluginsButtons(QList<QAction*>, QList<QAction*>)));
  connect(m_pPlugins,
          SIGNAL(availablePlugins(QList<IEditorPlugin*>, QList<QObject*>)),
          m_pSettings,
          SIGNAL(availablePlugins(QList<IEditorPlugin*>, QList<QObject*>)));

  this->setCurrentEditor();

  // TODO: Find alternative for QWebEngine
#ifdef USEQTWEBKIT
  m_pWebview = new QWebView(this);
  connect(m_pWebview->page(), SIGNAL(scrollRequested(int, int, QRect)),
          this, SLOT(syncScrollbarsWebview()));
  connect(m_pFileOperations, SIGNAL(movedEditorScrollbar()),
          this, SLOT(syncScrollbarsEditor()));
#else
  m_pWebview = new QWebEngineView(this);
#endif
  m_pWebview->installEventFilter(this);

  m_bEditorScrolling = false;
  m_bWebviewScrolling = false;

  m_pUtils = new CUtils(this);
  connect(m_pUtils, SIGNAL(setWindowsUpdateCheck(bool)),
          m_pSettings, SLOT(setWindowsCheckUpdate(bool)));

  m_pPreviewTimer = new QTimer(this);
}

// ----------------------------------------------------------------------------

void CInyokaEdit::setCurrentEditor() {
  m_pCurrentEditor = m_pFileOperations->getCurrentEditor();
  m_pPlugins->setCurrentEditor(m_pCurrentEditor);
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

  m_pFrameLayout = new QBoxLayout(QBoxLayout::LeftToRight);
  m_pFrameLayout->addWidget(m_pWebview);

  connect(m_pWebview, SIGNAL(loadFinished(bool)),
          this, SLOT(loadPreviewFinished(bool)));

  m_pWidgetSplitter = new QSplitter;

  m_pWidgetSplitter->addWidget(m_pDocumentTabs);
  m_pWidgetSplitter->addWidget(m_pWebview);
  setCentralWidget(m_pWidgetSplitter);
  m_pWidgetSplitter->restoreState(m_pSettings->getSplitterState());

  this->updateEditorSettings();
  connect(m_pSettings, SIGNAL(updateEditorSettings()),
          this, SLOT(updateEditorSettings()));
  connect(m_pFileOperations, SIGNAL(newEditor()),
          this, SLOT(updateEditorSettings()));

  // Show an empty website after start
  if (!m_bOpenFileAfterStart) {
    this->previewInyokaPage();
  }

  this->setUnifiedTitleAndToolBarOnMac(true);

  connect(m_pDownloadModule, SIGNAL(sendArticleText(QString, QString)),
          this, SLOT(displayArticleText(QString, QString)));

  // Restore window and toolbar settings
  // Settings have to be restored after toolbars are created!
  this->restoreGeometry(m_pSettings->getWindowGeometry());
  // Restore toolbar position etc.
  this->restoreState(m_pSettings->getWindowState());

  m_pUi->aboutAct->setText(
        m_pUi->aboutAct->text() + " " + qApp->applicationName());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void CInyokaEdit::createActions() {
  qDebug() << "Calling" << Q_FUNC_INFO;

  // File menu
  // New file
  m_pUi->newAct->setShortcuts(QKeySequence::New);
  m_pUi->newAct->setIcon(QIcon::fromTheme(
                           "document-new", QIcon(":/images/document-new.png")));
  connect(m_pUi->newAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(newFile()));
  // Open file
  m_pUi->openAct->setShortcuts(QKeySequence::Open);
  m_pUi->openAct->setIcon(QIcon::fromTheme(
                            "document-open", QIcon(":/images/document-open.png")));
  connect(m_pUi->openAct, SIGNAL(triggered()),
          this, SLOT(openFile()));
  // Save file
  m_pUi->saveAct->setShortcuts(QKeySequence::Save);
  m_pUi->saveAct->setIcon(QIcon::fromTheme(
                            "document-save", QIcon(":/images/document-save.png")));
  connect(m_pUi->saveAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(save()));
  // Save file as...
  m_pUi->saveAsAct->setShortcuts(QKeySequence::SaveAs);
  m_pUi->saveAsAct->setIcon(QIcon::fromTheme(
                              "document-save-as", QIcon(":/images/document-save-as.png")));
  connect(m_pUi->saveAsAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(saveAs()));
  // Print preview
  m_pUi->printPreviewAct->setShortcut(QKeySequence::Print);
  m_pUi->printPreviewAct->setIcon(QIcon::fromTheme(
                                    "document-print", QIcon(":/images/document-print.png")));
  connect(m_pUi->printPreviewAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(printPreview()));
#if QT_VERSION >= 0x050600
  m_pUi->printPreviewAct->setEnabled(false);
  // TODO: Check print functionality again with Qt 5.7
#endif

  // Exit application
  m_pUi->exitAct->setShortcuts(QKeySequence::Quit);
  m_pUi->exitAct->setIcon(QIcon::fromTheme("application-exit"));
  connect(m_pUi->exitAct, SIGNAL(triggered()),
          this, SLOT(close()));

  // ------------------------------------------------------------------------
  // EDIT MENU

  // Find
  m_pUi->searchAct->setShortcuts(QKeySequence::Find);
  m_pUi->searchAct->setIcon(QIcon::fromTheme(
                              "edit-find", QIcon(":/images/edit-find.png")));
  connect(m_pUi->searchAct, SIGNAL(triggered()),
          m_pFileOperations, SIGNAL(triggeredFind()));
  // Replace
  m_pUi->replaceAct->setShortcuts(QKeySequence::Replace);
  m_pUi->replaceAct->setIcon(QIcon::fromTheme(
                               "edit-find-replace", QIcon(":/images/edit-find-replace.png")));
  connect(m_pUi->replaceAct, SIGNAL(triggered()),
          m_pFileOperations, SIGNAL(triggeredReplace()));
  // Find next
  m_pUi->findNextAct->setShortcuts(QKeySequence::FindNext);
  m_pUi->findNextAct->setIcon(QIcon::fromTheme(
                                "go-down", QIcon(":/images/go-down.png")));
  connect(m_pUi->findNextAct, SIGNAL(triggered()),
          m_pFileOperations, SIGNAL(triggeredFindNext()));
  // Find previous
  m_pUi->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
  m_pUi->findPreviousAct->setIcon(QIcon::fromTheme(
                                    "go-up", QIcon(":/images/go-up.png")));
  connect(m_pUi->findPreviousAct, SIGNAL(triggered()),
          m_pFileOperations, SIGNAL(triggeredFindPrevious()));

  // Cut
  m_pUi->cutAct->setShortcuts(QKeySequence::Cut);
  m_pUi->cutAct->setIcon(QIcon::fromTheme(
                           "edit-cut", QIcon(":/images/edit-cut.png")));
  connect(m_pUi->cutAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(cut()));
  // Copy
  m_pUi->copyAct->setShortcuts(QKeySequence::Copy);
  m_pUi->copyAct->setIcon(QIcon::fromTheme(
                            "edit-copy", QIcon(":/images/edit-copy.png")));
  connect(m_pUi->copyAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(copy()));
  // Paste
  m_pUi->pasteAct->setShortcuts(QKeySequence::Paste);
  m_pUi->pasteAct->setIcon(QIcon::fromTheme(
                             "edit-paste", QIcon(":/images/edit-paste.png")));
  connect(m_pUi->pasteAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(paste()));

  connect(m_pFileOperations, SIGNAL(copyAvailable(bool)),
          m_pUi->cutAct, SLOT(setEnabled(bool)));
  connect(m_pFileOperations, SIGNAL(copyAvailable(bool)),
          m_pUi->copyAct, SLOT(setEnabled(bool)));
  m_pUi->cutAct->setEnabled(false);
  m_pUi->copyAct->setEnabled(false);

  // Undo
  m_pUi->undoAct->setShortcuts(QKeySequence::Undo);
  m_pUi->undoAct->setIcon(QIcon::fromTheme(
                            "edit-undo", QIcon(":/images/edit-undo.png")));
  connect(m_pUi->undoAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(undo()));
  // Redo
  m_pUi->redoAct->setShortcuts(QKeySequence::Redo);
  m_pUi->redoAct->setIcon(QIcon::fromTheme(
                            "edit-redo", QIcon(":/images/edit-redo.png")));
  connect(m_pUi->redoAct, SIGNAL(triggered()),
          m_pFileOperations, SLOT(redo()));

  connect(m_pFileOperations, SIGNAL(undoAvailable(bool)),
          m_pUi->undoAct, SLOT(setEnabled(bool)));
  connect(m_pFileOperations, SIGNAL(redoAvailable(bool)),
          m_pUi->redoAct, SLOT(setEnabled(bool)));
  connect(m_pFileOperations, SIGNAL(undoAvailable2(bool)),
          m_pUi->undoAct, SLOT(setEnabled(bool)));
  connect(m_pFileOperations, SIGNAL(redoAvailable2(bool)),
          m_pUi->redoAct, SLOT(setEnabled(bool)));
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
  m_pUi->preferencesAct->setIcon(
        QIcon::fromTheme("preferences-system",
                         QIcon(":/images/preferences-system.png")));
  connect(m_pUi->preferencesAct, SIGNAL(triggered()),
          m_pSettings, SIGNAL(showSettingsDialog()));

  // ------------------------------------------------------------------------

  // Show html preview
  m_pUi->previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
  connect(m_pUi->previewAct, SIGNAL(triggered()),
          this, SLOT(previewInyokaPage()));

  // Download Inyoka article
  connect(m_pUi->downloadArticleAct, SIGNAL(triggered()),
          m_pDownloadModule, SLOT(downloadArticle()));

  // Upload Inyoka article
  connect(m_pUi->uploadArticleAct, SIGNAL(triggered()),
          m_pFileOperations, SIGNAL(triggeredUpload()));

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
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->boldAct, "boldAct");
  connect(m_pUi->boldAct, SIGNAL(triggered()),
          m_pSigMapMainEditorToolbar, SLOT(map()));

  // Insert italic element
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->italicAct, "italicAct");
  connect(m_pUi->italicAct, SIGNAL(triggered()),
          m_pSigMapMainEditorToolbar, SLOT(map()));

  // Insert monotype element
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->monotypeAct, "monotypeAct");
  connect(m_pUi->monotypeAct, SIGNAL(triggered()),
          m_pSigMapMainEditorToolbar, SLOT(map()));

  // Insert unsorted list
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->listAct, "listAct");
  connect(m_pUi->listAct, SIGNAL(triggered()),
          m_pSigMapMainEditorToolbar, SLOT(map()));

  // Insert sorted list
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->numberedListAct, "numberedListAct");
  connect(m_pUi->numberedListAct, SIGNAL(triggered()),
          m_pSigMapMainEditorToolbar, SLOT(map()));

  // Insert wiki link
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->wikilinkAct, "wikilinkAct");
  connect(m_pUi->wikilinkAct, SIGNAL(triggered()),
          m_pSigMapMainEditorToolbar, SLOT(map()));

  // Insert extern link
  m_pSigMapMainEditorToolbar->setMapping(m_pUi->externalLinkAct, "externalLinkAct");
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
  sListHighlightText << trUtf8("Raw text")
                     << trUtf8("Code without highlighting")
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
                         m_sSharePath + "/community/" +
                         m_pSettings->getInyokaCommunity() + "/templates/",
                         m_TplActions, m_pTemplates->getTPLs());

  connect(m_pSigMapTemplates, SIGNAL(mapped(QString)),
          this, SLOT(insertMacro(QString)));

  // ------------------------------------------------------------------------
  // INTERWIKI LINKS MENU

  m_pSigMapInterWikiLinks = new QSignalMapper(this);

  this->createXmlActions(m_pSigMapInterWikiLinks,
                         m_sSharePath + "/community/" +
                         m_pSettings->getInyokaCommunity() + "/iWikiLinks/",
                         m_iWikiLinksActions, m_pTemplates->getIWLs());

  connect(m_pSigMapInterWikiLinks, SIGNAL(mapped(QString)),
          this, SLOT(insertInterwikiLink(QString)));

  // ------------------------------------------------------------------------
  // ABOUT MENU

  // Show syntax overview
  connect(m_pUi->showSyntaxOverviewAct, SIGNAL(triggered()),
          this, SLOT(showSyntaxOverview()));

  // Report a bug
  connect(m_pUi->reportBugAct, SIGNAL(triggered()),
          m_pUtils, SLOT(reportBug()));

  // Open about windwow
  m_pUi->aboutAct->setIcon(QIcon::fromTheme(
                             "help-about", QIcon(":/images/help-browser.png")));
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

  for (int i = 0; i < pXmlMenu->getGrouplist().size(); i++) {
    listActions.append(emptyActionList);
    for (int j = 0; j < pXmlMenu->getElementNames()[i].size(); j++) {
      listActions[i].append(
            new QAction(QIcon(sIconPath + pXmlMenu->getElementIcons()[i][j]),
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

  QDir articleTemplateDir(m_sSharePath + "/community/" +
                          m_pSettings->getInyokaCommunity() +
                          "/templates/articles");
  QDir userArticleTemplateDir(m_UserDataDir.absolutePath() + "/community/" +
                              m_pSettings->getInyokaCommunity() +
                              "/templates/articles");

  // File menu (new from template)
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
          m_pFileOperations->m_pSigMapOpenTemplate, SIGNAL(mapped(QString)));

  if (0 == m_OpenTemplateFilesActions.size()) {
    m_pUi->fileMenuFromTemplate->setDisabled(true);
  }

  // File menu (recent opened files)
  m_pUi->fileMenuLastOpened->setIcon(QIcon::fromTheme("document-open-recent"));
  m_pUi->fileMenuLastOpened->addActions(m_pFileOperations->getLastOpenedFiles());
  if (0 == m_pSettings->getRecentFiles().size()) {
    m_pUi->fileMenuLastOpened->setEnabled(false);
  }
  connect(m_pFileOperations, SIGNAL(setMenuLastOpenedEnabled(bool)),
          m_pUi->fileMenuLastOpened, SLOT(setEnabled(bool)));

  // Insert TPL menu
  m_pTplMenu = new QMenu(m_pTemplates->getTPLs()->getMenuName(), this);
  this->insertXmlMenu(m_pTplMenu, m_TplGroups,
                      m_sSharePath + "/community/" +
                      m_pSettings->getInyokaCommunity() + "/templates/",
                      m_TplActions, m_pTemplates->getTPLs(),
                      m_pUi->toolsMenu->menuAction());

  // Insert IWL menu
  m_piWikiMenu = new QMenu(m_pTemplates->getIWLs()->getMenuName(), this);
  this->insertXmlMenu(m_piWikiMenu, m_iWikiGroups,
                      m_sSharePath + "/community/" +
                      m_pSettings->getInyokaCommunity() + "/iWikiLinks/",
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

  for (int i = 0; i < pXmlMenu->getGrouplist().size(); i++) {
    pMenuGroup.append(pMenu->addMenu(QIcon(sIconPath +
                                           pXmlMenu->getGroupIcons()[i]),
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
  m_pUi->goBackBrowserAct->setIcon(
        QIcon::fromTheme("go-previous",
                         QIcon(":/images/go-previous.png")));
  connect(m_pUi->goBackBrowserAct, SIGNAL(triggered()),
          m_pWebview, SLOT(back()));
  m_pUi->goForwardBrowserAct->setIcon(
        QIcon::fromTheme("go-next",
                         QIcon(":/images/go-next.png")));
  connect(m_pUi->goForwardBrowserAct, SIGNAL(triggered()),
          m_pWebview, SLOT(forward()));
  m_pUi->reloadBrowserAct->setIcon(
        QIcon::fromTheme("view-refresh",
                         QIcon(":/images/view-refresh.png")));
  connect(m_pUi->reloadBrowserAct, SIGNAL(triggered()),
          m_pWebview, SLOT(reload()));

  connect(m_pWebview, SIGNAL(urlChanged(QUrl)),
          this, SLOT(changedUrl()));

  // TODO: Find alternative for QWebEngine. QWebEngineUrlRequestInterceptor ?
#ifdef USEQTWEBKIT
  m_pWebview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(m_pWebview, SIGNAL(linkClicked(QUrl)),
          this, SLOT(clickedLink(QUrl)));
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Add plugins to plugin toolbar and tools menu
void CInyokaEdit::addPluginsButtons(QList<QAction *> ToolbarEntries,
                                    QList<QAction *> MenueEntries) {
  m_pUi->pluginsBar->addActions(ToolbarEntries);
  if (m_pUi->toolsMenu->actions().size() > 0) {
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    MenueEntries << separator;
    m_pUi->toolsMenu->insertActions(m_pUi->toolsMenu->actions().first(),
                                    MenueEntries);
  } else {
    m_pUi->toolsMenu->addActions(MenueEntries);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::openFile() {
  // Reset scroll position
#ifdef USEQTWEBKIT
  m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
#else
  m_pWebview->scroll(0, 0);
#endif
  m_pFileOperations->open();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Call parser
void CInyokaEdit::previewInyokaPage() {
  m_pWebview->history()->clear();  // Clear history (clicked links)

  QString sRetHTML("");
  sRetHTML = m_pParser->genOutput(m_pFileOperations->getCurrentFile(),
                                  m_pCurrentEditor->document());

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

  // Store scroll position
  // TODO: Find alternative for WebEngine
#ifdef USEQTWEBKIT
  m_WebviewScrollPosition =
      m_pWebview->page()->mainFrame()->scrollPosition();
#else
  m_WebviewScrollPosition = QPoint(0, 0);
#endif

  m_pWebview->load(
        QUrl::fromLocalFile(
          QFileInfo(tmphtmlfile).absoluteFilePath()));
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
    if (!m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
      m_pCurrentEditor->insertPlainText(
            sHeadTag + " " + m_pCurrentEditor->textCursor().selectedText() + " " + sHeadTag);
    } else {
      // Select text sHeadline if no text was selected
      m_pCurrentEditor->insertPlainText(
            sHeadTag + " " + sHeadline + " " + sHeadTag);

      QTextCursor textCursor(m_pCurrentEditor->textCursor());
      textCursor.setPosition(
            m_pCurrentEditor->textCursor().position() - sHeadline.length() - nSelection);
      textCursor.setPosition(
            m_pCurrentEditor->textCursor().position() - nSelection,
            QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(textCursor);
    }
  }

  // Reset selection
  m_pHeadlineBox->setCurrentIndex(0);

  m_pCurrentEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Macro (combobox in toolbar)
void CInyokaEdit::insertDropDownTextmacro(const int nSelection) {
  if (nSelection != 0 && nSelection != 1) {
    QString sTmp("");
    QString sName(m_pTemplates->getDropTPLs()->getElementUrls()[0][nSelection - 2]);
    sName.remove(".tpl");
    sName.remove(".macro");

    int nIndex = m_pTemplates->getListTplNamesALL().indexOf(sName);
    if (nIndex >= 0) {
      QString sMacro(m_pTemplates->getListTplMacrosALL()[nIndex]);
      sMacro.replace("\\n", "\n");
      int nPlaceholder1 = sMacro.indexOf("%%");
      int nPlaceholder2 = sMacro.lastIndexOf("%%");

      // No text selected
      if (m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
        int nCurrentPos = m_pCurrentEditor->textCursor().position();

        // Insert macro
        sMacro.remove("%%");  // Remove placeholder
        m_pCurrentEditor->insertPlainText(sMacro);

        // Select placeholder
        if ((nPlaceholder1 != nPlaceholder2)
            && nPlaceholder1 >= 0
            && nPlaceholder2 >= 0) {
          QTextCursor textCursor(m_pCurrentEditor->textCursor());
          textCursor.setPosition(nCurrentPos + nPlaceholder1);
          textCursor.setPosition(nCurrentPos + nPlaceholder2 - 2,
                                 QTextCursor::KeepAnchor);
          m_pCurrentEditor->setTextCursor(textCursor);
        }
      } else {
        // Some text is selected
        sTmp = sMacro;
        if ((nPlaceholder1 != nPlaceholder2)
            && nPlaceholder1 >= 0
            && nPlaceholder2 >= 0) {
          sTmp.replace(nPlaceholder1, nPlaceholder2 - nPlaceholder1,
                       m_pCurrentEditor->textCursor().selectedText());
          m_pCurrentEditor->insertPlainText(sTmp.remove("%%"));
        } else {
          // Problem with placeholder
          m_pCurrentEditor->insertPlainText(sMacro.remove("%%"));
        }
      }
    } else {
      qWarning()  << "Unknown macro chosen:" << sName;
    }
  }

  m_pTextmacrosBox->setCurrentIndex(0);
  m_pCurrentEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Text format (combobox in toolbar)
void CInyokaEdit::insertDropDownTextformat(const int nSelection) {
  QString sInsert(m_pCurrentEditor->textCursor().selectedText());

  if (nSelection != 0 && nSelection != 1) {
    quint16 nFormatLength(0);

    // -1 because of separator (considered as "item")
    switch (nSelection-1) {
      default:
      case 1:  // Folders
        if (sInsert.isEmpty()) {
          sInsert = trUtf8("Folders");
          nFormatLength = 3;
        }
        m_pCurrentEditor->insertPlainText("'''" + sInsert + "'''");
        break;
      case 2:  // Menus
        if (sInsert.isEmpty()) {
          sInsert = trUtf8("Menu -> sub menu -> menu entry");
          nFormatLength = 3;
        }
        m_pCurrentEditor->insertPlainText("''\"" + sInsert + "\"''");
        break;
      case 3:  // Files
        if (sInsert.isEmpty()) {
          sInsert = trUtf8("Files");
          nFormatLength = 3;
        }
        m_pCurrentEditor->insertPlainText("'''" + sInsert + "'''");
        break;
      case 4:  // Commands
        if (sInsert.isEmpty()) {
          sInsert = trUtf8("Command");
          nFormatLength = 1;
        }
        m_pCurrentEditor->insertPlainText("`" + sInsert + "`");
        break;
    }

    // Reset selection
    m_pTextformatBox->setCurrentIndex(0);

    if (nFormatLength > 0) {
      QTextCursor textCursor(m_pCurrentEditor->textCursor());
      textCursor.setPosition(m_pCurrentEditor->textCursor().position() -
                             sInsert.length() - nFormatLength);
      textCursor.setPosition(m_pCurrentEditor->textCursor().position() -
                             nFormatLength, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(textCursor);
    }

    m_pCurrentEditor->setFocus();
  }
}

// ----------------------------------------------------------------------------

// Insert main syntax element from toolbar (bold, italic, ...)
void CInyokaEdit::insertMainEditorButtons(const QString &sAction) {
  QString sInsert(m_pCurrentEditor->textCursor().selectedText());
  quint16 nFormatLength(65530);

  if ("boldAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("Bold");
      nFormatLength = 3;
    }
    m_pCurrentEditor->insertPlainText("'''" + sInsert + "'''");
  } else if ("italicAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("Italic");
      nFormatLength = 2;
    }
    m_pCurrentEditor->insertPlainText("''" + sInsert + "''");
  } else if ("monotypeAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("Monotype");
      nFormatLength = 1;
    }
    m_pCurrentEditor->insertPlainText("`" + sInsert + "`");
  } else if ("listAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("List");
      nFormatLength = 0;
    }
    m_pCurrentEditor->insertPlainText(" * " + sInsert);
  } else if ("numberedListAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("Numbered list");
      nFormatLength = 0;
    }
    m_pCurrentEditor->insertPlainText(" 1. " + sInsert);
  } else if ("wikilinkAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("Site name");
      nFormatLength = 2;
    }
    m_pCurrentEditor->insertPlainText("[:" + sInsert + ":]");
  } else if ("externalLinkAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("http://www.example.org/");
      nFormatLength = 1;
    }
    m_pCurrentEditor->insertPlainText("[" + sInsert + "]");
  } else if ("imageAct" == sAction) {
    if (sInsert.isEmpty()) {
      sInsert = trUtf8("Image.png");
      nFormatLength = 3;
    }
    m_pCurrentEditor->insertPlainText("[[" + m_pTemplates->getTransImage() + "("
                                      +  sInsert + ")]]");
  } else {
    QMessageBox::warning(this, qApp->applicationName(),
                         "Error while inserting syntax element: "
                         "Unknown action");
    qWarning() << "Error while inserting syntax element:"
               << "UNKNOWN ACTION" << sAction;
  }

  if (65530 != nFormatLength) {
    QTextCursor textCursor(m_pCurrentEditor->textCursor());
    textCursor.setPosition(m_pCurrentEditor->textCursor().position() -
                           sInsert.length() - nFormatLength);
    textCursor.setPosition(m_pCurrentEditor->textCursor().position() -
                           nFormatLength, QTextCursor::KeepAnchor);
    m_pCurrentEditor->setTextCursor(textCursor);
  }

  m_pCurrentEditor->setFocus();
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
      if (m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
        int nCurrentPos =  m_pCurrentEditor->textCursor().position();

        // Insert macro
        sMacro.remove("%%");  // Remove placeholder
        m_pCurrentEditor->insertPlainText(sMacro);

        // Select placeholder
        if ((nPlaceholder1 != nPlaceholder2)
            && nPlaceholder1 >= 0
            && nPlaceholder2 >= 0) {
          QTextCursor textCursor(m_pCurrentEditor->textCursor());
          textCursor.setPosition(nCurrentPos + nPlaceholder1);
          textCursor.setPosition(nCurrentPos + nPlaceholder2 - 2,
                                 QTextCursor::KeepAnchor);
          m_pCurrentEditor->setTextCursor(textCursor);
        }
      } else {
        // Some text is selected
        sTmp = sMacro;
        if ((nPlaceholder1 != nPlaceholder2)
            && nPlaceholder1 >= 0
            && nPlaceholder2 >= 0) {
          sTmp.replace(nPlaceholder1, nPlaceholder2 - nPlaceholder1,
                       m_pCurrentEditor->textCursor().selectedText());
          m_pCurrentEditor->insertPlainText(sTmp.remove("%%"));
        } else {
          // Problem with placeholder
          m_pCurrentEditor->insertPlainText(sMacro.remove("%%"));
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

  m_pCurrentEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Insert interwiki-link
void CInyokaEdit::insertInterwikiLink(const QString &sMenuEntry) {
  // Get indices for links
  QStringList sTmp = sMenuEntry.split(",");

  // Check if right number of indices found
  if (sTmp.size() == 2) {
    // No text selected
    if (m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
      QString sSitename(trUtf8("Sitename"));
      QString sText(trUtf8("Text"));

      // Insert InterWiki-Link
      m_pCurrentEditor->insertPlainText(
            "["
            + m_pTemplates->getIWLs()->getElementTypes()[sTmp[0].toInt()][sTmp[1].toInt()]
          + ":" + sSitename + ":" + sText + "]");

      // Select site name in InterWiki-Link
      QTextCursor textCursor(m_pCurrentEditor->textCursor());
      textCursor.setPosition(
            m_pCurrentEditor->textCursor().position() - sSitename.length() - sText.length() - 2);
      textCursor.setPosition(
            m_pCurrentEditor->textCursor().position() - sText.length() - 2,
            QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(textCursor);
    } else {
      // Some text is selected
      // Insert InterWiki-Link with selected text
      m_pCurrentEditor->insertPlainText(
            "["
            + m_pTemplates->getIWLs()->getElementTypes()[sTmp[0].toInt()][sTmp[1].toInt()]
          + ":" + m_pCurrentEditor->textCursor().selectedText() + ":]");
    }
  } else {
    // Problem with indices
    QMessageBox::warning(this, qApp->applicationName(),
                         "Error while inserting InterWiki link: "
                         "InterWiki indice");
    qWarning() << "Error while inserting InterWiki link - IWL indice:"
               << sMenuEntry;
  }

  m_pCurrentEditor->setFocus();
}

// ----------------------------------------------------------------------------

// Insert code block
void CInyokaEdit::insertCodeblock(const QString &sCodeStyle) {
  // No text selected
  if (m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
    QString sCode("Code");

    // Insert code block
    m_pCurrentEditor->insertPlainText("{{{"
                                      + sCodeStyle + "\n"
                                      + sCode
                                      + "\n}}}\n");

    // Select the word "code"
    QTextCursor textCursor(m_pCurrentEditor->textCursor());
    textCursor.setPosition(m_pCurrentEditor->textCursor().position() -
                           sCode.length() - 5);
    textCursor.setPosition(m_pCurrentEditor->textCursor().position() -
                           5, QTextCursor::KeepAnchor);
    m_pCurrentEditor->setTextCursor(textCursor);
  } else {
    // Some text is selected
    // Insert code block with selected text
    m_pCurrentEditor->insertPlainText("{{{" + sCodeStyle + "\n"
                                      + m_pCurrentEditor->textCursor().selectedText()
                                      + "\n}}}\n");
  }

  m_pCurrentEditor->setFocus();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::displayArticleText(const QString &sArticleText,
                                     const QString &sSitename) {
  m_pFileOperations->newFile(sSitename);
  m_pCurrentEditor->setPlainText(sArticleText);
  m_pCurrentEditor->document()->setModified(true);
  this->previewInyokaPage();

  // Reset scroll position
  m_WebviewScrollPosition = QPoint(0, 0);
#ifdef USEQTWEBKIT
  m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
#else
  m_pWebview->scroll(0, 0);
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Wait until loading has finished
void CInyokaEdit::loadPreviewFinished(const bool bSuccess) {
  if (bSuccess) {
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
    // TODO: Find alternative for WebEngine
#ifdef USEQTWEBKIT
    m_pWebview->page()->mainFrame()->setScrollPosition(m_WebviewScrollPosition);
#else
    m_pWebview->scroll(0, 0);
#endif
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
  m_pParser->updateSettings(m_pSettings->getInyokaUrl(),
                            m_pSettings->getCheckLinks());

  if (m_pSettings->getPreviewHorizontal()) {
    m_pWidgetSplitter->setOrientation(Qt::Vertical);
  } else {
    m_pWidgetSplitter->setOrientation(Qt::Horizontal);
  }

  m_pPreviewTimer->stop();
  if (m_pSettings->getTimedPreview() != 0) {
    m_pPreviewTimer->start(m_pSettings->getTimedPreview() * 1000);
  }

  m_pDownloadModule->updateSettings(m_pSettings->getAutomaticImageDownload(),
                                    m_pSettings->getInyokaUrl(),
                                    m_pSettings->getInyokaCommunity());

  m_pPlugins->setEditorlist(m_pFileOperations->getEditors());

  // Setting proxy if available
  CUtils::setProxy(m_pSettings->getProxyHostName(),
                   m_pSettings->getProxyPort(),
                   m_pSettings->getProxyUserName(),
                   m_pSettings->getProxyPassword());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CInyokaEdit::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_pCurrentEditor && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

    // Bug fix for LP: #922808
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
    bool isSHIFT(keyMod.testFlag(Qt::ShiftModifier));
    bool isCTRL(keyMod.testFlag(Qt::ControlModifier));

    if (keyEvent->key() == Qt::Key_Right
        && isSHIFT && isCTRL) {
      // CTRL + SHIFT + arrow right
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
      cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    } else if (keyEvent->key() == Qt::Key_Right
               && !isSHIFT && isCTRL) {
      // CTRL + arrow right
      m_pCurrentEditor->moveCursor(QTextCursor::Right);
      m_pCurrentEditor->moveCursor(QTextCursor::EndOfWord);
      return true;
    } else if (keyEvent->key() == Qt::Key_Up
               && isSHIFT && isCTRL) {
      // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    } else if (keyEvent->key() == Qt::Key_Down
               && isSHIFT && isCTRL) {
      // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
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
        docBackup.setPlainText(m_pCurrentEditor->document()->toPlainText());
        m_pParser->replaceTemplates(m_pCurrentEditor->document());
      } else {
        m_pCurrentEditor->setText(docBackup.toPlainText());
      }
      bToggle = !bToggle;
      return true;
    }
    // --------------------------------------------------------------------
    // --------------------------------------------------------------------
    // Reload preview at F5 or defined button
    else if ((Qt::Key_F5 == keyEvent->key()
              || m_pSettings->getReloadPreviewKey() == keyEvent->key())
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

// Delete old auto save backup files
void CInyokaEdit::deleteAutoSaveBackups() {
  QDir dir(m_UserDataDir.absolutePath());
  QFileInfoList fiListFiles = dir.entryInfoList(
                                QDir::NoDotAndDotDot | QDir::Files);
  foreach (QFileInfo fi, fiListFiles) {
    if ("bak~" == fi.suffix() && fi.baseName().startsWith("AutoSave")) {
      if (!dir.remove(fi.fileName())) {
        qWarning() << "Could not delete auto save backup file:" <<
                      fi.fileName();
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::syncScrollbarsEditor() {
  // TODO: Find alternative for WebEngine
#ifdef USEQTWEBKIT
  if (!m_bWebviewScrolling && true == m_pSettings->getSyncScrollbars()) {
    int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
    int nSizeWebviewBar = m_pWebview->page()->mainFrame()->scrollBarMaximum(
                            Qt::Vertical);
    float nRatio = static_cast<float>(nSizeWebviewBar) / nSizeEditorBar;

    m_bEditorScrolling = true;
    m_pWebview->page()->mainFrame()->setScrollPosition(
          QPoint(0, m_pCurrentEditor->verticalScrollBar()->sliderPosition() * nRatio));
    m_bEditorScrolling = false;
  }
#endif
}

// ----------------------------------------------------------------------------

void CInyokaEdit::syncScrollbarsWebview() {
  // TODO: Find alternative for WebEngine
#ifdef USEQTWEBKIT
  if (!m_bEditorScrolling && true == m_pSettings->getSyncScrollbars()) {
    int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
    int nSizeWebviewBar = m_pWebview->page()->mainFrame()->scrollBarMaximum(
                            Qt::Vertical);
    float nRatio = static_cast<float>(nSizeEditorBar) / nSizeWebviewBar;

    m_bWebviewScrolling = true;
    m_pCurrentEditor->verticalScrollBar()->setSliderPosition(
          m_pWebview->page()->mainFrame()->scrollPosition().y() * nRatio);
    m_bWebviewScrolling = false;
  }
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CInyokaEdit::showSyntaxOverview() {
  QDialog* dialog = new QDialog(this, this->windowFlags()
                                & ~Qt::WindowContextHelpButtonHint);
  QGridLayout* layout = new QGridLayout(dialog);
#ifdef USEQTWEBKIT
  QWebView* webview = new QWebView();
#else
  QWebEngineView* webview = new QWebEngineView();
#endif
  QTextDocument* pTextDocument = new QTextDocument(this);

  QFile OverviewFile(m_sSharePath + "/community/" +
                     m_pSettings->getInyokaCommunity() +
                     "/SyntaxOverview.tpl");

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
  if (m_pFileOperations->closeAllmaybeSave()) {
    m_pSettings->writeSettings(saveGeometry(), saveState(),
                               m_pWidgetSplitter->saveState());
    event->accept();
  } else {
    event->ignore();
  }
}
