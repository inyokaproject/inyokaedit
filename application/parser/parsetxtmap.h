// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSETXTMAP_H_
#define APPLICATION_PARSER_PARSETXTMAP_H_

#include <QPair>
#include <QStringList>

class QTextDocument;

class ParseTxtMap {
 public:
  ParseTxtMap();
  static void startParsing(QTextDocument *pRawDoc,
                           const QPair<QStringList, QStringList> &map);
};

#endif  // APPLICATION_PARSER_PARSETXTMAP_H_
