/**
 * \file CParser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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
    CParser(QTextDocument *pRawDocument,
            const QDir &tmpFileOutputDir,
            const QDir &tmpImgDir,
            CSettings *pSettings,
            CTemplates *pTemplates);
    // Destructor
    ~CParser();

    // PUBLIC FOR DEBUGGING
    void replaceTemplates(QTextDocument *p_rawDoc);

    // Starts generating HTML-code
    QString genOutput(const QString &sActFile);

  private:
    // void replaceTemplates(QTextDocument *p_rawDoc);
    void replaceTextformat(QTextDocument *p_rawDoc);
    QString reinstertNoTranslate(const QString &sRawDoc);
    void replaceFlags(QTextDocument *p_rawDoc);
    void replaceKeys(QTextDocument *p_rawDoc);
    void replaceImages(QTextDocument *p_rawDoc);
    void replaceBreaks(QTextDocument *p_rawDoc);
    void replaceHorLine(QTextDocument *p_rawDoc);

    // Parse Macros ([[Vorlage(...) etc.)
    QString parseMacro(const QTextBlock actParagraph);
    // Generates table of content
    QString parseTableOfContents(const QTextBlock tabofcontents);
    // Generate tags (#tag: ...)
    QString generateTags(const QTextBlock actParagraph);
    // Parse headline
    QString parseHeadline(const QTextBlock actParagraph);
    // Parse text samples ({{{#!vorlage)
    QString parseTextSample(const QString &s_ActParagraph);
    // Parse code blocks ({{{ and {{{#!code)
    QString parseCodeBlock(const QString &s_ActParagraph);
    // Parse image collection[[Vorlage(Bildersammlung, ... )]]
    QString parseImageCollection(const QString &s_ActParagraph);
    // Parse List *
    QString parseList(const QString &s_ActParagraph);

    // Insert a box
    QString insertBox(const QString &sClass, const QString &sHeadline,
                      const QString &sContents, const QString &sRemark = "");

    // Text from editor
    QTextDocument *m_pRawText;

    QStringList m_sListNoTranslate;

    CParseLinks *m_pLinkParser;
    CParseImgMap *m_pMapParser;

    QDir m_tmpFileDir;
    QDir m_tmpImgDir;
    CSettings *m_pSettings;
    CTemplates *m_pTemplates;

    const QString m_sSEPARATOR;
};

#endif  // INYOKAEDIT_CPARSER_H_
