// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *pDoc, QObject *pParent)
    : QSyntaxHighlighter(pDoc) {
  Q_UNUSED(pParent)
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
  for (const auto &rule : std::as_const(m_highlightingRules)) {
    QRegularExpression express(
        rule.regexp.pattern(),
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
