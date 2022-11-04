/**
 * \file inyokaedit.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Main application generation (GUI, object creation etc.).
 */

#include "./inyokaedit.h"

#include <QComboBox>
#include <QDesktopServices>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QTextBlock>
#include <QTimer>
#include <QToolButton>
#include <QToolTip>

#ifdef USEQTWEBKIT
#include <QWebFrame>
#include <QWebHistory>
#include <QtWebKitWidgets/QWebView>
#endif
#ifdef USEQTWEBENGINE
#include <QWebEngineHistory>
#include <QWebEngineView>
#endif

#include "./download.h"
#include "./fileoperations.h"
#include "./ieditorplugin.h"
#include "./parser/parser.h"
#include "./plugins.h"
#include "./session.h"
#include "./settings.h"
#include "./templates/templates.h"
#include "./texteditor.h"
#include "./upload.h"
#include "./utils.h"
#include "./xmlparser.h"
#include "ui_inyokaedit.h"

InyokaEdit::InyokaEdit(const QDir &userDataDir, const QDir &sharePath,
                       const QString &sArg, QWidget *parent)
    : QMainWindow(parent),
      m_pUi(new Ui::InyokaEdit),
      m_sCurrLang(QLatin1String("")),
      m_sSharePath(sharePath.absolutePath()),
      m_UserDataDir(userDataDir),
      m_sPreviewFile(m_UserDataDir.absolutePath() + "/tmpinyoka.html"),
      m_tmpPreviewImgDir(m_UserDataDir.absolutePath() + "/tmpImages"),
      m_pPreviewTimer(new QTimer(this)),
      m_bOpenFileAfterStart(false),
      m_bEditorScrolling(false),
      m_bWebviewScrolling(false),
      m_bReloadPreviewBlocked(false) {
  m_pUi->setupUi(this);

  if (!sharePath.exists()) {
    QMessageBox::warning(nullptr, QStringLiteral("Warning"),
                         QStringLiteral("App share folder not found!"));
    qWarning() << "Share folder does not exist:" << m_sSharePath;
    exit(-1);
  }

  // Create folder for downloaded article images
  if (!m_tmpPreviewImgDir.exists()) {
    // Create folder including possible parent directories (mkPATH)!
    m_tmpPreviewImgDir.mkpath(m_tmpPreviewImgDir.absolutePath());
  }

  if (!sArg.isEmpty()) {
    m_bOpenFileAfterStart = true;  // Checked in setupEditor()
  }

  // After definition of StylesAndImagesDir AND m_tmpPreviewImgDir!
  this->createObjects();
  this->setupEditor();
  this->createActions();
  this->createMenus();
  // this->createXmlMenus();  // Already called by LanguageChange event
  this->setUnifiedTitleAndToolBarOnMac(true);

  if (!QFile(m_UserDataDir.absolutePath() + "/community/" +
             m_pSettings->getInyokaCommunity())
           .exists()) {
    m_UserDataDir.mkpath(m_UserDataDir.absolutePath() + "/community/" +
                         m_pSettings->getInyokaCommunity());
  }

  if (Utils::getOnlineState() && m_pSettings->getWindowsCheckUpdate()) {
    m_pUtils->checkWindowsUpdate();
  }

  // Load file via command line
  if (m_bOpenFileAfterStart) {
    m_pFileOperations->loadFile(sArg, true);
  }

  m_pPlugins->loadPlugins(m_pSettings->getGuiLanguage());
  this->updateEditorSettings();
  this->deleteAutoSaveBackups();
  m_pCurrentEditor->setFocus();
}

