/**
 * \file xmlparser.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Xml parser for importing menus/dropdown/toolbars.
 */

#include "./xmlparser.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>

XmlParser::XmlParser() = default;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto XmlParser::parseXml(const QString &sXmlFile) -> bool {
  QFile xmlFile(sXmlFile);
  // Check if file exist and it's readable
  if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical() << "ERROR: Can not open \"" << xmlFile.fileName() << "\".";
    QMessageBox::critical(nullptr, QStringLiteral("Error"),
                          "Can not open \"" + xmlFile.fileName() + "\".");
    return false;
  }

  m_bInMenu = false;
  m_bInGroup = false;
  m_sMenuName = QLatin1String("");
  m_sListGroups.clear();
  m_sListGroupIcons.clear();
  m_sTmpGroupName = QLatin1String("");
  m_sTmpGroupIcon = QLatin1String("");
  m_sListNames.clear();
  m_sListInserts.clear();
  m_sListIcons.clear();
  m_sListTmpNames.clear();
  m_sListTmpInserts.clear();
  m_sListTmpIcons.clear();
  m_pXmlReader = new QXmlStreamReader(&xmlFile);

  while (!m_pXmlReader->atEnd() && !m_pXmlReader->hasError()) {
    QXmlStreamReader::TokenType token = m_pXmlReader->readNext();
    if (token == QXmlStreamReader::StartDocument) {
      continue;  // Just StartDocument - go to next
    }

    if (token == QXmlStreamReader::StartElement) {
      if (m_pXmlReader->name() == "menu") {
        if (!this->parseMenu()) {
          qCritical() << "ERROR: Parsing \"" << xmlFile.fileName() << "\"";
          qCritical() << "XML contains multiple \"menu\" entries! Line:" <<
                         m_pXmlReader->lineNumber();
          QMessageBox::critical(nullptr, QStringLiteral("Error"),
                                "Error while parsing \""
                                + xmlFile.fileName() + "\".");
          return false;
        }
        m_bInMenu = true;
        continue;
      }

      if (m_pXmlReader->name() == "group") {
        if (!this->parseGroup()) {
          qCritical() << "ERROR: Parsing \"" << xmlFile.fileName() << "\"";
          qCritical() << "Found \"group\" outside of \"menu\" or" <<
                         "\"group\" inside \"group\"! Line:" <<
                         m_pXmlReader->lineNumber();
          QMessageBox::critical(nullptr, QStringLiteral("Error"),
                                "Error while parsing \""
                                + xmlFile.fileName() + "\".");
          return false;
        }
        m_bInGroup = true;
        continue;
      }

      if (m_pXmlReader->name() == "element") {
        if (!this->parseElement()) {
          qCritical() << "ERROR: Parsing \"" << xmlFile.fileName() << "\"";
          qCritical() << "Found \"element\" outside of \"menu\" or" <<
                         "\"group\"! Line:" << m_pXmlReader->lineNumber();
          QMessageBox::critical(nullptr, QStringLiteral("Error"),
                                "Error while parsing \""
                                + xmlFile.fileName() + "\".");
          return false;
        }
        continue;
      }
    }

    if (token == QXmlStreamReader::EndElement) {
      if (m_pXmlReader->name() == "menu") {
        m_bInMenu = false;
      }

      if (m_pXmlReader->name() == "group") {
        m_sListGroups << m_sTmpGroupName;
        m_sListGroupIcons << m_sTmpGroupIcon;
        m_sTmpGroupName = QLatin1String("");
        m_sTmpGroupIcon = QLatin1String("");
        m_sListNames << m_sListTmpNames;
        m_sListInserts << m_sListTmpInserts;
        m_sListIcons << m_sListTmpIcons;
        m_sListTmpNames.clear();
        m_sListTmpInserts.clear();
        m_sListTmpIcons.clear();
        m_bInGroup = false;
      }
    }
  }

  if (m_pXmlReader->hasError()) {
    qCritical() << "ERROR: Parsing \"" << xmlFile.fileName() << "\"";
    qCritical() << "Line:" << m_pXmlReader->lineNumber() <<
                   "Column:" <<  m_pXmlReader->columnNumber();
    qCritical() << m_pXmlReader->errorString();
    QMessageBox::critical(nullptr, QStringLiteral("Error"),
                          "Error while parsing \""
                          + xmlFile.fileName() + "\".");
    return false;
  }

  m_pXmlReader->clear();
  delete m_pXmlReader;
  m_pXmlReader = nullptr;
  xmlFile.close();
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto XmlParser::parseMenu() -> bool {
  if (m_bInMenu || !m_sMenuName.isEmpty()) {
    return false;
  }

  m_sMenuName = m_pXmlReader->attributes().value(
                  QStringLiteral("name")).toString();
  if (m_sMenuName.isEmpty()) {
    m_sMenuName = QStringLiteral("Unnamed");
  }
  m_sPath = m_pXmlReader->attributes().value(
              QStringLiteral("path")).toString();

  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto XmlParser::parseGroup() -> bool {
  if (!m_bInMenu || m_bInGroup ||
      !m_sTmpGroupName.isEmpty() || !m_sTmpGroupIcon.isEmpty()) {
    return false;
  }

  m_sTmpGroupName = m_pXmlReader->attributes().value(
                      QStringLiteral("name")).toString();
  m_sTmpGroupIcon = m_pXmlReader->attributes().value(
                      QStringLiteral("icon")).toString();

  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto XmlParser::parseElement() -> bool {
  if (!m_bInMenu || !m_bInGroup) {
    return false;
  }

  m_sListTmpNames << m_pXmlReader->attributes().value(
                       QStringLiteral("name")).toString();
  if (m_sListTmpNames.last().isEmpty()) {
    m_sListTmpNames.last() = QStringLiteral("NAME NOT FOUND");
  }
  m_sListTmpInserts << m_pXmlReader->attributes().value(
                         QStringLiteral("insert")).toString();
  if (m_sListTmpInserts.last().isEmpty()) {
    m_sListTmpInserts.last() = QStringLiteral("INSERT NOT FOUND");
  }
  m_sListTmpIcons << m_pXmlReader->attributes().value(
                       QStringLiteral("icon")).toString();
  if (m_sListTmpIcons.last().isEmpty()) {
    m_sListTmpIcons.last() = QStringLiteral("ICON NOT FOUND");
  }

  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto XmlParser::getMenuName() const -> QString {
  return m_sMenuName;
}
auto XmlParser::getPath() const -> QString {
  return m_sPath;
}

auto XmlParser::getGroupNames() const -> QStringList {
  return m_sListGroups;
}
auto XmlParser::getGroupIcons() const -> QStringList {
  return m_sListGroupIcons;
}

auto XmlParser::getElementNames() const -> QList<QStringList> {
  return m_sListNames;
}
auto XmlParser::getElementInserts() const -> QList<QStringList> {
  return m_sListInserts;
}
auto XmlParser::getElementIcons() const -> QList<QStringList> {
  return m_sListIcons;
}
