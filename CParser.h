/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CParser.cpp
* Purpose:    Class definition for parser
***************************************************************************/

#ifndef CPARSER_H
#define CPARSER_H

#include <QWidget>
#include <QDir>

#include "CInyokaEdit.h"

// Qt classes
class QString;
class QTextDocument;
class QFile;
class QDir;

class CParser : public QObject
{
    Q_OBJECT

public:
    // Constructor
    CParser(QTextDocument *rawDocument, const QString &sUrlToWiki, const QDir TmpFileOutputDir);
    // Destructor
    ~CParser();

    // Starts generating HTML-code
    bool genOutput(const QString sActFile);

signals:
    void callShowPreview(const QString &);
    void callShowMessageBox(const QString &, const QString &);

private:

    void replaceLinks(QTextDocument *myRawDoc);
    void replaceTextformat(QTextDocument *myRawDoc);
    void replaceFlags(QTextDocument *myRawDoc);
    void replaceKeys(QTextDocument *myRawDoc);
    void replaceImages(QTextDocument *myRawDoc);
    void replaceAnchor(QTextDocument *myRawDoc);

    // Parse Macros ([[Vorlage(...) etc.)
    QString parseMacro(QTextBlock actParagraph);
    // Generates table of content
    QString parseTableOfContents(QTextBlock tabofcontents);
    // Generate tags (#tag: ...)
    QString generateTags(QTextBlock actParagraph);
    // Parse heading
    QString parseHeading(QTextBlock actParagraph);
    // Parse text samples ({{{#!vorlage)
    QString parseTextSample(QString actParagraph);
    // Parse code blocks ({{{ and {{{#!code)
    QString parseCodeBlock(QString actParagraph);
    // Parse image collection[[Vorlage(Bildersammlung, ... )]]
    QString parseImageCollection(QString actParagraph);
    // Parse List *
    QString parseList(QString actParagraph);

    // Text from editor
    QTextDocument *rawText, *copyOfrawText;

    // List for interwiki links (keywords, Url)
    QStringList sListInterwikiKey, sListInterwikiLink;
    // List for text formats (start keyword, end keyword, start HTML code, end HTML code)
    QStringList sListFormatStart, sListFormatEnd, sListFormatHtmlStart, sListFormatHtmlEnd;
    // List for flags
    QStringList sListFlags;

    const QString sWikiUrl;
    QDir TmpFileDir;
    bool bShowedMsgBoxAlready;

};

#endif // CPARSER_H
