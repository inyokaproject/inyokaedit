// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_INYOKAEDIT_H_
#define APPLICATION_INYOKAEDIT_H_

#include <QAction>  // Cannot use forward declaration (since Qt 6)
#include <QDir>
#include <QMainWindow>
#include <QTranslator>

class QComboBox;
class QFile;
class QSplitter;
class QToolButton;
#ifdef USEQTWEBENGINE
class QWebEngineView;
#endif

class Download;
class FileOperations;
class Parser;
class Plugins;
class Session;
class Templates;
class TextEditor;
class Upload;
class Utils;

#include "./settings.h"
#include "./settingsdialog.h"

namespace Ui {
class InyokaEdit;
}

class InyokaEdit : public QMainWindow {
  Q_OBJECT

 public:
  explicit InyokaEdit(const QDir &userDataDir,
                      const QString &sArg = QLatin1String(""),
                      QWidget *parent = nullptr);
  ~InyokaEdit();

 public slots:
  void displayArticleText(const QString &sArticleText,
                          const QString &sSitename);
  void addPluginsButtons(const QList<QAction *> &ToolbarEntries,
                         QList<QAction *> MenueEntries);
  void updateEditorSettings();

 protected:
  void changeEvent(QEvent *pEvent) override;
  void closeEvent(QCloseEvent *pEvent) override;
  auto eventFilter(QObject *pObj, QEvent *pEvent) -> bool override;

 signals:
  void updateUiLang(const QString &sLanguage);
  void retranslate();

 private slots:
  void loadLanguage(const QString &sLang);
  void openFile();
  void setCurrentEditor();
  void changedNumberOfEditors();
  void insertMacro(const QString &sInsert);
  void dropdownXmlChanged(int nIndex);
  void deleteTempImages();
  void highlightSyntaxError(const QPair<int, QString> &error);
  static QColor getHighlightErrorColor();
  // Preview
  void previewInyokaPage();
  void syncScrollbarsEditor();
  void syncScrollbarsWebview();
  void showAbout();
#ifdef USEQTWEBENGINE
  void loadPreviewFinished(const bool bSuccess);
  void clickedLink(const QUrl &newUrl);
  void changedUrl();
  void showSyntaxOverview();
#endif

 private:
  void createObjects();
  void createActions();
  void createMenus();
  void setupEditor();
  void clearXmlMenus();
  void createXmlMenus();
  void deleteAutoSaveBackups();
  static auto switchTranslator(QTranslator *translator, const QString &sFile,
                               const QString &sPath = QLatin1String(""))
      -> bool;

  Ui::InyokaEdit *m_pUi;
  QTranslator m_translator;    // App translations
  QTranslator m_translatorQt;  // Qt translations
  QString m_sCurrLang;
  Templates *m_pTemplates{};
  FileOperations *m_pFileOperations{};
  TextEditor *m_pCurrentEditor{};
  Plugins *m_pPlugins{};
  Parser *m_pParser{};
  Settings *m_pSettings{};
  SettingsDialog *m_pSettingsDialog{};
  Session *m_pSession{};
  Download *m_pDownloadModule{};
  Upload *m_pUploadModule{};
  Utils *m_pUtils{};
  QSplitter *m_pWidgetSplitter{};
  QTabWidget *m_pDocumentTabs{};
  QPoint m_WebviewScrollPosition;
#ifdef USEQTWEBENGINE
  QWebEngineView *m_pWebview{};
#endif

  QList<QAction *> m_OpenTemplateFilesActions;

  // Dynamically created menus/drop-downs/tool-buttons via XML file
  QList<QMenu *> m_pXmlMenus;
  QList<QMenu *> m_pXmlSubMenus;
  QList<QComboBox *> m_pXmlDropdowns;
  QList<QToolBar *> m_pXmlToolbars;
  QList<QToolButton *> m_pXmlToolbuttons;
  QList<QAction *> m_pXmlActions;

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
