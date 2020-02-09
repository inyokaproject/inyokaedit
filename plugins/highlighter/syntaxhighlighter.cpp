/**
 * \file syntaxhighlighter.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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

#include "./syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *pDoc)
  : QSyntaxHighlighter(pDoc) {
}

SyntaxHighlighter::~SyntaxHighlighter() = default;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SyntaxHighlighter::setRules(const QVector<HighlightingRule> &rules) {
  m_highlightingRules = rules;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Apply collected highlighting rules
void SyntaxHighlighter::highlightBlock(const QString &sText) {
  // Go through each highlighting rule
  // rules for every syntax element had been appended in constructor
  QRegularExpressionMatchIterator i;
  QRegularExpressionMatch match;
  foreach (const HighlightingRule &rule, m_highlightingRules) {
    QRegularExpression express(rule.regexp.pattern(),
                               rule.regexp.patternOptions() |
                               QRegularExpression::InvertedGreedinessOption);
    i = express.globalMatch(sText);
    while (i.hasNext()) {
        match = i.next();
        if (match.hasMatch()) {
             this->setFormat(match.capturedStart(), match.capturedLength(),
                             rule.format);
        }
    }
  }
  setCurrentBlockState(0);
}
