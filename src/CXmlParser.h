/**
 * \file CXmlParser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Class definition.
 */

#ifndef INYOKAEDIT_CXMLPARSER_H_
#define INYOKAEDIT_CXMLPARSER_H_

#include <QXmlSimpleReader>

/**
 * \class CXmlParser
 * \brief Delivering templates (groups, url, icon) from xml file.
 */
class CXmlParser {
class CHandler;

  public:
    /**
    * \brief Constructor
    * \param sAppName Application name
    * \param sAppPath Application path
    * \param sFilePath (Relative) path and filename to xml file
    */
    CXmlParser(const QString &sAppName, const QString &sAppPath,
               const QString &sFilePath);

    /**
    * \brief Get menu name
    * \return String with name
    */
    QString getMenuName() const;

    /**
    * \brief Get list of template groups
    * \return List with group names
    */
    QStringList getGrouplist() const;

    /**
    * \brief Get list of template group icons
    * \return List with group icons
    */
    QStringList getGroupIcons() const;

    /**
    * \brief Get list of list template types
    * \return List of list of template types
    */
    QList<QStringList> getElementTypes() const;

    /**
    * \brief Get list of list of template urls
    * \return List of list of template urls
    */
    QList<QStringList> getElementUrls() const;

    /**
    * \brief Get list of list of template names
    * \return List of list of template names
    */
    QList<QStringList> getElementNames() const;

    /**
    * \brief Get list of list of template icons
    * \return List of list of template icons
    */
    QList<QStringList> getElementIcons() const;

  private:
    QXmlInputSource *m_myXmlSource;  /**< Pointer to xml input source file */
    CHandler *m_myHandler;  /**< Handle to xml parser module */

    QString m_sMenuName;
    QStringList m_sListGroups;        /**< Interwiki link groups list */
    QStringList m_sListGroupIcons;    /**< Interwiki link group icons list */
    QList<QStringList> m_sListTypes;  /**< List of list interwiki links */
    QList<QStringList> m_sListUrls;   /**< List of list interwiki link urls */
    QList<QStringList> m_sListNames;  /**< List of list interwiki link names */
    QList<QStringList> m_sListIcons;  /**< List of list interwiki link icons */
};

// ----------------------------------------------------------------------------

/**
 * \class CXmlParser::Handler
 * \brief Reading definitions from xml file.
 */
class CXmlParser::CHandler : public QXmlDefaultHandler {
    friend class CXmlParser;

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
    bool endElement(const QString&, const QString&, const QString &name);

    /**
    * \brief Found start of a xml element
    * \param name Element name
    * \param attrs Attribute name
    * \return True
    */
    bool startElement(const QString&, const QString&, const QString &name,
                      const QXmlAttributes &attrs);

  private:
    bool m_bInElement;

    QString m_tmpMenuName;
    QStringList m_tmpListTypes;
    QStringList m_tmpListUrls;
    QStringList m_tmpListNames;
    QStringList m_tmpListIcons;

    QString m_sMenuName_2;
    QStringList m_sListGroups_2;
    QStringList m_sListGroupIcons_2;
    QList<QStringList> m_sListTypes_2;
    QList<QStringList> m_sListUrls_2;
    QList<QStringList> m_sListNames_2;
    QList<QStringList> m_sListIcons_2;
};

#endif  // INYOKAEDIT_CINTERWIKI_H_
