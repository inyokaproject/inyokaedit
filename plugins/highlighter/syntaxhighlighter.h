/**
 * \file syntaxhighlighter.h
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Class definition for syntax highlighting.
 */

#ifndef PLUGINS_HIGHLIGHTER_SYNTAXHIGHLIGHTER_H_
#define PLUGINS_HIGHLIGHTER_SYNTAXHIGHLIGHTER_H_

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

struct HighlightingRule {
  QRegularExpression regexp;
  QTextCharFormat format;
};
class QTextDocument;

/**
 * \class SyntaxHighlighter
 * \brief Syntax highlighting
 */
class SyntaxHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

 public:
    explicit SyntaxHighlighter(QTextDocument *pDoc = nullptr);
    ~SyntaxHighlighter();
    void setRules(const QVector<HighlightingRule> &rules);

 protected:
    // Apply highlighting rules
    void highlightBlock(const QString &sText);

 private:
    QVector<HighlightingRule> m_highlightingRules;
};

#endif  // PLUGINS_HIGHLIGHTER_SYNTAXHIGHLIGHTER_H_
