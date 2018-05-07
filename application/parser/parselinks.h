/**
 * \file parselinks.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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
 * Class definition for link parser.
 */

#ifndef APPLICATION_PARSER_PARSELINKS_H_
#define APPLICATION_PARSER_PARSELINKS_H_

#include <QEventLoop>
#include <QList>
#include <QNetworkConfigurationManager>
#include <QNetworkReply>
#include <QStringList>
#include <QTextDocument>

/**
 * \class ParseLinks
 * \brief Part of parser module responsible for any kind of links.
 */
class ParseLinks : public QObject {
  Q_OBJECT

  public:
    ParseLinks(const QString &sUrlToWiki,
               const QStringList sListIWiki,
               const QStringList sListIWikiUrl,
               const bool bCheckLinks,
               QObject *pParent = 0);

    void startParsing(QTextDocument *pRawDoc);

  public slots:
    void updateSettings(const QString sUrlToWiki, const bool bCheckLinks);

  private:
    void replaceHyperlinks(QTextDocument *pRawDoc);
    void replaceInyokaWikiLinks(QTextDocument *pRawDoc);
    void replaceInterwikiLinks(QTextDocument *pRawDoc);
    void replaceAnchorLinks(QTextDocument *pRawDoc);
    void replaceKnowledgeBoxLinks(QTextDocument *pRawDoc);

    QString m_sWikiUrl;   // Inyoka wiki url
    QStringList m_sListInterwikiKey;   // Interwiki link keywords
    QStringList m_sListInterwikiLink;  // Interwiki link urls

    bool m_bCheckLinks;
    QString m_sLinkClassAddition;
    QNetworkAccessManager *m_NWAManager;
    QNetworkReply *m_NWreply;
};

#endif  // APPLICATION_PARSER_PARSELINKS_H_
