/**
 * \file syntaxcheck.cpp
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
                                    const QStringList &sListTplTrans) -> QPair<int, QString> {
  QPair<int, QString> ret(-1, QLatin1String(""));
  ret = SyntaxCheck::checkParenthesis(pRawDoc, sListSmilies);
  if (-1 == ret.first) {
    ret = SyntaxCheck::checkKnownTemplates(pRawDoc, sListTplMacros,
                                           sListTplTrans);
  }

  return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SyntaxCheck::checkParenthesis(const QTextDocument *pRawDoc,
                                   const QStringList &sListSmilies) -> QPair<int, QString> {
  QList<QChar> listParenthesis;
  QList<qint32> listPos;
  QString sDoc(pRawDoc->toPlainText());
  QString sReplace(QLatin1String(""));

  // Replace smilies, since most of them are including open parenthesis
  for (const auto &s : sListSmilies) {
    sDoc = sDoc.replace(s, sReplace.fill('X', s.length()));
  }
  // Replace left/right text alignment in tables
  sDoc = sDoc.replace(QLatin1String("<(>"), sReplace.fill('X', 3));
  sDoc = sDoc.replace(QLatin1String("<)>"), sReplace.fill('X', 3));

  SyntaxCheck::filterMonotype(sDoc);

  listParenthesis.clear();
  QPair<int, QString> ret(-1, QLatin1String(""));
  qint32 nCnt(-1);
  for (const auto c : qAsConst(sDoc)) {
    nCnt++;
    if ('(' == c || '{' == c || '[' == c) {
      listParenthesis.push_back(c);
      listPos.push_back(nCnt);
    } else if (')' == c || '}' == c || ']' == c) {
      if (listParenthesis.isEmpty() ||
          !SyntaxCheck::checkParenthesisPair(listParenthesis.last(), c)) {
        ret.first = nCnt;
        ret.second = QStringLiteral("OPEN_PAR_MISSING");
        return ret;
      }
      listParenthesis.pop_back();
      listPos.pop_back();
    }
  }

  if (!listParenthesis.isEmpty()) {
    ret.first = listPos.last();
    ret.second = QStringLiteral("CLOSE_PAR_MISSING");
    return ret;
  }
  return ret;
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
                                      const QStringList &sListTplTrans) -> QPair<int, QString> {
  QStringList sListTplRegExp;
  QStringList sListTrans;
  for (const auto &s : sListTplTrans) {
    sListTplRegExp << "\\{\\{\\{#!" + s + " .+\\}\\}\\}"
                   << "\\[\\[" + s + "\\s*\\(.+\\)\\]\\]";
    sListTrans << s << s;
  }
  QString sDoc(pRawDoc->toPlainText());
  SyntaxCheck::filterMonotype(sDoc);
  QPair<int, QString> ret(-1, QLatin1String(""));

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
        ret.first = match.capturedStart();
        ret.second = QStringLiteral("UNKNOWN_TPL|") + sMacro;
      }
    }
  }

  return ret;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SyntaxCheck::filterMonotype(QString &sDoc) {
  QString sReplace(QLatin1String(""));
  QStringList sListMonotype;
  sListMonotype << QStringLiteral("``");
  sListMonotype << QStringLiteral("`");
  for (int i = 0; i < sListMonotype.size(); i++) {
    int nStart = -1;
    int nEnd = -1;
    int nIndex = 0;
    while (-1 != sDoc.indexOf(sListMonotype[i], nIndex)) {
      if (-1 == nStart) {
        nStart = sDoc.indexOf(sListMonotype[i], nIndex);
        nIndex = nStart + sListMonotype[i].length();
      } else {
        nEnd = sDoc.indexOf(sListMonotype[i], nIndex);
        sDoc.replace(nStart, nEnd - nStart + sListMonotype[i].length(),
                     sReplace.fill('X',
                                   nEnd - nStart + sListMonotype[i].length()));
        nIndex = nEnd + sListMonotype[i].length();
        nStart = -1;
      }
    }
  }
}
