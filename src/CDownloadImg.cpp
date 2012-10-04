/**
 * \file CDownloadImg.cpp
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
 * Download manager for images.
 */

#include <QDebug>
#include <QMessageBox>
#include <QMetaEnum>
#include <QFileInfo>

#include "CDownloadImg.h"

CDownloadImg::CDownloadImg()
{
    qDebug() << "Start" << Q_FUNC_INFO;

    connect( &m_NwManager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(downloadFinished(QNetworkReply*)) );

    qDebug() << "End" << Q_FUNC_INFO;
}

void CDownloadImg::setDLs( const QStringList sListUrls, const QStringList sListSavePath )
{
    m_sListUrls = sListUrls;
    m_sListSavePath = sListSavePath;
}
// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CDownloadImg::startDownloads()
{
    if( m_sListUrls.size() == m_sListSavePath.size() && m_sListUrls.size() > 0 )
    {
        for( int i = 0; i < m_sListUrls.size(); i++ )
        {
            QUrl url = QUrl::fromEncoded( m_sListUrls[i].toLocal8Bit() );
            this->doDownload( url, m_sListSavePath[i] );
        }
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CDownloadImg::doDownload( const QUrl &url, const QString sSavePath, const QString sBase )
{
    QNetworkRequest request(url);
    QNetworkReply *reply = m_NwManager.get(request);

    m_listDownloadReplies.append( reply );
    m_sListRepliesPath.append( sSavePath );
    m_sListBasename.append( sBase );
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

void CDownloadImg::downloadFinished( QNetworkReply *reply )
{
    QIODevice *data(reply);
    int nIndex = m_listDownloadReplies.indexOf( reply );

    // Check for redirection
    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    m_urlRedirectedTo = this->redirectUrl( possibleRedirectUrl.toUrl(), m_urlRedirectedTo );

    // Error
    if( reply->error() != QNetworkReply::NoError )
    {
        QMessageBox::critical( 0, "Download error", data->errorString() );
    }
    // No error
    else
    {
        QUrl url = reply->url();
        qDebug() << "Downloading URL: " << url.toString();

        // Basename has to be set before possible redirection (redirected file could have other basename)
        if( m_sListBasename[nIndex] == "" )
        {
            m_sListBasename[nIndex] = url.toString().mid( url.toString().lastIndexOf("/") + 1 );
        }

        // If the URL is not empty, we're being redirected
        if( !m_urlRedirectedTo.isEmpty() )
        {
            url = m_urlRedirectedTo;
            qDebug() << "Redirected to: " + url.toString();

            // New request with redirected url
            this->doDownload( url, m_sListRepliesPath[nIndex], m_sListBasename[nIndex] );
        }
        else
        {
            m_urlRedirectedTo.clear();

            // Save file
            QFile file( m_sListRepliesPath[nIndex] + "/" + m_sListBasename[nIndex] );
            if( !file.open(QIODevice::WriteOnly) )
            {
                qWarning() << "Could not open " + file.fileName() + " for writing: " + file.errorString();
            }
            else
            {
                file.write( data->readAll() );
                file.close();

                qDebug() << "Download of " + m_sListBasename[nIndex] + " succeeded - saved to " + m_sListRepliesPath[nIndex];
            }
        }
    }

    m_listDownloadReplies.removeAll(reply);
    m_sListRepliesPath.removeAt( nIndex );
    m_sListRedirect.removeAt( nIndex );
    m_sListBasename.removeAt( nIndex );
    reply->deleteLater();

    // All downloads finished
    if( m_listDownloadReplies.isEmpty() )
    {
        qDebug() << "All downloads finished...";
        emit finsihedImageDownload();
    }
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------

QUrl CDownloadImg::redirectUrl( const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl ) const
{
    QUrl redirectUrl;
    /*
     * Check if the URL is empty and
     * that we aren't being fooled into a infinite redirect loop.
     * We could also keep track of how many redirects we have been to
     * and set a limit to it, but we'll leave that to you.
     */
    if( !possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl )
    {
        redirectUrl = possibleRedirectUrl;
    }
    return redirectUrl;
}
