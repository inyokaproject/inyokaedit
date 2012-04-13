/**
 * \file CInterwiki.cpp
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
 * Xml parser for importing interwiki links.
 */

#include <QDebug>
#include "CInterwiki.h"

CInterWiki::CInterWiki( const QApplication *pApp )
{
    qDebug() << "Begin" << Q_FUNC_INFO;

    QFile XmlFile;

    // Path from normal installation
    if ( QFile::exists("/usr/share/" + pApp->applicationName().toLower() + "/iWikiLinks/iWikiLinks.xml") )
    {
        XmlFile.setFileName( "/usr/share/" + pApp->applicationName().toLower() + "/iWikiLinks/iWikiLinks.xml" );
    }
    // No installation: Use app path
    else
    {
        XmlFile.setFileName( pApp->applicationDirPath() + "/iWikiLinks/iWikiLinks.xml" );
    }

    // Check if file exist and it's readable
    if ( !XmlFile.open(QFile::ReadOnly | QFile::Text) )
    {
        // Call message box from CInyokaEdit
        std::cerr << "ERROR: Can not open \"" << XmlFile.fileName().toStdString() << "\"." << std::endl;
        QMessageBox::critical( 0, pApp->applicationName(), "Can not open \"" + XmlFile.fileName() + "\"." );
        exit (-5);
    }

    QXmlSimpleReader myXmlReader;

    m_myXmlSource = new QXmlInputSource(&XmlFile);
    m_myHandler = new CIWikiLinksParser;

    myXmlReader.setContentHandler(m_myHandler);
    myXmlReader.setErrorHandler(m_myHandler);

    bool ok = myXmlReader.parse(m_myXmlSource);
    if ( !ok )
    {
        std::cerr << "ERROR: Parsing \"" << XmlFile.fileName().toStdString() << "\"failed." << std::endl;
        QMessageBox::critical( 0, pApp->applicationName(), "Error while parsing \"" + XmlFile.fileName() + "\"." );
        exit (-7);
    }

    m_sListGroups = m_myHandler->m_sListGroups_2;
    m_sListGroupIcons = m_myHandler->m_sListGroupIcons_2;
    m_sListInterWikiLinks = m_myHandler->m_sListInterWikiLinks_2;
    m_sListInterWikiLinksUrls = m_myHandler->m_sListInterWikiLinksUrls_2;
    m_sListInterWikiLinksNames = m_myHandler->m_sListInterWikiLinksNames_2;
    m_sListInterWikiLinksIcons = m_myHandler->m_sListInterWikiLinksIcons_2;

    /*
    for ( int i = 0; i < sListGroups.size(); i++ )
    {
        qDebug() << "\n" << sListGroups[i].toUpper() << " (" << sListGroupIcons[i] << "): ";
        for ( int j = 0; j < sListInterWikiLinks[i].size(); j++ )
        {
            qDebug() << sListInterWikiLinks[i][j] << " " << sListInterWikiLinksUrls[i][j] << " " << sListInterWikiLinksNames[i][j] << " " << sListInterWikiLinksIcons[i][j];
        }
    }
    */
    qDebug() << "Finished" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------

QStringList CInterWiki::getInterwikiLinksGroups() const
{
    return m_sListGroups;
}

QStringList CInterWiki::getInterwikiLinksGroupIcons() const
{
    return m_sListGroupIcons;
}

QList<QStringList> CInterWiki::getInterwikiLinks() const
{
    return m_sListInterWikiLinks;
}

QList<QStringList> CInterWiki::getInterwikiLinksUrls() const
{
    return m_sListInterWikiLinksUrls;
}

QList<QStringList> CInterWiki::getInterwikiLinksNames() const
{
    return m_sListInterWikiLinksNames;
}

QList<QStringList> CInterWiki::getInterwikiLinksIcons() const
{
    return m_sListInterWikiLinksIcons;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

bool CInterWiki::CIWikiLinksParser::startDocument()
{
    m_bInInterWikiLink = false;
    return true;
}

// -----------------------------------------------------------------------------------------------

bool CInterWiki::CIWikiLinksParser::endElement( const QString&, const QString&, const QString &sName )
{
    if ( "menu" == sName )
    {
        m_bInInterWikiLink = false;
    }
    else if ( "group" == sName )
    {
        m_sListInterWikiLinks_2 << m_tmpListiWikiLinks;
        m_sListInterWikiLinksUrls_2 << m_tmpListiWikiUrls;
        m_sListInterWikiLinksNames_2 << m_tmpListiWikiNames;
        m_sListInterWikiLinksIcons_2 << m_tmpListiWikiIcons;
    }

    return true;
}

// -----------------------------------------------------------------------------------------------

bool CInterWiki::CIWikiLinksParser::startElement( const QString&, const QString&, const QString &sElement, const QXmlAttributes &attrs )
{
    QString sGroupName("");
    QString sGroupIcon("");
    QString sType("");
    QString sUrl("");
    QString sName("");
    QString sIcon("");

    // Found group
    if ( m_bInInterWikiLink && "group" == sElement )
    {
        m_tmpListiWikiLinks.clear();
        m_tmpListiWikiUrls.clear();
        m_tmpListiWikiNames.clear();
        m_tmpListiWikiIcons.clear();

        sGroupName = "GROUPNAME NOT FOUND";
        sGroupIcon = "NO ICON";

        for ( int i = 0; i < attrs.count(); i++ )
        {
            if ( "name" == attrs.localName(i) )
            {
                sGroupName = attrs.value(i);
            }
            else if ( "icon"== attrs.localName(i) )
            {
                sGroupIcon = attrs.value(i);
            }
        }
        m_sListGroups_2 << sGroupName;
        m_sListGroupIcons_2 << sGroupIcon;
    }

    // Found interwikilink
    else if ( m_bInInterWikiLink && "iwikilink" == sElement )
    {
        sType = "TYPE NOT FOUND";
        sUrl = "URL NOT FOUND";
        sName = "NAME NOT FOUND";
        sIcon = "ICON NOT FOUND";

        for ( int i = 0; i < attrs.count(); i++ )
        {
            if ( "type" == attrs.localName(i) )
            {
                sType = attrs.value(i);
            }
            else if ( "url" == attrs.localName(i) )
            {
                sUrl = attrs.value(i);
            }
            else if ( "name" == attrs.localName(i) )
            {
                sName = attrs.value(i);
            }
            else if ( "icon" == attrs.localName(i) )
            {
                sIcon = attrs.value(i);
            }
        }

        m_tmpListiWikiLinks << sType;
        m_tmpListiWikiUrls << sUrl;
        m_tmpListiWikiNames << sName;
        m_tmpListiWikiIcons << sIcon;
    }

    // Found start of document
    else if ( "menu" == sElement )
    {
        m_bInInterWikiLink = true;
    }

    return true;
}
