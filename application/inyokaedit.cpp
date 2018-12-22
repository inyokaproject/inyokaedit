/**
 * \file inyokaedit.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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

#include "./inyokaedit.h"

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

#include "./xmlparser.h"
#include "ui_inyokaedit.h"

InyokaEdit::InyokaEdit(const QDir &userDataDir, const QDir &sharePath,
                       QWidget *parent)
  : QMainWindow(parent),
    m_pUi(new Ui::InyokaEdit),
    m_sCurrLang(""),
    m_pSigMapXmlActions(NULL),
    m_sSharePath(sharePath.absolutePath()),
    m_UserDataDir(userDataDir),
    m_sPreviewFile(m_UserDataDir.absolutePath() + "/tmpinyoka.html"),
    m_tmpPreviewImgDir(m_UserDataDir.absolutePath() + "/tmpImages"),
    m_pPreviewTimer(new QTimer(this)),
    m_bOpenFileAfterStart(false),
    m_bEditorScrolling(false),
    m_bWebviewScrolling(false),
    m_bReloadPreviewBlocked(false) {
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
  this->createXmlMenus();
  this->setUnifiedTitleAndToolBarOnMac(true);

  if (!QFile(m_UserDataDir.absolutePath() + "/community/" +
             m_pSettings->getInyokaCommunity()).exists()) {
    m_UserDataDir.mkpath(m_UserDataDir.absolutePath() + "/community/" +
                         m_pSettings->getInyokaCommunity());
  }

  if (Utils::getOnlineState() && m_pSettings->getWindowsCheckUpdate()) {
    m_pUtils->checkWindowsUpdate();
  }

  // Load file via command line
  if (m_bOpenFileAfterStart) {
    m_pFileOperations->loadFile(sFile, true);
  }

  m_pPlugins->loadPlugins(m_pSettings->getGuiLanguage());
  this->updateEditorSettings();
  this->deleteAutoSaveBackups();
  m_pCurrentEditor->setFocus();
}

InyokaEdit::~InyokaEdit() {
  if (NULL != m_pUi) {
    delete m_pUi;
  }
  m_pUi = NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::createObjects() {
  m_pSettings = new Settings(this, m_sSharePath);
  qDebug() << "Inyoka Community:" << m_pSettings->getInyokaCommunity();
  if (m_pSettings->getInyokaCommunity().isEmpty() ||
      !QDir(m_sSharePath + "/community/" +
            m_pSettings->getInyokaCommunity()).exists()) {
    qCritical() << "No Inyoka community files found / installed!";
    qCritical() << "Community path:" << m_sSharePath + "/community/" +
                   m_pSettings->getInyokaCommunity();
    QMessageBox::critical(this, qApp->applicationName(),
                          tr("No Inyoka community files found/installed!\n"
                             "Please check your installation and restart "
                             "the application."));
    exit(-2);
  }
  connect(m_pSettings, &Settings::changeLang,
          this, &InyokaEdit::loadLanguage);
  connect(this, &InyokaEdit::updateUiLang,
          m_pSettings, &Settings::updateUiLang);
  this->loadLanguage(m_pSettings->getGuiLanguage());

  // Has to be created before parser
  m_pTemplates = new Templates(m_pSettings->getInyokaCommunity(),
                               m_sSharePath, m_UserDataDir.absolutePath());

  m_pSession = new Session(this, m_pSettings->getInyokaUrl(),
                           m_pSettings->getInyokaHash());

  m_pDownloadModule = new Download(this, m_pSession,
                                   m_UserDataDir.absolutePath(),
                                   m_tmpPreviewImgDir.absolutePath(),
                                   m_sSharePath);

  m_pUploadModule = new Upload(this, m_pSession, m_pSettings->getInyokaUrl(),
                               m_pSettings->getInyokaConstructionArea());

  m_pParser = new Parser(m_sSharePath, m_tmpPreviewImgDir,
                         m_pSettings->getInyokaUrl(),
                         m_pSettings->getCheckLinks(),
                         m_pTemplates,
                         m_pSettings->getInyokaCommunity());
  connect(m_pParser, &Parser::hightlightSyntaxError,
          this, &InyokaEdit::highlightSyntaxError);

  m_pDocumentTabs = new QTabWidget;
  m_pDocumentTabs->setTabPosition(QTabWidget::North);
  m_pDocumentTabs->setTabsClosable(true);
  m_pDocumentTabs->setDocumentMode(true);
  // Attention: Currently tab order is fixed (same as m_pListEditors)
  m_pDocumentTabs->setMovable(false);

  m_pFileOperations = new FileOperations(this, m_pDocumentTabs, m_pSettings,
                                         m_sPreviewFile,
                                         m_UserDataDir.absolutePath(),
                                         m_pTemplates->getListTplMacrosALL());
  m_pCurrentEditor = m_pFileOperations->getCurrentEditor();

  connect(m_pFileOperations, &FileOperations::callPreview,
          this, &InyokaEdit::previewInyokaPage);
  connect(m_pFileOperations, &FileOperations::modifiedDoc,
          this, &InyokaEdit::setWindowModified);
  connect(m_pFileOperations, &FileOperations::changedCurrentEditor,
          this, &InyokaEdit::setCurrentEditor);

  m_pPlugins = new Plugins(this, m_pCurrentEditor,
                           m_pSettings->getDisabledPlugins(),
                           m_UserDataDir,
                           m_sSharePath);
  connect(m_pSettings, &Settings::changeLang, m_pPlugins, &Plugins::changeLang);
  connect(m_pPlugins, &Plugins::addMenuToolbarEntries,
          this, &InyokaEdit::addPluginsButtons);
  connect(m_pPlugins, &Plugins::availablePlugins,
          m_pSettings, &Settings::availablePlugins);

  this->setCurrentEditor();

  // TODO(volunteer): Find solution for QWebEngineView
#ifdef USEQTWEBKIT
  m_pWebview = new QWebView(this);
  connect(m_pWebview->page(), &QWebPage::scrollRequested,
          this, &InyokaEdit::syncScrollbarsWebview);
  connect(m_pFileOperations, &FileOperations::movedEditorScrollbar,
          this, &InyokaEdit::syncScrollbarsEditor);
#else
  m_pWebview = new QWebEngineView(this);
#endif
  m_pWebview->installEventFilter(this);

  m_pUtils = new Utils(this);
  connect(m_pUtils, &Utils::setWindowsUpdateCheck,
          m_pSettings, &Settings::setWindowsCheckUpdate);
}

// ----------------------------------------------------------------------------

void InyokaEdit::setCurrentEditor() {
  m_pCurrentEditor = m_pFileOperations->getCurrentEditor();
  m_pPlugins->setCurrentEditor(m_pCurrentEditor);
  m_pUploadModule->setEditor(m_pCurrentEditor, m_pCurrentEditor->getFileName());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::setupEditor() {
  qDebug() << "Calling" << Q_FUNC_INFO;

  // Timed preview
  connect(m_pPreviewTimer, &QTimer::timeout,
          this, &InyokaEdit::previewInyokaPage);

  m_pFrameLayout = new QBoxLayout(QBoxLayout::LeftToRight);
  m_pFrameLayout->addWidget(m_pWebview);

  connect(m_pWebview, &QWebView::loadFinished,
          this, &InyokaEdit::loadPreviewFinished);

  m_pWidgetSplitter = new QSplitter;

  m_pWidgetSplitter->addWidget(m_pDocumentTabs);
  m_pWidgetSplitter->addWidget(m_pWebview);
  setCentralWidget(m_pWidgetSplitter);
  m_pWidgetSplitter->restoreState(m_pSettings->getSplitterState());

  this->updateEditorSettings();
  connect(m_pSettings, &Settings::updateEditorSettings,
          this, &InyokaEdit::updateEditorSettings);
  connect(m_pFileOperations, &FileOperations::newEditor,
          this, &InyokaEdit::updateEditorSettings);

  // Show an empty website after start
  if (!m_bOpenFileAfterStart) {
    this->previewInyokaPage();
  }

  connect(m_pDownloadModule, &Download::sendArticleText,
          this, &InyokaEdit::displayArticleText);

  // Restore window and toolbar settings
  // Settings have to be restored after toolbars are created!
  this->restoreGeometry(m_pSettings->getWindowGeometry());
  // Restore toolbar position etc.
  this->restoreState(m_pSettings->getWindowState());

  m_pUi->aboutAct->setText(
        m_pUi->aboutAct->text() + " " + qApp->applicationName());

  // Browser buttons
  connect(m_pUi->goBackBrowserAct, &QAction::triggered,
          m_pWebview, &QWebView::back);
  connect(m_pUi->goForwardBrowserAct, &QAction::triggered,
          m_pWebview, &QWebView::forward);
  connect(m_pUi->reloadBrowserAct, &QAction::triggered,
          m_pWebview, &QWebView::reload);

  connect(m_pWebview, &QWebView::urlChanged, this, &InyokaEdit::changedUrl);

  // TODO(volunteer): Find solution for QWebEngineView
  // QWebEngineUrlRequestInterceptor ?
#ifdef USEQTWEBKIT
  m_pWebview->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(m_pWebview, &QWebView::linkClicked, this, &InyokaEdit::clickedLink);
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
  connect(m_pUi->newAct, &QAction::triggered,
          m_pFileOperations,
          [this]() { m_pFileOperations->newFile(QStringLiteral("")); });
  // Open file
  m_pUi->openAct->setShortcuts(QKeySequence::Open);
  connect(m_pUi->openAct, &QAction::triggered,
          this, &InyokaEdit::openFile);
  // Save file
  m_pUi->saveAct->setShortcuts(QKeySequence::Save);
  connect(m_pUi->saveAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::save);
  // Save file as...
  m_pUi->saveAsAct->setShortcuts(QKeySequence::SaveAs);
  connect(m_pUi->saveAsAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::saveAs);
  // Print preview
  m_pUi->printPreviewAct->setShortcut(QKeySequence::Print);
  connect(m_pUi->printPreviewAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::printPreview);
  // TODO(volunteer): Check print functionality again with Qt 5.7
#if QT_VERSION >= 0x050600
  m_pUi->printPreviewAct->setEnabled(false);
#endif

  // Exit application
  m_pUi->exitAct->setShortcuts(QKeySequence::Quit);
  connect(m_pUi->exitAct, &QAction::triggered, this, &InyokaEdit::close);

  // ------------------------------------------------------------------------
  // EDIT MENU

  // Find
  m_pUi->searchAct->setShortcuts(QKeySequence::Find);
  connect(m_pUi->searchAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::triggeredFind);
  // Replace
  m_pUi->replaceAct->setShortcuts(QKeySequence::Replace);
  connect(m_pUi->replaceAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::triggeredReplace);
  // Find next
  m_pUi->findNextAct->setShortcuts(QKeySequence::FindNext);
  connect(m_pUi->findNextAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::triggeredFindNext);
  // Find previous
  m_pUi->findPreviousAct->setShortcuts(QKeySequence::FindPrevious);
  connect(m_pUi->findPreviousAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::triggeredFindPrevious);

  // Cut
  m_pUi->cutAct->setShortcuts(QKeySequence::Cut);
  connect(m_pUi->cutAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::cut);
  // Copy
  m_pUi->copyAct->setShortcuts(QKeySequence::Copy);
  connect(m_pUi->copyAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::copy);
  // Paste
  m_pUi->pasteAct->setShortcuts(QKeySequence::Paste);
  connect(m_pUi->pasteAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::paste);

  connect(m_pFileOperations, &FileOperations::copyAvailable,
          m_pUi->cutAct, &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::copyAvailable,
          m_pUi->copyAct, &QAction::setEnabled);
  m_pUi->cutAct->setEnabled(false);
  m_pUi->copyAct->setEnabled(false);

  // Undo
  m_pUi->undoAct->setShortcuts(QKeySequence::Undo);
  connect(m_pUi->undoAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::undo);
  // Redo
  m_pUi->redoAct->setShortcuts(QKeySequence::Redo);
  connect(m_pUi->redoAct, &QAction::triggered,
          m_pFileOperations, &FileOperations::redo);

  connect(m_pFileOperations, &FileOperations::undoAvailable,
          m_pUi->undoAct, &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::redoAvailable,
          m_pUi->redoAct, &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::undoAvailable2,
          m_pUi->undoAct, &QAction::setEnabled);
  connect(m_pFileOperations, &FileOperations::redoAvailable2,
          m_pUi->redoAct, &QAction::setEnabled);
  m_pUi->undoAct->setEnabled(false);
  m_pUi->redoAct->setEnabled(false);

  // ------------------------------------------------------------------------
  // TOOLS MENU

  // Clear temp. image download folder
  connect(m_pUi->deleteTempImagesAct, &QAction::triggered,
          this, &InyokaEdit::deleteTempImages);

  // Show settings dialog
  connect(m_pUi->preferencesAct, &QAction::triggered,
          m_pSettings, &Settings::showSettingsDialog);

  // ------------------------------------------------------------------------

  // Show html preview
  m_pUi->previewAct->setShortcut(Qt::CTRL + Qt::Key_P);
  connect(m_pUi->previewAct, &QAction::triggered,
          this, &InyokaEdit::previewInyokaPage);

  // Download Inyoka article
  connect(m_pUi->downloadArticleAct, &QAction::triggered,
          m_pDownloadModule, [this] () {
    m_pDownloadModule->downloadArticle(QStringLiteral("")); });

  // Upload Inyoka article
  connect(m_pUi->uploadArticleAct, &QAction::triggered,
          m_pUploadModule, &Upload::clickUploadArticle);

  // ------------------------------------------------------------------------
  // ABOUT MENU

  // Show syntax overview
  connect(m_pUi->showSyntaxOverviewAct, &QAction::triggered,
          this, &InyokaEdit::showSyntaxOverview);

  // Report a bug
  connect(m_pUi->reportBugAct, &QAction::triggered,
          this, []() {
    QDesktopServices::openUrl(
          QUrl("https://github.com/inyokaproject/inyokaedit/issues")); });

  // Open about windwow
  connect(m_pUi->aboutAct, &QAction::triggered, m_pUtils, &Utils::showAbout);
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
        connect(m_OpenTemplateFilesActions.last(), &QAction::triggered,
                m_pSigMapOpenTemplate,
                static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
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

  connect(m_pSigMapOpenTemplate,
          static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          m_pFileOperations->m_pSigMapOpenTemplate,
          static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped));

  if (0 == m_OpenTemplateFilesActions.size()) {
    m_pUi->fileMenuFromTemplate->setDisabled(true);
  }

  // File menu (recent opened files)
  m_pUi->fileMenuLastOpened->addActions(
        m_pFileOperations->getLastOpenedFiles());
  if (0 == m_pSettings->getRecentFiles().size()) {
    m_pUi->fileMenuLastOpened->setEnabled(false);
  }
  connect(m_pFileOperations, &FileOperations::setMenuLastOpenedEnabled,
          m_pUi->fileMenuLastOpened, &QMenu::setEnabled);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::clearXmlMenus() {
  if (NULL != m_pSigMapXmlActions) {
    disconnect(m_pSigMapXmlActions, 0, 0, 0);
    foreach (QMenu *m, m_pXmlSubMenus) {
      m->clear();
    }
    foreach (QMenu *m, m_pXmlMenus) {
      m->clear();
      m->deleteLater();
    }
    foreach (QComboBox *q, m_pXmlDropdowns) {
      q->clear();
    }

    foreach (QToolBar *t, m_pXmlToolbars) {
      t->clear();
    }

    foreach (QAction *a, m_pXmlActions) {
      m_pUi->inyokaeditorBar->removeAction(a);
      m_pUi->menuBar->removeAction(a);
      m_pUi->samplesmacrosBar->removeAction(a);
      foreach (QToolButton *b, m_pXmlToolbuttons) {
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
    delete m_pSigMapXmlActions;
  }
  m_pSigMapXmlActions = NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::createXmlMenus() {
  XmlParser xmlParser;
  QFile xmlFile;
  QList<QAction *> tmplListActions;
  QStringList sListFolders;
  QString sTmpPath;
  sListFolders << m_sSharePath << m_UserDataDir.absolutePath();
  QStringList sListObjects;
  sListObjects << "menu" << "dropdown" << "toolbar";

  this->clearXmlMenus();

  m_pSigMapXmlActions = new QSignalMapper(this);

  // Check share and user path
  foreach (QString sPath, sListFolders) {
    // Search for menu/dropdown/toolbar
    foreach (QString sObj, sListObjects) {
      // Search for max 9 files
      for (int n = 1; n < 9; n++) {
        sTmpPath = sPath + "/community/" +
                   m_pSettings->getInyokaCommunity() + "/xml/";
        // File name e.g. menu_1_de.xml
        xmlFile.setFileName(
              sTmpPath + sObj + "_" + QString::number(n) + "_" +
              m_pSettings->getGuiLanguage() + ".xml");

        if (!xmlFile.exists() && m_pSettings->getGuiLanguage() != "en") {
          if (1 == n && sPath == m_sSharePath) {
            qWarning() << "Xml menu file not found:" << xmlFile.fileName() <<
                          "- Trying to load English fallback.";
          }
          // Try English fallback
          QString sTemp(xmlFile.fileName());
          sTemp.replace("_" + m_pSettings->getGuiLanguage() + ".xml",
                        "_en.xml");
          xmlFile.setFileName(sTemp);
        }

        if (xmlFile.exists()) {
          // qDebug() << "Read XML" << xmlFile.fileName();
          if (xmlParser.parseXml(xmlFile.fileName())) {
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
                      static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
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
              for (int j = 0;
                   j < xmlParser.getElementNames().at(i).size();
                   j++) {
                // qDebug() << "ELEMENTS" << xmlParser.getElementNames()[i][j];
                m_pXmlActions.append(
                      new QAction(QIcon(
                                    sTmpPath + xmlParser.getPath() + "/" +
                                    xmlParser.getElementIcons().at(i).at(j)),
                                  xmlParser.getElementNames().at(i).at(j),
                                  this));
                tmplListActions.append(m_pXmlActions.last());

                if ("dropdown" == sObj) {
                  m_pXmlDropdowns.last()->addItem(
                        xmlParser.getElementNames().at(i).at(j),
                        QVariant::fromValue(m_pXmlActions.last()));
                }

                // qDebug() << "INSERT" << xmlParser.getElementInserts()[i][j];
                if (xmlParser.getElementInserts().at(i).at(j).endsWith(
                      ".tpl", Qt::CaseInsensitive) ||
                    xmlParser.getElementInserts().at(i).at(j).endsWith(
                      ".macro", Qt::CaseInsensitive)) {
                  QFile tmp(sTmpPath + xmlParser.getPath() + "/" +
                            xmlParser.getElementInserts().at(i).at(j));
                  if (tmp.exists()) {
                    m_pSigMapXmlActions->setMapping(m_pXmlActions.last(),
                                                    tmp.fileName());
                  } else {
                    qWarning() << "Tpl/Macro file not found for XML menu:" <<
                                  tmp.fileName();
                  }
                } else {
                  m_pSigMapXmlActions->setMapping(
                        m_pXmlActions.last(),
                        xmlParser.getElementInserts().at(i).at(j));
                }
                connect(m_pXmlActions.last(), &QAction::triggered,
                        m_pSigMapXmlActions,
                        static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
              }
              if ("menu" == sObj) {
                if (xmlParser.getGroupNames().at(i).isEmpty()) {
                  m_pXmlMenus.last()->addActions(tmplListActions);
                } else {
                  m_pXmlSubMenus.append(
                        new QMenu(xmlParser.getGroupNames().at(i), this));
                  m_pXmlSubMenus.last()->setIcon(
                        QIcon(sTmpPath + xmlParser.getPath() +
                              "/" + xmlParser.getGroupIcons().at(i)));
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
                        QIcon(sTmpPath + xmlParser.getPath() +
                              "/" + xmlParser.getGroupIcons().at(i)));
                  m_pXmlToolbuttons.last()->setPopupMode(
                        QToolButton::InstantPopup);
                  m_pXmlSubMenus.append(new QMenu(
                                          xmlParser.getGroupNames().at(i),
                                          this));
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
  connect(m_pSigMapXmlActions,
          static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),
          this, &InyokaEdit::insertMacro);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Add plugins to plugin toolbar and tools menu
void InyokaEdit::addPluginsButtons(const QList<QAction *> &ToolbarEntries,
                                   QList<QAction *> &MenueEntries) {
  m_pUi->pluginsBar->addActions(ToolbarEntries);
  if (m_pUi->toolsMenu->actions().size() > 0) {
    QAction *separator = new QAction(this);
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
#else
  m_pWebview->scroll(0, 0);
#endif
  m_pFileOperations->open();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Call parser
void InyokaEdit::previewInyokaPage() {
  m_pWebview->history()->clear();  // Clear history (clicked links)

  QString sRetHTML("");
  sRetHTML = m_pParser->genOutput(m_pFileOperations->getCurrentFile(),
                                  m_pCurrentEditor->document(),
                                  m_pSettings->getSyntaxCheck());

  // File for temporary html output
  QFile tmphtmlfile(m_sPreviewFile);

  // No write permission
  if (!tmphtmlfile.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, qApp->applicationName(),
                         tr("Could not create temporary HTML file!"));
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
  // TODO(volunteer): Find solution for QWebEngineView
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

void InyokaEdit::highlightSyntaxError(const qint32 nPos) {
  QList<QTextEdit::ExtraSelection> extras;
  QTextEdit::ExtraSelection selection;

  selection.format.setBackground(m_colorSyntaxError);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);

  extras.clear();
  if (-1 != nPos) {
    selection.cursor = m_pCurrentEditor->textCursor();
    selection.cursor.setPosition(nPos);
    selection.cursor.clearSelection();
    extras << selection;
    m_pCurrentEditor->setTextCursor(selection.cursor);
  }

  m_pCurrentEditor->setExtraSelections(extras);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QColor InyokaEdit::getHighlightErrorColor() {
#if defined _WIN32
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(),
                     qApp->applicationName().toLower());
#else
  QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(),
                     qApp->applicationName().toLower());
#endif
  QString sStyle = settings.value("Plugin_highlighter/Style",
                                  "standard-style").toString();

#if defined _WIN32
  QSettings styleset(QSettings::IniFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(), sStyle);
#else
  QSettings styleset(QSettings::NativeFormat, QSettings::UserScope,
                     qApp->applicationName().toLower(), sStyle);
#endif
  sStyle = styleset.value("Style/SyntaxError",
                          "---|---|---|0xffff00").toString();

  QColor color(sStyle.right(8).replace("0x", "#"));
  if (color.isValid()) {
    return color;
  }
  return Qt::yellow;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void InyokaEdit::dropdownXmlChanged(int nIndex) {
  QComboBox *tmpCombo = qobject_cast<QComboBox *>(sender());
  if (tmpCombo != NULL) {
    QAction *action = tmpCombo->itemData(nIndex,
                                          Qt::UserRole).value<QAction *>();
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
  QString sMacro("");
  QString sTmp("");

  if (sInsert.endsWith(".tpl", Qt::CaseInsensitive) ||
      sInsert.endsWith(".macro", Qt::CaseInsensitive)) {
    QFile tplFile(sInsert);
    if (tplFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&tplFile);
      in.setCodec("UTF-8");
      sMacro = in.readLine().trimmed();  // First line HAS to include ## Macro
      sMacro = sMacro.remove("## Macro=");
      tplFile.close();
    } else {
      QMessageBox::warning(this, "Warning", "Could not open macro file: \n" +
                           tplFile.fileName());
      qWarning() << "Could not open macro file:" << tplFile.fileName();
      return;
    }

    if (sMacro.isEmpty()) {
      QMessageBox::warning(this, "Warning", "Macro file was empty!");
      qWarning() << "Macro file was empty:" << tplFile.fileName();
      return;
    }
  } else {
    sMacro = sInsert;
  }

  sMacro.replace("\\n", "\n");
  int nPlaceholder1(sMacro.indexOf("%%"));
  int nPlaceholder2(sMacro.lastIndexOf("%%"));

  // No text selected
  if (m_pCurrentEditor->textCursor().selectedText().isEmpty()) {
    int nCurrentPos = m_pCurrentEditor->textCursor().position();
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
      // No placeholder defined (or problem with placeholder)
      m_pCurrentEditor->insertPlainText(sMacro.remove("%%"));
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
#else
  m_pWebview->scroll(0, 0);
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Wait until loading has finished
void InyokaEdit::loadPreviewFinished(const bool bSuccess) {
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
    // TODO(volunteer): Find solution for QWebEngineView
#ifdef USEQTWEBKIT
    m_pWebview->page()->mainFrame()->setScrollPosition(m_WebviewScrollPosition);
#else
    m_pWebview->scroll(0, 0);
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

void InyokaEdit::updateEditorSettings() {
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
                                    m_pSettings->getInyokaConstructionArea());

  m_pPlugins->setEditorlist(m_pFileOperations->getEditors());

  m_colorSyntaxError = this->getHighlightErrorColor();

  // Setting proxy if available
  Utils::setProxy(m_pSettings->getProxyHostName(),
                  m_pSettings->getProxyPort(),
                  m_pSettings->getProxyUserName(),
                  m_pSettings->getProxyPassword());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool InyokaEdit::eventFilter(QObject *pObj, QEvent *pEvent) {
  if (pObj == m_pCurrentEditor && pEvent->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(pEvent);

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
    } else if (keyEvent->key() == Qt::Key_Right && !bShift && bCtrl) {
      // CTRL + arrow right
      m_pCurrentEditor->moveCursor(QTextCursor::Right);
      m_pCurrentEditor->moveCursor(QTextCursor::EndOfWord);
      return true;
    } else if (keyEvent->key() == Qt::Key_Up && bShift && bCtrl) {
      // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    } else if (keyEvent->key() == Qt::Key_Down && bShift && bCtrl) {
      // CTRL + SHIFT arrow down (Bug fix for LP: #889321)
      QTextCursor cursor(m_pCurrentEditor->textCursor());
      cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
      m_pCurrentEditor->setTextCursor(cursor);
      return true;
    } else if ((Qt::Key_F5 == keyEvent->key() ||
                m_pSettings->getReloadPreviewKey() == keyEvent->key()) &&
               !m_bReloadPreviewBlocked) {  // Preview F5 or defined button
      m_bReloadPreviewBlocked = true;
      previewInyokaPage();
    }
  } else if (pObj == m_pWebview && pEvent->type() == QEvent::MouseButtonPress) {
    // Forward / backward mouse button
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(pEvent);

    if (Qt::XButton1 == mouseEvent->button()) {
      m_pWebview->back();
    } else if (Qt::XButton2 == mouseEvent->button()) {
      m_pWebview->forward();
    }
  }

  return QObject::eventFilter(pObj, pEvent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Delete images in temp folder (images downloaded with articles / from inyzip)
void InyokaEdit::deleteTempImages() {
  int nRet = QMessageBox::question(this, qApp->applicationName(),
                                   tr("Do you really want to delete all "
                                      "temporay article images?"),
                                   QMessageBox::Yes | QMessageBox::No);

  if (QMessageBox::Yes== nRet) {
    // Remove all files in current folder
    QFileInfoList fiListFiles = m_tmpPreviewImgDir.entryInfoList(
                                  QDir::NoDotAndDotDot | QDir::Files);
    foreach (QFileInfo fi, fiListFiles) {
      if (!m_tmpPreviewImgDir.remove(fi.fileName())) {
        QMessageBox::warning(this, qApp->applicationName(),
                             tr("Could not delete file: ") + fi.fileName());
        qWarning() << "Could not delete files:" <<
                      fi.fileName();
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

void InyokaEdit::syncScrollbarsEditor() {
  // TODO(volunteer): Find solution for QWebEngineView
#ifdef USEQTWEBKIT
  if (!m_bWebviewScrolling && true == m_pSettings->getSyncScrollbars()) {
    int nSizeEditorBar = m_pCurrentEditor->verticalScrollBar()->maximum();
    int nSizeWebviewBar = m_pWebview->page()->mainFrame()->scrollBarMaximum(
                            Qt::Vertical);
    float nR = static_cast<float>(nSizeWebviewBar) / nSizeEditorBar;

    m_bEditorScrolling = true;
    m_pWebview->page()->mainFrame()->setScrollPosition(
          QPoint(0,
                 m_pCurrentEditor->verticalScrollBar()->sliderPosition() * nR));
    m_bEditorScrolling = false;
  }
#endif
}

// ----------------------------------------------------------------------------

void InyokaEdit::syncScrollbarsWebview() {
  // TODO(volunteer): Find solution for QWebEngineView
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

void InyokaEdit::loadLanguage(const QString &sLang) {
  if (m_sCurrLang != sLang) {
    m_sCurrLang = sLang;
    if (!this->switchTranslator(&m_translatorQt, "qt_" + sLang,
                                QLibraryInfo::location(
                                  QLibraryInfo::TranslationsPath))) {
      this->switchTranslator(&m_translatorQt, "qt_" + sLang,
                             m_sSharePath + "/lang");
    }

    if (!this->switchTranslator(
          &m_translator,
          ":/" + qApp->applicationName().toLower() + "_" + sLang + ".qm")) {
      this->switchTranslator(
            &m_translator, qApp->applicationName().toLower() + "_" + sLang,
            m_sSharePath + "/lang");
    }
  }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool InyokaEdit::switchTranslator(QTranslator *translator,
                                  const QString &sFile, const QString &sPath) {
  qApp->removeTranslator(translator);
  if (translator->load(sFile, sPath)) {
    qApp->installTranslator(translator);
  } else {
    if (!sFile.endsWith("_en") && !sFile.endsWith("_en.qm")) {
      // EN is build in translation -> no file
      qWarning() << "Could not find translation" << sFile << "in" << sPath;
    }
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void InyokaEdit::changeEvent(QEvent *pEvent) {
  if (0 != pEvent) {
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

void InyokaEdit::showSyntaxOverview() {
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
                         tr("Could not open syntax overview file!"));
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
  dialog->setWindowTitle(tr("Syntax overview"));

  webview->setHtml(pTextDocument->toPlainText(),
                   QUrl::fromLocalFile(m_UserDataDir.absolutePath() + "/"));
  dialog->show();
}

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
