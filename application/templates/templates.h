/**
 * \file templates.h
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
 * Class definition.
 */

#ifndef APPLICATION_TEMPLATES_TEMPLATES_H_
#define APPLICATION_TEMPLATES_TEMPLATES_H_

#include <QMap>
#include <QString>
#include <QStringList>

class Templates {
  public:
    Templates(const QString &sCommunity, const QString &sSharePath,
              const QString &sUserDataDir);

    QString getPreviewTemplate() const;
    QStringList getListTplNamesINY() const;
    QStringList getListTemplatesINY() const;
    QStringList getListTplMacrosINY() const;
    QStringList getListTplNamesALL() const;
    QStringList getListTplMacrosALL() const;

    QStringList getListFormatStart() const;
    QStringList getListFormatEnd() const;
    QStringList getListFormatHtmlStart() const;
    QStringList getListFormatHtmlEnd() const;

    // Mappings
    QMap<QString, QString> getIwlMap() const;
    QMap<QString, QString> getFlagMap() const;
    QMap<QString, QString> getSmileyMap() const;
    QMap<QString, QString> getTestedWithMap() const;
    QMap<QString, QString> getTestedWithTouchMap() const;

  private:
    void initTemplates(const QString &sTplPath);
    void initHtmlTpl(const QString &sFileName);
    void initMappings(const QString &sFileName, QMap<QString, QString> &map);
    void initTextformats(const QString &sFileName);

    QString m_sPreviewTemplate;
    QStringList m_sListTplNamesINY;
    QStringList m_sListTemplatesINY;
    QStringList m_sListTplMacrosINY;

    QStringList m_sListTplNamesALL;
    QStringList m_sListTplMacrosALL;

    // List for text formats (start keyword, end keyword,
    // start HTML code, end HTML code)
    QStringList m_sListFormatStart;
    QStringList m_sListFormatEnd;
    QStringList m_sListFormatHtmlStart;
    QStringList m_sListFormatHtmlEnd;

    // Mappings
    QMap<QString, QString> m_mapIwl;
    QMap<QString, QString> m_mapFlag;
    QMap<QString, QString> m_mapSmiley;
    QMap<QString, QString> m_mapTestedWith;
    QMap<QString, QString> m_mapTestedWithTouch;
};

#endif  // APPLICATION_TEMPLATES_TEMPLATES_H_
