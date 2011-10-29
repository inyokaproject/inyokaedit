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
      std::cerr << "ERROR: mySettings - bad_alloc caught: " << ba.what() << std::endl;
      emit callShowMessageBox(trUtf8("Fehler bei der Speicherallokierung: QSettings"), "critical");
      exit (-1);
    }
}

CSettings::~CSettings(){
    delete mySettingsObject;
    mySettingsObject = NULL;
}


void CSettings::readSettings(){
    bCodeCompletion = mySettingsObject->value("CodeCompletion", true).toBool();
    emit sendCodeCompState(bCodeCompletion);

    bPreviewInEditor = mySettingsObject->value("PreviewInEditor", true).toBool();

    sInyokaUrl = mySettingsObject->value("InyokaUrl", "http://wiki.ubuntuusers.de").toString();
    if (sInyokaUrl.endsWith("/")) {
        sInyokaUrl.remove(sInyokaUrl.length()-1, 1);
    }

    LastOpenedDir = mySettingsObject->value("LastOpenedDir", QDir::homePath()).toString();

    bAutomaticImageDownload = mySettingsObject->value("AutomaticImageDownload", false).toBool();

    mySettingsObject->beginGroup("Window");
    aWindowGeometry = mySettingsObject->value("Geometry").toByteArray();
    aWindowState = mySettingsObject->value("WindowState").toByteArray();  // Restore toolbar position etc.
    mySettingsObject->endGroup();

    pFDialog->readSettings(*mySettingsObject);
    pFRDialog->readSettings(*mySettingsObject);
}


void CSettings::writeSettings(QByteArray WinGeometry, QByteArray WinState){
    mySettingsObject->setValue("CodeCompletion", bCodeCompletion);
    mySettingsObject->setValue("PreviewInEditor", bPreviewInEditor);
    mySettingsObject->setValue("InyokaUrl", sInyokaUrl);
    mySettingsObject->setValue("LastOpenedDir", LastOpenedDir.absolutePath());
    mySettingsObject->setValue("AutomaticImageDownload", bAutomaticImageDownload);

    pFDialog->writeSettings(*mySettingsObject);
    pFRDialog->writeSettings(*mySettingsObject);

    // Save toolbar position etc.
    mySettingsObject->beginGroup("Window");
    mySettingsObject->setValue("Geometry", WinGeometry);
    mySettingsObject->setValue("WindowState", WinState);
    mySettingsObject->endGroup();

}

QString CSettings::getInyokaUrl() const {
    return sInyokaUrl;
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

QByteArray CSettings::getWindowGeometry() const {
    return aWindowGeometry;
}

QByteArray CSettings::getWindowState() const {
    return aWindowState;
}
