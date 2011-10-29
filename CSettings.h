/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CSettings.h
* Purpose:    Class definition for reading and writing settings
***************************************************************************/

#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QDir>
#include <QSettings>

#include <iostream>
#include <stdlib.h>

#include "qtfindreplacedialog/finddialog.h"
#include "qtfindreplacedialog/findreplacedialog.h"


class CSettings : public QObject
{
    Q_OBJECT

public:
    CSettings(const QDir SettingsDir, const QString &sName, FindDialog &FDialog, FindReplaceDialog &FRDialog);
    ~CSettings();

    // Load / save application settings
    void readSettings();
    void writeSettings(const QByteArray WinGeometry, const QByteArray WinState);

    void setLastOpenedDir(const QDir LastDir);

    QString getInyokaUrl() const;
    bool getAutomaticImageDownload() const;
    bool getPreviewInEditor() const;
    QDir getLastOpenedDir() const;
    QByteArray getWindowState() const;
    QByteArray getWindowGeometry() const;

signals:
    void callShowMessageBox(const QString &, const QString &);
    void sendCodeCompState(bool);

private:

    QSettings *mySettingsObject;

    bool bCodeCompletion;  // Enable / disable code completion
    bool bPreviewInEditor; // Enable / disable preview in edior
    QString sInyokaUrl;    // Url to inyoka
    QDir LastOpenedDir;
    bool bAutomaticImageDownload;  // Enable / disable download of article images
    QByteArray aWindowState;
    QByteArray aWindowGeometry;

    FindDialog *pFDialog;
    FindReplaceDialog *pFRDialog;

};

#endif // CSETTINGS_H
