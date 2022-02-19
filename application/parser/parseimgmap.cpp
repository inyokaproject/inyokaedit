/**
 * \file parseimgmap.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
 * Parse all image mapped elements (simple replacement).
 */

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
      sDoc.replace(i.key(),
                   "<img src=\"" + sSharePath + "/community/" + sCommunity +
                   "/" + i.value() + "\" />");
    }
  } else {
    qCritical() << "Error while parsing image map - map is empty!";
  }

  // Replace raw document with new replaced doc
  pRawDoc->setPlainText(sDoc);
}
