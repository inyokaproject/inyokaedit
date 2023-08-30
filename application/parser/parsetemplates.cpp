/**
 * \file parsetemplates.cpp
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
 * Parsing templates.
 */

#include "./parsetemplates.h"

#include <QDebug>
#include <QRegularExpression>
#include <QTextDocument>

#include "./provisionaltplparser.h"

ParseTemplates::ParseTemplates(
    const QStringList &sListTransTpl, const QStringList &sListTplNames,
    const QStringList &sListHtmlStart, const QString &sSharePath,
    const QDir &tmpImgDir, const QHash<QString, QString> &TestedWithMap,
    const QHash<QString, QString> &TestedWithTouchMap,
    const QString &sCommunity)
    : m_sListTransTpl(sListTransTpl), m_sListTplNames(sListTplNames) {
  m_pProvTplTarser =
      new ProvisionalTplParser(sListHtmlStart, sSharePath, tmpImgDir,
                               TestedWithMap, TestedWithTouchMap, sCommunity);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ParseTemplates::startParsing(QTextDocument *pRawDoc,
                                  const QString &sCurrentFile) {
  m_sCurrentFile = sCurrentFile;

  QStringList sListTplRegExp;
  QStringList sListTrans;
  for (const auto &s : qAsConst(m_sListTransTpl)) {
    sListTplRegExp << "\\{\\{\\{#!" + s + " .+\\}\\}\\}"
                   << "\\[\\[" + s + "\\s*\\(.+\\)\\]\\]";
    sListTrans << s << s;
  }
  QString sDoc(pRawDoc->toPlainText());
  QStringList sListArguments;
  static QRegularExpression splitQuote(QStringLiteral("\""));
  static QRegularExpression splitComma(QStringLiteral(",+"));
  static QRegularExpression splitNewline(QStringLiteral("\\n"));
  static QRegularExpression splitSpace(QStringLiteral("\\s+"));

  for (int k = 0; k < sListTplRegExp.size(); k++) {
    QRegularExpression findTemplate(
        sListTplRegExp[k],
        QRegularExpression::InvertedGreedinessOption |  // Only smallest match
            QRegularExpression::DotMatchesEverythingOption |
            QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match;
    int nPos = 0;

    while ((match = findTemplate.match(sDoc, nPos)).hasMatch()) {
      QString sMacro = match.captured(0);
      QString sBackupMacro = sMacro;
      if (sMacro.startsWith("[[" + sListTrans[k], Qt::CaseInsensitive)) {
        // Step needed because of possible spaces
        sMacro.remove("[[" + sListTrans[k], Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
      }
      sListArguments.clear();

      // Check if macro exists
      for (int i = 0; i < m_sListTplNames.size(); i++) {
        if (sMacro.startsWith("(" + m_sListTplNames[i], Qt::CaseInsensitive)) {
          sMacro.remove(0, 1);  // Remove (
          sMacro.remove(QStringLiteral("\n)]]"));
          sMacro.remove(QStringLiteral(")]]"));

          // Extract arguments
          // Split by ',' but don't split quoted strings with comma
          const QStringList tmpList = sMacro.split(splitQuote);
          bool bInside = false;
          for (const auto &s : tmpList) {
            if (bInside) {
              // If 's' is inside quotes, get the whole string
              sListArguments.append(s);
            } else {
              // If 's' is outside quotes, get the split string
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
              sListArguments.append(
                  s.split(splitComma, QString::SkipEmptyParts));
#else
              sListArguments.append(s.split(splitComma, Qt::SkipEmptyParts));
#endif
            }
            bInside = !bInside;
          }
          sListArguments.removeAll(QStringLiteral(" "));

          // In addition to ',' arguments can be separated by '\n'...
          for (int m = 0; m < sListArguments.size(); m++) {
            if (sListArguments[m].contains(QLatin1String("\n"))) {
              QString sTmp = sListArguments[m];
              QStringList tmpArgs;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
              tmpArgs << sTmp.split(splitNewline, QString::SkipEmptyParts);
#else
              tmpArgs << sTmp.split(splitNewline, Qt::SkipEmptyParts);
#endif
              for (int j = 0; j < tmpArgs.size(); j++) {
                sListArguments.insert(m + j + 1, tmpArgs[j]);
              }
              sListArguments.removeAt(m);
            }
          }
        } else if (sMacro.startsWith(
                       "{{{#!" + sListTrans[k] + " " + m_sListTplNames[i],
                       Qt::CaseInsensitive)) {
          sMacro.remove("{{{#!" + sListTrans[k] + " ", Qt::CaseInsensitive);
          sMacro.remove(QStringLiteral("\n\\}}}"));
          sMacro.remove(QStringLiteral("\\}}}"));
          sMacro.remove(QStringLiteral("\n}}}"));
          sMacro.remove(QStringLiteral("}}}"));
          sListArguments.clear();

          // Extract arguments
          sListArguments = sMacro.split(splitNewline);

          if (!sListArguments.isEmpty()) {
            // Split by ' ' - don't split quoted strings with space
            QStringList sList;
            const QStringList sL = sListArguments[0].split(splitQuote);
            bool bInside = false;
            for (const auto &s : sL) {
              if (bInside) {
                // If 's' is inside quotes, get the whole string
                sList.append(s);
              } else {
                // If 's' is outside quotes, get split string
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                sList.append(s.split(splitSpace, QString::SkipEmptyParts));
#else
                sList.append(s.split(splitSpace, Qt::SkipEmptyParts));
#endif
              }
              bInside = !bInside;
            }
            if (sList.size() > 1) {
              sListArguments.removeFirst();
              for (int n = sList.size() - 1; n >= 0; n--) {
                if ("," != sList[n]) {
                  sListArguments.push_front(sList[n]);
                }
              }
            }
            if (sListArguments[0].endsWith(QLatin1String(","))) {
              sListArguments[0] =
                  sListArguments[0].remove(sListArguments[0].size() - 1, 1);
            }
          }
        }
      }

      for (int j = 0; j < sListArguments.size(); j++) {
        sListArguments[j] = sListArguments[j].trimmed();
      }

      // qDebug() << "TPL:" << sListArguments;
      sMacro = m_pProvTplTarser->parseTpl(sListArguments, m_sCurrentFile);
      if (sMacro.isEmpty()) {
        sMacro = sBackupMacro;
      }
      sDoc.replace(match.capturedStart(), match.capturedLength(), sMacro);

      // Go on with new start position
      nPos += sMacro.length();
    }
  }

  pRawDoc->setPlainText(sDoc);
}
