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

#ifndef CPARSER_H
#define CPARSER_H

#include <QWidget>
#include <QtGui>
#include <QDir>
#include <QMessageBox>

#include "CInyokaEdit.h"
#include "CParseLinks.h"

// Qt classes
class QString;
class QTextDocument;
class QFile;
class QDir;

class CParseLinks;

/**
 * \class CParser
 * \brief Main parser module.
 * \todo Separate some parts in smaler modules.
 */
class CParser : public QObject
{
    Q_OBJECT

public:
    // Constructor
    CParser( QTextDocument *pRawDocument,
             const QString &sUrlToWiki,
             const QDir tmpFileOutputDir,
             const QDir tmpImgDir,
             const QList<QStringList> sListIWiki,
             const QList<QStringList> sListIWikiUrl,
             const bool bCheckLinks,
             const QString &sAppName,
             const QString &sAppDirPath,
             const QString &sTemplateLang );
    // Destructor
    ~CParser();

    // PUBLIC FOR DEBUGGING
    void replaceTemplates( QTextDocument *p_rawDoc );

    // Starts generating HTML-code
    bool genOutput( const QString sActFile );

    QStringList getFlaglist() const;
    QString getTransTemplate() const;
    QString getTransTOC() const;
    QString getTransImage() const;
    QString getTransCodeBlock() const;
    QString getTransAttachment() const;
    QString getTransAnchor() const;
    QString getTransDate() const;
    QString getTransOverview() const;

signals:
    void callShowPreview( const QString & );

private:
    void initTemplates( const QString sAppName, const QString sAppDirPath, const QString sTplLang );
    void initHTML( const QString sAppName, const QString sAppDirPath, const QString sFileName );
    void initFlags( const QString sAppName, const QString sAppDirPath, const QString sFileName );
    void initTextformats( const QString sAppName, const QString sAppDirPath, const QString sFileName );
    void initTranslations( const QString sAppName, const QString sAppDirPath, const QString sTplLang, const QString sFileName );

    //void replaceTemplates( QTextDocument *p_rawDoc );
    void replaceTextformat( QTextDocument *p_rawDoc );
    void replaceFlags( QTextDocument *p_rawDoc );
    void replaceKeys( QTextDocument *p_rawDoc );
    void replaceImages( QTextDocument *p_rawDoc );
    void replaceBreaks( QTextDocument *p_rawDoc );
    void replaceHorLine( QTextDocument *p_rawDoc );

    QString m_sPreviewTemplate;
    QStringList m_sListTplNames;
    QStringList m_sListTemplates;

    // Translations
    QString m_sTransTemplate;
    QString m_sTransTOC;
    QString m_sTransImage;
    QString m_sTransCodeBlock;
    QString m_sTransAttachment;
    QString m_sTransAnchor;
    QString m_sTransDate;
    QString m_sTransOverview;
    QString m_sRevText;
    QString m_sTagText;

    // Parse Macros ([[Vorlage(...) etc.)
    QString parseMacro( QTextBlock actParagraph );
    // Generates table of content
    QString parseTableOfContents( QTextBlock tabofcontents );
    // Generate tags (#tag: ...)
    QString generateTags( QTextBlock actParagraph );
    // Parse headline
    QString parseHeadline( QTextBlock actParagraph );
    // Parse text samples ({{{#!vorlage)
    QString parseTextSample( QString actParagraph );
    // Parse code blocks ({{{ and {{{#!code)
    QString parseCodeBlock( QString actParagraph );
    // Parse image collection[[Vorlage(Bildersammlung, ... )]]
    QString parseImageCollection( QString actParagraph );
    // Parse List *
    QString parseList( QString actParagraph );

    // Insert a box
    QString insertBox( const QString &sClass, const QString &sHeadline, const QString &sContents, const QString &sRemark = "" );

    // Text from editor
    QTextDocument *m_pRawText, *m_pCopyOfrawText;

    // List for text formats (start keyword, end keyword, start HTML code, end HTML code)
    QStringList m_sListFormatStart, m_sListFormatEnd, m_sListFormatHtmlStart, m_sListFormatHtmlEnd;
    // List for flags
    QStringList m_sListFlags;

    CParseLinks *m_pLinkParser;

    const QString m_sWikiUrl;
    QDir m_tmpFileDir;
    QDir m_tmpImgDir;
    bool m_bShowedMsgBoxAlready;
};

#endif // CPARSER_H
