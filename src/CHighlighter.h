/**
 * \file CHighlighter.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Class definition for syntax highlighting.
 */

#ifndef INYOKAEDIT_CHIGHLIGHTER_H_
#define INYOKAEDIT_CHIGHLIGHTER_H_

#include <QHash>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "./CTemplates.h"

// Qt classes
class QSettings;
class QTextDocument;

/**
 * \class CHighlighter
 * \brief Syntax highlighting
 */
class CHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

  public:
    // Constructor
    CHighlighter(CTemplates *pTemplates, const QString &sAppName,
                 const QString &sStyleFile, QTextDocument *pParent = 0);
    // Destrcutor
    ~CHighlighter();

    QColor getForeground() const;
    QColor getBackground() const;
    void saveStyle();

  protected:
    // Apply highlighting rules
    void highlightBlock(const QString &sText);

  private:
    void readStyle();
    void getTranslations();
    void defineRules();
    void writeFormat(const QString &sKey, const QTextCharFormat &charFormat);

    void evalKey(const QString &sKey, QTextCharFormat &charFormat);

    QSettings *m_pStyleSet;
    CTemplates *m_pTemplates;
    QStringList m_sListMacroKeywords;
    QStringList m_sListParserKeywords;

    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };
    // Collects highlighting rules
    QVector<HighlightingRule> m_highlightingRules;

    QTextCharFormat m_headingsFormat;
    QTextCharFormat m_interwikiLinksFormat;
    QTextCharFormat m_linksFormat;
    QTextCharFormat m_tablecellsFormat;
    QTextCharFormat m_newTableLineFormat;
    QTextCharFormat m_macrosFormat;
    QTextCharFormat m_parserFormat;
    QTextCharFormat m_textformatFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_imgMapFormat;
    QTextCharFormat m_listFormat;
    QTextCharFormat m_miscFormat;

    bool bSystemForeground;
    bool bSystemBackground;
    QColor m_colorForeground;
    QColor m_colorBackground;

    const QString m_sSEPARATOR;
};

#endif  // INYOKAEDIT_CHIGHLIGHTER_H_
