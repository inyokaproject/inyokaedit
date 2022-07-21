/**
 * \file parsetextformats.cpp
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
 * Parse all text formats.
 */

#include "./parsetextformats.h"

#include <QRegularExpression>
#include <QTextDocument>

ParseTextformats::ParseTextformats() = default;

void ParseTextformats::startParsing(
    QTextDocument *pRawDoc,
    const QPair<QStringList, QStringList> &FormatStartMap,
    const QPair<QStringList, QStringList> &FormatEndMap) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression patternTextformat;
  QString sTmpRegExp;
  int nIndex;
  int nLength;

  patternTextformat.setPatternOptions(
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
      QRegularExpression::DotMatchesEverythingOption |
      QRegularExpression::CaseInsensitiveOption);

  for (int i = 0; i < FormatStartMap.first.size(); i++) {
    bool bFoundStart = true;

    // Start and end is not identical
    if (FormatStartMap.first.at(i) != FormatEndMap.first.at(i)) {
      if (!FormatStartMap.first.at(i).startsWith(QLatin1String("RegExp="))) {
        sDoc.replace(FormatStartMap.first.at(i), FormatStartMap.second.at(i));
      } else {
        sTmpRegExp = FormatStartMap.first.at(i);
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
            sDoc.replace(nIndex, nLength, FormatStartMap.second.at(i));
            nIndex += FormatStartMap.second.at(i).length();
          } else {
            sDoc.replace(nIndex, nLength,
                         FormatStartMap.second.at(i).arg(sCap));
            nIndex += FormatStartMap.second.at(i).arg(sCap).length();
          }
        }
      }
      if (!FormatEndMap.first.at(i).startsWith(QLatin1String("RegExp="))) {
        sDoc.replace(FormatEndMap.first.at(i), FormatEndMap.second.at(i));
      } else {
        sTmpRegExp = FormatEndMap.first.at(i);
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
            sDoc.replace(nIndex, nLength, FormatEndMap.second.at(i));
            nIndex += FormatEndMap.second.at(i).length();
          } else {
            sDoc.replace(nIndex, nLength, FormatEndMap.second.at(i).arg(sCap));
            nIndex += FormatEndMap.second.at(i).arg(sCap).length();
          }
        }
      }
    } else {  // Start and end is identical
      if (!FormatStartMap.first.at(i).startsWith(QLatin1String("RegExp="))) {
        while (-1 != sDoc.indexOf(FormatStartMap.first.at(i))) {
          if (bFoundStart) {
            sDoc.replace(sDoc.indexOf(FormatStartMap.first.at(i)),
                         FormatStartMap.first.at(i).length(),
                         FormatStartMap.second.at(i));
          } else {
            sDoc.replace(sDoc.indexOf(FormatStartMap.first.at(i)),
                         FormatStartMap.first.at(i).length(),
                         FormatEndMap.second.at(i));
          }
          bFoundStart = !bFoundStart;
        }
      } else {
        sTmpRegExp = FormatStartMap.first.at(i);
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
              sDoc.replace(nIndex, nLength, FormatStartMap.second.at(i));
              nIndex += FormatStartMap.second.at(i).length();
            } else {
              sDoc.replace(nIndex, nLength, FormatEndMap.second.at(i));
              nIndex += FormatEndMap.second.at(i).length();
            }
          } else {
            if (bFoundStart) {
              sDoc.replace(nIndex, nLength,
                           FormatStartMap.second.at(i).arg(sCap));
              nIndex += FormatStartMap.second.at(i).arg(sCap).length();
            } else {
              sDoc.replace(nIndex, nLength,
                           FormatEndMap.second.at(i).arg(sCap));
              nIndex += FormatEndMap.second.at(i).arg(sCap).length();
            }
          }
        }
      }
    }
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}
