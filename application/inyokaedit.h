/**
 * \file inyokaedit.h
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
 * Class definition main application.
 */

#ifndef APPLICATION_INYOKAEDIT_H_
#define APPLICATION_INYOKAEDIT_H_

#include <QBoxLayout>
#include <QMainWindow>
#include <QSplitter>
#include <QToolButton>

#include "./download.h"
#include "./fileoperations.h"
#include "./parser/parser.h"
#include "./plugins.h"
#include "./settings.h"
#include "./templates/templates.h"
#include "./texteditor.h"
#include "./utils.h"

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

class Download;

namespace Ui {
  class InyokaEdit;
}

/**
 * \class InyokaEdit
 * \brief Main application definition (gui, objects, etc.)
 */
class InyokaEdit : public QMainWindow {
  Q_OBJECT

  public:
    explicit InyokaEdit(const QDir &userDataDir, const QDir &sharePath,
                        QWidget *parent = 0);
    ~InyokaEdit();

  public slots:
    void displayArticleText(const QString &sArticleText,
                            const QString &sSitename);
    void addPluginsButtons(QList<QAction *> ToolbarEntries,
                           QList<QAction *> MenueEntries);
    void updateEditorSettings();

  protected:
    void closeEvent(QCloseEvent *pEvent);
    bool eventFilter(QObject *pObj, QEvent *pEvent);

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

    Ui::InyokaEdit *m_pUi;
    Templates *m_pTemplates;
    FileOperations *m_pFileOperations;
    TextEditor *m_pCurrentEditor;
    Plugins *m_pPlugins;
    Parser *m_pParser;
    Settings *m_pSettings;
    Download *m_pDownloadModule;
    Utils *m_pUtils;
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

#endif  // APPLICATION_INYOKAEDIT_H_
