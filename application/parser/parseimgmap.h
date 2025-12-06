// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSEIMGMAP_H_
#define APPLICATION_PARSER_PARSEIMGMAP_H_

#include <QHash>

class QString;
class QTextDocument;

class ParseImgMap {
 public:
  ParseImgMap();
  static void startParsing(QTextDocument *pRawDoc,
                           const QHash<QString, QString> &map,
                           const QString &sSharePath,
                           const QString &sCommunity);
};

#endif  // APPLICATION_PARSER_PARSEIMGMAP_H_
