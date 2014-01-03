/**
 * \file CFileOperations.h
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
 * Class definition for file operations (new, load, save...).
 */

#ifndef INYOKAEDIT_CFILEOPERATIONS_H_
#define INYOKAEDIT_CFILEOPERATIONS_H_

#include <QAction>
#include <QSignalMapper>

#include "./CSettings.h"
#include "./CTextEditor.h"

/**
 * \class CFileOperations
 * \brief File handling (open, load, save, etc.)
 */
class CFileOperations : public QObject {
    Q_OBJECT

  public:
    /**
    * \brief Constructor
    * \param pParent Pointer to parent window
    * \param pEditor Pointer to editor module
    * \param pSettings Pointer to settings module
    * \param sAppName Application name
    */
    CFileOperations(QWidget *pParent, CTextEditor *pEditor,
                    CSettings *pSettings, const QString &sAppName,
                    const QString &sPreviewFile);

    /**
    * \brief Get current file name
    * \return File name of currently opened file
    */
    QString getCurrentFile() const;

    /**
    * \brief Check if current file is saved or not
    * \return True or false if current file is saved or not
    */
    bool maybeSave();

    /**
    * \brief Get list of recent opened files
    * \return List of last opened files
    */
    QList<QAction *> getLastOpenedFiles() const;

  public slots:
    /** \brief Start with new clean file */
    void newFile();

    /** \brief Open an existing file */
    void open();

    /**
    * \brief Open recent opened file
    * \param nEntry Number of file which should be opened
    */
    void openRecentFile(const int nEntry);

    /** \brief Clear recent files list in file menu */
    void clearRecentFiles();

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
                  const bool bCheckSave = true);

    /**
    * \brief Save current file
    * \param sFileName Path and name of file which should be saved
    * \return True or false if saving was successful / not successful
    */
    bool saveFile(const QString &sFileName);

    /** \brief Print preview (printer / PDF) */
    void printPreview();

    /**
    * \brief Change / set current file name
    * \param sFileName Path and name of current file
    */
    void setCurrentFile(const QString &sFileName);

  signals:
    /**
    * \brief Signal for sending state of recent files menu entry
    */
    void setMenuLastOpenedEnabled(const bool);
    void loadedFile();

  private:
    /**
    * \brief Update list of recent opened files
    * \param sFileName Path and name of a newly opened file
    */
    void updateRecentFiles(const QString &sFileName);

    QSignalMapper *m_pSigMapLastOpenedFiles;  /**< Actions open recent files */

    QWidget *m_pParent;      /**< Pointer to parent window */
    CTextEditor *m_pEditor;  /**< Pointer to editor module */
    CSettings *m_pSettings;  /**< Pointer to settings module */

    QList<QAction *> m_LastOpenedFilesAct;  /**< Actions open recent files */

    QString m_sCurFile;  /**< Current file (full path and name) */
    QString m_sAppName;  /**< Application name */
    QString m_sPreviewFile;
    const QString m_sFileFilter;
};

#endif  // INYOKAEDIT_CFILEOPERATIONS_H_
