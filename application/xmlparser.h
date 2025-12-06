// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_XMLPARSER_H_
#define APPLICATION_XMLPARSER_H_

#include <QString>
#include <QStringList>

class QXmlStreamReader;

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
