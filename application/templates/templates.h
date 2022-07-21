/**
 * \file templates.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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

#ifndef APPLICATION_TEMPLATES_TEMPLATES_H_
#define APPLICATION_TEMPLATES_TEMPLATES_H_

#include <QHash>
#include <QString>
#include <QStringList>

class Templates {
 public:
  Templates(const QString &sCommunity, const QString &sSharePath,
            const QString &sUserDataDir);

  auto getPreviewTemplate() const -> QString;
  auto getTemplateMap() const -> QHash<QString, QString>;
  auto getInyokaTplLangMap() const -> QHash<QString, QString>;
  auto getAllBoilerplates() const -> QStringList;

  // Mappings
  auto getIwlMap() const -> QHash<QString, QString>;
  auto getFlagMap() const -> QHash<QString, QString>;
  auto getTestedWithMap() const -> QHash<QString, QString>;
  auto getTestedWithTouchMap() const -> QHash<QString, QString>;
  // Txt mapping
  auto getSmiliesTxtMap() const -> QPair<QStringList, QStringList>;
  auto getFormatStartMap() const -> QPair<QStringList, QStringList>;
  auto getFormatEndMap() const -> QPair<QStringList, QStringList>;
  auto getFormatStartNoTranslateMap() const -> QPair<QStringList, QStringList>;
  auto getFormatEndNoTranslateMap() const -> QPair<QStringList, QStringList>;

 private:
  void initTemplates(const QString &sTplPath);
  void initHtmlTpl(const QString &sTplFile);
  static void initMap(const QString &sFile, const QChar cSplit,
                      QHash<QString, QString> *map);
  static void initTxtMap(const QString &sFile, const QChar cSplit,
                         QPair<QStringList, QStringList> *map);
  void initTextformats(const QString &sFileName);

  QString m_sPreviewTemplate;

  // Mappings
  QHash<QString, QString> m_MacroMap;
  QHash<QString, QString> m_TemplateMap;
  QHash<QString, QString> m_InyokaTplLangMap;

  QHash<QString, QString> m_IwlMap;
  QHash<QString, QString> m_FlagMap;
  QHash<QString, QString> m_TestedWithMap;
  QHash<QString, QString> m_TestedWithTouchMap;
  // Txt mapping (only if order of the mappings is important)
  QPair<QStringList, QStringList> m_SmiliesTxtMap;
  QPair<QStringList, QStringList> m_FormatStartMap;
  QPair<QStringList, QStringList> m_FormatEndMap;
  QPair<QStringList, QStringList> m_FormatStartNoTranslateMap;
  QPair<QStringList, QStringList> m_FormatEndNoTranslateMap;
};

#endif  // APPLICATION_TEMPLATES_TEMPLATES_H_
