// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./parseimgmap.h"

#include <QDebug>
#include <QString>
#include <QTextDocument>

ParseImgMap::ParseImgMap() = default;

void ParseImgMap::startParsing(QTextDocument *pRawDoc,
                               const QHash<QString, QString> &map,
                               const QString &sSharePath,
                               const QString &sCommunity) {
  QString sDoc(pRawDoc->toPlainText());

  QHashIterator<QString, QString> i(map);
  if (!map.isEmpty()) {
    while (i.hasNext()) {
      i.next();
      sDoc.replace(i.key(), "<img src=\"" + sSharePath + "/community/" +
                                sCommunity + "/" + i.value() + "\">");
    }
  } else {
    qCritical() << "Error while parsing image map - map is empty!";
  }

  // Replace raw document with new replaced doc
  pRawDoc->setPlainText(sDoc);
}
