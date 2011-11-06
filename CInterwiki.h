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
* File Name:  CInterwiki.h
* Purpose:    Class definition
***************************************************************************/

#ifndef CINTERWIKI_H
#define CINTERWIKI_H

#include <iostream>
#include <cstdlib>  // exit()

#include <QStringList>
#include <QXmlSimpleReader>
#include <QMessageBox>

class CInterWiki
{

public:
    CInterWiki(const QString &sName);

    QStringList getInterwikiLinksGroups() const;
    QStringList getInterwikiLinksGroupIcons() const;
    QList<QStringList> getInterwikiLinks() const;
    QList<QStringList> getInterwikiLinksUrls() const;
    QList<QStringList> getInterwikiLinksNames() const;
    QList<QStringList> getInterwikiLinksIcons() const;

private:
    QXmlInputSource *myXmlSource;

    QStringList sListGroups;
    QStringList sListGroupIcons;
    QList<QStringList> sListInterWikiLinks;
    QList<QStringList> sListInterWikiLinksUrls;
    QList<QStringList> sListInterWikiLinksNames;
    QList<QStringList> sListInterWikiLinksIcons;

    class IWikiLinksParser;
};

// -----------------------------------------------------------------------------------------------

class CInterWiki::IWikiLinksParser : public QXmlDefaultHandler
{
    friend class CInterWiki;

public:
    bool startDocument();
    bool endElement( const QString&, const QString&, const QString &name );
    bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );

private:
    bool bInInterWikiLink;

    QStringList tmpListiWikiLinks;
    QStringList tmpListiWikiUrls;
    QStringList tmpListiWikiNames;
    QStringList tmpListiWikiIcons;

    QStringList sListGroups_2;
    QStringList sListGroupIcons_2;
    QList<QStringList> sListInterWikiLinks_2;
    QList<QStringList> sListInterWikiLinksUrls_2;
    QList<QStringList> sListInterWikiLinksNames_2;
    QList<QStringList> sListInterWikiLinksIcons_2;
};

#endif // CINTERWIKI_H
