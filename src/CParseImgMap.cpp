/**
 * \file CParseImgMap.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Parse all image mapped elents (simple replacement).
 */

#include <QDebug>
#include <QRegExp>
#include <QStringList>

#include "./CParseImgMap.h"

CParseImgMap::CParseImgMap() {
    qDebug() << "Calling" << Q_FUNC_INFO;
}

void CParseImgMap::startParsing(QTextDocument *pRawDoc,
                                QStringList sListElements,
                                QStringList sListImages) {
    QString sMyDoc = pRawDoc->toPlainText();

    for (int i = 0; i < sListElements.size(); i++) {
        if (0 == i && "error" == sListElements[0].toLower()) {
            break;
        }
        sMyDoc.replace(sListElements[i], "<img src=\"" + sListImages[i] + "\" />");
    }

    // Replace raw document with new replaced doc
    pRawDoc->setPlainText(sMyDoc);
}
