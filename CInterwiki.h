/**
 * @file CInterwiki.h
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
 * Class definition.
 */

#ifndef CINTERWIKI_H
#define CINTERWIKI_H

#include <iostream>
#include <cstdlib>  // exit()

#include <QStringList>
#include <QXmlSimpleReader>
#include <QMessageBox>
#include <QApplication>

/**
 * \class CInterWiki
 * \brief Delivering InterWiki links (groups, url, icon)
 */
class CInterWiki
{

class CIWikiLinksParser;

public:
    CInterWiki( const QApplication *pApp );

    QStringList getInterwikiLinksGroups() const;
    QStringList getInterwikiLinksGroupIcons() const;
    QList<QStringList> getInterwikiLinks() const;
    QList<QStringList> getInterwikiLinksUrls() const;
    QList<QStringList> getInterwikiLinksNames() const;
    QList<QStringList> getInterwikiLinksIcons() const;

private:
    QXmlInputSource *m_myXmlSource;
    CIWikiLinksParser *m_myHandler;

    QStringList m_sListGroups;
    QStringList m_sListGroupIcons;
    QList<QStringList> m_sListInterWikiLinks;
    QList<QStringList> m_sListInterWikiLinksUrls;
    QList<QStringList> m_sListInterWikiLinksNames;
    QList<QStringList> m_sListInterWikiLinksIcons;

    class CIWikiLinksParser;
};

// -----------------------------------------------------------------------------------------------

/**
 * \class CInterWiki::CIWikiLinksParser
 * \brief Reading InterWiki link definitions from xml file.
 */
class CInterWiki::CIWikiLinksParser : public QXmlDefaultHandler
{
    friend class CInterWiki;

public:
    bool startDocument();
    bool endElement( const QString&, const QString&, const QString &name );
    bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );

private:
    bool m_bInInterWikiLink;

    QStringList m_tmpListiWikiLinks;
    QStringList m_tmpListiWikiUrls;
    QStringList m_tmpListiWikiNames;
    QStringList m_tmpListiWikiIcons;

    QStringList m_sListGroups_2;
    QStringList m_sListGroupIcons_2;
    QList<QStringList> m_sListInterWikiLinks_2;
    QList<QStringList> m_sListInterWikiLinksUrls_2;
    QList<QStringList> m_sListInterWikiLinksNames_2;
    QList<QStringList> m_sListInterWikiLinksIcons_2;
};

#endif // CINTERWIKI_H
