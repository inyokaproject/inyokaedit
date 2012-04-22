/**
 * \file CParseLinks.cpp
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
 * Parse all kind of links (external, InterWiki, etc.)
 */

#include <QDebug>
#include "CParseLinks.h"

CParseLinks::CParseLinks( QTextDocument *pRawDocument,
                          const QString &sUrlToWiki,
                          const QList<QStringList> sListIWiki,
                          const QList<QStringList> sListIWikiUrl,
                          const bool bCheckLinks,
                          const QString sTransAnchor )
    : m_pRawText(pRawDocument),
      m_sWikiUrl(sUrlToWiki),
      m_bCheckLinks(bCheckLinks),
      m_sTransAnchor( sTransAnchor )
{
    qDebug() << "Start" << Q_FUNC_INFO;

    // Copy interwiki links to lists
    for ( int i = 0; i < sListIWiki.size(); i++ )
    {
        for ( int j = 0; j < sListIWiki[i].size(); j++ )
        {
            this->m_sListInterwikiKey << sListIWiki[i][j];
            this->m_sListInterwikiLink << sListIWikiUrl[i][j];
        }
    }

    m_NWAManager = new QNetworkAccessManager(this);

    qDebug() << "End" << Q_FUNC_INFO;
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CParseLinks::startParsing( QTextDocument *pRawDoc )
{
    this->replaceHyperlinks( pRawDoc );
    this->replaceInyokaWikiLinks( pRawDoc );
    this->replaceInterwikiLinks( pRawDoc );
    this->replaceAnchorLinks( pRawDoc );
    this->replaceKnowledgeBoxLinks( pRawDoc );
    this->createAnchor( pRawDoc );
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// External links [http://www.ubuntu.com]
void CParseLinks::replaceHyperlinks( QTextDocument *pRawDoc )
{
    QRegExp findHyperlink( "\\[{1,1}\\b(http|https|ftp|ftps|file|ssh|mms|svn|git|dict|nntp|irc|rsync|smb|apt)\\b://" );
    QString sMyDoc = pRawDoc->toPlainText();
    int nIndex;
    int nLength;
    QString sLink;
    int nSpace;

    nIndex = findHyperlink.indexIn( sMyDoc );
    while ( nIndex >= 0 )
    {
        // Found end of link
        if ( sMyDoc.indexOf( "]", nIndex ) != -1 )
        {
            nLength = sMyDoc.indexOf( "]", nIndex ) - nIndex + 1;  // Find end of link "]"
            sLink = sMyDoc.mid( nIndex, nLength );
            //qDebug() << "FOUND: " << sLink;

            sLink.remove( "[" );
            sLink.remove( "]" );

            nSpace = sLink.indexOf( " ", 0 );
            // Link with description
            if ( nSpace != -1 )
            {
                QString sHref = sLink;
                sMyDoc.replace( nIndex, nLength, "<a href=\"" + sHref.remove( nSpace, nLength ) + "\" rel=\"nofollow\" class=\"external\">" + sLink.remove( 0, nSpace + 1 ) + "</a>" );
            }
            // Plain link
            else
            {
                sMyDoc.replace( nIndex, nLength, "<a href=\"" + sLink + "\" rel=\"nofollow\" class=\"external\">" + sLink + "</a>" );
            }

            // Go on with next
            nIndex = findHyperlink.indexIn( sMyDoc, nIndex + nLength );
        }

        // Skip not closed link and go on with next
        else
        {
            nIndex = findHyperlink.indexIn( sMyDoc, nIndex + 1 );
        }
    }

    // Replace myRawDoc with document with HTML links
    pRawDoc->setPlainText( sMyDoc );
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Inyoka wiki links [:Wikipage:]
void CParseLinks::replaceInyokaWikiLinks( QTextDocument *pRawDoc )
{
    QRegExp findInyokaWikiLink( "\\[{1,1}\\:[0-9A-Za-z.]" );
    QString sMyDoc = pRawDoc->toPlainText();
    int nIndex;
    int nLength = 0;
    QString sLink("");
    QString sLinkURL("");

    // Check for internet connection
    m_bIsOnline = false;
    #if QT_VERSION >= 0x040700
        QNetworkConfigurationManager mgr;
        m_bIsOnline = mgr.isOnline();
    #endif

    nIndex = findInyokaWikiLink.indexIn( sMyDoc );
    while ( nIndex >= 0 )
    {
        // Found end of link
        if ( sMyDoc.indexOf( "]", nIndex ) != -1 )
        {
            nLength = sMyDoc.indexOf( "]", nIndex ) - nIndex + 1;  // Find end of link "]"
            sLink = sMyDoc.mid( nIndex, nLength );
            if ( 2 == sLink.count(":") )
            {
                //qDebug() << "FOUND: " << sLink;
                sLink.remove( "[:" );

                // No description
                if ( sLink.endsWith( ":]" ) )
                {
                    sLink.remove( ":]" );
                    //qDebug() << sLink;
                    QString sLink2 = sLink;
                    sLink2.replace("_", " ");
                    sLinkURL = m_sWikiUrl + "/" + sLink;

                    m_sLinkClassAddition = "";
                    if ( m_bIsOnline && m_bCheckLinks )
                    {
                        m_NWreply = m_NWAManager->get(QNetworkRequest(QUrl( sLinkURL + "?action=metaexport" )));
                        QEventLoop loop;  // Workaround for getting reply synchronously
                        connect(m_NWreply, SIGNAL(finished()), &loop, SLOT(quit()));
                        loop.exec();

                        if ( QNetworkReply::NoError == m_NWreply->error() )
                        {
                            m_sLinkClassAddition = "";
                        }
                        else
                        {
                            m_sLinkClassAddition = " missing";
                        }
                    }
                    sMyDoc.replace( nIndex, nLength, "<a href=\"" + sLinkURL + "\" class=\"internal" + m_sLinkClassAddition + "\">" + sLink2 + "</a>");
                }
                else
                {
                    sLink.remove( "]" );
                    //qDebug() << sLink.mid( 0, sLink.indexOf(":")) << " - " << sLink.mid( sLink.indexOf(":") + 1, nLength );
                    sLinkURL = m_sWikiUrl + "/" + sLink.mid( 0, sLink.indexOf(":"));
                    if ( m_bIsOnline && m_bCheckLinks )
                    {
                        m_NWreply = m_NWAManager->get(QNetworkRequest(QUrl( sLinkURL + "?action=metaexport" )));
                        QEventLoop loop;
                        connect(m_NWreply, SIGNAL(finished()), &loop, SLOT(quit()));
                        loop.exec();

                        if ( QNetworkReply::NoError == m_NWreply->error() )
                        {
                            m_sLinkClassAddition = "";
                        }
                        else
                        {
                            m_sLinkClassAddition = " missing";
                        }
                    }
                    sMyDoc.replace( nIndex, nLength, "<a href=\"" + sLinkURL + "\" class=\"internal" + m_sLinkClassAddition + "\">" + sLink.mid( sLink.indexOf(":") + 1, nLength ).trimmed() + "</a>");
                }
            }
        }

        // Skip not closed link and go on with next
        else
        {
            nIndex = findInyokaWikiLink.indexIn( sMyDoc, nIndex + 1 );
        }

        // Go on with next
        nIndex = findInyokaWikiLink.indexIn( sMyDoc, nIndex + nLength );
    }

    // Replace myRawDoc with document with HTML links
    pRawDoc->setPlainText( sMyDoc );
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Interwiki links [wikipedia:Site:Text]
void CParseLinks::replaceInterwikiLinks( QTextDocument *pRawDoc )
{
    QString sMyDoc = pRawDoc->toPlainText();
    int nIndex;
    int nLength = 0;
    QString sLink;
    QStringList sListLink;
    QString sClass;

    // Generate pattern
    QString sPattern = "\\[{1,1}\\b(";
    for (int i = 0; i < m_sListInterwikiKey.size(); i++ )
    {
        sPattern += m_sListInterwikiKey[i];
        if ( i != m_sListInterwikiKey.size() -1 )
        {
            sPattern += "|";
        }
    }
    sPattern += ")\\b:";
    QRegExp findInterwikiLink( sPattern );
    //qDebug() << sPattern;

    nIndex = findInterwikiLink.indexIn( sMyDoc );
    while ( nIndex >= 0 )
    {
        // Found end of link
        if ( sMyDoc.indexOf( "]", nIndex ) != -1 )
        {
            nLength = sMyDoc.indexOf( "]", nIndex ) - nIndex + 1;  // Find end of link "]"
            sLink = sMyDoc.mid( nIndex, nLength );
            if ( 2 == sLink.count(":") )
            {
                sLink.remove( "[" );
                sLink.remove( "]" );
                sListLink = sLink.split( ":" );
                //qDebug() << sListLink;

                if ( sListLink.size() > 1 )
                {
                    if ( sListLink[0] == "user" )
                    {
                        sClass = "crosslink user";
                    }
                    else
                    {
                        sClass = "interwiki interwiki-" + sListLink[0];
                    }

                    if ( sListLink.size() == 3 )
                    {
                        // With description
                        if (sListLink[2] != "")
                        {
                            sMyDoc.replace(nIndex, nLength, "<a href=\"" + m_sListInterwikiLink[ m_sListInterwikiKey.indexOf( sListLink[0] ) ] + sListLink[1] + "\" class=\"" + sClass + "\">" + sListLink[2] + "</a>");
                        }
                        // Without description
                        else
                        {
                            sMyDoc.replace(nIndex, nLength, "<a href=\"" + m_sListInterwikiLink[ m_sListInterwikiKey.indexOf( sListLink[0] ) ] + sListLink[1] + "\" class=\"" + sClass + "\">" + sListLink[1] + "</a>");
                        }
                    }
                }

            }
        }

        // Skip not closed link and go on with next
        else
        {
            nIndex = findInterwikiLink.indexIn( sMyDoc, nIndex + 1 );
        }

        // Go on with next
        nIndex = findInterwikiLink.indexIn( sMyDoc, nIndex + nLength );
    }

    // Replace myRawDoc with document with HTML links
    pRawDoc->setPlainText( sMyDoc );

}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Anchor [#Headline Text]
void CParseLinks::replaceAnchorLinks( QTextDocument *pRawDoc )
{
    QRegExp findAnchorLink( "\\[{1,1}\\#" );
    QString sMyDoc = pRawDoc->toPlainText();
    int nIndex;
    int nLength = 0;
    QString sLink;
    int nSplit;

    nIndex = findAnchorLink.indexIn( sMyDoc );
    while ( nIndex >= 0 )
    {
        // Found end of link
        if ( sMyDoc.indexOf( "]", nIndex ) != -1 )
        {
            nLength = sMyDoc.indexOf( "]", nIndex ) - nIndex + 1;  // Find end of link "]"
            sLink = sMyDoc.mid( nIndex, nLength );

            sLink.remove( "[#" );
            sLink.remove( "]" );
            nSplit = sLink.indexOf(" ");
            //qDebug() << sLink.mid(0, nSplit) << sLink.mid(nSplit + 1 , nLength);

            // With description
            if (nSplit != -1)
            {
                sMyDoc.replace(nIndex, nLength, "<a href=\"#" + sLink.mid(0, nSplit) + "\" class=\"crosslink\">" + sLink.mid(nSplit + 1 , nLength) + "</a>");
            }
            // Without descrition
            else
            {
                sMyDoc.replace(nIndex, nLength, "<a href=\"#" + sLink.mid(0, nSplit) + "\" class=\"crosslink\">#" + sLink.mid(0, nSplit) + "</a>");
            }
        }

        // Skip not closed link and go on with next
        else
        {
            nIndex = findAnchorLink.indexIn( sMyDoc, nIndex + 1 );
        }

        // Go on with next
        nIndex = findAnchorLink.indexIn( sMyDoc, nIndex + nLength );
    }

    // Replace myRawDoc with document with HTML links
    pRawDoc->setPlainText( sMyDoc );
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Link to knowledge box entry
void CParseLinks::replaceKnowledgeBoxLinks( QTextDocument *pRawDoc )
{
    QRegExp findKnowledgeBoxLink( "\\[{1,1}[0-9]{1,}\\]{1,1}" );
    QString sMyDoc = pRawDoc->toPlainText();
    int nIndex;
    int nLength;
    QString sLink;

    nIndex = findKnowledgeBoxLink.indexIn( sMyDoc );
    while ( nIndex >= 0 )
    {
        nLength = findKnowledgeBoxLink.matchedLength();
        sLink = findKnowledgeBoxLink.cap();
        //qDebug() << sLink;

        sLink.remove("[");
        sLink.remove("]");

        if ( sLink.toUShort() != 0 )
        {
            sMyDoc.replace(nIndex, nLength, "<sup><a href=\"#source-" + sLink + "\">&#091;" + sLink + "&#093;</a></sup>");
        }

        // Go on with next
        nIndex = findKnowledgeBoxLink.indexIn( sMyDoc, nIndex + nLength );
    }

    // Replace myRawDoc with document with HTML links
    pRawDoc->setPlainText( sMyDoc );
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

// Create anchor
void CParseLinks::createAnchor( QTextDocument *pRawDoc )
{
    QRegExp findAnchor( "\\[{2,2}\\b(" + m_sTransAnchor + ")\\([A-Za-z_\\s-]+\\)\\]{2,2}" );
    QString sMyDoc = pRawDoc->toPlainText();
    int nIndex;
    int nLength;
    QString sAnchor;

    nIndex = findAnchor.indexIn(sMyDoc);
    while (nIndex >= 0) {
        nLength = findAnchor.matchedLength();
        sAnchor = findAnchor.cap();
        //qDebug() << sAnchor;

        sAnchor.remove("[[" + m_sTransAnchor + "(");
        sAnchor.remove(")]]");
        sAnchor = sAnchor.trimmed();

        // Replace characters for valid links (ä, ü, ö, spaces)
        sAnchor.replace(" ", "-");
        sAnchor.replace(QString::fromUtf8("Ä"), "Ae");
        sAnchor.replace(QString::fromUtf8("Ü"), "Ue");
        sAnchor.replace(QString::fromUtf8("Ö"), "Oe");
        sAnchor.replace(QString::fromUtf8("ä"), "ae");
        sAnchor.replace(QString::fromUtf8("ü"), "ue");
        sAnchor.replace(QString::fromUtf8("ö"), "oe");

        // Replace
        sMyDoc.replace(nIndex, nLength, "<a id=\"" + sAnchor + "\" href=\"#" + sAnchor + "\" class=\"crosslink anchor\"> </a>");

        // Go on with RegExp-Search
        nIndex = findAnchor.indexIn(sMyDoc, nIndex + nLength);
    }

    // Replace myRawDoc with document with HTML links
    pRawDoc->setPlainText(sMyDoc);

}
