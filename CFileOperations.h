/**
 * @file CFileOperations.h
 * @author See AUTHORS
 *
 * @section LICENSE
 *
 * Copyright (C) 2011-2012 by the respective authors (see AUTHORS)
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
 * @section DESCRIPTION
 * Class definition for file operations (new, load, save...).
 */

#ifndef CFILEOPERATIONS_H
#define CFILEOPERATIONS_H

#include "CInyokaEdit.h"

class CTextEditor;
class CSettings;

class CFileOperations : public QObject
{
    Q_OBJECT

public:
    CFileOperations(QWidget *pParent, CTextEditor *pEditor, CSettings *pSettings, const QString &sAppName);

    QString getCurrentFile() const;
    bool maybeSave();

    QList<QAction *> getLastOpenedFiles() const;

public slots:
    void newFile();
    void open();
    void openRecentFile(int iEntry);
    void clearRecentFiles();
    bool save();
    bool saveAs();

    // Load / save file
    void loadFile(const QString &sFileName);
    bool saveFile(const QString &sFileName);

    void setCurrentFile(const QString &sFileName);

signals:
    void setMenuLastOpenedEnabled(const bool);
    void setStatusbarMessage(QString);

private:
    void updateRecentFiles(const QString &sFileName);
    QSignalMapper *mySigMapLastOpenedFiles;

    QWidget *m_pParent;
    CTextEditor *m_pEditor;
    CSettings *m_pSettings;

    QList<QAction *> m_LastOpenedFilesAct;

    QString m_sCurFile;      // Current file
    QString m_sAppName;
};

#endif // CFILEOPERATIONS_H
