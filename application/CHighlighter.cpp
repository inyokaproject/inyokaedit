/**
 * \file CHighlighter.cpp
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
 * Syntax highlighting with builtin Qt functions.
 */

#include <QApplication>
#include <QtGui>

#include "./CHighlighter.h"

// TODO: Move highlighter to plugin -> TPL macros from template library?
// Add snap-in for highlighter into settings dialog (additional tab)

CHighlighter::CHighlighter(CTemplates *pTemplates,
                           const QString &sStyleFile,
                           QTextDocument *pParent)
    : QSyntaxHighlighter(pParent),
      m_pTemplates(pTemplates),
      m_sSEPARATOR("|") {
    qDebug() << "Calling" << Q_FUNC_INFO;

    this->getTranslations();

    m_pStyleSet = NULL;
    QFileInfo fiStylePath(sStyleFile);
    this->readStyle(fiStylePath.baseName());
}

CHighlighter::~CHighlighter() {
    if (NULL != m_pStyleSet) {
        delete m_pStyleSet;
    }
    m_pStyleSet = NULL;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CHighlighter::readStyle(const QString &sStyle) {
    bool bOk = false;
    m_bSystemForeground = false;
    m_bSystemBackground = false;
    QString sTmpKey;
    QColor tmpColor;

    if (NULL != m_pStyleSet) {
        delete m_pStyleSet;
    }
    m_pStyleSet = NULL;

#if defined _WIN32
    m_pStyleSet = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                qApp->applicationName().toLower(), sStyle);
#else
    m_pStyleSet = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                qApp->applicationName().toLower(), sStyle);
