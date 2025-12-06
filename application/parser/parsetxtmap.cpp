// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./parsetxtmap.h"

#include <QDebug>
#include <QTextDocument>

ParseTxtMap::ParseTxtMap() = default;

void ParseTxtMap::startParsing(QTextDocument *pRawDoc,
                               const QPair<QStringList, QStringList> &map) {
  QString sDoc(pRawDoc->toPlainText());
  QString sReplace;

  if (map.first.size() != map.second.size() || map.first.isEmpty()) {
    qWarning() << "Error while parsing text map!";
    return;
  }

  for (int i = 0; i < map.first.size(); i++) {
    sReplace = map.second.at(i);
    if (sReplace.startsWith(QLatin1String("css-class:"))) {
      sReplace = sReplace.remove(QStringLiteral("css-class:"));
      sReplace = "<span class=\"" + sReplace + "\"></span>";
    }
    sDoc.replace(map.first.at(i), sReplace);
  }

  // Replace raw document with new replaced doc
  pRawDoc->setPlainText(sDoc);
}
