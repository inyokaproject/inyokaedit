// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSER_H_
#define APPLICATION_PARSER_PARSER_H_

#include <QDir>
#include <QString>
#include <QStringList>

class QTextDocument;

class Macros;
class ParseLinks;
class ParseTemplates;
class Templates;

class Parser : public QObject {
  Q_OBJECT

 public:
  Parser(const QString &sSharePath, const QDir &tmpImgDir,
         const QString &sInyokaUrl, const bool bCheckLinks,
         Templates *pTemplates, const QString &sCommunity,
         const QString &sPygmentize, QObject *pParent = nullptr);
  ~Parser();

  // Starts generating HTML-code
  QString genOutput(const QString &sActFile, QTextDocument *pRawDocument,
                    const bool bSyntaxCheck = false);

 public slots:
  void updateSettings(const bool bCheckLinks, const quint32 nTimedPreview);

 signals:
  void hightlightSyntaxError(const QPair<int, QString>);

 private:
  // void replaceTemplates(QTextDocument *pRawDoc);

  void filterEscapedChars(QTextDocument *pRawDoc);
  void filterNoTranslate(QTextDocument *pRawDoc);
  void replaceCodeblocks(QTextDocument *pRawDoc);
  void reinstertNoTranslate(QTextDocument *pRawDoc);

  static void removeComments(QTextDocument *pRawDoc);
  static void generateParagraphs(QTextDocument *pRawDoc);

#ifdef USEQTWEBENGINE
  void replaceFlags(QTextDocument *pRawDoc);
#endif
  static void replaceQuotes(QTextDocument *pRawDoc);
  static void replaceHorLines(QTextDocument *pRawDoc);
  static auto replaceHeadlines(QTextDocument *pRawDoc) -> QStringList;
  static void replaceFootnotes(QTextDocument *pRawDoc);
  auto generateTags(QTextDocument *pRawDoc) -> QString;
  auto highlightCode(const QString &sLanguage, const QString &sCode) -> QString;

  // Text from editor
  QTextDocument *m_pRawText;

  QStringList m_sListNoTranslate;

  ParseTemplates *m_pTemplateParser;
  ParseLinks *m_pLinkParser;

  const QString m_sSharePath;
  const QDir m_tmpImgDir;
  QString m_sInyokaUrl;
  QString m_sCurrentFile;
  Templates *m_pTemplates;
  Macros *m_pMacros;
  const QString m_sCommunity;
  const QString m_sPygmentize;
  quint32 m_nTimedPreview;
};

#endif  // APPLICATION_PARSER_PARSER_H_
