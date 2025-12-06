// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSELIST_H_
#define APPLICATION_PARSER_PARSELIST_H_

class QTextDocument;

class ParseList {
 public:
  ParseList();
  static void startParsing(QTextDocument *pRawDoc);
};

#endif  // APPLICATION_PARSER_PARSELIST_H_
