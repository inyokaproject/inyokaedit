/**
 * \file syntaxcheck.cpp
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
 * Inyoka syntax checks.
 */

#include <QMessageBox>

#include "./syntaxcheck.h"

SyntaxCheck::SyntaxCheck() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

qint32 SyntaxCheck::checkInyokaSyntax(const QTextDocument *pRawDoc,
                                      const QStringList &sListTplMacros,
                                      const QStringList &sListSmilies,
                                      const QStringList &sListTplTrans) {
  qint32 nRet(-1);

  nRet = SyntaxCheck::checkParenthesis(pRawDoc, sListSmilies);
  if (-1 == nRet) {
    nRet = SyntaxCheck::checkKnownTemplates(pRawDoc, sListTplMacros,
                                            sListTplTrans);
  }

  return nRet;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

qint32 SyntaxCheck::checkParenthesis(const QTextDocument *pRawDoc,
                                     const QStringList &sListSmilies) {
  QList<QChar> listParenthesis;
  QList<qint32> listPos;
  QString sDoc(pRawDoc->toPlainText());
  QString sReplace("");

  // Replace smilies, since most of them are including open parenthesis
  foreach (QString s, sListSmilies) {
    sDoc = sDoc.replace(s, sReplace.fill('X', s.length()));
  }

  listParenthesis.clear();
  qint32 nCnt(0);
  foreach (QChar c, sDoc) {
    if ('(' == c || '{' == c || '[' == c) {
      listParenthesis.push_back(c);
      listPos.push_back(nCnt);
    } else if (')' == c || '}' == c || ']' == c) {
      if (listParenthesis.isEmpty() ||
          !SyntaxCheck::checkParenthesisPair(listParenthesis.last(), c)) {
        QMessageBox::warning(NULL, trUtf8("Inyoka syntax check"),
                             trUtf8("Syntax error detected - "
                                    "closing parenthesis without "
                                    "opening paraenthesis!"));
        return nCnt;
      } else {
        listParenthesis.pop_back();
        listPos.pop_back();
      }
    }
    nCnt++;
  }

  if (!listParenthesis.isEmpty()) {
    QMessageBox::warning(NULL, trUtf8("Inyoka syntax check"),
                         trUtf8("Syntax error detected - open parenthesis!"));
    return listPos.last();
  }
  return -1;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SyntaxCheck::checkParenthesisPair(const QChar cLeft,
                                       const QChar cRight) {
  if ('[' == cLeft && ']' == cRight) {
    return true;
  } else if ('(' == cLeft && ')' == cRight) {
    return true;
  } else if ('{' == cLeft && '}' == cRight) {
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

qint32 SyntaxCheck::checkKnownTemplates(const QTextDocument *pRawDoc,
                                        const QStringList &sListTplMacros,
                                        const QStringList &sListTplTrans) {
  QStringList sListTplRegExp;
  QStringList sListTrans;
  foreach (QString s, sListTplTrans) {
    sListTplRegExp << "\\{\\{\\{#!" + s + " .+\\}\\}\\}"
                   << "\\[\\[" + s + "\\s*\\(.+\\)\\]\\]";
    sListTrans << s << s;
  }
  QString sDoc(pRawDoc->toPlainText());

  for (int i = 0; i < sListTplRegExp.size(); i++) {
    QRegExp findTemplate(sListTplRegExp[i], Qt::CaseInsensitive);
    findTemplate.setMinimal(true);
    int nPos = 0;

    while ((nPos = findTemplate.indexIn(sDoc, nPos)) != -1) {
      QString sMacro = findTemplate.cap(0);
      if (sMacro.startsWith("[[" + sListTrans[i],
                            Qt::CaseInsensitive)) {
        sMacro.remove("[[" + sListTrans[i], Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
        sMacro.remove(0, 1);  // Remove (
        sMacro = sMacro.left(sMacro.indexOf(",")).trimmed();

        if (sListTplMacros.contains(sMacro)) {
          sMacro.clear();
        }
      } else if (sMacro.startsWith("{{{#!" + sListTrans[i] + " ",
                                   Qt::CaseInsensitive)) {
        sMacro.remove("{{{#!" + sListTrans[i] + " ", Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
        if (-1 != sMacro.indexOf(" ") && -1 != sMacro.indexOf("\n")) {
            if (sMacro.indexOf(" ") < sMacro.indexOf("\n")) {
              sMacro = sMacro.left(sMacro.indexOf(" ")).trimmed();
            } else {
              sMacro = sMacro.left(sMacro.indexOf("\n")).trimmed();
            }
        } else {
          sMacro = sMacro.left(sMacro.indexOf("\n")).trimmed();
        }

        if (sListTplMacros.contains(sMacro)) {
          sMacro.clear();
        }
      }

      if (!sMacro.isEmpty()) {
        QMessageBox::warning(NULL, trUtf8("Inyoka syntax check"),
                             trUtf8("Found unknown template: %1")
                             .arg(sMacro));
        return nPos;
      }

      // Proceed with next position
      nPos += findTemplate.cap(0).length();
    }
  }

  return -1;
}
