/**
 * \file CSettings.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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
 * Read and write settings.
 */

#include <QDebug>
#include "./CSettings.h"

CSettings::CSettings( const QDir &SettingsDir, const QString &sName, FindDialog &FDialog, FindReplaceDialog &FRDialog ) :
    m_pFDialog(&FDialog), m_pFRDialog(&FRDialog)
{
    qDebug() << "Start" << Q_FUNC_INFO;

#if defined _WIN32
    QSettings::setPath( QSettings::IniFormat, QSettings::UserScope, SettingsDir.absolutePath() );
    mySettingsObject = new QSettings( QSettings::IniFormat, QSettings::UserScope, sName );
#else
    QSettings::setPath( QSettings::NativeFormat, QSettings::UserScope, SettingsDir.absolutePath() );
    mySettingsObject = new QSettings( QSettings::NativeFormat, QSettings::UserScope, sName );
#endif

    this->readSettings();

    qDebug() << "End" << Q_FUNC_INFO;
}

CSettings::~CSettings()
{
    if ( mySettingsObject )
    {
        delete mySettingsObject;
        mySettingsObject = NULL;
    }
}

// -----------------------------------------------------------------------------------------------

void CSettings::readSettings()
{
    // General settings
    m_bCodeCompletion = mySettingsObject->value("CodeCompletion", true).toBool();
    m_bPreviewInEditor = mySettingsObject->value("PreviewInEditor", true).toBool();
    m_bPreviewAlongside = mySettingsObject->value("PreviewAlongside", false).toBool();
    m_sInyokaUrl = mySettingsObject->value("InyokaUrl", "http://wiki.ubuntuusers.de").toString();
    if ( m_sInyokaUrl.endsWith("/") )
    {
        m_sInyokaUrl.remove( m_sInyokaUrl.length()-1, 1 );
    }
    m_LastOpenedDir = mySettingsObject->value("LastOpenedDir", QDir::homePath()).toString();
    m_bAutomaticImageDownload = mySettingsObject->value("AutomaticImageDownload", false).toBool();
    m_sSpellCheckerLanguage = mySettingsObject->value("SpellCheckerLanguage", "de_DE").toString();
    m_bCheckLinks = mySettingsObject->value("CheckLinks", false).toBool();
    m_sTemplateLang = mySettingsObject->value("TemplateLanguage", "de").toString();
    m_nAutosave = mySettingsObject->value("AutoSave", 300).toUInt();
    m_sReloadPreviewKey = mySettingsObject->value("ReloadPreviewKey", "0x01000004").toString(); // 0x01000004 = Qt::Key_Return
    m_nTimedPreview = mySettingsObject->value("TimedPreview", 15).toUInt();
    m_bSyncScrollbars = mySettingsObject->value("SyncScrollbars", true).toBool();

    // Font settings
    mySettingsObject->beginGroup("Font");
    m_sFontFamily = mySettingsObject->value("FontFamily", "Monospace").toString();
    // Used string for font size because float isn't saved human readable...
    m_nFontsize = mySettingsObject->value("FontSize", "10.5").toFloat();
    if ( m_nFontsize <= 0 )
    {
        m_nFontsize = 10.5;
    }

    m_EditorFont.setFamily( m_sFontFamily );
    m_EditorFont.setFixedPitch( true );
    m_EditorFont.setStyleHint( QFont::TypeWriter );  // Font matcher prefers fixed pitch fonts
    m_EditorFont.setPointSizeF( m_nFontsize );

    mySettingsObject->endGroup();

    // Recent files
    mySettingsObject->beginGroup("RecentFiles");
    m_nMaxLastOpenedFiles = mySettingsObject->value("NumberOfRecentFiles", 5).toInt();
    if ( m_nMaxLastOpenedFiles < 0 )
    {
        m_nMaxLastOpenedFiles = 0;
    }
    if ( m_nMaxLastOpenedFiles > m_cMAXFILES )
    {
        m_nMaxLastOpenedFiles = m_cMAXFILES;
    }
    QString sTmpFile;
    for ( int i = 0; i < m_nMaxLastOpenedFiles; i++ )
    {
        sTmpFile = mySettingsObject->value("File_" + QString::number(i), "").toString();
        if ( sTmpFile != "" )
        {
            m_sListRecentFiles << sTmpFile;
        }
    }
    m_sListRecentFiles.removeDuplicates();
    mySettingsObject->endGroup();

    // Find/replace dialogs
    m_pFDialog->readSettings(*mySettingsObject);
    m_pFRDialog->readSettings(*mySettingsObject);

    // Window state
    mySettingsObject->beginGroup("Window");
    m_aWindowGeometry = mySettingsObject->value("Geometry").toByteArray();
    m_aWindowState = mySettingsObject->value("WindowState").toByteArray();  // Restore toolbar position etc.
    m_aSplitterState = mySettingsObject->value("SplitterState").toByteArray();
    mySettingsObject->endGroup();
}

// -----------------------------------------------------------------------------------------------

