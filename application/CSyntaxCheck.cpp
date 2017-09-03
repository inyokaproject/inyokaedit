/**
 * \file CSyntaxCheck.cpp
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
 * Inyoka syntax checks.
 */

#include <QMessageBox>

#include "./CSyntaxCheck.h"

CSyntaxCheck::CSyntaxCheck() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSyntaxCheck::checkInyokaSyntax(const QTextDocument *pRawDoc,
                                     const QStringList sListTplMacros,
                                     const QString sTransTpl) {
  CSyntaxCheck::checkParenthesis(pRawDoc);
  CSyntaxCheck::checkKnownTemplates(pRawDoc, sListTplMacros, sTransTpl);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSyntaxCheck::checkParenthesis(const QTextDocument *pRawDoc) {
  QList<QChar> listParenthesis;
  QString sDoc(pRawDoc->toPlainText());

  listParenthesis.clear();
  foreach (QChar c, sDoc) {
    if ('(' == c || '{' == c || '[' == c) {
      listParenthesis.push_back(c);
    } else if (')' == c || '}' == c || ']' == c) {
      if (listParenthesis.isEmpty() ||
          !CSyntaxCheck::checkParenthesisPair(listParenthesis.last(), c)) {
        QMessageBox::warning(NULL, trUtf8("Inyoka syntax check"),
                             trUtf8("Syntax error detected - "
                                    "closing parenthesis without "
                                    "opening paraenthesis!"));
        return;
      } else {
        listParenthesis.pop_back();
      }
    }
  }

  if (!listParenthesis.isEmpty()) {
    QMessageBox::warning(NULL, trUtf8("Inyoka syntax check"),
                         trUtf8("Syntax error detected - open parenthesis!"));
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CSyntaxCheck::checkParenthesisPair(const QChar cLeft,
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

void CSyntaxCheck::checkKnownTemplates(const QTextDocument *pRawDoc,
                                       const QStringList sListTplMacros,
                                       const QString sTransTpl) {
  QStringList sListTplRegExp;
  sListTplRegExp << "\\{\\{\\{#!" + sTransTpl + " .+\\}\\}\\}"
                 << "\\[\\[" + sTransTpl + "\\s*\\(.+\\)\\]\\]";
  QString sDoc(pRawDoc->toPlainText());
  QString sMacro("");
  int nPos;

  for (int i = 0; i < sListTplRegExp.size(); i++) {
    QRegExp findTemplate(sListTplRegExp[i], Qt::CaseInsensitive);
    findTemplate.setMinimal(true);
    nPos = 0;

    while ((nPos = findTemplate.indexIn(sDoc, nPos)) != -1) {
      sMacro = findTemplate.cap(0);
      if (sMacro.startsWith("[[" + sTransTpl,
                            Qt::CaseInsensitive)) {
        sMacro.remove("[[" + sTransTpl, Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
        sMacro.remove(0, 1);  // Remove (
        sMacro = sMacro.left(sMacro.indexOf(",")).trimmed();

        if (sListTplMacros.contains(sMacro)) {
          sMacro.clear();
        }
      } else if (sMacro.startsWith("{{{#!" + sTransTpl + " ",
                                   Qt::CaseInsensitive)) {
        sMacro.remove("{{{#!" + sTransTpl, Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
        sMacro = sMacro.left(sMacro.indexOf("\n")).trimmed();

        if (sListTplMacros.contains(sMacro)) {
          sMacro.clear();
        }
      }

      if (!sMacro.isEmpty()) {
        QMessageBox::warning(NULL, trUtf8("Inyoka syntax check"),
                             trUtf8("Found unknown template: %1")
                             .arg(sMacro));
      }

      // Proceed with next position
      nPos += findTemplate.cap(0).length();
    }
  }
}
