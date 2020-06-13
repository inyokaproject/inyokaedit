/**
 * \file parsetxtmap.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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

ParseTxtMap::ParseTxtMap() = default;

void ParseTxtMap::startParsing(QTextDocument *pRawDoc,
                               QStringList sListElements,
                               QStringList sListText) {
  QString sDoc(pRawDoc->toPlainText());
  QString sReplace("");

  for (int i = 0; i < sListElements.size(); i++) {
    if (0 == i && "error" == sListElements[0].toLower()) {
      qCritical() << "Error while parsing text map.";
      break;
    }
    sReplace = sListText[i];
    if (sReplace.startsWith("css-class:")) {
      sReplace = sReplace.remove("css-class:");
      sReplace = "<span class=\"" + sReplace + "\"></span>";
    }
    sDoc.replace(sListElements[i], sReplace);
  }

  // Replace raw document with new replaced doc
  pRawDoc->setPlainText(sDoc);
}
