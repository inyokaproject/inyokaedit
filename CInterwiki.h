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
    /**
    * \brief Constructor
    * \param pApp Pointer to parent window
    */
    CInterWiki( const QApplication *pApp );

    /**
    * \brief Get list of interwiki link groups
    * \return List with group names
    */
    QStringList getInterwikiLinksGroups() const;

    /**
    * \brief Get list of interwiki link group icons
    * \return List with group icons
    */
    QStringList getInterwikiLinksGroupIcons() const;

    /**
    * \brief Get list of list of interwiki links
    * \return List of list of interwiki links
    */
    QList<QStringList> getInterwikiLinks() const;

    /**
    * \brief Get list of list of interwiki link urls
    * \return List of list of interwiki link urls
    */
    QList<QStringList> getInterwikiLinksUrls() const;

    /**
    * \brief Get list of list of interwiki link names
    * \return List of list of interwiki link names
    */
    QList<QStringList> getInterwikiLinksNames() const;

    /**
    * \brief Get list of list of interwiki link icons
    * \return List of list of interwiki link icons
    */
    QList<QStringList> getInterwikiLinksIcons() const;

private:
    QXmlInputSource *m_myXmlSource;  /**< Pointer to xml input source file */
    CIWikiLinksParser *m_myHandler;  /**< Handle to xml parser module */

    QStringList m_sListGroups;       /**< Interwiki link groups list */
    QStringList m_sListGroupIcons;   /**< Interwiki link group icons list */
    QList<QStringList> m_sListInterWikiLinks;       /**< List of list of interwiki links */
    QList<QStringList> m_sListInterWikiLinksUrls;   /**< List of list of interwiki link urls */
    QList<QStringList> m_sListInterWikiLinksNames;  /**< List of list of interwiki link names */
    QList<QStringList> m_sListInterWikiLinksIcons;  /**< List of list of interwiki link icons */

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
    /**
    * \brief Called at document start
    * \return True
    */
    bool startDocument();

    /**
    * \brief Found end of a xml element
    * \param name Element name
    * \return True
    */
    bool endElement( const QString&, const QString&, const QString &name );

    /**
    * \brief Found start of a xml element
    * \param name Element name
    * \param attrs Attribute name
    * \return True
    */
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
