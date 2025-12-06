// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_PARSER_PARSELINKS_H_
#define APPLICATION_PARSER_PARSELINKS_H_

#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QTextDocument;

class ParseLinks : public QObject {
  Q_OBJECT

 public:
  ParseLinks(const QString &sUrlToWiki, const QHash<QString, QString> &IwlMap,
             const bool bCheckLinks, QObject *pParent = nullptr);

  void startParsing(QTextDocument *pRawDoc);

 public slots:
  void updateSettings(const bool bCheckLinks);

 private:
  static void replaceUrls(QTextDocument *pRawDoc);
  static void replaceHyperlinks(QTextDocument *pRawDoc);
  void replaceInyokaWikiLinks(QTextDocument *pRawDoc);
  void replaceInterwikiLinks(QTextDocument *pRawDoc);
  static void replaceAnchorLinks(QTextDocument *pRawDoc);
  static void replaceKnowledgeBoxLinks(QTextDocument *pRawDoc);

  QString m_sWikiUrl;                // Inyoka wiki URL
  QHash<QString, QString> m_IwlMap;  // Interwiki link keys/URLs

  bool m_bCheckLinks;
  QString m_sLinkClassAddition;
  QNetworkAccessManager *m_NWAManager;
  QNetworkReply *m_NWreply;
};

#endif  // APPLICATION_PARSER_PARSELINKS_H_
