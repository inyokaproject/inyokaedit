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
* File Name:  CHighlighter.h
* Purpose:    Class definition for syntax highlighting
***************************************************************************/

#ifndef CHIGHLIGHTER_H
#define CHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

// Qt classes
class QTextDocument;

class CHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    // Constructor
    CHighlighter(QTextDocument *parent = 0);
    // Destrcutor
    ~CHighlighter();

protected:
    // Apply highlighting rules
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    // Collects highlighting rules
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat headingsFormat;
    QTextCharFormat interwikiLinksFormat;
    QTextCharFormat linksFormat;
    QTextCharFormat tablecellsFormat;
    QTextCharFormat macrosFormat;
    QTextCharFormat parserFormat;
    QTextCharFormat textformatFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat flagsFormat;

};

#endif // CHIGHLIGHTER_H
