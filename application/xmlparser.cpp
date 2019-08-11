/**
 * \file xmlparser.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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
 * Xml parser for importing menus/dropdown/toolbars.
 */

#include "./xmlparser.h"

#include <QDebug>
#include <QMessageBox>

XmlParser::XmlParser() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool XmlParser::parseXml(const QString &sXmlFile) {
  QFile XmlFile(sXmlFile);
  // Check if file exist and it's readable
  if (!XmlFile.open(QFile::ReadOnly | QFile::Text)) {
    qCritical() << "ERROR: Can not open \"" << XmlFile.fileName() << "\".";
    QMessageBox::critical(nullptr, "Error",
                          "Can not open \"" + XmlFile.fileName() + "\".");
    return false;
  }

  QXmlSimpleReader xmlReader;
  m_pXmlSource = new QXmlInputSource(&XmlFile);
  m_pHandler = new Handler;

  xmlReader.setContentHandler(m_pHandler);
  xmlReader.setErrorHandler(m_pHandler);

  bool bOk = xmlReader.parse(m_pXmlSource);
  if (!bOk) {
    qCritical() << "ERROR: Parsing \"" << XmlFile.fileName() << "\"failed.";
    QMessageBox::critical(nullptr, "Error",
                          "Error while parsing \""
                          + XmlFile.fileName() + "\".");
    return false;
  }

  m_sMenuName = m_pHandler->m_sMenuName_2;
  m_sPath = m_pHandler->m_sPath_2;
  m_sListGroups = m_pHandler->m_sListGroups_2;
  m_sListGroupIcons = m_pHandler->m_sListGroupIcons_2;
  m_sListNames = m_pHandler->m_sListNames_2;
  m_sListInserts = m_pHandler->m_sListInserts_2;
  m_sListIcons = m_pHandler->m_sListIcons_2;

  if (nullptr != m_pXmlSource) {
    delete m_pXmlSource;
  }
  m_pXmlSource = nullptr;
  if (nullptr != m_pHandler) {
    delete m_pHandler;
  }
  m_pHandler = nullptr;

  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString XmlParser::getMenuName() const {
  return m_sMenuName;
}
QString XmlParser::getPath() const {
  return m_sPath;
}

QStringList XmlParser::getGroupNames() const {
  return m_sListGroups;
}
QStringList XmlParser::getGroupIcons() const {
  return m_sListGroupIcons;
}

QList<QStringList> XmlParser::getElementNames() const {
  return m_sListNames;
}
QList<QStringList> XmlParser::getElementInserts() const {
  return m_sListInserts;
}
QList<QStringList> XmlParser::getElementIcons() const {
  return m_sListIcons;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool XmlParser::Handler::startDocument() {
  m_bInElement = false;
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool XmlParser::Handler::endElement(const QString&, const QString&,
                                    const QString &sName) {
  if ("menu" == sName) {
    m_bInElement = false;
    m_sMenuName_2 = m_tmpMenuName;
    m_sPath_2 = m_tmpPath;
  } else if ("group" == sName) {
    m_sListNames_2 << m_tmpListNames;
    m_sListInserts_2 << m_tmpListInserts;
    m_sListIcons_2 << m_tmpListIcons;
  }

  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool XmlParser::Handler::startElement(const QString&, const QString&,
                                      const QString &sElement,
                                      const QXmlAttributes &attrs) {
  QString sMenuName("");
  QString sPath("");
  QString sGroupName("");
  QString sGroupIcon("");
  QString sName("");
  QString sInsert("");
  QString sIcon("");

  // Found group
  if (m_bInElement && "group" == sElement) {
    m_tmpListNames.clear();
    m_tmpListInserts.clear();
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
  } else if (m_bInElement && "element" == sElement) {  // Found element
    sName = "NAME NOT FOUND";
    sInsert = "INSERT NOT FOUND";
    sIcon = "ICON NOT FOUND";

    for (int i = 0; i < attrs.count(); i++) {
      if ("name" == attrs.localName(i)) {
        sName = attrs.value(i);
      } else if ("insert" == attrs.localName(i)) {
        sInsert = attrs.value(i);
      } else if ("icon" == attrs.localName(i)) {
        sIcon = attrs.value(i);
      }
    }

    m_tmpListNames << sName;
    m_tmpListInserts << sInsert;
    m_tmpListIcons << sIcon;
  } else if ("menu" == sElement) {  // Found start of document
    m_bInElement = true;

    sMenuName = "Unnamed";
    sPath = "";
    for (int i = 0; i < attrs.count(); i++) {
      if ("name" == attrs.localName(i)) {
        sMenuName = attrs.value(i);
      } else if ("path" == attrs.localName(i)) {
        sPath = attrs.value(i);
      }
    }
    m_tmpMenuName = sMenuName;
    m_tmpPath = sPath;
  }

  return true;
}