InyokaEdit::~InyokaEdit() {
  delete m_pUi;
  m_pUi = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::createObjects() {
  m_pSettings = new Settings(this, m_sSharePath);
  qDebug() << "Inyoka Community:" << m_pSettings->getInyokaCommunity();
  if (m_pSettings->getInyokaCommunity().isEmpty() ||
      !QDir(m_sSharePath + "/community/" + m_pSettings->getInyokaCommunity())
           .exists()) {
    qCritical() << "No Inyoka community files found / installed!";
    qCritical() << "Community path:"
                << m_sSharePath + "/community/" +
                       m_pSettings->getInyokaCommunity();
    QMessageBox::critical(this, qApp->applicationName(),
                          tr("No Inyoka community files found/installed!\n"
                             "Please check your installation and restart "
                             "the application."));
    exit(-2);
  }
  connect(m_pSettings, &Settings::changeLang, this, &InyokaEdit::loadLanguage);
  connect(this, &InyokaEdit::updateUiLang, m_pSettings,
          &Settings::updateUiLang);
  this->loadLanguage(m_pSettings->getGuiLanguage());

  // Has to be created before parser
  m_pTemplates = new Templates(m_pSettings->getInyokaCommunity(), m_sSharePath,
                               m_UserDataDir.absolutePath());

  m_pSession = new Session(this, m_pSettings->getInyokaHash());

  m_pDownloadModule =
      new Download(this, m_pSession, m_UserDataDir.absolutePath(),
                   m_tmpPreviewImgDir.absolutePath(), m_sSharePath);

  m_pUploadModule = new Upload(this, m_pSession, m_pSettings->getInyokaUrl(),
                               m_pSettings->getInyokaConstructionArea());

  m_pParser = new Parser(
      m_sSharePath, m_tmpPreviewImgDir, m_pSettings->getInyokaUrl(),
      m_pSettings->getCheckLinks(), m_pTemplates,
      m_pSettings->getInyokaCommunity(), m_pSettings->getPygmentize());
  connect(m_pParser, &Parser::hightlightSyntaxError, this,
          &InyokaEdit::highlightSyntaxError);

  m_pDocumentTabs = new QTabWidget;
  m_pDocumentTabs->setTabPosition(QTabWidget::North);
  m_pDocumentTabs->setTabsClosable(true);
  m_pDocumentTabs->setDocumentMode(true);
  // Attention: Currently tab order is fixed (same as m_pListEditors)
  m_pDocumentTabs->setMovable(false);

  m_pFileOperations = new FileOperations(
      this, m_pDocumentTabs, m_pSettings, m_sPreviewFile,
      m_UserDataDir.absolutePath(), m_pTemplates->getAllBoilerplates());
  m_pCurrentEditor = m_pFileOperations->getCurrentEditor();

  connect(m_pFileOperations, &FileOperations::callPreview, this,
          &InyokaEdit::previewInyokaPage);
  connect(m_pFileOperations, &FileOperations::modifiedDoc, this,
          &InyokaEdit::setWindowModified);
  connect(m_pFileOperations, &FileOperations::changedCurrentEditor, this,
          &InyokaEdit::setCurrentEditor);

  m_pPlugins =
      new Plugins(this, m_pCurrentEditor, m_pSettings->getDisabledPlugins(),
                  m_UserDataDir, m_sSharePath);
  connect(m_pSettings, &Settings::changeLang, m_pPlugins, &Plugins::changeLang);
  connect(m_pPlugins, &Plugins::addMenuToolbarEntries, this,
          &InyokaEdit::addPluginsButtons);
  connect(m_pPlugins, &Plugins::availablePlugins, m_pSettings,
          &Settings::availablePlugins);

  this->setCurrentEditor();

#ifdef USEQTWEBKIT
  m_pWebview = new QWebView(this);
  connect(m_pWebview->page(), &QWebPage::scrollRequested, this,
          &InyokaEdit::syncScrollbarsWebview);
  connect(m_pFileOperations, &FileOperations::movedEditorScrollbar, this,
          &InyokaEdit::syncScrollbarsEditor);

  m_pWebview->settings()->setDefaultTextEncoding(QStringLiteral("utf-8"));
#endif
#ifdef USEQTWEBENGINE
  m_pWebview = new QWebEngineView(this);
  m_pWebview->pageAction(QWebEnginePage::SavePage)->setVisible(false);
  m_pWebview->pageAction(QWebEnginePage::ViewSource)->setVisible(false);
  m_pWebview->pageAction(QWebEnginePage::OpenLinkInNewTab)->setVisible(false);
  m_pWebview->pageAction(QWebEnginePage::DownloadLinkToDisk)->setVisible(false);
  m_pWebview->pageAction(QWebEnginePage::OpenLinkInNewWindow)
      ->setVisible(false);

  connect(m_pWebview->page(), &QWebEnginePage::scrollPositionChanged, this,
          &InyokaEdit::syncScrollbarsWebview);
  connect(m_pFileOperations, &FileOperations::movedEditorScrollbar, this,
          &InyokaEdit::syncScrollbarsEditor);
#endif
#ifndef NOPREVIEW
  m_pWebview->installEventFilter(this);
#endif

  m_pUtils = new Utils(this);
  connect(m_pUtils, &Utils::setWindowsUpdateCheck, m_pSettings,
          &Settings::setWindowsCheckUpdate);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::setCurrentEditor() {
  m_pCurrentEditor = m_pFileOperations->getCurrentEditor();
  m_pPlugins->setCurrentEditor(m_pCurrentEditor);
  m_pUploadModule->setEditor(m_pCurrentEditor, m_pCurrentEditor->getFileName());
}

// ----------------------------------------------------------------------------

void InyokaEdit::changedNumberOfEditors() {
  m_pPlugins->setEditorlist(m_pFileOperations->getEditors());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::setupEditor() {
  qDebug() << "Calling" << Q_FUNC_INFO;

  // Timed preview
  connect(m_pPreviewTimer, &QTimer::timeout, this,
          &InyokaEdit::previewInyokaPage);

#ifdef USEQTWEBKIT
  connect(m_pWebview, &QWebView::loadFinished, this,
          &InyokaEdit::loadPreviewFinished);
#endif
#ifdef USEQTWEBENGINE
  connect(m_pWebview, &QWebEngineView::loadFinished, this,
          &InyokaEdit::loadPreviewFinished);
#endif

  m_pWidgetSplitter = new QSplitter;

  m_pWidgetSplitter->addWidget(m_pDocumentTabs);
#ifndef NOPREVIEW
  m_pWidgetSplitter->addWidget(m_pWebview);
#endif
  setCentralWidget(m_pWidgetSplitter);
  m_pWidgetSplitter->restoreState(m_pSettings->getSplitterState());

  connect(m_pSettings, &Settings::updateEditorSettings, this,
          &InyokaEdit::updateEditorSettings);
  connect(m_pFileOperations, &FileOperations::changedNumberOfEditors, this,
          &InyokaEdit::changedNumberOfEditors);

#ifndef NOPREVIEW
  // Show an empty website after start
  if (!m_bOpenFileAfterStart) {
    this->previewInyokaPage();
  }
#endif

  connect(m_pDownloadModule, &Download::sendArticleText, this,
          &InyokaEdit::displayArticleText);

  // Restore window and toolbar settings
  // Settings have to be restored after toolbars are created!
  this->restoreGeometry(m_pSettings->getWindowGeometry());
  // Restore toolbar position etc.
  this->restoreState(m_pSettings->getWindowState());

  m_pUi->aboutAct->setText(m_pUi->aboutAct->text() + " " +
                           qApp->applicationName());

#ifdef USEQTWEBKIT
  connect(m_pUi->goBackBrowserAct, &QAction::triggered, m_pWebview,
          &QWebView::back);
  connect(m_pUi->goForwardBrowserAct, &QAction::triggered, m_pWebview,
          &QWebView::forward);
  connect(m_pUi->reloadBrowserAct, &QAction::triggered, m_pWebview,
          &QWebView::reload);

  connect(m_pWebview, &QWebView::urlChanged, this, &InyokaEdit::changedUrl);

  m_pWebview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(m_pWebview, &QWebView::linkClicked, this, &InyokaEdit::clickedLink);
#endif
#ifdef USEQTWEBENGINE
  connect(m_pUi->goBackBrowserAct, &QAction::triggered, m_pWebview,
          &QWebEngineView::back);
  connect(m_pUi->goForwardBrowserAct, &QAction::triggered, m_pWebview,
          &QWebEngineView::forward);
  connect(m_pUi->reloadBrowserAct, &QAction::triggered, m_pWebview,
          &QWebEngineView::reload);

  connect(m_pWebview, &QWebEngineView::urlChanged, this,
          &InyokaEdit::changedUrl);

  // TODO(volunteer): Find solution for QWebEngineView
  // QWebEngineUrlRequestInterceptor ?
  // m_pWebview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  // connect(m_pWebview, &QWebView::linkClicked,
  //         this, &InyokaEdit::clickedLink);
#endif
#ifdef NOPREVIEW
  m_pUi->browserBar->clear();
  this->removeToolBar(m_pUi->browserBar);
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Generate actions (buttons / menu entries)
void InyokaEdit::createActions() {
  qDebug() << "Calling" << Q_FUNC_INFO;

  // File menu
  // New file
  m_pUi->newAct->setShortcuts(QKeySequence::New);
  connect(m_pUi->newAct, &QAction::triggered, m_pFileOperations,
          [this]() { m_pFileOperations->newFile(QString()); });
  // Open file
  m_pUi->openAct->setShortcuts(QKeySequence::Open);
  connect(m_pUi->openAct, &QAction::triggered, this, &InyokaEdit::openFile);
  // Save file
  m_pUi->saveAct->setShortcuts(QKeySequence::Save);
  connect(m_pUi->saveAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::save);
  // Save file as...
  m_pUi->saveAsAct->setShortcuts(QKeySequence::SaveAs);
  connect(m_pUi->saveAsAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::saveAs);
  // Print preview
  m_pUi->printPdfPreviewAct->setEnabled(false);
#ifdef USEQTWEBENGINE
  m_pUi->printPdfPreviewAct->setEnabled(true);
  m_pUi->printPdfPreviewAct->setShortcut(QKeySequence::Print);
  connect(m_pUi->printPdfPreviewAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::printPdfPreview);
#endif

  // Exit application
  m_pUi->exitAct->setShortcuts(QKeySequence::Quit);
  connect(m_pUi->exitAct, &QAction::triggered, this, &InyokaEdit::close);

  // ------------------------------------------------------------------------
  // EDIT MENU

  // Find
  m_pUi->searchAct->setShortcuts(QKeySequence::Find);
  connect(m_pUi->searchAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::triggeredFind);
  // Replace
  m_pUi->replaceAct->setShortcuts(QKeySequence::Replace);
  connect(m_pUi->replaceAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::triggeredReplace);
  // Find next
  m_pUi->findNextAct->setShortcuts(QKeySequence::FindNext);
  connect(m_pUi->findNextAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::triggeredFindNext);
  // Find previous
  m_pUi->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
  connect(m_pUi->findPreviousAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::triggeredFindPrevious);

  // Cut
  m_pUi->cutAct->setShortcuts(QKeySequence::Cut);
  connect(m_pUi->cutAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::cut);
  // Copy
  m_pUi->copyAct->setShortcuts(QKeySequence::Copy);
  connect(m_pUi->copyAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::copy);
  // Paste
  m_pUi->pasteAct->setShortcuts(QKeySequence::Paste);
  connect(m_pUi->pasteAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::paste);

  connect(m_pFileOperations, &FileOperations::copyAvailable, m_pUi->cutAct,
          &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::copyAvailable, m_pUi->copyAct,
          &QAction::setEnabled);
  m_pUi->cutAct->setEnabled(false);
  m_pUi->copyAct->setEnabled(false);

  // Undo
  m_pUi->undoAct->setShortcuts(QKeySequence::Undo);
  connect(m_pUi->undoAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::undo);
  // Redo
  m_pUi->redoAct->setShortcuts(QKeySequence::Redo);
  connect(m_pUi->redoAct, &QAction::triggered, m_pFileOperations,
          &FileOperations::redo);

  connect(m_pFileOperations, &FileOperations::undoAvailable, m_pUi->undoAct,
          &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::redoAvailable, m_pUi->redoAct,
          &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::undoAvailable2, m_pUi->undoAct,
          &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::redoAvailable2, m_pUi->redoAct,
          &QAction::setEnabled);
  m_pUi->undoAct->setEnabled(false);
  m_pUi->redoAct->setEnabled(false);

  // ------------------------------------------------------------------------
  // TOOLS MENU

  // Clear temp. image download folder
  connect(m_pUi->deleteTempImagesAct, &QAction::triggered, this,
          &InyokaEdit::deleteTempImages);

  // Show settings dialog
  connect(m_pUi->preferencesAct, &QAction::triggered, m_pSettings,
          &Settings::showSettingsDialog);

  // ------------------------------------------------------------------------

  // Show html preview
  connect(m_pUi->previewAct, &QAction::triggered, this,
          &InyokaEdit::previewInyokaPage);

  // Download Inyoka article
  connect(m_pUi->downloadArticleAct, &QAction::triggered, m_pDownloadModule,
          [this]() { m_pDownloadModule->downloadArticle(QString()); });

  // Upload Inyoka article
  connect(m_pUi->uploadArticleAct, &QAction::triggered, m_pUploadModule,
          &Upload::clickUploadArticle);

  if (this->window()->palette().window().color().lightnessF() <
      m_pSettings->getDarkThreshold()) {
    m_pUi->previewAct->setIcon(
        QIcon(QLatin1String(":/toolbar/dark/preview.png")));
    m_pUi->downloadArticleAct->setIcon(
        QIcon(QLatin1String(":/toolbar/dark/cloud_download.png")));
    m_pUi->uploadArticleAct->setIcon(
        QIcon(QLatin1String(":/toolbar/dark/cloud_upload.png")));
  }

  // ------------------------------------------------------------------------
  // ABOUT MENU

  // Show syntax overview
#ifdef NOPREVIEW
  m_pUi->showSyntaxOverviewAct->setVisible(false);
#else
  connect(m_pUi->showSyntaxOverviewAct, &QAction::triggered, this,
          &InyokaEdit::showSyntaxOverview);
#endif

  // Report a bug
  connect(m_pUi->reportBugAct, &QAction::triggered, this, []() {
    QDesktopServices::openUrl(QUrl(
        QStringLiteral("https://github.com/inyokaproject/inyokaedit/issues")));
  });

  // Open about window
  connect(m_pUi->aboutAct, &QAction::triggered, this, &InyokaEdit::showAbout);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Generate menus
void InyokaEdit::createMenus() {
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

  bool bFirstLoop(true);
  for (const auto &tplDir : qAsConst(listTplDirs)) {
    const QFileInfoList fiListFiles =
        tplDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    for (const auto &fi : fiListFiles) {
      if ("tpl" == fi.suffix()) {
        m_OpenTemplateFilesActions << new QAction(
            fi.baseName().replace(QLatin1String("_"), QLatin1String(" ")),
            this);
        connect(m_OpenTemplateFilesActions.last(), &QAction::triggered, this,
                [this, fi]() {
                  m_pFileOperations->loadFile(fi.absoluteFilePath());
                });
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

  if (m_OpenTemplateFilesActions.isEmpty()) {
    m_pUi->fileMenuFromTemplate->setDisabled(true);
  }

  // File menu (recent opened files)
  m_pUi->fileMenuLastOpened->addActions(
      m_pFileOperations->getLastOpenedFiles());
  if (m_pSettings->getRecentFiles().isEmpty()) {
    m_pUi->fileMenuLastOpened->setDisabled(true);
  }
  connect(m_pFileOperations, &FileOperations::setMenuLastOpenedEnabled,
          m_pUi->fileMenuLastOpened, &QMenu::setEnabled);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::clearXmlMenus() {
  for (auto *m : qAsConst(m_pXmlSubMenus)) {
    m->disconnect();
    m->clear();
  }
  for (auto *m : qAsConst(m_pXmlMenus)) {
    m->disconnect();
    m->clear();
    m->deleteLater();
  }
  for (auto *q : qAsConst(m_pXmlDropdowns)) {
    q->disconnect();
    q->clear();
  }

  for (auto *t : qAsConst(m_pXmlToolbars)) {
    t->disconnect();
    t->clear();
  }

  for (auto *a : qAsConst(m_pXmlActions)) {
    m_pUi->inyokaeditorBar->removeAction(a);
    m_pUi->menuBar->removeAction(a);
    m_pUi->samplesmacrosBar->removeAction(a);
    for (auto *b : qAsConst(m_pXmlToolbuttons)) {
      b->removeAction(a);
    }
    a->deleteLater();
  }
  m_pUi->inyokaeditorBar->clear();
  m_pUi->samplesmacrosBar->clear();

  m_pXmlActions.clear();
  m_pXmlSubMenus.clear();
  m_pXmlMenus.clear();
  m_pXmlDropdowns.clear();
  m_pXmlToolbuttons.clear();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::createXmlMenus() {
  qDebug() << "Calling" << Q_FUNC_INFO;
  XmlParser xmlParser;
  QFile xmlFile;
  QList<QAction *> tmplListActions;
  QStringList sListFolders;
  QString sTmpPath;
  sListFolders << m_sSharePath << m_UserDataDir.absolutePath();
  QStringList sListObjects;
  sListObjects << QStringLiteral("menu") << QStringLiteral("dropdown")
               << QStringLiteral("toolbar");
  const int MAXFILES = 9;

  this->clearXmlMenus();

  // Check share and user path
  for (const auto &sPath : qAsConst(sListFolders)) {
    // Search for menu/dropdown/toolbar
    for (const auto &sObj : qAsConst(sListObjects)) {
      // Search for max 9 files
      for (int n = 1; n < MAXFILES; n++) {
        sTmpPath =
            sPath + "/community/" + m_pSettings->getInyokaCommunity() + "/xml/";
        // File name e.g. menu_1_de.xml
        xmlFile.setFileName(sTmpPath + sObj + "_" + QString::number(n) + "_" +
                            m_pSettings->getGuiLanguage() + ".xml");

        if (!xmlFile.exists() &&
            m_pSettings->getGuiLanguage() != QLatin1String("en")) {
          if (1 == n && sPath == m_sSharePath) {
            qWarning() << "Xml menu file not found:" << xmlFile.fileName()
                       << "- Trying to load English fallback.";
          }
          // Try English fallback
          QString sTemp(xmlFile.fileName());
          sTemp.replace("_" + m_pSettings->getGuiLanguage() + ".xml",
                        QLatin1String("_en.xml"));
          xmlFile.setFileName(sTemp);
        }

        if (xmlFile.exists()) {
          // qDebug() << "Read XML" << xmlFile.fileName();
          if (xmlParser.parseXml(xmlFile.fileName())) {
            QString sIconPath(xmlParser.getPath());
            if (this->window()->palette().window().color().lightnessF() <
                    m_pSettings->getDarkThreshold() &&
                sIconPath.contains(QLatin1String("light"))) {
              sIconPath.replace(QLatin1String("light"), QLatin1String("dark"),
                                Qt::CaseInsensitive);
            }

            if ("menu" == sObj) {
              m_pXmlMenus.append(new QMenu(xmlParser.getMenuName(), this));
              m_pUi->menuBar->insertMenu(m_pUi->toolsMenu->menuAction(),
                                         m_pXmlMenus.last());
            } else if ("dropdown" == sObj) {
              m_pXmlDropdowns.append(new QComboBox(this));
              m_pUi->samplesmacrosBar->addWidget(m_pXmlDropdowns.last());
              m_pXmlDropdowns.last()->addItem(xmlParser.getMenuName());
              m_pXmlDropdowns.last()->insertSeparator(1);
              connect(m_pXmlDropdowns.last(),
                      static_cast<void (QComboBox::*)(int)>(
                          &QComboBox::currentIndexChanged),
                      this, &InyokaEdit::dropdownXmlChanged);
            } else if ("toolbar" == sObj) {
              m_pXmlToolbars.append(
                  new QToolBar(xmlParser.getMenuName(), this));
              m_pUi->inyokaeditorBar->addWidget(m_pXmlToolbars.last());
            }

            // Create action and menu/dropdown/toolbar entry for each element
            for (int i = 0; i < xmlParser.getGroupNames().size(); i++) {
              // qDebug() << "GROUP:" << xmlParser.getGroupNames()[i];
              tmplListActions.clear();
              for (int j = 0; j < xmlParser.getElementNames().at(i).size();
                   j++) {
                // qDebug() << "ELEMENTS" << xmlParser.getElementNames()[i][j];
                m_pXmlActions.append(
                    new QAction(QIcon(sTmpPath + sIconPath + "/" +
                                      xmlParser.getElementIcons().at(i).at(j)),
                                xmlParser.getElementNames().at(i).at(j), this));
                tmplListActions.append(m_pXmlActions.last());

                if ("dropdown" == sObj) {
                  m_pXmlDropdowns.last()->addItem(
                      xmlParser.getElementNames().at(i).at(j),
                      QVariant::fromValue(m_pXmlActions.last()));
                }

                // qDebug() << "INSERT" << xmlParser.getElementInserts()[i][j];
                if (xmlParser.getElementInserts().at(i).at(j).endsWith(
                        QLatin1String(".tpl"), Qt::CaseInsensitive) ||
                    xmlParser.getElementInserts().at(i).at(j).endsWith(
                        QLatin1String(".macro"), Qt::CaseInsensitive)) {
                  QFile tmp(sTmpPath + xmlParser.getPath() + "/" +
                            xmlParser.getElementInserts().at(i).at(j));
                  if (tmp.exists()) {
                    QString s(tmp.fileName());
                    connect(m_pXmlActions.last(), &QAction::triggered, this,
                            [this, s]() { insertMacro(s); });
                  } else {
                    qWarning() << "Tpl/Macro file not found for XML menu:"
                               << tmp.fileName();
                  }
                } else {
                  QString s(xmlParser.getElementInserts().at(i).at(j));
                  connect(m_pXmlActions.last(), &QAction::triggered, this,
                          [this, s]() { insertMacro(s); });
                }
              }
              if ("menu" == sObj) {
                if (xmlParser.getGroupNames().at(i).isEmpty()) {
                  m_pXmlMenus.last()->addActions(tmplListActions);
                } else {
                  m_pXmlSubMenus.append(
                      new QMenu(xmlParser.getGroupNames().at(i), this));
                  m_pXmlSubMenus.last()->setIcon(
                      QIcon(sTmpPath + sIconPath + "/" +
                            xmlParser.getGroupIcons().at(i)));
                  m_pXmlSubMenus.last()->addActions(tmplListActions);
                  m_pXmlMenus.last()->addMenu(m_pXmlSubMenus.last());
                }
              }
              if ("toolbar" == sObj) {
                if (xmlParser.getGroupNames().at(i).isEmpty()) {
                  m_pXmlToolbars.last()->addActions(tmplListActions);
                } else {
                  m_pXmlToolbuttons.append(new QToolButton(this));
                  m_pXmlToolbuttons.last()->setIcon(
                      QIcon(sTmpPath + sIconPath + "/" +
                            xmlParser.getGroupIcons().at(i)));
                  m_pXmlToolbuttons.last()->setPopupMode(
                      QToolButton::InstantPopup);
                  m_pXmlSubMenus.append(
                      new QMenu(xmlParser.getGroupNames().at(i), this));
                  m_pXmlSubMenus.last()->addActions(tmplListActions);
                  m_pXmlToolbuttons.last()->setMenu(m_pXmlSubMenus.last());
                  m_pUi->inyokaeditorBar->addWidget(m_pXmlToolbuttons.last());
                }
              }
            }
          }
        } else {
          if (1 == n && sPath == m_sSharePath) {
            qWarning() << "Xml menu file not found:" << xmlFile.fileName();
          }
          break;
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Add plugins to plugin toolbar and tools menu
void InyokaEdit::addPluginsButtons(const QList<QAction *> &ToolbarEntries,
                                   QList<QAction *> MenueEntries) {
  m_pUi->pluginsBar->addActions(ToolbarEntries);
  if (!m_pUi->toolsMenu->actions().isEmpty()) {
    auto *separator = new QAction(this);
    separator->setSeparator(true);
    MenueEntries << separator;
    m_pUi->toolsMenu->insertActions(m_pUi->toolsMenu->actions().at(0),
                                    MenueEntries);
  } else {
    m_pUi->toolsMenu->addActions(MenueEntries);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::openFile() {
  // Reset scroll position
#ifdef USEQTWEBKIT
  m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
#endif
#ifdef USEQTWEBENGINE
  m_pWebview->scroll(0, 0);
#endif
  m_pFileOperations->open();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Call parser
void InyokaEdit::previewInyokaPage() {
#ifndef NOPREVIEW
  m_pWebview->history()->clear();  // Clear history (clicked links)
#endif

  QString sRetHTML(QLatin1String(""));
  sRetHTML = m_pParser->genOutput(m_pFileOperations->getCurrentFile(),
                                  m_pCurrentEditor->document(),
                                  m_pSettings->getSyntaxCheck());

  // File for temporary html output
  QFile tmphtmlfile(m_sPreviewFile);

  // No write permission
  if (!tmphtmlfile.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, qApp->applicationName(),
                         tr("Could not create temporary HTML file!"));
    qWarning() << "Could not create temporary HTML file:" << m_sPreviewFile;
    return;
  }

  // Stream for output in file
  QTextStream tmpoutputstream(&tmphtmlfile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  // Since Qt 6 UTF-8 is used by default
  tmpoutputstream.setCodec("UTF-8");
#endif
  tmpoutputstream.setAutoDetectUnicode(true);

  // Write HTML code into output file
  tmpoutputstream << sRetHTML;
  tmphtmlfile.close();

  // Store scroll position
#ifdef USEQTWEBKIT
  m_WebviewScrollPosition = m_pWebview->page()->mainFrame()->scrollPosition();
#endif
#ifdef USEQTWEBENGINE
  m_WebviewScrollPosition = m_pWebview->page()->scrollPosition().toPoint();
#endif

#ifdef NOPREVIEW
  static bool bOpenedBrowser = false;
  if (!bOpenedBrowser) {
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(QFileInfo(tmphtmlfile).absoluteFilePath()));
    bOpenedBrowser = true;
  }
#else
  m_pWebview->load(
      QUrl::fromLocalFile(QFileInfo(tmphtmlfile).absoluteFilePath()));
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::highlightSyntaxError(const QPair<int, QString> &error) {
  QList<QTextEdit::ExtraSelection> extras;
  QTextEdit::ExtraSelection selection;

  selection.format.setBackground(m_colorSyntaxError);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);

  extras.clear();
  if (-1 != error.first) {
    QTextCursor initialCur = m_pCurrentEditor->textCursor();
    selection.cursor = m_pCurrentEditor->textCursor();
    selection.cursor.setPosition(error.first);
    selection.cursor.clearSelection();
    extras << selection;
    m_pCurrentEditor->setTextCursor(selection.cursor);

    QFontMetrics fm1(QToolTip::font());
    QFontMetrics fm2(m_pSettings->getEditorFont());
    QPoint cur = m_pCurrentEditor->cursorRect().topLeft();
    cur.setY(
        cur.y() +
        m_pCurrentEditor->viewport()->mapToGlobal(m_pCurrentEditor->pos()).y() -
        fm1.height() - fm2.height() - 10);
    cur.setX(
        cur.x() +
        m_pCurrentEditor->viewport()->mapToGlobal(m_pCurrentEditor->pos()).x());
    QString sError(error.second);
    if ("OPEN_PAR_MISSING" == sError) {
      sError = tr("Opening parenthesis missing!");
    } else if ("CLOSE_PAR_MISSING" == sError) {
      sError = tr("Closing parenthesis missing!");
    } else if (sError.startsWith(QLatin1String("UNKNOWN_TPL|"))) {
      sError = sError.remove(QStringLiteral("UNKNOWN_TPL|"));
      sError = tr("Unknown template:") + " " + sError;
    } else {
      qWarning() << "Unknown syntax error code: " + sError;
      sError = tr("Syntax error");
    }
    QToolTip::showText(cur, sError);

    // Reset cursor to initial position to not disturb writing
    m_pCurrentEditor->setTextCursor(initialCur);
  }

  m_pCurrentEditor->setExtraSelections(extras);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto InyokaEdit::getHighlightErrorColor() -> QColor {
#if defined __linux__
  QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(),
                     qApp->applicationName().toLower());
#else
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(),
                     qApp->applicationName().toLower());
#endif
  QString sStyle =
      settings
          .value(QStringLiteral("Plugin_highlighter/Style"), "standard-style")
          .toString();

#if defined __linux__
  QSettings styleset(QSettings::NativeFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(), sStyle);
#else
  QSettings styleset(QSettings::IniFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(), sStyle);
#endif
  sStyle =
      styleset
          .value(QStringLiteral("Style/SyntaxError"), "---|---|---|0xffff00")
          .toString();

  QColor color(
      sStyle.right(8).replace(QLatin1String("0x"), QLatin1String("#")));
  if (color.isValid()) {
    return color;
  }
  return Qt::yellow;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::dropdownXmlChanged(int nIndex) {
  auto *tmpCombo = qobject_cast<QComboBox *>(sender());
  if (tmpCombo != nullptr) {
    auto *action = tmpCombo->itemData(nIndex, Qt::UserRole).value<QAction *>();
    if (action) {
      action->trigger();  // Triggering insertMacro() slot
    }
    tmpCombo->setCurrentIndex(0);  // Reset selection
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Insert macro/template/IWL/text from XML menu/toolbar/dropdown
void InyokaEdit::insertMacro(const QString &sInsert) {
  QString sMacro(QLatin1String(""));

  if (sInsert.endsWith(QLatin1String(".tpl"), Qt::CaseInsensitive) ||
      sInsert.endsWith(QLatin1String(".macro"), Qt::CaseInsensitive)) {
    QFile tplFile(sInsert);
    if (tplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&tplFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      // Since Qt 6 UTF-8 is used by default
      in.setCodec("UTF-8");
#endif
      sMacro = in.readLine().trimmed();  // First line HAS to include ## Macro
      sMacro = sMacro.remove(QStringLiteral("## Macro="));
      tplFile.close();
    } else {
      QMessageBox::warning(
          this, QStringLiteral("Warning"),
          "Could not open macro file: \n" + tplFile.fileName());
      qWarning() << "Could not open macro file:" << tplFile.fileName();
      return;
    }

    if (sMacro.isEmpty()) {
      QMessageBox::warning(this, QStringLiteral("Warning"),
                           QStringLiteral("Macro file was empty!"));
      qWarning() << "Macro file was empty:" << tplFile.fileName();
      return;
    }
  } else {
    sMacro = sInsert;
  }

  sMacro.replace(QLatin1String("\\n"), QLatin1String("\n"));
  int nPlaceholder1(sMacro.indexOf(QLatin1String("%%")));
  int nPlaceholder2(sMacro.lastIndexOf(QLatin1String("%%")));

  // No text selected
  if (m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
    int nCurrentPos = m_pCurrentEditor->textCursor().position();
    sMacro.remove(QStringLiteral("%%"));  // Remove placeholder
    m_pCurrentEditor->insertPlainText(sMacro);

    // Select placeholder
    if ((nPlaceholder1 != nPlaceholder2) && nPlaceholder1 >= 0 &&
        nPlaceholder2 >= 0) {
      QTextCursor textCursor(m_pCurrentEditor->textCursor());
      textCursor.setPosition(nCurrentPos + nPlaceholder1);
      textCursor.setPosition(nCurrentPos + nPlaceholder2 - 2,
                             QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(textCursor);
    }
  } else {
    // Some text is selected
    QString sTmp = sMacro;
    if ((nPlaceholder1 != nPlaceholder2) && nPlaceholder1 >= 0 &&
        nPlaceholder2 >= 0) {
      sTmp.replace(nPlaceholder1, nPlaceholder2 - nPlaceholder1,
                   m_pCurrentEditor->textCursor().selectedText());
      m_pCurrentEditor->insertPlainText(sTmp.remove(QStringLiteral("%%")));
    } else {
      // No placeholder defined (or problem with placeholder)
      m_pCurrentEditor->insertPlainText(sMacro.remove(QStringLiteral("%%")));
    }
  }

  m_pCurrentEditor->setFocus();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::displayArticleText(const QString &sArticleText,
                                    const QString &sSitename) {
  m_pFileOperations->newFile(sSitename);
  m_pCurrentEditor->setPlainText(sArticleText);
  m_pCurrentEditor->document()->setModified(true);
  this->previewInyokaPage();

  // Reset scroll position
  m_WebviewScrollPosition = QPoint(0, 0);
#ifdef USEQTWEBKIT
  m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(0, 0));
#endif
#ifdef USEQTWEBENGINE
  m_pWebview->scroll(0, 0);
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifndef NOPREVIEW
// Wait until loading has finished
void InyokaEdit::loadPreviewFinished(const bool bSuccess) {
  if (bSuccess) {
    // Enable / disable back button
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
#ifdef USEQTWEBKIT
    m_pWebview->page()->mainFrame()->setScrollPosition(m_WebviewScrollPosition);
#endif
#ifdef USEQTWEBENGINE
    m_pWebview->page()->runJavaScript(QStringLiteral("window.scrollTo(%1, %2);")
                                          .arg(m_WebviewScrollPosition.x())
                                          .arg(m_WebviewScrollPosition.y()));
#endif
    m_bReloadPreviewBlocked = false;
  } else {
    QMessageBox::warning(this, qApp->applicationName(),
                         tr("Error while loading preview."));
    qWarning() << "Error while loading preview:" << m_sPreviewFile;
  }
}

void InyokaEdit::changedUrl() {
  // Disable forward / backward button
  m_pUi->goForwardBrowserAct->setEnabled(false);
  m_pUi->goBackBrowserAct->setEnabled(false);
}

void InyokaEdit::clickedLink(const QUrl &newUrl) {
  if (!newUrl.toString().contains(m_sPreviewFile) && newUrl.isLocalFile()) {
    qDebug() << "Trying to open file:" << newUrl;
    QDesktopServices::openUrl(newUrl);
  } else {
    m_pWebview->load(newUrl);
  }
}
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::updateEditorSettings() {
  m_pParser->updateSettings(m_pSettings->getInyokaUrl(),
                            m_pSettings->getCheckLinks(),
                            m_pSettings->getTimedPreview());

  if (m_pSettings->getPreviewHorizontal()) {
    m_pWidgetSplitter->setOrientation(Qt::Vertical);
  } else {
    m_pWidgetSplitter->setOrientation(Qt::Horizontal);
  }

  m_pPreviewTimer->stop();
  if (m_pSettings->getTimedPreview() != 0) {
    m_pPreviewTimer->start(
        static_cast<int>(m_pSettings->getTimedPreview() * 1000));
  }

  m_pSession->updateSettings(m_pSettings->getInyokaUrl(),
                             m_pSettings->getInyokaUser(),
                             m_pSettings->getInyokaPassword());

  m_pDownloadModule->updateSettings(m_pSettings->getAutomaticImageDownload(),
                                    m_pSettings->getInyokaUrl(),
                                    m_pSettings->getInyokaConstructionArea());

  m_pPlugins->setEditorlist(m_pFileOperations->getEditors());

  m_colorSyntaxError = InyokaEdit::getHighlightErrorColor();

  // Setting proxy if available
  Utils::setProxy(m_pSettings->getProxyHostName(), m_pSettings->getProxyPort(),
                  m_pSettings->getProxyUserName(),
                  m_pSettings->getProxyPassword());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto InyokaEdit::eventFilter(QObject *pObj, QEvent *pEvent) -> bool {
  if (pObj == m_pCurrentEditor && pEvent->type() == QEvent::KeyPress) {
    auto *keyEvent = static_cast<QKeyEvent *>(pEvent);

    if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
      QString sBlock = m_pCurrentEditor->textCursor().block().text();
      if (sBlock.startsWith(QLatin1String(" * ")) && sBlock.length() > 3) {
        m_pCurrentEditor->textCursor().insertText(QStringLiteral("\n * "));
        return true;
      }
      if (sBlock.startsWith(QLatin1String(" * ")) && 3 == sBlock.length()) {
        m_pCurrentEditor->textCursor().deletePreviousChar();
        m_pCurrentEditor->textCursor().deletePreviousChar();
        m_pCurrentEditor->textCursor().deletePreviousChar();
        // return true;
      }
    }

    // Bug fix for LP: #922808
    Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();
    bool bShift(keyMod.testFlag(Qt::ShiftModifier));
    bool bCtrl(keyMod.testFlag(Qt::ControlModifier));

    if (keyEvent->key() == Qt::Key_Right && bShift && bCtrl) {
      // CTRL + SHIFT + arrow right
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
      cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    }
    if (keyEvent->key() == Qt::Key_Right && !bShift && bCtrl) {
      // CTRL + arrow right
      m_pCurrentEditor->moveCursor(QTextCursor::Right);
      m_pCurrentEditor->moveCursor(QTextCursor::EndOfWord);
      return true;
    }
    if (keyEvent->key() == Qt::Key_Up && bShift && bCtrl) {
      // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    }
    if (keyEvent->key() == Qt::Key_Down && bShift && bCtrl) {
      // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    }
    if ((Qt::Key_F5 == keyEvent->key() ||
         m_pSettings->getReloadPreviewKey() == keyEvent->key()) &&
        !m_bReloadPreviewBlocked) {  // Preview F5 or defined button
      m_bReloadPreviewBlocked = true;
      previewInyokaPage();
    }
  }
#ifndef NOPREVIEW
  else if (pObj == m_pWebview && pEvent->type() == QEvent::MouseButtonPress) {
    // Forward / backward mouse button
    auto *mouseEvent = static_cast<QMouseEvent *>(pEvent);

    if (Qt::XButton1 == mouseEvent->button()) {
      m_pWebview->back();
    } else if (Qt::XButton2 == mouseEvent->button()) {
      m_pWebview->forward();
    }
  }
#endif

  return QObject::eventFilter(pObj, pEvent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Delete images in temp folder (images downloaded with articles / from inyzip)
void InyokaEdit::deleteTempImages() {
  int nRet = QMessageBox::question(this, qApp->applicationName(),
                                   tr("Do you really want to delete all "
                                      "temporary article images?"),
                                   QMessageBox::Yes | QMessageBox::No);

  if (QMessageBox::Yes == nRet) {
    // Remove all files in current folder
    const QFileInfoList fiListFiles =
        m_tmpPreviewImgDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    for (const auto &fi : fiListFiles) {
      if (!m_tmpPreviewImgDir.remove(fi.fileName())) {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr("Could not delete file: ") + fi.fileName());
        qWarning() << "Could not delete files:" << fi.fileName();
        return;
      }
    }
    QMessageBox::information(this, qApp->applicationName(),
                             tr("Images successfully deleted."));
  } else {
    return;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Delete old auto save backup files
void InyokaEdit::deleteAutoSaveBackups() {
  QDir dir(m_UserDataDir.absolutePath());
  const QFileInfoList fiListFiles =
      dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
  for (const auto &fi : fiListFiles) {
    if ("bak~" == fi.suffix() &&
        fi.baseName().startsWith(QLatin1String("AutoSave"))) {
      if (!dir.remove(fi.fileName())) {
        qWarning() << "Could not delete auto save backup file:"
                   << fi.fileName();
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::syncScrollbarsEditor() {
#ifdef USEQTWEBKIT
  if (!m_bWebviewScrolling && m_pSettings->getSyncScrollbars()) {
    int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
    int nSizeWebviewBar =
        m_pWebview->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    auto nR = static_cast<float>(nSizeWebviewBar) / nSizeEditorBar;

    m_bEditorScrolling = true;
    m_pWebview->page()->mainFrame()->setScrollPosition(QPoint(
        0, static_cast<int>(
               m_pCurrentEditor->verticalScrollBar()->sliderPosition() * nR)));
    m_bEditorScrolling = false;
  }
#endif
#ifdef USEQTWEBENGINE
  if (!m_bWebviewScrolling && m_pSettings->getSyncScrollbars()) {
    m_pWebview->page()->runJavaScript(
        QStringLiteral("document.documentElement.scrollHeight"),
        [this](const QVariant &v) {
          int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
          int nSizeWebviewBar = v.toInt();
          auto nR = static_cast<float>(nSizeWebviewBar) / nSizeEditorBar;
          nSizeWebviewBar = static_cast<int>(
              m_pCurrentEditor->verticalScrollBar()->sliderPosition() * nR);

          m_bEditorScrolling = true;
          m_pWebview->page()->runJavaScript(
              QStringLiteral("window.scrollTo(0,%1);").arg(nSizeWebviewBar));
          m_bEditorScrolling = false;
        });
  }
#endif
}

// ----------------------------------------------------------------------------

void InyokaEdit::syncScrollbarsWebview() {
#ifdef USEQTWEBKIT
  if (!m_bEditorScrolling && m_pSettings->getSyncScrollbars()) {
    int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
    int nSizeWebviewBar =
        m_pWebview->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    auto nRatio = static_cast<float>(nSizeEditorBar) / nSizeWebviewBar;

    m_bWebviewScrolling = true;
    m_pCurrentEditor->verticalScrollBar()->setSliderPosition(static_cast<int>(
        m_pWebview->page()->mainFrame()->scrollPosition().y() * nRatio));
    m_bWebviewScrolling = false;
  }
#endif
#ifdef USEQTWEBENGINE
  if (!m_bEditorScrolling && m_pSettings->getSyncScrollbars()) {
    m_pWebview->page()->runJavaScript(
        QStringLiteral("document.documentElement.scrollHeight"),
        [this](const QVariant &v) {
          int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
          int nSizeWebviewBar = v.toInt();
          auto nRatio = static_cast<float>(nSizeEditorBar) / nSizeWebviewBar;

          m_bWebviewScrolling = true;
          m_pCurrentEditor->verticalScrollBar()->setSliderPosition(
              static_cast<int>(m_pWebview->page()->scrollPosition().y() *
                               nRatio));
          m_bWebviewScrolling = false;
        });
  }
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::loadLanguage(const QString &sLang) {
  if (m_sCurrLang != sLang) {
    m_sCurrLang = sLang;
    if (!InyokaEdit::switchTranslator(&m_translatorQt, "qt_" + sLang,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                                      QLibraryInfo::path(
#else
                                      QLibraryInfo::location(
#endif
                                          QLibraryInfo::TranslationsPath))) {
      InyokaEdit::switchTranslator(&m_translatorQt, "qt_" + sLang,
                                   m_sSharePath + "/lang");
    }

    if (!InyokaEdit::switchTranslator(
            &m_translator,
            ":/" + qApp->applicationName().toLower() + "_" + sLang + ".qm")) {
      InyokaEdit::switchTranslator(
          &m_translator, qApp->applicationName().toLower() + "_" + sLang,
          m_sSharePath + "/lang");
    }
  }
  m_pUi->retranslateUi(this);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

auto InyokaEdit::switchTranslator(QTranslator *translator, const QString &sFile,
                                  const QString &sPath) -> bool {
  qApp->removeTranslator(translator);
  if (translator->load(sFile, sPath)) {
    qApp->installTranslator(translator);
  } else {
    qWarning() << "Could not find translation" << sFile << "in" << sPath;
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void InyokaEdit::changeEvent(QEvent *pEvent) {
  if (nullptr != pEvent) {
    if (QEvent::LanguageChange == pEvent->type()) {
      m_pUi->retranslateUi(this);
      this->createXmlMenus();
      emit updateUiLang();
    }
  }
  QMainWindow::changeEvent(pEvent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifndef NOPREVIEW
void InyokaEdit::showSyntaxOverview() {
  auto *pDialog =
      new QDialog(this, this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  auto *pLayout = new QGridLayout(pDialog);
#ifdef USEQTWEBKIT
  auto *pWebview = new QWebView();
#endif
#ifdef USEQTWEBENGINE
  auto *pWebview = new QWebEngineView();
#endif
  auto *pTextDocument = new QTextDocument(this);

  QFile OverviewFile(m_sSharePath + "/community/" +
                     m_pSettings->getInyokaCommunity() + "/SyntaxOverview.tpl");

  QTextStream in(&OverviewFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  // Since Qt 6 UTF-8 is used by default
  in.setCodec("UTF-8");
#endif
  if (!OverviewFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, QStringLiteral("Warning"),
                         tr("Could not open syntax overview file!"));
    qWarning() << "Could not open syntax overview file:"
               << OverviewFile.fileName();
    delete pLayout;
    pLayout = nullptr;
    delete pWebview;
    pWebview = nullptr;
    delete pTextDocument;
    pTextDocument = nullptr;
    return;
  }
  pTextDocument->setPlainText(in.readAll());
  OverviewFile.close();

  QString sRet(m_pParser->genOutput(QLatin1String(""), pTextDocument));
  sRet.remove(
      QRegularExpression(QStringLiteral("<h1 class=\"pagetitle\">.*</h1>"),
                         QRegularExpression::DotMatchesEverythingOption));
  sRet.remove(
      QRegularExpression(QStringLiteral("<p class=\"meta\">.*</p>"),
                         QRegularExpression::DotMatchesEverythingOption));
  sRet.replace(QLatin1String("</style>"),
               QLatin1String("#page table{margin:0px;}</style>"));
  pTextDocument->setPlainText(sRet);

  pLayout->setContentsMargins(2, 2, 2, 2);
  pLayout->setSpacing(0);
  pLayout->addWidget(pWebview);
  pDialog->setWindowTitle(tr("Syntax overview"));

  pWebview->setHtml(pTextDocument->toPlainText(),
                    QUrl::fromLocalFile(m_UserDataDir.absolutePath() + "/"));
  pDialog->show();
}
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::showAbout() {
  QMessageBox::about(
      this, tr("About") + " " + qApp->applicationName(),
      QString::fromLatin1("<big><b>%1 %2</b></big><br />"
                          "%3<br />"
                          "<small>%4</small><br /><br />"
                          "%5<br />"
                          "%6<br />"
                          "<small>%7</small><br /><br />"
                          "%8")
          .arg(qApp->applicationName(), qApp->applicationVersion(),
               tr("Editor for Inyoka-based portals"), APP_COPY,
               "URL: <a href=\"https://github.com/inyokaproject/inyokaedit\">"
               "https://github.com/inyokaproject/inyokaedit</a>",
               tr("License") +
                   ": <a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">"
                   "GNU General Public License Version 3</a>",
               tr("This application uses icons from Tango project of "
                  "<a href=\"https://www.freedesktop.org\">"
                  "freedesktop.org</a>."),
               tr("Special thanks to all contributors and testers from "
                  "<a href=\"https://ubuntuusers.de\"> "
                  "ubuntuusers.de</a>.")));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Close event (File -> Close or X)
void InyokaEdit::closeEvent(QCloseEvent *pEvent) {
  if (m_pFileOperations->closeAllmaybeSave()) {
    m_pSettings->writeSettings(saveGeometry(), saveState(),
                               m_pWidgetSplitter->saveState());
    pEvent->accept();
  } else {
    pEvent->ignore();
  }
}
