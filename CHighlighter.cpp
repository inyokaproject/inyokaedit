/**
 * \file CHighlighter.cpp
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
 * Syntax highlighting with builtin Qt functions.
 */

#include <QtGui>

#include "CHighlighter.h"

CHighlighter::CHighlighter( const QList<QStringList> sListIWiki,
                            const QStringList sListFlags,
                            const QString sTransOverview,
                            const QStringList sListMacroKeywords,
                            const QStringList sListParserKeywords,
                            QTextDocument *pParent )
    : QSyntaxHighlighter(pParent)
{
    qDebug() << "Start" << Q_FUNC_INFO;

    HighlightingRule myRule;
    QStringList interwikiLinksPatterns, macroPatterns, parserPatterns, textformatPatterns, flagsPatterns;

    // Headings (= Heading =)
    m_headingsFormat.setFontWeight(QFont::Bold);
    m_headingsFormat.setForeground(Qt::darkGreen);
    // 1-5 = at beginning and end, between A-Z, a-z, 0-9 (\\w), space (\\s), :, ", !, -, _, +, "
    //myrule.pattern = QRegExp("={1,5}[A-Za-z0-9\\s\\?\\(\\):!-_\\+/\"]+={1,5}");
    myRule.pattern = QRegExp("={1,5}[\\w\\s\\?\\(\\):!-_\\+/\"]+={1,5}");
    myRule.format = m_headingsFormat;
    m_highlightingRules.append(myRule);  // Collecting highlighting rules

    // Links (everything between [...])
    m_linksFormat.setForeground(Qt::darkBlue);
    // 1-5 = at beginning and end, between A-Z, a-z, 0-9, space, -, _, #
    myRule.pattern = QRegExp("\\[{1,1}[\\w\\s-_:\\(\\)/\\.#]+\\]{1,1}");
    myRule.format = m_linksFormat;
    m_highlightingRules.append(myRule);  // Collecting highlighting rules

    // Cells style in tables
    m_tablecellsFormat.setForeground(Qt::darkMagenta);
    myRule.pattern = QRegExp("\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
    myRule.format = m_tablecellsFormat;
    m_highlightingRules.append(myRule);

    // Define interwiki link keywords
    foreach( QStringList tmpStrList, sListIWiki ) {
        foreach( QString tmpStr, tmpStrList )
            interwikiLinksPatterns << "\\b" + tmpStr + "\\b";
    }

    // Format Interwiki Links
    m_interwikiLinksFormat.setForeground(Qt::blue);
    // Collecting highlighting rules
    foreach ( const QString &sPattern, interwikiLinksPatterns )
    {
        myRule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        myRule.format = m_interwikiLinksFormat;
        m_highlightingRules.append(myRule);
    }

    // Define macro keywords ([[Vorlage(...) etc.)
    foreach ( QString tmpStr, sListMacroKeywords )
    {
        macroPatterns << QRegExp::escape( "[[" + tmpStr + "(" );
    }
    macroPatterns << QRegExp::escape( ")]]" );

    // Format macros
    m_macrosFormat.setForeground(Qt::darkCyan);
    // Collecting highlighting rules
    foreach ( const QString &sPattern, macroPatterns )
    {
        myRule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        myRule.format = m_macrosFormat;
        m_highlightingRules.append(myRule);
    }

    // Define parser keywords ({{{#!code etc.)
    foreach ( QString tmpStr, sListParserKeywords )
    {
        parserPatterns << QRegExp::escape( "{{{#!" + tmpStr );
    }
    parserPatterns << QRegExp::escape( "{{{" ) << QRegExp::escape( "}}}" );

    // Format parser
    m_parserFormat.setForeground(Qt::darkRed);
    m_parserFormat.setFontWeight(QFont::Bold);
    // Collecting highlighting rules
    foreach ( const QString &sPattern, parserPatterns )
    {
        myRule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        myRule.format = m_parserFormat;
        m_highlightingRules.append(myRule);
    }

    // Define textformat keywords (Bold, italic etc.)
    textformatPatterns << QRegExp::escape("'''") << QRegExp::escape("''") << QRegExp::escape("`") << QRegExp::escape(" * ")
            << QRegExp::escape("__") << QRegExp::escape("--(") << QRegExp::escape(")--") << QRegExp::escape("^^(")
            << QRegExp::escape(")^^") << QRegExp::escape(",,(") << QRegExp::escape("),,") << QRegExp::escape("~+(")
            << QRegExp::escape(")+~") << QRegExp::escape("~-(") << QRegExp::escape(")-~") << QRegExp::escape("[[BR]]")
            << QRegExp::escape("\\\\") << QRegExp::escape("#tag:") << QRegExp::escape("# tag:") << QRegExp::escape("+++")
            << QRegExp::escape(" 1. ") << QRegExp::escape("----");

    // Format textformats
    m_textformatFormat.setForeground(Qt::red);
    // Collecting highlighting rules
    foreach ( const QString &sPattern, textformatPatterns )
    {
        myRule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        myRule.format = m_textformatFormat;
        m_highlightingRules.append(myRule);
    }

    // Comments (## comment)
    m_singleLineCommentFormat.setForeground(Qt::gray);
    myRule.pattern = QRegExp("^##[^\n]*");
    myRule.format = m_singleLineCommentFormat;
    m_highlightingRules.append(myRule); // Collecting highlighting rules

    // Define flags
    foreach ( QString tmpStr, sListFlags )
    {
        flagsPatterns << QRegExp::escape("{" + tmpStr + "}");
    }
    // Overview flag
    flagsPatterns << QRegExp::escape("{" + sTransOverview + "}");

    // Format flags
    m_flagsFormat.setForeground(Qt::darkYellow);
    // Collecting highlighting rules
    foreach ( const QString &sPattern, flagsPatterns )
    {
        myRule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        myRule.format = m_flagsFormat;
        m_highlightingRules.append(myRule);
    }
    qDebug() << "End" << Q_FUNC_INFO;
}

CHighlighter::~CHighlighter()
{
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

/****************************************************************************
*****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This code is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this code under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

// Apply collected highlighting rules
void CHighlighter::highlightBlock( const QString &sText )
{
    // Go through each highlighting rule
    // rules for every syntax element had been appended in constructor
    foreach ( const HighlightingRule &myRule, m_highlightingRules )
    {
        QRegExp express(myRule.pattern);
        int nIndex = express.indexIn(sText);
        while ( nIndex >= 0 )
        {
            int nLength = express.matchedLength();
            this->setFormat( nIndex, nLength, myRule.format );
            nIndex = express.indexIn(sText, nIndex + nLength);
        }
    }
    setCurrentBlockState(0);
}
