/**
 * \file CSyntaxCheck.h
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

#ifndef INYOKAEDIT_CSYNTAXCHECK_H_
#define INYOKAEDIT_CSYNTAXCHECK_H_

#include <QObject>
#include <QTextDocument>

class CSyntaxCheck : public QObject {
  Q_OBJECT

 public:
  explicit CSyntaxCheck();

  static void checkInyokaSyntax(const QTextDocument *pRawDoc,
                                const QStringList sListTplMacros,
                                const QString sTransTpl);

 private:
  static void checkParenthesis(const QTextDocument *pRawDoc);
  static bool checkParenthesisPair(const QChar cLeft, const QChar cRight);
  static void checkKnownTemplates(const QTextDocument *pRawDoc,
                                  const QStringList sListTplMacros,
                                  const QString sTransTpl);

  const QStringList m_sListTplMacros;
  const QString sTransTpl;
};

#endif  // INYOKAEDIT_CSYNTAXCHECK_H_