void CSettings::writeSettings( const QByteArray WinGeometry, const QByteArray WinState, const QByteArray SplitterState )
{
    // General settings
    mySettingsObject->setValue("CodeCompletion", m_bCodeCompletion);
    mySettingsObject->setValue("PreviewInEditor", m_bPreviewInEditor);
    mySettingsObject->setValue("PreviewAlongside", m_bPreviewAlongside);
    mySettingsObject->setValue("InyokaUrl", m_sInyokaUrl);
    mySettingsObject->setValue("LastOpenedDir", m_LastOpenedDir.absolutePath());
    mySettingsObject->setValue("AutomaticImageDownload", m_bAutomaticImageDownload);
    mySettingsObject->setValue("SpellCheckerLanguage", m_sSpellCheckerLanguage);
    mySettingsObject->setValue("CheckLinks", m_bCheckLinks);
    mySettingsObject->setValue("TemplateLanguage", m_sTemplateLang);
    mySettingsObject->setValue("AutoSave", m_nAutosave);
    mySettingsObject->setValue("ReloadPreviewKey", m_sReloadPreviewKey);
    mySettingsObject->setValue("TimedPreview", m_nTimedPreview);
    mySettingsObject->setValue("SyncScrollbars", m_bSyncScrollbars);

    // Remove obsolete entry
    mySettingsObject->remove( "ConfVersion" );
    mySettingsObject->remove( "ShowStatusbar" );

    // Font settings
    mySettingsObject->beginGroup("Font");
    mySettingsObject->setValue("FontFamily", m_EditorFont.family());
    mySettingsObject->setValue("FontSize", m_EditorFont.pointSizeF());
    mySettingsObject->endGroup();

    // Recent files
    mySettingsObject->beginGroup("RecentFiles");
    mySettingsObject->setValue("NumberOfRecentFiles", m_nMaxLastOpenedFiles);
    for ( int i = 0; i < m_cMAXFILES; i++ )
    {
        if ( i < m_sListRecentFiles.size() )
        {
            mySettingsObject->setValue("File_" + QString::number(i), m_sListRecentFiles[i]);
        }
        else
        {
            mySettingsObject->setValue("File_" + QString::number(i), "");
        }
    }
    mySettingsObject->endGroup();

    // Find/replace dialogs
    m_pFDialog->writeSettings(*mySettingsObject);
    m_pFRDialog->writeSettings(*mySettingsObject);

    // Save toolbar position etc.
    mySettingsObject->beginGroup("Window");
    mySettingsObject->setValue("Geometry", WinGeometry);
    mySettingsObject->setValue("WindowState", WinState);
    if ( true == m_bPreviewAlongside && true == m_bPreviewInEditor )
    {
        mySettingsObject->setValue("SplitterState", SplitterState);
    }
    else
    {
        mySettingsObject->setValue("SplitterState", m_aSplitterState);
    }

    mySettingsObject->endGroup();
}

// -----------------------------------------------------------------------------------------------
// Get / set methods

QString CSettings::getInyokaUrl() const
{
    return m_sInyokaUrl;
}

bool CSettings::getCodeCompletion() const
{
    return m_bCodeCompletion;
}

bool CSettings::getAutomaticImageDownload() const
{
    return m_bAutomaticImageDownload;
}

bool CSettings::getPreviewInEditor() const
{
    return m_bPreviewInEditor;
}

bool CSettings::getPreviewAlongside() const
{
    return m_bPreviewAlongside;
}

QDir CSettings::getLastOpenedDir() const
{
    return m_LastOpenedDir;
}

void CSettings::setLastOpenedDir( const QDir &LastDir )
{
    m_LastOpenedDir = LastDir;
}

QString CSettings::getSpellCheckerLanguage() const
{
    return m_sSpellCheckerLanguage;
}

bool CSettings::getCheckLinks() const
{
    return m_bCheckLinks;
}

QString CSettings::getTemplateLanguage() const
{
    return m_sTemplateLang;
}

quint32 CSettings::getAutoSave() const
{
    return m_nAutosave;
}


qint32 CSettings::getReloadPreviewKey() const
{
    QString sTmp = m_sReloadPreviewKey;
    return sTmp.remove("0x", Qt::CaseInsensitive).toInt(0, 16);
}

quint32 CSettings::getTimedPreview() const
{
    return m_nTimedPreview;
}

bool CSettings::getSyncScrollbars() const
{
    return m_bSyncScrollbars;
}


// ----------------------------------------------------

QFont CSettings::getEditorFont() const
{
    return m_EditorFont;
}

// ----------------------------------------------------

quint16 CSettings::getNumOfRecentFiles() const
{
    return (quint16)m_nMaxLastOpenedFiles;
}

quint16 CSettings::getMaxNumOfRecentFiles() const
{
    return (quint16)m_cMAXFILES;
}

QStringList CSettings::getRecentFiles() const
{
    return m_sListRecentFiles;
}

void CSettings::setRecentFiles( const QStringList &sListNewRecent )
{
    quint16 iCnt;
    if ( sListNewRecent.size() > m_cMAXFILES )
    {
        iCnt = m_cMAXFILES;
    }
    else
    {
        iCnt = sListNewRecent.size();
    }

    m_sListRecentFiles.clear();
    for ( int i = 0; i < iCnt; i++ )
    {
        m_sListRecentFiles << sListNewRecent[i];
    }
}

// ----------------------------------------------------

QByteArray CSettings::getWindowGeometry() const
{
    return m_aWindowGeometry;
}

QByteArray CSettings::getWindowState() const
{
    return m_aWindowState;
}

QByteArray CSettings::getSplitterState() const
{
    return m_aSplitterState;
}
