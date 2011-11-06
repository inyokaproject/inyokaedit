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
* File Name:  CInterwiki.cpp
* Purpose:    Xml parser for importing interwiki links
***************************************************************************/

#include "CInterwiki.h"

CInterWiki::CInterWiki(const QString &sName)
{
    QFile XmlFile("/usr/share/" + sName.toLower() + "/iWikiLinks/iWikiLinks.xml");

    // Check if file exist and it's readable
    if (!XmlFile.open(QFile::ReadOnly | QFile::Text)) {
        // Call message box from CInyokaEdit
        std::cerr << "ERROR: Can not open \"" << XmlFile.fileName().toStdString() << "\"." << std::endl;
        QMessageBox::critical(0, sName, "Can not open \"" + XmlFile.fileName() + "\".");
        exit (-5);
    }

    QXmlSimpleReader myXmlReader;

    try
    {
        myXmlSource = new QXmlInputSource(&XmlFile);
    }
    catch (std::bad_alloc& ba)
    {
        std::cerr << "ERROR: myXmlSource - bad_alloc caught: " << ba.what() << std::endl;
        QMessageBox::critical(0, sName, "ERROR: bad_alloc XmlSource");
        exit (-6);
    }

    IWikiLinksParser *myHandler = new IWikiLinksParser;
    myXmlReader.setContentHandler(myHandler);
    myXmlReader.setErrorHandler(myHandler);

    bool ok = myXmlReader.parse(myXmlSource);
    if (!ok) {
        std::cerr << "ERROR: Parsing \"" << XmlFile.fileName().toStdString() << "\"failed." << std::endl;
        QMessageBox::critical(0, sName, "Error while parsing \"" + XmlFile.fileName() + "\".");
        exit (-7);
    }

    sListGroups = myHandler->sListGroups_2;
    sListGroupIcons = myHandler->sListGroupIcons_2;
    sListInterWikiLinks = myHandler->sListInterWikiLinks_2;
    sListInterWikiLinksUrls = myHandler->sListInterWikiLinksUrls_2;
    sListInterWikiLinksNames = myHandler->sListInterWikiLinksNames_2;
    sListInterWikiLinksIcons = myHandler->sListInterWikiLinksIcons_2;

    /*
    for (int i = 0; i < sListGroups.size(); i++) {
        qDebug() << "\n" << sListGroups[i].toUpper() << " (" << sListGroupIcons[i] << "): ";
         for (int j = 0; j < sListInterWikiLinks[i].size(); j++) {
             qDebug() << sListInterWikiLinks[i][j] << " " << sListInterWikiLinksUrls[i][j] << " " << sListInterWikiLinksNames[i][j] << " " << sListInterWikiLinksIcons[i][j];
         }
    }
    */
}

// -----------------------------------------------------------------------------------------------

QStringList CInterWiki::getInterwikiLinksGroups() const {
    return sListGroups;
}

QStringList CInterWiki::getInterwikiLinksGroupIcons() const {
    return sListGroupIcons;
}

QList<QStringList> CInterWiki::getInterwikiLinks() const {
    return sListInterWikiLinks;
}

QList<QStringList> CInterWiki::getInterwikiLinksUrls() const {
    return sListInterWikiLinksUrls;
}

QList<QStringList> CInterWiki::getInterwikiLinksNames() const {
    return sListInterWikiLinksNames;
}

QList<QStringList> CInterWiki::getInterwikiLinksIcons() const {
    return sListInterWikiLinksIcons;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

bool CInterWiki::IWikiLinksParser::startDocument() {
    bInInterWikiLink = false;
    return true;
}

// -----------------------------------------------------------------------------------------------

bool CInterWiki::IWikiLinksParser::endElement( const QString&, const QString&, const QString &name ) {
    if( name == "menu" ) {
        bInInterWikiLink = false;
    }

    else if (name == "group") {
        sListInterWikiLinks_2 << tmpListiWikiLinks;
        sListInterWikiLinksUrls_2 << tmpListiWikiUrls;
        sListInterWikiLinksNames_2 << tmpListiWikiNames;
        sListInterWikiLinksIcons_2 << tmpListiWikiIcons;
    }

    return true;
}

// -----------------------------------------------------------------------------------------------

bool CInterWiki::IWikiLinksParser::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs ) {
    QString sGroupName, sGroupIcon, sType, sUrl, sName, sIcon;

    // Found group
    if( bInInterWikiLink && name == "group" ) {
        tmpListiWikiLinks.clear();
        tmpListiWikiUrls.clear();
        tmpListiWikiNames.clear();
        tmpListiWikiIcons.clear();

        sGroupName = "GROUPNAME NOT FOUND";
        sGroupIcon = "NO ICON";

        for( int i = 0; i < attrs.count(); i++ ) {
            if( attrs.localName( i ) == "name" ) {
                sGroupName = attrs.value( i );
            }
            else if( attrs.localName( i ) == "icon" ) {
                sGroupIcon = attrs.value( i );
            }
        }
        sListGroups_2 << sGroupName;
        sListGroupIcons_2 << sGroupIcon;
    }

    // Found interwikilink
    else if( bInInterWikiLink && name == "iwikilink" ) {

        sType = "TYPE NOT FOUND";
        sUrl = "URL NOT FOUND";
        sName = "NAME NOT FOUND";
        sIcon = "ICON NOT FOUND";

        for( int i = 0; i < attrs.count(); i++ ) {
            if( attrs.localName( i ) == "type" ) {
                sType = attrs.value( i );
            }
            else if( attrs.localName( i ) == "url" ) {
                sUrl = attrs.value( i );
            }
            else if( attrs.localName( i ) == "name" ) {
                sName = attrs.value( i );
            }
            else if( attrs.localName( i ) == "icon" ) {
                sIcon = attrs.value( i );
            }
        }

        tmpListiWikiLinks << sType;
        tmpListiWikiUrls << sUrl;
        tmpListiWikiNames << sName;
        tmpListiWikiIcons << sIcon;
    }

    // Found start of document
    else if( name == "menu" ) {
        bInInterWikiLink = true;
    }

    return true;
}
