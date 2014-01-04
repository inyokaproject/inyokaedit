/**
 * \file CParser.h
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
 * Class definition for parser.
 */

#ifndef INYOKAEDIT_CPARSER_H_
#define INYOKAEDIT_CPARSER_H_

#include <QWidget>
#include <QtGui>
#include <QDir>
#include <QMessageBox>

#include "./CParseTemplates.h"
#include "./CParseLinks.h"
#include "./CParseImgMap.h"
#include "./CSettings.h"
#include "./CTemplates.h"

// Qt classes
class QString;
class QTextDocument;
class QDir;


/**
 * \class CParser
 * \brief Main parser module.
 * \todo Separate some parts in smaler modules.
 */
class CParser : public QObject {
    Q_OBJECT

  public:
    // Constructor
    CParser(const QDir &tmpFileOutputDir,
            const QDir &tmpImgDir,
            const QString &sInyokaUrl,
            const bool bCheckLinks,
            CTemplates *pTemplates);
    // Destructor
    ~CParser();

    // PUBLIC FOR DEBUGGING
    void replaceTemplates(QTextDocument *p_rawDoc);

    // Starts generating HTML-code
    QString genOutput(const QString &sActFile, QTextDocument *p_rawDocument);

  public slots:
    void updateSettings(const QString &sInyokaUrl, const bool bCheckLinks);

  private:
    // void replaceTemplates(QTextDocument *p_rawDoc);

    void replaceCodeblocks(QTextDocument *p_rawDoc);
    void filterNoTranslate(QTextDocument *p_rawDoc);
    void reinstertNoTranslate(QTextDocument *p_rawDoc);

    void removeComments(QTextDocument *p_rawDoc);
    void generateParagraphs(QTextDocument *p_rawDoc);

    void replaceTextformat(QTextDocument *p_rawDoc,
                           const QStringList &sListFormatStart,
                           const QStringList &sListFormatEnd,
                           const QStringList &sListHtmlStart,
                           const QStringList &sListHtmlEnd);

    void replaceTables(QTextDocument *p_rawDoc);
    QString createTable(const QStringList &sListLines);
    void replaceLists(QTextDocument *p_rawDoc);
    void replaceFlags(QTextDocument *p_rawDoc);
    void replaceImages(QTextDocument *p_rawDoc);
    void replaceQuotes(QTextDocument *p_rawDoc);
    void replaceBreaks(QTextDocument *p_rawDoc);
    void replaceHorLine(QTextDocument *p_rawDoc);
    void replaceHeadlines(QTextDocument *p_rawDoc);
    void replaceTableOfContents(QTextDocument *p_rawDoc);
    void replaceDates(QTextDocument *p_rawDoc);
    QString generateTags(QTextDocument *p_rawDoc);
    QString highlightCode(const QString &sLanguage, const QString &sCode);

    // Text from editor
    QTextDocument *m_pRawText;

    QStringList m_sListNoTranslate;
    QStringList m_sListHeadline_1;
    QString m_sTags;

    CParseTemplates *m_pTemplateParser;
    CParseLinks *m_pLinkParser;
    CParseImgMap *m_pMapParser;

    QDir m_tmpFileDir;
    QDir m_tmpImgDir;
    QString m_sInyokaUrl;
    QString m_sCurrentFile;
    CTemplates *m_pTemplates;
};

#endif  // INYOKAEDIT_CPARSER_H_
