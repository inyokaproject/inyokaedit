/**
 * \file templates.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2021 The InyokaEdit developers
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

#include <QString>
#include <QStringList>

class Templates {
 public:
    Templates(const QString &sCommunity, const QString &sSharePath,
              const QString &sUserDataDir);

    auto getPreviewTemplate() const -> QString;
    auto getListTplNamesINY() const -> QStringList;
    auto getListTemplatesINY() const -> QStringList;
    auto getListTplMacrosINY() const -> QStringList;
    auto getListTplNamesALL() const -> QStringList;
    auto getListTplMacrosALL() const -> QStringList;

    auto getListFormatStart() const -> QStringList;
    auto getListFormatEnd() const -> QStringList;
    auto getListFormatHtmlStart() const -> QStringList;
    auto getListFormatHtmlEnd() const -> QStringList;

    // Mappings
    auto getListIWLs() const -> QStringList;
    auto getListIWLUrls() const -> QStringList;
    auto getListFlags() const -> QStringList;
    auto getListFlagsImg() const -> QStringList;
    auto getListSmilies() const -> QStringList;
    auto getListSmiliesImg() const -> QStringList;
    auto getListTestedWith() const -> QStringList;
    auto getListTestedWithStrings() const -> QStringList;
    auto getListTestedWithTouch() const -> QStringList;
    auto getListTestedWithTouchStrings() const -> QStringList;

 private:
    void initTemplates(const QString &sTplPath);
    void initHtmlTpl(const QString &sTplFile);
    static void initMappings(const QString &sFileName,
                             const QChar cSplit,
                             QStringList &sListElements,
                             QStringList &sListMapping);
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
    QStringList m_sListIWLs;
    QStringList m_sListIWLUrls;
    QStringList m_sListFlags;
    QStringList m_sListFlagsImg;
    QStringList m_sListSmilies;
    QStringList m_sListSmiliesImg;
    QStringList m_sListTestedWith;
    QStringList m_sListTestedWithStrings;
    QStringList m_sListTestedWithTouch;
    QStringList m_sListTestedWithTouchStrings;
};

#endif  // APPLICATION_TEMPLATES_TEMPLATES_H_
