// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_MACROS_H_
#define APPLICATION_PARSER_MACROS_H_

#include <QDir>
#include <QString>
#include <QStringList>

class QTextDocument;

struct MACRO {
  QString name;
  QStringList translations;
};

class Macros {
 public:
  Macros(const QString &sSharePath, const QDir &tmpImgDir);
  void startParsing(QTextDocument *pRawDoc, const QString &sCurrentFile,
                    const QString &sCommunity, QStringList &sListHeadlines);
  auto getTplTranslations() const -> QStringList;

 private:
  static void replaceAnchors(QTextDocument *pRawDoc, const QString &sTrans);
  static void replaceAttachments(QTextDocument *pRawDoc, const QString &sTrans);
  static void replaceDates(QTextDocument *pRawDoc, const QString &sTrans);
  static void replaceNewline(QTextDocument *pRawDoc, const QString &sTrans);
  void replacePictures(QTextDocument *pRawDoc, const QString &sTrans,
                       const QString &sCurrentFile, const QString &sCommunity);
  static void replaceTableOfContents(QTextDocument *pRawDoc,
                                     const QString &sTrans,
                                     QStringList &sListHeadlines);
  static void replaceSpan(QTextDocument *pRawDoc, const QString &sTrans);

  const QString m_sSharePath;
  const QDir m_tmpImgDir;
  QList<MACRO> m_listMacros;
  QStringList m_sListTplTranslations;
};

#endif  // APPLICATION_PARSER_MACROS_H_
