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

#include "CSettings.h"

CSettings::CSettings( const QDir SettingsDir, const QString &sName, FindDialog &FDialog, FindReplaceDialog &FRDialog ) :
    m_pFDialog(&FDialog), m_pFRDialog(&FRDialog)
{

    QSettings::setPath( QSettings::NativeFormat, QSettings::UserScope, SettingsDir.absolutePath() );

    try
    {
        mySettingsObject = new QSettings( QSettings::NativeFormat, QSettings::UserScope, sName );
    }
    catch ( std::bad_alloc& ba )
    {
      std::cerr << "ERROR: Caught bad_alloc in \"CSettings\": " << ba.what() << std::endl;
      QMessageBox::critical( 0, sName, "Error while memory allocation: CSettings" );
      exit (-10);
    }
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
    m_bShowStatusbar = mySettingsObject->value("ShowStatusbar", false).toBool();
#ifndef DISABLE_SPELLCHECKER
    m_sSpellCheckerLanguage = mySettingsObject->value("SpellCheckerLanguage", "de_DE").toString();
#endif

    // Font settings
    mySettingsObject->beginGroup("Font");
    // Used string for font size because float isn't saved human readable...
    QString sFontsize = mySettingsObject->value("FontSize", "10.5").toString();
    m_nFontsize = sFontsize.toFloat();
    if ( m_nFontsize <= 0 )
    {
        m_nFontsize = 10.5;
    }
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
    mySettingsObject->setValue("ShowStatusbar", m_bShowStatusbar);
#ifndef DISABLE_SPELLCHECKER
    mySettingsObject->setValue("SpellCheckerLanguage", m_sSpellCheckerLanguage);
#endif

    // Remove obsolete entry
    mySettingsObject->remove( "ConfVersion" );

    // Font settings
    mySettingsObject->beginGroup("Font");
    mySettingsObject->setValue("FontSize", QString::number(m_nFontsize));
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

void CSettings::setLastOpenedDir( const QDir LastDir )
{
    m_LastOpenedDir = LastDir;
}

bool CSettings::getShowStatusbar() const
{
    return m_bShowStatusbar;
}

#ifndef DISABLE_SPELLCHECKER
QString CSettings::getSpellCheckerLanguage() const
{
    return m_sSpellCheckerLanguage;
}
#endif

// ----------------------------------------------------

float CSettings::getFontsize() const
{
    return m_nFontsize;
}

// ----------------------------------------------------

unsigned short CSettings::getNumOfRecentFiles() const
{
    return (unsigned short)m_nMaxLastOpenedFiles;
}

unsigned short CSettings::getMaxNumOfRecentFiles() const
{
    return (unsigned short)m_cMAXFILES;
}

QStringList CSettings::getRecentFiles() const
{
    return m_sListRecentFiles;
}

void CSettings::setRecentFiles( const QStringList &sListNewRecent )
{
    unsigned short iCnt;
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
