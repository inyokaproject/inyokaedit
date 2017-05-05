/**
 * \file CXmlParser.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
#include <QMessageBox>

#include "./CXmlParser.h"

CXmlParser::CXmlParser(const QString &sXmlFile) {
  qDebug() << "Calling" << Q_FUNC_INFO;

  QFile XmlFile(sXmlFile);
  // Check if file exist and it's readable
  if (!XmlFile.open(QFile::ReadOnly | QFile::Text)) {
    qCritical() << "ERROR: Can not open \"" << XmlFile.fileName() << "\".";
    QMessageBox::critical(0, "Error",
                          "Can not open \"" + XmlFile.fileName() + "\".");
    exit(-4);
  }

  QXmlSimpleReader xmlReader;

  m_pXmlSource = new QXmlInputSource(&XmlFile);
  m_pHandler = new CHandler;

  xmlReader.setContentHandler(m_pHandler);
  xmlReader.setErrorHandler(m_pHandler);

  bool bOk = xmlReader.parse(m_pXmlSource);
  if (!bOk) {
    qCritical() << "ERROR: Parsing \"" << XmlFile.fileName() << "\"failed.";
    QMessageBox::critical(0, "Error",
                          "Error while parsing \""
                          + XmlFile.fileName() + "\".");
    exit(-5);
  }

  m_sMenuName = m_pHandler->m_sMenuName_2;
  m_sListGroups = m_pHandler->m_sListGroups_2;
  m_sListGroupIcons = m_pHandler->m_sListGroupIcons_2;
  m_sListTypes = m_pHandler->m_sListTypes_2;
  m_sListUrls = m_pHandler->m_sListUrls_2;
  m_sListNames = m_pHandler->m_sListNames_2;
  m_sListIcons = m_pHandler->m_sListIcons_2;

  /*
    for (int i = 0; i < sListGroups.size(); i++ ) {
        qDebug() << "\n" << sListGroups[i].toUpper()
                 << " (" << sListGroupIcons[i] << "): ";
        for (int j = 0; j < sListInterWikiLinks[i].size(); j++) {
            qDebug() << sListInterWikiLinks[i][j] << " "
                     << sListInterWikiLinksUrls[i][j] << " "
                     << sListInterWikiLinksNames[i][j] << " "
                     << sListInterWikiLinksIcons[i][j];
        }
    }
    */
}

// ----------------------------------------------------------------------------

QString CXmlParser::getMenuName() const {
  return m_sMenuName;
}

QStringList CXmlParser::getGrouplist() const {
  return m_sListGroups;
}

QStringList CXmlParser::getGroupIcons() const {
  return m_sListGroupIcons;
}

QList<QStringList> CXmlParser::getElementTypes() const {
  return m_sListTypes;
}

QList<QStringList> CXmlParser::getElementUrls() const {
  return m_sListUrls;
}

QList<QStringList> CXmlParser::getElementNames() const {
  return m_sListNames;
}

QList<QStringList> CXmlParser::getElementIcons() const {
  return m_sListIcons;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CXmlParser::CHandler::startDocument() {
  m_bInElement = false;
  return true;
}

// ----------------------------------------------------------------------------

bool CXmlParser::CHandler::endElement(const QString&, const QString&,
                                      const QString &sName) {
  if ("menu" == sName) {
    m_bInElement = false;
    m_sMenuName_2 = m_tmpMenuName;
  } else if ("group" == sName) {
    m_sListTypes_2 << m_tmpListTypes;
    m_sListUrls_2 << m_tmpListUrls;
    m_sListNames_2 << m_tmpListNames;
    m_sListIcons_2 << m_tmpListIcons;
  }

  return true;
}

// ----------------------------------------------------------------------------

bool CXmlParser::CHandler::startElement(const QString&, const QString&,
                                        const QString &sElement,
                                        const QXmlAttributes &attrs) {
  QString sMenuName("");
  QString sGroupName("");
  QString sGroupIcon("");
  QString sType("");
  QString sUrl("");
  QString sName("");
  QString sIcon("");

  // Found group
  if (m_bInElement && "group" == sElement) {
    m_tmpListTypes.clear();
    m_tmpListUrls.clear();
    m_tmpListNames.clear();
    m_tmpListIcons.clear();

    sGroupName = "GROUPNAME NOT FOUND";
    sGroupIcon = "NO ICON";

    for (int i = 0; i < attrs.count(); i++) {
      if ("name" == attrs.localName(i)) {
        sGroupName = attrs.value(i);
      } else if ("icon"== attrs.localName(i)) {
        sGroupIcon = attrs.value(i);
      }
    }
    m_sListGroups_2 << sGroupName;
    m_sListGroupIcons_2 << sGroupIcon;
  } else if (m_bInElement && "element" == sElement) {  // Found interwikilink
    sType = "TYPE NOT FOUND";
    sUrl = "URL NOT FOUND";
    sName = "NAME NOT FOUND";
    sIcon = "ICON NOT FOUND";

    for (int i = 0; i < attrs.count(); i++) {
      if ("type" == attrs.localName(i)) {
        sType = attrs.value(i);
      } else if ("url" == attrs.localName(i)) {
        sUrl = attrs.value(i);
      } else if ("name" == attrs.localName(i)) {
        sName = attrs.value(i);
      } else if ("icon" == attrs.localName(i)) {
        sIcon = attrs.value(i);
      }
    }

    m_tmpListTypes << sType;
    m_tmpListUrls << sUrl;
    m_tmpListNames << sName;
    m_tmpListIcons << sIcon;
  } else if ("menu" == sElement) {  // Found start of document
    m_bInElement = true;

    sMenuName = "Unnamed";
    for (int i = 0; i < attrs.count(); i++) {
      if ("name" == attrs.localName(i)) {
        sMenuName = attrs.value(i);
      }
    }
    m_tmpMenuName = sMenuName;
  }

  return true;
}
