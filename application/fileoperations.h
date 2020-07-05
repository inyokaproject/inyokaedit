/**
 * \file fileoperations.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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
    /**
    * \brief Constructor
    * \param pParent Pointer to parent window
    * \param pEditor Pointer to editor module
    * \param pSettings Pointer to settings module
    * \param sAppName Application name
    */
    FileOperations(QWidget *pParent, QTabWidget *pTabWidget,
                   Settings *pSettings, const QString &sPreviewFile,
                   const QString &sUserDataDir,
                   const QStringList &sListTplMacros);

    void newFile(QString sFileName);

    auto getCurrentEditor() -> TextEditor*;
    auto getEditors() const -> QList<TextEditor *>;

    /**
    * \brief Get current file name
    * \return File name of currently opened file
    */
    auto getCurrentFile() const -> QString;

    /**
    * \brief Check if current file is saved or not
    * \return True or false if current file is saved or not
    */
    auto maybeSave() -> bool;

    /**
    * \brief Get list of recent opened files
    * \return List of last opened files
    */
    auto getLastOpenedFiles() const -> QList<QAction *>;

    auto closeAllmaybeSave() -> bool;

 public slots:
    /** \brief Open an existing file */
    void open();

    /**
    * \brief Open recent opened file
    * \param nEntry Number of file which should be opened
    */
    void openRecentFile(const int nEntry);

    /**
    * \brief Save current file
    * \return True or false if saving was successful / not successful
    */
    bool save();

    /**
    * \brief Save current file under new name
    * \return True or false if saving was successful / not successful
    */
    bool saveAs();

    /**
    * \brief Load existing file
    * \param sFileName Path and name of file which should be loaded
    */
    void loadFile(const QString &sFileName, const bool bUpdateRecent = false,
                  const bool bArchive = false);

    /**
    * \brief Load article from archive with images
    */
    void loadInyArchive(const QString &sArchive);

    /**
    * \brief Save current file
    * \param sFileName Path and name of file which should be saved
    * \return True or false if saving was successful / not successful
    */
    bool saveFile(QString sFileName);

    /**
    * \brief Save article into archive with images
    */
    bool saveInyArchive(const QString &sArchive);

    /** \brief Print preview (printer / PDF) */
    void printPreview();

    void copy();
    void cut();
    void paste();
    void undo();
    void redo();

 signals:
    /**
    * \brief Signal for sending state of recent files menu entry
    */
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
    /** \brief Clear recent files list in file menu */
    void clearRecentFiles();

    void changedDocTab(int nIndex);
    bool closeDocument(int nIndex);

    void updateEditorSettings();
    void saveDocumentAuto();

 private:
    /**
    * \brief Update list of recent opened files
    * \param sFileName Path and name of a newly opened file
    */
    void updateRecentFiles(const QString &sFileName);

    void setCurrentEditor();

    QWidget *m_pParent;      /**< Pointer to parent window */
    QTabWidget *m_pDocumentTabs;
    TextEditor *m_pCurrentEditor;  /**< Pointer to editor module */
    Settings *m_pSettings;  /**< Pointer to settings module */

    QList<QAction *> m_LastOpenedFilesAct;  /**< Actions open recent files */

    QString m_sPreviewFile;
    const QString m_sFileFilter;

    bool m_bLoadPreview;
    bool m_bCloseApp;
    QTimer *m_pTimerAutosave;
    const QString m_sUserDataDir;
    const QString m_sExtractDir;

    FindReplace *m_pFindReplace;

    QList<TextEditor *> m_pListEditors;

    // File menu: Clear recent opened files list
    QAction *m_pClearRecentFilesAct;

    QStringList m_sListTplMacros;
};

#endif  // APPLICATION_FILEOPERATIONS_H_
