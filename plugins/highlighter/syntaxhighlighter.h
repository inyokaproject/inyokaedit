// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUGINS_HIGHLIGHTER_SYNTAXHIGHLIGHTER_H_
#define PLUGINS_HIGHLIGHTER_SYNTAXHIGHLIGHTER_H_

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class QTextDocument;

struct HighlightingRule {
  QRegularExpression regexp;
  QTextCharFormat format;
};

class SyntaxHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

 public:
  explicit SyntaxHighlighter(QTextDocument *pDoc = nullptr,
                             QObject *pParent = nullptr);
  ~SyntaxHighlighter();
  void setRules(const QVector<HighlightingRule> &rules);

 protected:
  // Apply highlighting rules
  void highlightBlock(const QString &sText) override;

 private:
  QVector<HighlightingRule> m_highlightingRules;
};

#endif  // PLUGINS_HIGHLIGHTER_SYNTAXHIGHLIGHTER_H_
