// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

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
