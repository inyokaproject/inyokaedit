// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_FILEOPERATIONS_H_
#define APPLICATION_FILEOPERATIONS_H_

#include <QAction>  // Cannot use forward declaration (since Qt 6)
#include <QObject>

class QTabWidget;
class QTimer;
#ifdef USEQTWEBENGINE
class QWebEngineView;
#endif

class FindReplace;
class Settings;
class TextEditor;

class FileOperations : public QObject {
  Q_OBJECT

 public:
  FileOperations(QWidget *pParent, QTabWidget *pTabWidget,
                 const QString &sPreviewFile, const QString &sUserDataDir,
                 const QStringList &sListTplMacros, QObject *pObj = nullptr);

  void newFile(QString sFileName);

  auto getCurrentEditor() -> TextEditor *;
  auto getEditors() const -> QList<TextEditor *>;
  auto getCurrentFile() const -> QString;
  auto maybeSave() -> bool;
  auto getLastOpenedFiles() const -> QList<QAction *>;

  auto closeAllmaybeSave() -> bool;

 public slots:
  void open();
  void openRecentFile(const int nEntry);
  bool save();
  bool saveAs();
  void loadFile(const QString &sFileName, const bool bUpdateRecent = false,
                const bool bArchive = false);
  void loadInyArchive(const QString &sArchive);
  bool saveFile(QString sFileName);
  bool saveInyArchive(const QString &sArchive);
#ifdef USEQTWEBENGINE
  void printPdfPreview();
#endif
  void updateEditorSettings();

  void copy();
  void cut();
  void paste();
  void undo();
  void redo();

 signals:
  void setMenuLastOpenedEnabled(const bool);
  void changedCurrentEditor();
  void changedNumberOfEditors();
  void callPreview();
  void modifiedDoc(bool bModified);
  void movedEditorScrollbar();

  void triggeredFind();
  void triggeredReplace();
  void triggeredFindNext();
  void triggeredFindPrevious();
  void copyAvailable(bool);
  void undoAvailable(bool);
  void redoAvailable(bool);
  void undoAvailable2(bool);
  void redoAvailable2(bool);

 private slots:
  void clearRecentFiles();
  void changedDocTab(int nIndex);
  bool closeDocument(int nIndex);
  void saveDocumentAuto();

 private:
  void updateRecentFiles(const QString &sFileName);
  void setCurrentEditor();

  QWidget *m_pParent;
  QTabWidget *m_pDocumentTabs;
  TextEditor *m_pCurrentEditor;
  Settings *m_pSettings;

  QList<QAction *> m_LastOpenedFilesAct;

  QString m_sPreviewFile;
#ifdef USEQTWEBENGINE
  QWebEngineView *m_pPreviewWebView;
#endif
  const QString m_sFileFilter;

  bool m_bLoadPreview;
  bool m_bCloseApp;
  QTimer *m_pTimerAutosave;
  const QString m_sUserDataDir;
  const QString m_sExtractDir;

  FindReplace *m_pFindReplace;

  QList<TextEditor *> m_pListEditors;

  QAction *m_pClearRecentFilesAct;

  QStringList m_sListTplMacros;
};

#endif  // APPLICATION_FILEOPERATIONS_H_
