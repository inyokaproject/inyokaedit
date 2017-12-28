/**
 * \file syntaxcheck.h
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
 * Class definition for Inyoka syntax checks.
 */

#ifndef INYOKAEDIT_SYNTAXCHECK_H_
#define INYOKAEDIT_SYNTAXCHECK_H_

#include <QObject>
#include <QTextDocument>

class SyntaxCheck : public QObject {
  Q_OBJECT

 public:
  explicit SyntaxCheck();

  static qint32 checkInyokaSyntax(const QTextDocument *pRawDoc,
                                  const QStringList &sListTplMacros,
                                  const QString &sTransTpl,
                                  const QStringList &sListSmilies);

 private:
  static qint32 checkParenthesis(const QTextDocument *pRawDoc,
                                 const QStringList &sListSmilies);
  static bool checkParenthesisPair(const QChar cLeft, const QChar cRight);
  static qint32 checkKnownTemplates(const QTextDocument *pRawDoc,
                                    const QStringList &sListTplMacros,
                                    const QString &sTransTpl);
};

#endif  // INYOKAEDIT_SYNTAXCHECK_H_
