/**
 * \file CTemplates.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2014 The InyokaEdit developers
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

#ifndef INYOKAEDIT_CTEMPLATES_H_
#define INYOKAEDIT_CTEMPLATES_H_

#include "./CXmlParser.h"

#if defined(TEMPLATES_LIBRARY)
#  define TEMPLATES_EXPORT Q_DECL_EXPORT
#else
#  define TEMPLATES_EXPORT Q_DECL_IMPORT
#endif

class TEMPLATES_EXPORT CTemplates : public QObject {
    Q_OBJECT

  public:
    CTemplates(const QString &sTplLang, const bool &bDebug);
    ~CTemplates();

    QString getPreviewTemplate() const;
    QStringList getListTplNamesINY() const;
    QStringList getListTemplatesINY() const;
    QStringList getListTplMacrosINY() const;
    QStringList getListTplNamesALL() const;
    QStringList getListTplMacrosALL() const;
    QString getTransTemplate() const;
    QString getTransTOC() const;
    QString getTransImage() const;
    QString getTransCodeBlock() const;
    QString getTransAttachment() const;
    QString getTransAnchor() const;
    QString getTransDate() const;
    QString getTransTable() const;
    QString getTransRev() const;
    QString getTransTag() const;
    QString getTransKnowledge() const;

    QStringList getListFormatStart() const;
    QStringList getListFormatEnd() const;
    QStringList getListFormatHtmlStart() const;
    QStringList getListFormatHtmlEnd() const;

    // Image map
    QStringList getListFlags() const;
    QStringList getListFlagsImg() const;
    QStringList getListSmilies() const;
    QStringList getListSmiliesImg() const;

    CXmlParser *getTPLs() const;
    CXmlParser *getIWLs() const;
    CXmlParser *getDropTPLs() const;

  private:
    void initTemplates();
    void initHtmlTpl(const QString &sFileName);
    void initImgMap(const QString &sFileName,
                    QStringList &sListElements,
                    QStringList &sListImgSource);
    void initTextformats(const QString &sFileName);
    void initTranslations(const QString &sFileName);

    QString m_sTplLang;
    bool m_bDebug;
    QString m_sAppName;
    QString m_sAppPath;

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

    // Lists for image maps
    QStringList m_sListFlags;
    QStringList m_sListFlagsImg;
    QStringList m_sListSmilies;
    QStringList m_sListSmiliesImg;

    // Translations
    QString m_sTransTemplate;
    QString m_sTransTOC;
    QString m_sTransImage;
    QString m_sTransCodeBlock;
    QString m_sTransAttachment;
    QString m_sTransAnchor;
    QString m_sTransDate;
    QString m_sTransRevText;
    QString m_sTransTagText;
    QString m_sTransTable;
    QString m_sTransKnowledge;

    CXmlParser *m_pInterWikiLinks;
    CXmlParser *m_pMarkupTemplates;
    CXmlParser *m_pDropdownTemplates;
};

#endif  // INYOKAEDIT_CTEMPLATES_H_
