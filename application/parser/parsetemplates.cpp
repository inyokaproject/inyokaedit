/**
 * \file parsetemplates.cpp
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
 * Parsing templates.
 */

#include "./parsetemplates.h"

#include <QDebug>

ParseTemplates::ParseTemplates(const QStringList &sListTransTpl,
                               const QStringList &sListTplNames,
                               const QStringList &sListHtmlStart,
                               const QString &sSharePath,
                               const QDir &tmpImgDir,
                               const QStringList &sListTestedWith,
                               const QStringList &sListTestedWithStrings,
                               const QStringList &sListTestedWithTouch,
                               const QStringList &sListTestedWithTouchStrings,
                               const QString &sCommunity)
  : m_sListTransTpl(sListTransTpl),
    m_sListTplNames(sListTplNames) {
  m_pProvTplTarser = new ProvisionalTplParser(sListHtmlStart,
                                              sSharePath,
                                              tmpImgDir,
                                              sListTestedWith,
                                              sListTestedWithStrings,
                                              sListTestedWithTouch,
                                              sListTestedWithTouchStrings,
                                              sCommunity);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ParseTemplates::startParsing(QTextDocument *pRawDoc,
                                  const QString &sCurrentFile) {
  m_sCurrentFile = sCurrentFile;

  QStringList sListTplRegExp;
  QStringList sListTrans;
  foreach (QString s, m_sListTransTpl) {
    sListTplRegExp << "\\{\\{\\{#!" + s + " .+\\}\\}\\}"
                   << "\\[\\[" + s + "\\s*\\(.+\\)\\]\\]";
    sListTrans << s << s;
  }
  QString sDoc(pRawDoc->toPlainText());
  QStringList sListArguments;

  for (int k = 0; k < sListTplRegExp.size(); k++) {
    QRegExp findTemplate(sListTplRegExp[k], Qt::CaseInsensitive);
    findTemplate.setMinimal(true);
    int nPos = 0;

    while ((nPos = findTemplate.indexIn(sDoc, nPos)) != -1) {
      QString sMacro = findTemplate.cap(0);
      QString sBackupMacro = sMacro;
      if (sMacro.startsWith("[[" + sListTrans[k], Qt::CaseInsensitive)) {
        // Step needed because of possible spaces
        sMacro.remove("[[" + sListTrans[k], Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
      }
      sListArguments.clear();

      // Check if macro exists
      for (int i = 0; i < m_sListTplNames.size(); i++) {
        if (sMacro.startsWith("(" + m_sListTplNames[i],
                              Qt::CaseInsensitive)) {
          sMacro.remove(0, 1);  // Remove (
          sMacro.remove("\n)]]");
          sMacro.remove(")]]");

          // Extract arguments
          // Split by ',' but don't split quoted strings with comma
          QStringList tmpList = sMacro.split(QRegExp("\""));
          bool bInside = false;
          foreach (QString s, tmpList) {
            if (bInside) {
              // If 's' is inside quotes, get the whole string
              sListArguments.append(s);
            } else {
              // If 's' is outside quotes, get the splitted string
              sListArguments.append(
                    s.split(QRegExp(",+"),
                            QString::SkipEmptyParts));
            }
            bInside = !bInside;
          }
          sListArguments.removeAll(" ");

          // In addition to ',' arguments can be separated by '\n'...
          for (int m = 0; m < sListArguments.size(); m++) {
            if (sListArguments[m].contains("\n")) {
              QString sTmp = sListArguments[m];
              QStringList tmpArgs;
              tmpArgs << sTmp.split(QRegExp("\\n"),
                                    QString::SkipEmptyParts);
              for (int j = 0; j < tmpArgs.size(); j++) {
                sListArguments.insert(m + j + 1, tmpArgs[j]);
              }
              sListArguments.removeAt(m);
            }
          }
        } else if (sMacro.startsWith("{{{#!" + sListTrans[k] + " "
                                     + m_sListTplNames[i],
                                     Qt::CaseInsensitive)) {
          sMacro.remove("{{{#!" + sListTrans[k] + " ",
                        Qt::CaseInsensitive);
          sMacro.remove("\n\\}}}");
          sMacro.remove("\\}}}");
          sMacro.remove("\n}}}");
          sMacro.remove("}}}");
          sListArguments.clear();

          // Extract arguments
          sListArguments = sMacro.split(QRegExp("\\n"));

          if (sListArguments.size() > 0) {
            // Split by ' ' - don't split quoted strings with space
            QStringList sList;
            QStringList sL = sListArguments[0].split(QRegExp("\""));
            bool bInside = false;
            foreach (QString s, sL) {
              if (bInside) {
                // If 's' is inside quotes, get the whole string
                sList.append(s);
              } else {
                // If 's' is outside quotes, get splitted string
                sList.append(s.split(
                               QRegExp("\\s+"),
                               QString::SkipEmptyParts));
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
            if (sListArguments[0].endsWith(",")) {
              sListArguments[0] = sListArguments[0].remove(
                                    sListArguments[0].size() - 1, 1);
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
      sDoc.replace(nPos, findTemplate.matchedLength(), sMacro);

      // Go on with new start position
      nPos += sMacro.length();
    }
  }

  pRawDoc->setPlainText(sDoc);
}
