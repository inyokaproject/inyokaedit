// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSETEMPLATES_H_
#define APPLICATION_PARSER_PARSETEMPLATES_H_

#include <QString>
#include <QStringList>

class QDir;
class QTextDocument;

class ProvisionalTplParser;

class ParseTemplates {
 public:
  ParseTemplates(const QStringList &sListTransTpl,
                 const QStringList &sListTplNames,
                 const QStringList &sListHtmlStart, const QString &sSharePath,
                 const QDir &tmpImgDir,
                 const QHash<QString, QString> &TestedWithMap,
                 const QHash<QString, QString> &TestedWithTouchMap,
                 const QString &sCommunity);

  void startParsing(QTextDocument *pRawDoc, const QString &sCurrentFile);

 private:
  ProvisionalTplParser *m_pProvTplTarser;
  QStringList m_sListTransTpl;
  QStringList m_sListTplNames;
  QString m_sCurrentFile;
};

#endif  // APPLICATION_PARSER_PARSETEMPLATES_H_
