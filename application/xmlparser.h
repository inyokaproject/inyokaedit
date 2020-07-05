/**
 * \file xmlparser.h
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
 * Class definition.
 */

#ifndef APPLICATION_XMLPARSER_H_
#define APPLICATION_XMLPARSER_H_

#include <QString>
#include <QStringList>

class QXmlStreamReader;

/**
 * \class XmlParser
 * \brief Delivering menu structure from xml file.
 */
class XmlParser {

 public:
    XmlParser();
    auto parseXml(const QString &sXmlFile) -> bool;

    auto getMenuName() const -> QString;
    auto getPath() const -> QString;
    auto getGroupNames() const -> QStringList;
    auto getGroupIcons() const -> QStringList;
    auto getElementNames() const -> QList<QStringList>;
    auto getElementInserts() const -> QList<QStringList>;
    auto getElementIcons() const -> QList<QStringList>;

 private:
    auto parseMenu() -> bool;
    auto parseGroup() -> bool;
    auto parseElement() -> bool;

    QXmlStreamReader *m_pXmlReader;
    bool m_bInMenu;
    bool m_bInGroup;
    QString m_sMenuName;
    QString m_sPath;
    QStringList m_sListGroups;
    QString m_sTmpGroupName;
    QStringList m_sListGroupIcons;
    QString m_sTmpGroupIcon;
    QList<QStringList> m_sListNames;
    QStringList m_sListTmpNames;
    QList<QStringList> m_sListInserts;
    QStringList m_sListTmpInserts;
    QList<QStringList> m_sListIcons;
    QStringList m_sListTmpIcons;
};

#endif  // APPLICATION_XMLPARSER_H_
