// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSETABLE_H_
#define APPLICATION_PARSER_PARSETABLE_H_

#include <QString>

class QTextDocument;

class ParseTable {
 public:
  ParseTable();
  static void startParsing(QTextDocument *pRawDoc);

 private:
  static auto createTable(const QStringList &sListLines) -> QString;
};

#endif  // APPLICATION_PARSER_PARSETABLE_H_
