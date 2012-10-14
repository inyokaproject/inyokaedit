/**
 * \file CParseLinks.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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

#ifndef INYOKAEDIT_CPARSELINKS_H_
#define INYOKAEDIT_CPARSELINKS_H_

#include <QTextDocument>
#include <QList>
#include <QStringList>
#include <QNetworkReply>
#include <QEventLoop>
#if QT_VERSION >= 0x040700
#include <QNetworkConfigurationManager>
#endif


/**
 * \class CParseLinks
 * \brief Part of parser module responsible for any kind of links.
 */
class CParseLinks : public QObject {
    Q_OBJECT

  public:
    CParseLinks(const QString &sUrlToWiki,
                const QList<QStringList> sListIWiki,
                const QList<QStringList> sListIWikiUrl,
                const bool bCheckLinks,
                const QString &sTransAnchor);

    void startParsing(QTextDocument *pRawDoc);

  private:
    void replaceHyperlinks(QTextDocument *pRawDoc);
    void replaceInyokaWikiLinks(QTextDocument *pRawDoc);
    void replaceInterwikiLinks(QTextDocument *pRawDoc);
    void replaceAnchorLinks(QTextDocument *pRawDoc);
    void replaceKnowledgeBoxLinks(QTextDocument *pRawDoc);
    void createAnchor(QTextDocument *pRawDoc);

    const QString m_sWikiUrl;   // Inyoka wiki url
    QStringList m_sListInterwikiKey;   // Interwiki link keywords
    QStringList m_sListInterwikiLink;  // Interwiki link urls

    bool m_bIsOnline;
    bool m_bCheckLinks;
    QString m_sTransAnchor;
    QString m_sLinkClassAddition;
    QNetworkAccessManager *m_NWAManager;
    QNetworkReply *m_NWreply;
};

#endif  // INYOKAEDIT_CPARSELINKS_H_
