/**
 * \file CSyntaxHighlighter.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2015 The InyokaEdit developers
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

#ifndef INYOKAEDIT_CSYNTAXHIGHLIGHTER_H_
#define INYOKAEDIT_CSYNTAXHIGHLIGHTER_H_

#include <QHash>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

struct HighlightingRule {
    QRegExp pattern;
    QTextCharFormat format;
};

class QTextDocument;

/**
 * \class CSyntaxHighlighter
 * \brief Syntax highlighting
 */
class CSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

  public:
    CSyntaxHighlighter(QTextDocument *pDoc = 0);
    ~CSyntaxHighlighter();
    void setRules(QVector<HighlightingRule> rules);

  protected:
    // Apply highlighting rules
    void highlightBlock(const QString &sText);

  private:
    QVector<HighlightingRule> m_highlightingRules;
};

#endif  // INYOKAEDIT_CSYNTAXHIGHLIGHTER_H_
