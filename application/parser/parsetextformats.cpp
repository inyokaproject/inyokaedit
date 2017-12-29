/**
 * \file parsetextformats.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Parse all text formats.
 */

#include <QRegExp>
#include <QStringList>

#include "./parsetextformats.h"

ParseTextformats::ParseTextformats() {
}

void ParseTextformats::startParsing(QTextDocument *pRawDoc,
                                    const QStringList &sListFormatStart,
                                    const QStringList &sListFormatEnd,
                                    const QStringList &sListHtmlStart,
                                    const QStringList &sListHtmlEnd) {
  QString sDoc(pRawDoc->toPlainText());
  QRegExp patternTextformat;
  QString sFormatedText;
  QString sTmpRegExp;
  bool bFoundStart;
  int nIndex;
  int nLength;

  patternTextformat.setCaseSensitivity(Qt::CaseInsensitive);
  patternTextformat.setMinimal(true);  // Search only for smallest match

  for (int i = 0; i < sListFormatStart.size(); i++) {
    bFoundStart = true;

    // Start and end is not identical
    if (sListFormatStart[i] != sListFormatEnd[i]) {
      if (!sListFormatStart[i].startsWith("RegExp=")) {
        sDoc.replace(sListFormatStart[i], sListHtmlStart[i]);
      } else {
        sTmpRegExp = sListFormatStart[i];
        sTmpRegExp.remove("RegExp=");
        sTmpRegExp = sTmpRegExp.trimmed();
        patternTextformat.setPattern(sTmpRegExp);

        nIndex = patternTextformat.indexIn(sDoc);

        while (nIndex >= 0) {
          QString sCap("");
          nLength = patternTextformat.matchedLength();
          sFormatedText = patternTextformat.cap();
          sCap = patternTextformat.cap(1);

          if (sCap.isEmpty()) {
            sDoc.replace(nIndex, nLength, sListHtmlStart[i]);
          } else {
            sDoc.replace(nIndex, nLength,
                         sListHtmlStart[i].arg(sCap));
          }

          // Go on with RegExp-Search
          nIndex = patternTextformat.indexIn(sDoc, nIndex + nLength);
        }
      }
      if (!sListFormatEnd[i].startsWith("RegExp=")) {
        sDoc.replace(sListFormatEnd[i], sListHtmlEnd[i]);
      } else {
        sTmpRegExp = sListFormatEnd[i];
        sTmpRegExp.remove("RegExp=");
        sTmpRegExp = sTmpRegExp.trimmed();
        patternTextformat.setPattern(sTmpRegExp);

        nIndex = patternTextformat.indexIn(sDoc);

        while (nIndex >= 0) {
          QString sCap("");
          nLength = patternTextformat.matchedLength();
          sFormatedText = patternTextformat.cap();
          sCap = patternTextformat.cap(1);

          if (sCap.isEmpty()) {
            sDoc.replace(nIndex, nLength, sListHtmlEnd[i]);
          } else {
            sDoc.replace(nIndex, nLength,
                         sListHtmlEnd[i].arg(sCap));
          }

          // Go on with RegExp-Search
          nIndex = patternTextformat.indexIn(sDoc, nIndex + nLength);
        }
      }
    } else {  // Start and end is identical
      if (!sListFormatStart[i].startsWith("RegExp=")) {
        while (-1 != sDoc.indexOf(sListFormatStart[i])) {
          if (bFoundStart) {
            sDoc.replace(sDoc.indexOf(sListFormatStart[i]),
                         sListFormatStart[i].length(),
                         sListHtmlStart[i]);
          } else {
            sDoc.replace(sDoc.indexOf(sListFormatStart[i]),
                         sListFormatStart[i].length(),
                         sListHtmlEnd[i]);
          }
          bFoundStart = !bFoundStart;
        }
      } else {
        sTmpRegExp = sListFormatStart[i];
        sTmpRegExp.remove("RegExp=");
        sTmpRegExp = sTmpRegExp.trimmed();
        patternTextformat.setPattern(sTmpRegExp);

        nIndex = patternTextformat.indexIn(sDoc);

        while (nIndex >= 0) {
          QString sCap("");
          nLength = patternTextformat.matchedLength();
          sFormatedText = patternTextformat.cap();
          sCap = patternTextformat.cap(1);

          if (sCap.isEmpty()) {
            if (bFoundStart) {
              sDoc.replace(nIndex, nLength, sListHtmlStart[i]);
            } else {
              sDoc.replace(nIndex, nLength, sListHtmlEnd[i]);
            }
          } else {
            if (bFoundStart) {
              sDoc.replace(nIndex, nLength,
                           sListHtmlStart[i].arg(sCap));
            } else {
              sDoc.replace(nIndex, nLength,
                           sListHtmlEnd[i].arg(sCap));
            }
          }

          // Go on with RegExp-Search
          nIndex = patternTextformat.indexIn(sDoc, nIndex + nLength);
        }
      }
    }
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}
