/**
 * \file fileoperations.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2021 The InyokaEdit developers
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
 * Class definition for file operations (new, load, save...).
 */

#ifndef APPLICATION_FILEOPERATIONS_H_
#define APPLICATION_FILEOPERATIONS_H_

#include <QObject>

class QAction;
class QTabWidget;
class QTimer;

class FindReplace;
class Settings;
class TextEditor;

/**
 * \class FileOperations
 * \brief File handling (open, load, save, etc.)
 */
class FileOperations : public QObject {
  Q_OBJECT

 public:
    FileOperations(QWidget *pParent, QTabWidget *pTabWidget,
                   Settings *pSettings, const QString &sPreviewFile,
                   const QString &sUserDataDir,
                   const QStringList &sListTplMacros,
                   QObject *pObj = nullptr);

    void newFile(QString sFileName);

    auto getCurrentEditor() -> TextEditor*;
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
#ifndef NOPREVIEW
    void printPreview();
#endif

    void copy();
    void cut();
    void paste();
    void undo();
    void redo();

 signals:
    void setMenuLastOpenedEnabled(const bool);
    void changedCurrentEditor();
    void newEditor();
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

    void updateEditorSettings();
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
