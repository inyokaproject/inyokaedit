/**
 * \file syntaxcheck.h
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
 * Class definition for Inyoka syntax checks.
 */

#ifndef APPLICATION_SYNTAXCHECK_H_
#define APPLICATION_SYNTAXCHECK_H_

#include <QObject>

class QTextDocument;

class SyntaxCheck : public QObject {
  Q_OBJECT

 public:
    explicit SyntaxCheck(QObject *pParent = nullptr);

    static auto checkInyokaSyntax(
        const QTextDocument *pRawDoc,
        const QStringList &sListTplMacros,
        const QStringList &sListSmilies,
        const QStringList &sListTplTrans) -> QPair <int, QString>;

 private:
    static auto checkParenthesis(
        const QTextDocument *pRawDoc,
        const QStringList &sListSmilies) -> QPair <int, QString>;
    static auto checkParenthesisPair(const QChar cLeft,
                                     const QChar cRight) -> bool;
    static auto checkKnownTemplates(
        const QTextDocument *pRawDoc,
        const QStringList &sListTplMacros,
        const QStringList &sListTplTrans) -> QPair <int, QString>;

    static void filterMonotype(QString *sDoc);
};

#endif  // APPLICATION_SYNTAXCHECK_H_
