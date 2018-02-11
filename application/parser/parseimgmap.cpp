/**
 * \file parseimgmap.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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
 * along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Parse all image mapped elements (simple replacement).
 */

#include "./parseimgmap.h"

#include <QDebug>
#include <QRegExp>
#include <QStringList>

ParseImgMap::ParseImgMap() {
}

void ParseImgMap::startParsing(QTextDocument *pRawDoc,
                               QStringList sListElements,
                               QStringList sListImages,
                               const QString &sSharePath,
                               const QString &sCommunity) {
  QString sDoc(pRawDoc->toPlainText());

  for (int i = 0; i < sListElements.size(); i++) {
    if (0 == i && "error" == sListElements[0].toLower()) {
      qCritical() << "Error while parsing image map.";
      break;
    }
    sDoc.replace(sListElements[i],
                 "<img src=\"" + sSharePath + "/community/" + sCommunity +
                 "/web/" + sListImages[i] + "\" />");
  }

  // Replace raw document with new replaced doc
  pRawDoc->setPlainText(sDoc);
}
