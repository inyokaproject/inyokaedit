// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_SYNTAXCHECK_H_
#define APPLICATION_SYNTAXCHECK_H_

#include <QObject>

class QTextDocument;

class SyntaxCheck : public QObject {
  Q_OBJECT

 public:
  explicit SyntaxCheck(QObject *pParent = nullptr);

  static auto checkInyokaSyntax(const QTextDocument *pRawDoc,
                                const QStringList &sListTplMacros,
                                const QStringList &sListSmilies,
                                const QStringList &sListTplTrans)
      -> QPair<int, QString>;

 private:
  static auto checkParenthesis(const QTextDocument *pRawDoc,
                               const QStringList &sListSmilies)
      -> QPair<int, QString>;
  static auto checkParenthesisPair(const QChar cLeft, const QChar cRight)
      -> bool;
  static auto checkKnownTemplates(const QTextDocument *pRawDoc,
                                  const QStringList &sListTplMacros,
                                  const QStringList &sListTplTrans)
      -> QPair<int, QString>;

  static void filterMonotype(QString *sDoc);
};

#endif  // APPLICATION_SYNTAXCHECK_H_
