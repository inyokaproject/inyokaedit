/**
 * \file parsetextformats.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2021 The InyokaEdit developers
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
 * Parse all text formats.
 */

#include "./parsetextformats.h"

#include <QRegularExpression>
#include <QTextDocument>

ParseTextformats::ParseTextformats() = default;

void ParseTextformats::startParsing(QTextDocument *pRawDoc,
                                    const QStringList &sListFormatStart,
                                    const QStringList &sListFormatEnd,
                                    const QStringList &sListHtmlStart,
                                    const QStringList &sListHtmlEnd) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression patternTextformat;
  QString sTmpRegExp;
  int nIndex;
  int nLength;

  patternTextformat.setPatternOptions(
        QRegularExpression::InvertedGreedinessOption |  // Only smallest match
        QRegularExpression::DotMatchesEverythingOption |
        QRegularExpression::CaseInsensitiveOption);

  for (int i = 0; i < sListFormatStart.size(); i++) {
    bool bFoundStart = true;

    // Start and end is not identical
    if (sListFormatStart[i] != sListFormatEnd[i]) {
      if (!sListFormatStart[i].startsWith(QLatin1String("RegExp="))) {
        sDoc.replace(sListFormatStart[i], sListHtmlStart[i]);
      } else {
        sTmpRegExp = sListFormatStart[i];
        sTmpRegExp.remove(QStringLiteral("RegExp="));
        sTmpRegExp = sTmpRegExp.trimmed();
        patternTextformat.setPattern(sTmpRegExp);

        nIndex = 0;
        QRegularExpressionMatch match;
        while ((match = patternTextformat.match(sDoc, nIndex)).hasMatch()) {
          QString sCap(match.captured(1));
          nIndex = match.capturedStart();
          nLength = match.capturedLength();

          if (sCap.isEmpty()) {
            sDoc.replace(nIndex, nLength, sListHtmlStart[i]);
            nIndex += sListHtmlStart[i].length();
          } else {
            sDoc.replace(nIndex, nLength,
                         sListHtmlStart[i].arg(sCap));
            nIndex += sListHtmlStart[i].arg(sCap).length();
          }
        }
      }
      if (!sListFormatEnd[i].startsWith(QLatin1String("RegExp="))) {
        sDoc.replace(sListFormatEnd[i], sListHtmlEnd[i]);
      } else {
        sTmpRegExp = sListFormatEnd[i];
        sTmpRegExp.remove(QStringLiteral("RegExp="));
        sTmpRegExp = sTmpRegExp.trimmed();
        patternTextformat.setPattern(sTmpRegExp);

        nIndex = 0;
        QRegularExpressionMatch match;
        while ((match = patternTextformat.match(sDoc, nIndex)).hasMatch()) {
          QString sCap(match.captured(1));
          nIndex = match.capturedStart();
          nLength = match.capturedLength();

          if (sCap.isEmpty()) {
            sDoc.replace(nIndex, nLength, sListHtmlEnd[i]);
            nIndex += sListHtmlEnd[i].length();
          } else {
            sDoc.replace(nIndex, nLength,
                         sListHtmlEnd[i].arg(sCap));
            nIndex += sListHtmlEnd[i].arg(sCap).length();
          }
        }
      }
    } else {  // Start and end is identical
      if (!sListFormatStart[i].startsWith(QLatin1String("RegExp="))) {
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
        sTmpRegExp.remove(QStringLiteral("RegExp="));
        sTmpRegExp = sTmpRegExp.trimmed();
        patternTextformat.setPattern(sTmpRegExp);

        nIndex = 0;
        QRegularExpressionMatch match;
        while ((match = patternTextformat.match(sDoc, nIndex)).hasMatch()) {
          QString sCap(match.captured(1));
          nIndex = match.capturedStart();
          nLength = match.capturedLength();

          if (sCap.isEmpty()) {
            if (bFoundStart) {
              sDoc.replace(nIndex, nLength, sListHtmlStart[i]);
              nIndex += sListHtmlStart[i].length();
            } else {
              sDoc.replace(nIndex, nLength, sListHtmlEnd[i]);
              nIndex += sListHtmlEnd[i].length();
            }
          } else {
            if (bFoundStart) {
              sDoc.replace(nIndex, nLength,
                           sListHtmlStart[i].arg(sCap));
              nIndex += sListHtmlStart[i].arg(sCap).length();
            } else {
              sDoc.replace(nIndex, nLength,
                           sListHtmlEnd[i].arg(sCap));
              nIndex += sListHtmlEnd[i].arg(sCap).length();
            }
          }
        }
      }
    }
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}
