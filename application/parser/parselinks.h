/**
 * \file parselinks.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * Class definition for link parser.
 */

#ifndef APPLICATION_PARSER_PARSELINKS_H_
#define APPLICATION_PARSER_PARSELINKS_H_

#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QTextDocument;

/**
 * \class ParseLinks
 * \brief Part of parser module responsible for any kind of links.
 */
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

  QString m_sWikiUrl;                // Inyoka wiki url
  QHash<QString, QString> m_IwlMap;  // Interwiki link keys/urls

  bool m_bCheckLinks;
  QString m_sLinkClassAddition;
  QNetworkAccessManager *m_NWAManager;
  QNetworkReply *m_NWreply;
};

#endif  // APPLICATION_PARSER_PARSELINKS_H_
