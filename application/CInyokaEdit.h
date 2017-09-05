/**
 * \file CInyokaEdit.h
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
 * Class definition main application.
 */

#ifndef INYOKAEDIT_CINYOKAEDIT_H_
#define INYOKAEDIT_CINYOKAEDIT_H_

#include <QBoxLayout>
#include <QMainWindow>
#include <QSplitter>
#include <QToolButton>

#include "./CDownload.h"
#include "./CFileOperations.h"
#include "./parser/CParser.h"
#include "./CPlugins.h"
#include "./CSettings.h"
#include "./templates/CTemplates.h"
#include "./CTextEditor.h"
#include "./CUtils.h"

// Qt classes
class QComboBox;
class QSignalMapper;
class QFile;
class QDir;
#ifdef USEQTWEBKIT
class QWebView;
#else
class QWebEngineView;
#endif

class CDownload;

namespace Ui {
  class CInyokaEdit;
}

/**
 * \class CInyokaEdit
 * \brief Main application definition (gui, objects, etc.)
 */
class CInyokaEdit : public QMainWindow {
  Q_OBJECT

 public:
  explicit CInyokaEdit(const QDir &userDataDir, const QDir &sharePath,
                       QWidget *parent = 0);
  ~CInyokaEdit();

 public slots:
  void displayArticleText(const QString &sArticleText,
                          const QString &sSitename);
  void addPluginsButtons(QList<QAction *> ToolbarEntries,
                         QList<QAction *> MenueEntries);
  void updateEditorSettings();

 protected:
  void closeEvent(QCloseEvent *event);
  bool eventFilter(QObject *obj, QEvent *event);

 private slots:
  void showSyntaxOverview();
  void openFile();
  void setCurrentEditor();
  void insertMacro(const QString &sInsert);
  void dropdownXmlChanged(int nIndex);
  void deleteTempImages();
  void highlightSyntaxError(const qint32 nPos);
  QColor getHighlightErrorColor();
  // Preview
  void previewInyokaPage();
  void loadPreviewFinished(const bool bSuccess);
  void changedUrl();
  void clickedLink(QUrl newUrl);
  void syncScrollbarsEditor();
  void syncScrollbarsWebview();

 private:
  void createObjects();  
  void createActions();
  void createMenus();
  void setupEditor();
  void createXmlMenus();
  void deleteAutoSaveBackups();
  void readSettings();
  void writeSettings();

  Ui::CInyokaEdit *m_pUi;
  CTemplates *m_pTemplates;
  CFileOperations *m_pFileOperations;
  CTextEditor *m_pCurrentEditor;
  CPlugins *m_pPlugins;
  CParser *m_pParser;
  CSettings *m_pSettings;
  CDownload *m_pDownloadModule;
  CUtils *m_pUtils;
  QSplitter *m_pWidgetSplitter;
  QBoxLayout *m_pFrameLayout;
  QTabWidget *m_pDocumentTabs;
  QPoint m_WebviewScrollPosition;
#ifdef USEQTWEBKIT
  QWebView *m_pWebview;
#else
  QWebEngineView *m_pWebview;
#endif

  QList<QAction *> m_OpenTemplateFilesActions;
  QSignalMapper *m_pSigMapOpenTemplate;

  // Dynamically created menus/dropdowns/toolbuttons via XML file
  QList<QMenu *> m_pXmlMenus;
  QList<QMenu *> m_pXmlSubMenus;
  QList<QComboBox *> m_pXmlDropdowns;
  QList<QToolBar *> m_pXmlToolbars;
  QList<QToolButton *> m_pXmlToolbuttons;
  QList<QAction *> m_pXmlActions;
  QSignalMapper *m_pSigMapXmlActions;

  const QString m_sSharePath;
  const QDir m_UserDataDir;
  const QString m_sPreviewFile;
  QColor m_colorSyntaxError;
  QDir m_tmpPreviewImgDir;
  QTimer *m_pPreviewTimer;
  bool m_bOpenFileAfterStart;
  bool m_bEditorScrolling;
  bool m_bWebviewScrolling;
  bool m_bReloadPreviewBlocked;
};

#endif  // INYOKAEDIT_CINYOKAEDIT_H_
