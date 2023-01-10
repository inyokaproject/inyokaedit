/**
 * \file parsetxtmap.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * Parse all text mapped elements (simple replacement).
 */

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
