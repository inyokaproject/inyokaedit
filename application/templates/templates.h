/**
 * \file templates.h
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
 * Class definition.
 */

#ifndef INYOKAEDIT_TEMPLATES_H_
#define INYOKAEDIT_TEMPLATES_H_


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
  QStringList getListIWLs() const;
  QStringList getListIWLUrls() const;
  QStringList getListFlags() const;
  QStringList getListFlagsImg() const;
  QStringList getListSmilies() const;
  QStringList getListSmiliesImg() const;
  QStringList getListTestedWith() const;
  QStringList getListTestedWithStrings() const;
  QStringList getListTestedWithTouch() const;
  QStringList getListTestedWithTouchStrings() const;

 private:
  void initTemplates(const QString &sTplPath);
  void initHtmlTpl(const QString &sFileName);
  void initMappings(const QString &sFileName,
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

  // Lists for mappings
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

#endif  // INYOKAEDIT_TEMPLATES_H_
