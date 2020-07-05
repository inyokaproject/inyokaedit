/**
 * \file syntaxcheck.cpp
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
 * Inyoka syntax checks.
 */

#include "./syntaxcheck.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QTextDocument>

SyntaxCheck::SyntaxCheck(QObject *pParent) {
  Q_UNUSED(pParent)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SyntaxCheck::checkInyokaSyntax(const QTextDocument *pRawDoc,
                                    const QStringList &sListTplMacros,
                                    const QStringList &sListSmilies,
                                    const QStringList &sListTplTrans) -> qint32 {
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

auto SyntaxCheck::checkParenthesis(const QTextDocument *pRawDoc,
                                   const QStringList &sListSmilies) -> qint32 {
  QList<QChar> listParenthesis;
  QList<qint32> listPos;
  QString sDoc(pRawDoc->toPlainText());
  QString sReplace(QLatin1String(""));

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
        QMessageBox::warning(nullptr, tr("Inyoka syntax check"),
                             tr("Syntax error detected - closing parenthesis "
                                "without opening paraenthesis!"));
        return nCnt;
      }
      listParenthesis.pop_back();
      listPos.pop_back();
    }
    nCnt++;
  }

  if (!listParenthesis.isEmpty()) {
    QMessageBox::warning(nullptr, tr("Inyoka syntax check"),
                         tr("Syntax error detected - open parenthesis!"));
    return listPos.last();
  }
  return -1;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SyntaxCheck::checkParenthesisPair(const QChar cLeft,
                                       const QChar cRight) -> bool {
  if ('[' == cLeft && ']' == cRight) {
    return true;
  }
  if ('(' == cLeft && ')' == cRight) {
    return true;
  }
  if ('{' == cLeft && '}' == cRight) {
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SyntaxCheck::checkKnownTemplates(const QTextDocument *pRawDoc,
                                      const QStringList &sListTplMacros,
                                      const QStringList &sListTplTrans) -> qint32 {
  QStringList sListTplRegExp;
  QStringList sListTrans;
  foreach (QString s, sListTplTrans) {
    sListTplRegExp << "\\{\\{\\{#!" + s + " .+\\}\\}\\}"
                   << "\\[\\[" + s + "\\s*\\(.+\\)\\]\\]";
    sListTrans << s << s;
  }
  QString sDoc(pRawDoc->toPlainText());

  for (int i = 0; i < sListTplRegExp.size(); i++) {
    QRegularExpression findTemplate(
          sListTplRegExp[i],
          QRegularExpression::InvertedGreedinessOption |
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator it = findTemplate.globalMatch(sDoc);

    while (it.hasNext()) {
      QRegularExpressionMatch match = it.next();
      QString sMacro = match.captured(0);
      if (sMacro.startsWith("[[" + sListTrans[i],
                            Qt::CaseInsensitive)) {
        sMacro.remove("[[" + sListTrans[i], Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
        sMacro.remove(0, 1);  // Remove (
        sMacro = sMacro.left(sMacro.indexOf(QLatin1String(","))).trimmed();
        sMacro.remove(QStringLiteral(")]]"));  // Needed if no "," is used
        sMacro = sMacro.trimmed();

        if (sListTplMacros.contains(sMacro, Qt::CaseInsensitive)) {
          sMacro.clear();
        }
      } else if (sMacro.startsWith("{{{#!" + sListTrans[i] + " ",
                                   Qt::CaseInsensitive)) {
        sMacro.remove("{{{#!" + sListTrans[i] + " ", Qt::CaseInsensitive);
        sMacro = sMacro.trimmed();
        if (-1 != sMacro.indexOf(QLatin1String(" ")) &&
            -1 != sMacro.indexOf(QLatin1String("\n"))) {
          if (sMacro.indexOf(QLatin1String(" ")) < sMacro.indexOf(
                QLatin1String("\n"))) {
            sMacro = sMacro.left(sMacro.indexOf(QLatin1String(" "))).trimmed();
          } else {
            sMacro = sMacro.left(sMacro.indexOf(QLatin1String("\n"))).trimmed();
          }
        } else {
          sMacro = sMacro.left(sMacro.indexOf(QLatin1String("\n"))).trimmed();
        }

        sMacro = sMacro.remove(',');
        sMacro = sMacro.trimmed();

        if (sListTplMacros.contains(sMacro, Qt::CaseInsensitive)) {
          sMacro.clear();
        }
      }

      if (!sMacro.isEmpty()) {
        QMessageBox::warning(nullptr, tr("Inyoka syntax check"),
                             tr("Found unknown template: %1").arg(sMacro));
        return match.capturedStart();
      }
    }
  }

  return -1;
}
