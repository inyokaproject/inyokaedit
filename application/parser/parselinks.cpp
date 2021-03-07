/**
 * \file parselinks.cpp
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
 * Parse all kind of links (external, InterWiki, etc.)
 */

// #include <QDebug>
#include <QEventLoop>
#include <QTextDocument>

#include "./parselinks.h"
#include "../utils.h"

ParseLinks::ParseLinks(const QString &sUrlToWiki,
                       const QStringList &sListIWiki,
                       const QStringList &sListIWikiUrl,
                       const bool bCheckLinks, QObject *pParent)
  : m_sWikiUrl(sUrlToWiki),
    m_sListInterwikiKey(sListIWiki),
    m_sListInterwikiLink(sListIWikiUrl),
    m_bCheckLinks(bCheckLinks),
    m_NWreply(nullptr) {
  Q_UNUSED(pParent)
  m_NWAManager = new QNetworkAccessManager(this);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ParseLinks::updateSettings(const QString &sUrlToWiki,
                                const bool bCheckLinks) {
  m_sWikiUrl = sUrlToWiki;
  m_bCheckLinks = bCheckLinks;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ParseLinks::startParsing(QTextDocument *pRawDoc) {
  ParseLinks::replaceHyperlinks(pRawDoc);
  this->replaceInyokaWikiLinks(pRawDoc);
  this->replaceInterwikiLinks(pRawDoc);
  ParseLinks::replaceAnchorLinks(pRawDoc);
  ParseLinks::replaceKnowledgeBoxLinks(pRawDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// External links [https://www.ubuntu.com]
void ParseLinks::replaceHyperlinks(QTextDocument *pRawDoc) {
  QRegExp findHyperlink(
        QString::fromLatin1("\\[{1,1}\\b(http|https|ftp|ftps|file|ssh|mms|svn"
                            "|git|dict|nntp|irc|rsync|smb|apt)\\b://"));
  QString sDoc(pRawDoc->toPlainText());
  int nIndex;
  int nLength;
  QString sLink;
  int nSpace;

  nIndex = findHyperlink.indexIn(sDoc);
  while (nIndex >= 0) {
    // Found end of link
    if (sDoc.indexOf(QLatin1String("]"), nIndex) != -1) {
      nLength = sDoc.indexOf(QLatin1String("]"), nIndex) - nIndex + 1;
      sLink = sDoc.mid(nIndex, nLength);
      // qDebug() << "FOUND: " << sLink;

      sLink.remove(QStringLiteral("["));
      sLink.remove(QStringLiteral("]"));

      nSpace = sLink.indexOf(QLatin1String(" "), 0);
      // Link with description
      if (nSpace != -1) {
        QString sHref = sLink;
        sDoc.replace(nIndex, nLength,
                     "<a href=\"" + sHref.remove(nSpace, nLength)
                     + "\" rel=\"nofollow\" class=\"external\">"
                     + sLink.remove(0, nSpace + 1) + "</a>");
      } else {
        // Plain link
        sDoc.replace(nIndex, nLength,
                     "<a href=\"" + sLink
                     + "\" rel=\"nofollow\" class=\"external\">"
                     + sLink + "</a>");
      }

      // Go on with next
      nIndex = findHyperlink.indexIn(sDoc, nIndex + nLength);
    } else {
      // Skip not closed link and go on with next
      nIndex = findHyperlink.indexIn(sDoc, nIndex + 1);
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Inyoka wiki links [:Wikipage:]
void ParseLinks::replaceInyokaWikiLinks(QTextDocument *pRawDoc) {
  QRegExp findInyokaWikiLink(QLatin1String("\\[{1,1}\\:[0-9A-Za-z:.]"));
  QString sDoc(pRawDoc->toPlainText());
  int nIndex;
  int nLength;
  QString sLink;
  QString sLinkURL;
  bool bIsOnline(Utils::getOnlineState());

  nIndex = findInyokaWikiLink.indexIn(sDoc);
  while (nIndex >= 0) {
    // Found end of link
    if (sDoc.indexOf(QLatin1String("]"), nIndex) != -1) {
      nLength = sDoc.indexOf(QLatin1String("]"), nIndex) - nIndex + 1;
      sLink = sDoc.mid(nIndex, nLength);
      if (2 <= sLink.count(QStringLiteral(":"))) {
        // qDebug() << "FOUND: " << sLink;
        sLink.remove(QStringLiteral("[:"));

        // No description
        if (sLink.endsWith(QLatin1String(":]"))) {
          QString sAnchor(QLatin1String(""));
          sLink.remove(QStringLiteral(":]"));
          // qDebug() << sLink;
          QString sLink2 = sLink;
          sLink2.replace(QLatin1String("_"), QLatin1String(" "));
          sLinkURL = m_sWikiUrl + "/" + sLink;

          // Contains anchor link
          if (sLink.contains('#')) {
            sAnchor = sLink.mid(sLink.indexOf('#') + 1);
            sLink2 = sLink2.remove("#" + sAnchor);
            sAnchor = " (" + tr("Section") + " \"" + sAnchor + "\")";
          }

          m_sLinkClassAddition = QLatin1String("");
          if (bIsOnline && m_bCheckLinks) {
            m_NWreply = m_NWAManager->get(
                          QNetworkRequest(
                            QUrl(sLinkURL + "/a/export/meta/")));
            QEventLoop loop;  // Workaround getting synchron reply
            connect(m_NWreply, &QNetworkReply::finished,
                    &loop, &QEventLoop::quit);
            loop.exec();

            if (QNetworkReply::NoError == m_NWreply->error()) {
              m_sLinkClassAddition = QLatin1String("");
            } else {
              m_sLinkClassAddition = QStringLiteral(" missing");
            }
          }
          sDoc.replace(nIndex, nLength,
                       "<a href=\"" + sLinkURL
                       + "\" class=\"internal"
                       + m_sLinkClassAddition + "\">"
                       + sLink2 + sAnchor + "</a>");
        } else {
          sLink.remove(QStringLiteral("]"));
          // qDebug() << sLink.mid(0, sLink.indexOf(":"))
          //          << " - "
          //          << sLink.mid(sLink.indexOf(":") + 1, nLength);
          sLinkURL = m_sWikiUrl + "/"
                     + sLink.mid(0, sLink.indexOf(QLatin1String(":")));
          if (bIsOnline && m_bCheckLinks) {
            m_NWreply = m_NWAManager->get(
                          QNetworkRequest(
                            QUrl(sLinkURL + "/a/export/meta/")));
            QEventLoop loop;
            connect(m_NWreply, &QNetworkReply::finished,
                    &loop, &QEventLoop::quit);
            loop.exec();

            if (QNetworkReply::NoError == m_NWreply->error()) {
              m_sLinkClassAddition = QLatin1String("");
            } else {
              m_sLinkClassAddition = QStringLiteral(" missing");
            }
          }
          sDoc.replace(nIndex, nLength,
                       "<a href=\"" + sLinkURL
                       + "\" class=\"internal"
                       + m_sLinkClassAddition + "\">"
                       + sLink.mid(sLink.indexOf(QLatin1String(":"))
                                   + 1, nLength).trimmed() + "</a>");
        }
      }

      // Go on with next
      nIndex = findInyokaWikiLink.indexIn(sDoc, nIndex + nLength);
    } else {
      // Skip not closed link and go on with next
      nIndex = findInyokaWikiLink.indexIn(sDoc, nIndex + 1);
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Interwiki links [wikipedia:Site:Text]
void ParseLinks::replaceInterwikiLinks(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  int nIndex;
  int nLength;
  QString sLink;
  QStringList sListLink;
  QString sClass;

  // Generate pattern
  QString sPattern = QStringLiteral("\\[{1,1}\\b(");
  for (int i = 0; i < m_sListInterwikiKey.size(); i++) {
    sPattern += m_sListInterwikiKey[i];
    if (i != m_sListInterwikiKey.size() -1) {
      sPattern += QLatin1String("|");
    }
  }
  sPattern += QLatin1String(")\\b:");

  QRegExp findInterwikiLink(sPattern);
  // qDebug() << sPattern;
  nIndex = findInterwikiLink.indexIn(sDoc);
  while (nIndex >= 0) {
    // Found end of link
    if (sDoc.indexOf(QLatin1String("]"), nIndex) != -1) {
      nLength = sDoc.indexOf(QLatin1String("]"), nIndex) - nIndex + 1;
      sLink = sDoc.mid(nIndex, nLength);
      if (2 <= sLink.count(QStringLiteral(":"))) {
        sLink.remove(QStringLiteral("["));
        sLink.remove(QStringLiteral("]"));
        sListLink = sLink.split(QStringLiteral(":"));
        // qDebug() << sListLink;

        if (sListLink.size() > 1) {
          if (sListLink[0] == QLatin1String("user")) {
            sClass = QStringLiteral("crosslink user");
          } else if (sListLink[0] == QLatin1String("ikhaya")) {
            sClass = QStringLiteral("crosslink ikhaya");
          } else if (sListLink[0] == QLatin1String("paste")) {
            sClass = QStringLiteral("crosslink paste");
          } else {
            sClass = "interwiki interwiki-" + sListLink[0];
          }

          if (sListLink.size() >= 3) {
            QString sTmpUrl(
                  m_sListInterwikiLink[m_sListInterwikiKey.indexOf(
                  sListLink[0])]);
            // Check for iWikilink with PAGE
            if (sTmpUrl.contains(QLatin1String("PAGE"), Qt::CaseSensitive)) {
              sTmpUrl.replace(QLatin1String("PAGE"), sListLink[1],
                  Qt::CaseSensitive);
            } else {
              sTmpUrl.append(sListLink[1]);
            }

            // Default: Description = sitename
            QString sTmpDescr(sListLink[1]);

            // With description
            if (!sListLink[2].isEmpty()) {
              sTmpDescr = sListLink[2];
              // Append description with ":" if any exist
              for (int i = 3; i < sListLink.size(); i++) {
                sTmpDescr.append(":" + sListLink[i]);
              }
            }

            // Replace link
            sDoc.replace(nIndex, nLength,
                         "<a href=\"" + sTmpUrl + "\" class=\""
                         + sClass + "\">" + sTmpDescr + "</a>");
          }
        }
      }

      // Go on with next
      nIndex = findInterwikiLink.indexIn(sDoc, nIndex + nLength);
    } else {
      // Skip not closed link and go on with next
      nIndex = findInterwikiLink.indexIn(sDoc, nIndex + 1);
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Anchor [#Headline Text]
void ParseLinks::replaceAnchorLinks(QTextDocument *pRawDoc) {
  QRegExp findAnchorLink(QLatin1String("\\[{1,1}\\#"));
  QString sDoc(pRawDoc->toPlainText());
  int nIndex;
  int nLength;
  QString sLink;
  int nSplit;

  nIndex = findAnchorLink.indexIn(sDoc);
  while (nIndex >= 0) {
    // Found end of link
    if (sDoc.indexOf(QLatin1String("]"), nIndex) != -1) {
      nLength = sDoc.indexOf(QLatin1String("]"), nIndex) - nIndex + 1;
      sLink = sDoc.mid(nIndex, nLength);

      sLink.remove(QStringLiteral("[#"));
      sLink.remove(QStringLiteral("]"));
      nSplit = sLink.indexOf(QLatin1String(" "));
      // qDebug() << sLink.mid(0, nSplit)
      //          << sLink.mid(nSplit + 1 , nLength);

      // With description
      if (nSplit != -1) {
        sDoc.replace(nIndex, nLength,
                     "<a href=\"#" + sLink.mid(0, nSplit)
                     + "\" class=\"crosslink\">"
                     + sLink.mid(nSplit + 1 , nLength) + "</a>");
      } else {
        // Without descrition
        sDoc.replace(nIndex, nLength,
                     "<a href=\"#" + sLink.mid(0, nSplit)
                     + "\" class=\"crosslink\">#"
                     + sLink.mid(0, nSplit) + "</a>");
      }

      // Go on with next
      nIndex = findAnchorLink.indexIn(sDoc, nIndex + nLength);
    } else {
      // Skip not closed link and go on with next
      nIndex = findAnchorLink.indexIn(sDoc, nIndex + 1);
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Link to knowledge box entry
void ParseLinks::replaceKnowledgeBoxLinks(QTextDocument *pRawDoc) {
  QRegExp findKnowledgeBoxLink(QLatin1String("\\[{1,1}[0-9]{1,}\\]{1,1}"));
  QString sDoc(pRawDoc->toPlainText());
  int nIndex;

  nIndex = findKnowledgeBoxLink.indexIn(sDoc);
  while (nIndex >= 0) {
    int nLength = findKnowledgeBoxLink.matchedLength();
    QString sLink = findKnowledgeBoxLink.cap();
    // qDebug() << sLink;

    sLink.remove(QStringLiteral("["));
    sLink.remove(QStringLiteral("]"));

    if (sLink.toUShort() != 0) {
      sDoc.replace(nIndex, nLength,
                   "<sup><a href=\"#source-" + sLink + "\">&#091;"
                   + sLink + "&#093;</a></sup>");
    }

    // Go on with next
    nIndex = findKnowledgeBoxLink.indexIn(sDoc, nIndex + nLength);
  }

  pRawDoc->setPlainText(sDoc);
}
