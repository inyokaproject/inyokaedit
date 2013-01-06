/**
 * \file CDownloadImg.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
#include <QFileInfo>

#include "./CDownloadImg.h"

CDownloadImg::CDownloadImg(const QString &sAppName)
    : m_sAppname(sAppName) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    connect(&m_NwManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloadFinished(QNetworkReply*)));
}

void CDownloadImg::setDLs(const QStringList &sListUrls,
                          const QStringList &sListSavePath) {
    m_sListUrls = sListUrls;
    m_sListSavePath = sListSavePath;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownloadImg::startDownloads() {
    // Create progress dialog
    nProgress = 0;
    m_progessDialog = new QProgressDialog(tr("Downloading images..."),
                                          tr("Cancel"), nProgress,
                                          m_sListUrls.size(), 0,
                                          Qt::WindowTitleHint
                                          | Qt::WindowSystemMenuHint);
    m_progessDialog->setWindowTitle(m_sAppname);
    m_progessDialog->setMinimumDuration(100);
    m_progessDialog->setModal(true);
    m_progessDialog->setMaximumSize(m_progessDialog->size());
    m_progessDialog->setMinimumSize(m_progessDialog->size());
    connect(m_progessDialog, SIGNAL(canceled()),
            this, SLOT(cancelDownloads()));

    if (m_sListUrls.size() == m_sListSavePath.size()
            && m_sListUrls.size() > 0) {
        for (int i = 0; i < m_sListUrls.size(); i++) {
            QUrl url = QUrl::fromEncoded(m_sListUrls[i].toLocal8Bit());
            this->doDownload(url, m_sListSavePath[i]);
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownloadImg::doDownload(const QUrl &url,
                              const QString &sSavePath,
                              const QString sBase) {
    QNetworkRequest request(url);
    QNetworkReply *reply = m_NwManager.get(request);

    m_listDownloadReplies.append(reply);
    m_sListRepliesPath.append(sSavePath);
    m_sListBasename.append(sBase);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownloadImg::downloadFinished(QNetworkReply *reply) {
    QIODevice *data(reply);
    int nIndex = m_listDownloadReplies.indexOf(reply);
    m_progessDialog->setValue(nProgress);

    // Check for redirection
    QVariant possibleRedirectUrl = reply->attribute(
                QNetworkRequest::RedirectionTargetAttribute);
    m_urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),
                                          m_urlRedirectedTo);

    // Error
    if (QNetworkReply::NoError != reply->error()) {
        if (QNetworkReply::OperationCanceledError != reply->error()) {
            QMessageBox::critical(0, "Download error", data->errorString());
        }
        qWarning() << "Image download error: " + data->errorString();

        m_progessDialog->setValue(nProgress);
        nProgress++;
    } else {
        // No error
        QUrl url = reply->url();
        qDebug() << "Downloading URL: " << url.toString();

        // Basename has to be set before possible redirection
        // (redirected file could have other basename)
        if ("" == m_sListBasename[nIndex]) {
            m_sListBasename[nIndex] = url.toString().mid(
                        url.toString().lastIndexOf("/") + 1);
        }

        // If the URL is not empty, we're being redirected
        if (!m_urlRedirectedTo.isEmpty()) {
            url = m_urlRedirectedTo;
            qDebug() << "Redirected to: " + url.toString();

            // New request with redirected url
            this->doDownload(url, m_sListRepliesPath[nIndex],
                             m_sListBasename[nIndex]);
        } else {
            m_urlRedirectedTo.clear();
            m_progessDialog->setValue(nProgress);
            nProgress++;

            // Save file
            QFile file(m_sListRepliesPath[nIndex]
                       + "/" + m_sListBasename[nIndex]);
            if (!file.open(QIODevice::WriteOnly)) {
                qWarning() << "Could not open " + file.fileName()
                              + " for writing: " + file.errorString();
            } else {
                file.write(data->readAll());
                file.close();

                qDebug() << "Download of " + m_sListBasename[nIndex]
                            + " succeeded - saved to "
                            +  m_sListRepliesPath[nIndex];
            }
        }
    }

    m_listDownloadReplies.removeAll(reply);
    m_sListRepliesPath.removeAt(nIndex);
    m_sListRedirect.removeAt(nIndex);
    m_sListBasename.removeAt(nIndex);
    reply->deleteLater();

    // All downloads finished
    if (m_listDownloadReplies.isEmpty()) {
        m_progessDialog->setValue(m_progessDialog->maximum());
        m_progessDialog->deleteLater();

        m_sListRepliesPath.clear();
        m_sListBasename.clear();
        m_sListRedirect.clear();

        qDebug() << "All downloads finished...";
        emit finsihedImageDownload();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QUrl CDownloadImg::redirectUrl(const QUrl& possibleRedirectUrl,
                               const QUrl& oldRedirectUrl) const {
    QUrl redirectUrl;
    if (!possibleRedirectUrl.isEmpty()
            && possibleRedirectUrl != oldRedirectUrl) {
        redirectUrl = possibleRedirectUrl;
    }
    return redirectUrl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownloadImg::cancelDownloads() {
    foreach (QNetworkReply *reply, m_listDownloadReplies) {
        reply->abort();
    }

    m_listDownloadReplies.clear();
    m_sListRepliesPath.clear();
    m_sListBasename.clear();
    m_sListRedirect.clear();

    qDebug() << "Canceled downloads...";
    emit finsihedImageDownload();
}
