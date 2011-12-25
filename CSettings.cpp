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
* File Name:  CSettings.cpp
* Purpose:    Read and write settings
***************************************************************************/

#include "CSettings.h"

CSettings::CSettings(const QDir SettingsDir, const QString &sName, FindDialog &FDialog, FindReplaceDialog &FRDialog) :
    pFDialog(&FDialog), pFRDialog(&FRDialog)
{

    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, SettingsDir.absolutePath());

    try
    {
        mySettingsObject = new QSettings(QSettings::NativeFormat, QSettings::UserScope, sName);
    }
    catch (std::bad_alloc& ba)
    {
      std::cerr << "ERROR: Caught bad_alloc in \"CSettings\": " << ba.what() << std::endl;
      QMessageBox::critical(0, sName, "Error while memory allocation: CSettings");
      exit (-10);
    }
}

CSettings::~CSettings(){
    delete mySettingsObject;
    mySettingsObject = NULL;
}

// -----------------------------------------------------------------------------------------------

void CSettings::readSettings(){

    // General settings
    bCodeCompletion = mySettingsObject->value("CodeCompletion", true).toBool();
    bPreviewInEditor = mySettingsObject->value("PreviewInEditor", true).toBool();
    sInyokaUrl = mySettingsObject->value("InyokaUrl", "http://wiki.ubuntuusers.de").toString();
    if (sInyokaUrl.endsWith("/")) {
        sInyokaUrl.remove(sInyokaUrl.length()-1, 1);
    }
    LastOpenedDir = mySettingsObject->value("LastOpenedDir", QDir::homePath()).toString();
    bAutomaticImageDownload = mySettingsObject->value("AutomaticImageDownload", false).toBool();
    sConfVersion = mySettingsObject->value("ConfVersion", "0.0.0").toString();
    bShowStatusbar = mySettingsObject->value("ShowStatusbar", true).toBool();
    sSpellCheckerLanguage = mySettingsObject->value("SpellCheckerLanguage", "de_DE").toString();

    // Font settings
    mySettingsObject->beginGroup("Font");
    // Used string for font size because float isn't saved human readable...
    QString sFontsize = mySettingsObject->value("FontSize", "10.5").toString();
    iFontsize = sFontsize.toFloat();
    if (iFontsize < 0) { iFontsize = -iFontsize; }
    if (0 == iFontsize) { iFontsize = 10.5; }
    mySettingsObject->endGroup();

    // Recent files
    mySettingsObject->beginGroup("RecentFiles");
    iMaxLastOpenedFiles = mySettingsObject->value("NumberOfRecentFiles", 5).toInt();
    if (iMaxLastOpenedFiles < 0) { iMaxLastOpenedFiles = 0; }
    if (iMaxLastOpenedFiles > cMAXFILES) { iMaxLastOpenedFiles = cMAXFILES; }
    QString sTmpFile;
    for (int i = 0; i < iMaxLastOpenedFiles; i++) {
        sTmpFile = mySettingsObject->value("File_" + QString::number(i), "").toString();
        if (sTmpFile != "") {
            sListRecentFiles << sTmpFile;
        }
    }
    sListRecentFiles.removeDuplicates();
    mySettingsObject->endGroup();

    // Find/replace dialogs
    pFDialog->readSettings(*mySettingsObject);
    pFRDialog->readSettings(*mySettingsObject);

    // Window state
    mySettingsObject->beginGroup("Window");
    aWindowGeometry = mySettingsObject->value("Geometry").toByteArray();
    aWindowState = mySettingsObject->value("WindowState").toByteArray();  // Restore toolbar position etc.
    mySettingsObject->endGroup();
}

// -----------------------------------------------------------------------------------------------

void CSettings::writeSettings(QByteArray WinGeometry, QByteArray WinState){

    // General settings
    mySettingsObject->setValue("CodeCompletion", bCodeCompletion);
    mySettingsObject->setValue("PreviewInEditor", bPreviewInEditor);
    mySettingsObject->setValue("InyokaUrl", sInyokaUrl);
    mySettingsObject->setValue("LastOpenedDir", LastOpenedDir.absolutePath());
    mySettingsObject->setValue("AutomaticImageDownload", bAutomaticImageDownload);
    mySettingsObject->setValue("ConfVersion", sConfVersion);
    mySettingsObject->setValue("ShowStatusbar", bShowStatusbar);
    mySettingsObject->setValue("SpellCheckerLanguage", sSpellCheckerLanguage);

    // Font settings
    mySettingsObject->beginGroup("Font");
    mySettingsObject->setValue("FontSize", QString::number(iFontsize));
    mySettingsObject->endGroup();

    // Recent files
    mySettingsObject->beginGroup("RecentFiles");
    mySettingsObject->setValue("NumberOfRecentFiles", iMaxLastOpenedFiles);
    for (int i = 0; i < cMAXFILES; i++) {
        if (i < sListRecentFiles.size()) {
            mySettingsObject->setValue("File_" + QString::number(i), sListRecentFiles[i]);
        }
        else {
            mySettingsObject->setValue("File_" + QString::number(i), "");
        }
    }
    mySettingsObject->endGroup();

    // Find/replace dialogs
    pFDialog->writeSettings(*mySettingsObject);
    pFRDialog->writeSettings(*mySettingsObject);

    // Save toolbar position etc.
    mySettingsObject->beginGroup("Window");
    mySettingsObject->setValue("Geometry", WinGeometry);
    mySettingsObject->setValue("WindowState", WinState);
    mySettingsObject->endGroup();
}

// -----------------------------------------------------------------------------------------------
// Get / set methods

QString CSettings::getInyokaUrl() const {
    return sInyokaUrl;
}

bool CSettings::getCodeCompletion() const {
    return bCodeCompletion;
}

bool CSettings::getAutomaticImageDownload() const {
    return bAutomaticImageDownload;
}

bool CSettings::getPreviewInEditor() const {
    return bPreviewInEditor;
}

QDir CSettings::getLastOpenedDir() const {
    return LastOpenedDir;
}

void CSettings::setLastOpenedDir(const QDir LastDir) {
    LastOpenedDir = LastDir;
}

QString CSettings::getConfVersion() const {
    return sConfVersion;
}

void CSettings::setConfVersion(const QString &sNewVersion) {
    sConfVersion = sNewVersion;
}

bool CSettings::getShowStatusbar() const {
    return bShowStatusbar;
}

QString CSettings::getSpellCheckerLanguage() const {
    return sSpellCheckerLanguage;
}

// ----------------------------------------------------

float CSettings::getFontsize() const {
    return iFontsize;
}

// ----------------------------------------------------

unsigned short CSettings::getNumOfRecentFiles() const {
    return (unsigned short)iMaxLastOpenedFiles;
}

unsigned short CSettings::getMaxNumOfRecentFiles() const {
    return (unsigned short)cMAXFILES;
}

QStringList CSettings::getRecentFiles() const {
    return sListRecentFiles;
}

void CSettings::setRecentFiles(const QStringList &sListNewRecent) {
    unsigned short iCnt;
    if (sListNewRecent.size() > cMAXFILES) {
        iCnt = cMAXFILES;
    }
    else {
        iCnt = sListNewRecent.size();
    }

    sListRecentFiles.clear();
    for (int i = 0; i < iCnt; i++) {
        sListRecentFiles << sListNewRecent[i];
    }
}

// ----------------------------------------------------

QByteArray CSettings::getWindowGeometry() const {
    return aWindowGeometry;
}

QByteArray CSettings::getWindowState() const {
    return aWindowState;
}