#endif

    if (!QFile::exists(m_pStyleSet->fileName())) {
        qWarning() << "Could not find/open highlighting style file:" <<
                      m_pStyleSet->fileName();
    }

    m_colorForeground = QApplication::palette().color(QPalette::Text);
    sTmpKey = m_pStyleSet->value("Foreground", "System")
            .toString();
    if (sTmpKey.toLower() != "system") {
        tmpColor.setRgb(sTmpKey.toInt(&bOk, 16));
        if (bOk) {
            m_colorForeground = tmpColor;
        }
    } else {
        m_bSystemForeground = true;
    }

    m_colorBackground = QApplication::palette().color(QPalette::Base);
    sTmpKey = m_pStyleSet->value("Background", "System")
            .toString();
    if (sTmpKey.toLower() != "system") {
        tmpColor.setRgb(sTmpKey.toInt(&bOk, 16));
        if (bOk) {
            m_colorBackground = tmpColor;
        }
    } else {
        m_bSystemBackground = true;
    }

    m_pStyleSet->beginGroup("Style");
    sTmpKey = m_pStyleSet->value("Heading", "0x008000" + m_sSEPARATOR + "true")
            .toString();
    this->evalKey(sTmpKey, m_headingsFormat);
    sTmpKey = m_pStyleSet->value("Hyperlink", "0x000080").toString();
    this->evalKey(sTmpKey, m_linksFormat);
    sTmpKey = m_pStyleSet->value("InterWiki", "0x0000ff").toString();
    this->evalKey(sTmpKey, m_interwikiLinksFormat);
    sTmpKey = m_pStyleSet->value("Macro", "0x008080").toString();
    this->evalKey(sTmpKey, m_macrosFormat);
    sTmpKey = m_pStyleSet->value("Parser", "0x800000" + m_sSEPARATOR + "true")
            .toString();
    this->evalKey(sTmpKey, m_parserFormat);
    sTmpKey = m_pStyleSet->value("Comment", "0xa0a0a4").toString();
    this->evalKey(sTmpKey, m_commentFormat);
    sTmpKey = m_pStyleSet->value("ImgMap", "0x808000").toString();
    this->evalKey(sTmpKey, m_imgMapFormat);
    sTmpKey = m_pStyleSet->value("TableCellFormating", "0x800080").toString();
    this->evalKey(sTmpKey, m_tablecellsFormat);
    sTmpKey = m_pStyleSet->value("TextFormating", "0xff0000").toString();
    this->evalKey(sTmpKey, m_textformatFormat);
    sTmpKey = m_pStyleSet->value("List", "0xff0000").toString();
    this->evalKey(sTmpKey, m_listFormat);
    sTmpKey = m_pStyleSet->value("NewTableLine", "0xff0000").toString();
    this->evalKey(sTmpKey, m_newTableLineFormat);
    sTmpKey = m_pStyleSet->value("Misc", "0xff0000").toString();
    this->evalKey(sTmpKey, m_miscFormat);
    m_pStyleSet->endGroup();

    this->defineRules();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CHighlighter::evalKey(const QString &sKey, QTextCharFormat &charFormat) {
    bool bOk = false;
    QColor tmpColor;
    QBrush tmpBrush;

    // Set defaults
    tmpBrush.setColor(Qt::transparent);
    tmpBrush.setStyle(Qt::SolidPattern);
    charFormat.setBackground(tmpBrush);
    tmpBrush.setColor(Qt::black);
    tmpBrush.setStyle(Qt::SolidPattern);
    charFormat.setForeground(tmpBrush);
    charFormat.setFontWeight(QFont::Normal);
    charFormat.setFontItalic(false);

    QStringList sListTmp = sKey.split(m_sSEPARATOR);

    // Foreground color
    if (sListTmp.size() > 0) {
        tmpColor.setRgb(sListTmp[0].trimmed().toInt(&bOk, 16));
        if (bOk) {
            tmpBrush.setColor(tmpColor);
            charFormat.setForeground(tmpBrush);
        }
        // Font weight
        if (sListTmp.size() > 1) {
            if ("true" == sListTmp[1].trimmed().toLower()) {
                charFormat.setFontWeight(QFont::Bold);
            }
            // Italic
            if (sListTmp.size() > 2) {
                if ("true" == sListTmp[2].trimmed().toLower()) {
                    charFormat.setFontItalic(true);
                }
                // Background
                if (sListTmp.size() > 3) {
                    tmpColor.setRgb(sListTmp[3].trimmed().toInt(&bOk, 16));
                    if (bOk) {
                        tmpBrush.setColor(tmpColor);
                        charFormat.setBackground(tmpBrush);
                    }
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CHighlighter::saveStyle() {
    if (m_bSystemForeground) {
        m_pStyleSet->setValue("Foreground", "System");
    } else {
        m_pStyleSet->setValue("Foreground", "0x" + m_colorForeground.name()
                              .remove("#"));
    }
    if (m_bSystemBackground) {
        m_pStyleSet->setValue("Background", "System");
    } else {
        m_pStyleSet->setValue("Background", "0x" + m_colorBackground.name()
                              .remove("#"));
    }

    m_pStyleSet->beginGroup("Style");
    this->writeFormat("TextFormating", m_textformatFormat);
    this->writeFormat("Heading", m_headingsFormat);
    this->writeFormat("Hyperlink", m_linksFormat);
    this->writeFormat("InterWiki", m_interwikiLinksFormat);
    this->writeFormat("Macro", m_macrosFormat);
    this->writeFormat("Parser", m_parserFormat);
    this->writeFormat("List", m_listFormat);
    this->writeFormat("NewTableLine", m_newTableLineFormat);
    this->writeFormat("TableCellFormating", m_tablecellsFormat);
    this->writeFormat("ImgMap", m_imgMapFormat);
    this->writeFormat("Misc", m_miscFormat);
    this->writeFormat("Comment", m_commentFormat);
    m_pStyleSet->endGroup();
}

// ----------------------------------------------------------------------------

void CHighlighter::writeFormat(const QString &sKey,
                               const QTextCharFormat &charFormat) {
    // Foreground color
    QString sValue = "0x" + charFormat.foreground().color().name().remove("#");

    // Font weight
    if (QFont::Bold == charFormat.fontWeight()) {
        sValue += m_sSEPARATOR + "true";
    } else {
        sValue += m_sSEPARATOR + "false";
    }

    // Italic
    if (charFormat.fontItalic()) {
        sValue += m_sSEPARATOR + "true";
    } else {
        sValue += m_sSEPARATOR + "false";
    }

    // Background color
    if (charFormat.background().isOpaque()) {
        sValue += m_sSEPARATOR +
                  "0x" + charFormat.background().color().name().remove("#");
    }

    m_pStyleSet->setValue(sKey, sValue);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CHighlighter::getTranslations() {
    m_sListMacroKeywords << m_pTemplates->getTransTemplate()
                         << m_pTemplates->getTransTOC()
                         << m_pTemplates->getTransImage()
                         << m_pTemplates->getTransAnchor()
                         << m_pTemplates->getTransAttachment()
                         << m_pTemplates->getTransDate();

    m_sListParserKeywords << m_pTemplates->getTransTemplate().toLower()
                          << m_pTemplates->getTransCodeBlock().toLower();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CHighlighter::defineRules() {
    HighlightingRule rule;
    QStringList sListRegExpPatterns;
    QString sTmpRegExp;
    m_highlightingRules.clear();

    // Headings (= Heading =)
    rule.format = m_headingsFormat;
    for (int i = 5; i > 0; i--) {
        rule.pattern = QRegExp("^\\s*={" + QString::number(i) + "}[^=]+={" +
                                 QString::number(i) + "}\\s*$");
        m_highlightingRules.append(rule);
    }

    // Links - everything between [...]
    rule.format = m_linksFormat;
    rule.pattern = QRegExp("\\[{1,1}.+\\]{1,1}");
    m_highlightingRules.append(rule);

    // Cell style in tables
    rule.format = m_tablecellsFormat;
    rule.pattern = QRegExp("^\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
    m_highlightingRules.append(rule);
    rule.pattern = QRegExp("\\|\\|\\s*\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
    m_highlightingRules.append(rule);

    // New table line
    rule.format = m_newTableLineFormat;
    rule.pattern = QRegExp("^\\+{3}$");
    m_highlightingRules.append(rule);
    rule.pattern = QRegExp("\\|\\|");
    m_highlightingRules.append(rule);

    // InterWiki-Links
    sListRegExpPatterns.clear();
    foreach (QStringList tmpStrList, m_pTemplates->getIWLs()->getElementTypes()) {
        foreach (QString tmpStr, tmpStrList) {
            sListRegExpPatterns << "\\[{1,1}\\b" + tmpStr + "\\b:.+\\]{1,1}";
        }
    }
    foreach (const QString &sPattern, sListRegExpPatterns) {
        rule.format = m_interwikiLinksFormat;
        rule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        m_highlightingRules.append(rule);
    }

    // Macros ([[Vorlage(...) etc.)
    sListRegExpPatterns.clear();
    foreach (QString tmpStr, m_sListMacroKeywords) {
        sListRegExpPatterns << QRegExp::escape("[[" + tmpStr + "(");
    }
    // Bad workaround for space between keyword and (
    foreach (QString tmpStr, m_sListMacroKeywords) {
        sListRegExpPatterns << QRegExp::escape("[[" + tmpStr + " (");
    }
    sListRegExpPatterns << QRegExp::escape(")]]");
    foreach (const QString &sPattern, sListRegExpPatterns) {
        rule.format = m_macrosFormat;
        rule.pattern = QRegExp(sPattern, Qt::CaseInsensitive);
        m_highlightingRules.append(rule);
    }

    // Parser ({{{#!code etc.)
    sListRegExpPatterns.clear();
    foreach (QString tmpStr, m_sListParserKeywords) {
        sListRegExpPatterns << QRegExp::escape("{{{#!" + tmpStr);
    }
    sListRegExpPatterns << QRegExp::escape("{{{") << QRegExp::escape("}}}");
    foreach (const QString &sPattern, sListRegExpPatterns) {
        rule.format = m_parserFormat;
        rule.pattern = QRegExp(sPattern, Qt::CaseInsensitive);
        m_highlightingRules.append(rule);
    }

    // Define textformat keywords (bold, italic, etc.)
    sListRegExpPatterns.clear();
    sListRegExpPatterns.append(m_pTemplates->getListFormatStart());
    sListRegExpPatterns.append(m_pTemplates->getListFormatEnd());
    sListRegExpPatterns.removeDuplicates();
    foreach (QString sPattern, sListRegExpPatterns) {
        rule.format = m_textformatFormat;
        if (sPattern.startsWith("RegExp=")) {
            sTmpRegExp = sPattern.remove("RegExp=");
        } else {
            sTmpRegExp = QRegExp::escape(sPattern);
        }
        rule.pattern = QRegExp(sTmpRegExp, Qt::CaseSensitive);
        m_highlightingRules.append(rule);
    }

    // Comments (## comment)
    rule.format = m_commentFormat;
    rule.pattern = QRegExp("^##.*$");
    m_highlightingRules.append(rule);

    // Image map elements (flags, smilies, etc.)
    sListRegExpPatterns.clear();
    foreach (QString tmpStr, m_pTemplates->getListFlags()) {
        sListRegExpPatterns << QRegExp::escape(tmpStr);
    }
    foreach (QString tmpStr, m_pTemplates->getListSmilies()) {
        sListRegExpPatterns << QRegExp::escape(tmpStr) + "\\s+";
    }
    foreach (const QString &sPattern, sListRegExpPatterns) {
        rule.format = m_imgMapFormat;
        rule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        m_highlightingRules.append(rule);
    }

    // List
    rule.format = m_listFormat;
    rule.pattern = QRegExp("^\\s+\\*\\s+");
    m_highlightingRules.append(rule);
    rule.format = m_listFormat;
    rule.pattern = QRegExp("^\\s+1\\.\\s+");
    m_highlightingRules.append(rule);

    // Misc
    sListRegExpPatterns.clear();
    sListRegExpPatterns << QRegExp::escape("[[BR]]") << QRegExp::escape("\\\\");
    foreach (const QString &sPattern, sListRegExpPatterns) {
        rule.pattern = QRegExp(sPattern, Qt::CaseSensitive);
        rule.format = m_miscFormat;
        m_highlightingRules.append(rule);
    }
    rule.format = m_miscFormat;
    rule.pattern = QRegExp("^#tag:");
    m_highlightingRules.append(rule);
    rule.pattern = QRegExp("^# tag:");
    m_highlightingRules.append(rule);
    rule.pattern = QRegExp("^----$");
    m_highlightingRules.append(rule);
    rule.pattern = QRegExp("^>+");
    m_highlightingRules.append(rule);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CHighlighter::getHighlightBG() const {
    return m_colorBackground.name();
}
QString CHighlighter::getHighlightFG() const {
    return m_colorForeground.name();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/****************************************************************************
*****************************************************************************
**
** Copyright(C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation(qt-info@nokia.com)
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
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

// Apply collected highlighting rules
void CHighlighter::highlightBlock(const QString &sText) {
    // Go through each highlighting rule
    // rules for every syntax element had been appended in constructor
    foreach (const HighlightingRule &rule, m_highlightingRules) {
        QRegExp express(rule.pattern);
        express.setMinimal(true);
        int nIndex = express.indexIn(sText);
        while (nIndex >= 0) {
            int nLength = express.matchedLength();
            this->setFormat(nIndex, nLength, rule.format);
            nIndex = express.indexIn(sText, nIndex + nLength);
        }
    }
    setCurrentBlockState(0);
}
