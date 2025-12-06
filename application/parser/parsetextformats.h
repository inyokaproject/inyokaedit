// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSETEXTFORMATS_H_
#define APPLICATION_PARSER_PARSETEXTFORMATS_H_

#include <QStringList>

class QTextDocument;

class ParseTextformats {
 public:
  ParseTextformats();
  static void startParsing(
      QTextDocument *pRawDoc,
      const QPair<QStringList, QStringList> &FormatStartMap,
      const QPair<QStringList, QStringList> &FormatEndMap);
};

#endif  // APPLICATION_PARSER_PARSETEXTFORMATS_H_
