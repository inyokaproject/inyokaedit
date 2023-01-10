/**
 * \file download.cpp
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
 * Download functions: Styles, article, images
 */

#include "./download.h"

#include <QApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkReply>
#include <QTimer>

#include "./downloadimg.h"
#include "./session.h"
#include "./utils.h"

Download::Download(QWidget *pParent, Session *pSession,
                   const QString &sStylesDir, const QString &sImgDir,
                   const QString &sSharePath, QObject *pObj)
    : m_pParent(pParent),
      m_pSession(pSession),
      m_sStylesDir(sStylesDir),
      m_sImgDir(sImgDir),
      m_sInyokaUrl(QStringLiteral("https://wiki.ubuntuusers.de")),
      m_sConstructionArea(QLatin1String("")),
      m_bAutomaticImageDownload(false),
      m_sSharePath(sSharePath),
      m_bDownloadArticle(true) {
  Q_UNUSED(pObj)
  connect(m_pSession->getNwManager(), &QNetworkAccessManager::finished, this,
          &Download::replyFinished);

  m_DlImages = new DownloadImg(m_pSession->getNwManager());
  connect(m_DlImages, &DownloadImg::finsihedImageDownload, this,
          &Download::showArticle);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Download::updateSettings(const bool bDownloadImages,
                              const QString &sInyokaUrl,
                              const QString &sConstArea) {
  m_bAutomaticImageDownload = bDownloadImages;
  m_sInyokaUrl = sInyokaUrl;
  m_sConstructionArea = sConstArea;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Download::downloadArticle(QString sUrl) {
  // Check for internet connection
  if (!Utils::getOnlineState()) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("Download not possible, no active internet "
                            "connection found!"));
    return;
  }

  if (sUrl.isEmpty()) {
    m_sRevision = QLatin1String("");
    bool bOk;

    // Show input dialog
    m_sSitename = QInputDialog::getText(
        m_pParent, qApp->applicationName(),
        tr("Please insert name of the article which should be downloaded:"),
        QLineEdit::Normal, m_sConstructionArea + "/" + tr("Article"), &bOk);
    m_sSitename = m_sSitename.trimmed();

    if (!bOk || m_sSitename.isEmpty()) {
      return;
    }
    m_sSitename.replace(QLatin1String(" "), QLatin1String("_"));
    if (m_sSitename.endsWith('/')) {
      m_sSitename.remove(m_sSitename.length() - 1, 1);
    }

    // Download specific revision
    if (m_sSitename.contains(QLatin1String("@rev="))) {
      m_sRevision =
          m_sSitename.mid(m_sSitename.indexOf(QLatin1String("@rev=")));
      m_sRevision.remove(QStringLiteral("@rev="));
      m_sSitename.remove(m_sSitename.indexOf(QLatin1String("@rev=")),
                         m_sSitename.length());
      m_sRevision = m_sRevision + "/";
    }

    // Login needed for accessing ConstructionArea
    if (m_sSitename.startsWith(m_sConstructionArea)) {
      m_pSession->checkSession();
      if (!m_pSession->isLoggedIn()) {
        qWarning() << "Download failed - user not logged in!";
        return;
      }
    }

    sUrl = m_sInyokaUrl + "/" + m_sSitename + "/a/export/raw/" + m_sRevision;
  }

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  m_bDownloadArticle = true;
  qDebug() << "DOWNLOADING article:" << sUrl;
  QNetworkRequest request(sUrl);
  request.setOriginatingObject(this);
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::ManualRedirectPolicy);
  m_urlRedirectedTo = sUrl;
  QNetworkReply *reply = m_pSession->getNwManager()->get(request);
  m_listDownloadReplies.append(reply);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Download::downloadImages() {
  m_bDownloadArticle = false;

  QString sUrl(m_sInyokaUrl + "/" + m_sSitename + "/a/export/meta/");
  qDebug() << "DOWNLOADING meta data:" << sUrl;
  QNetworkRequest request(sUrl);
  request.setOriginatingObject(this);
  m_urlRedirectedTo = sUrl;
  QNetworkReply *reply = m_pSession->getNwManager()->get(request);
  m_listDownloadReplies.append(reply);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Download::replyFinished(QNetworkReply *pReply) {
  if (this != pReply->request().originatingObject()) {
    // Handle only requests from Download class
    return;
  }

#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  QIODevice *pData(pReply);
  // Check for redirection
  QVariant possibleRedirectUrl =
      pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  m_urlRedirectedTo =
      this->redirectUrl(possibleRedirectUrl.toUrl(), m_urlRedirectedTo);

  if (QNetworkReply::NoError != pReply->error()) {
    QMessageBox::critical(m_pParent, qApp->applicationName(),
                          pData->errorString());
    qCritical() << "Error (#" << pReply->error()
                << ") while NW reply:" << pData->errorString();
    // qDebug() << "Reply content:" << pReply->readAll();
    return;
  }

  // No error
  QUrl url;
  // qDebug() << "Downloading URL: " << pReply->url().toString();

  // If the URL is not empty, we're being redirected
  if (!m_urlRedirectedTo.isEmpty()) {
    url = m_urlRedirectedTo;
    qDebug() << "Redirected to: " + url.toString();
    this->downloadArticle(url.toString() + "a/export/raw/" + m_sRevision);
  } else {
    m_urlRedirectedTo.clear();
    QString sTmpArticle = QString::fromUtf8(pData->readAll());

    if (m_bDownloadArticle) {
      // Replace windows specific newlines
      sTmpArticle.replace(QLatin1String("\r\r\n"), QLatin1String("\n"));

      // Site does not exist etc.
      if (sTmpArticle.isEmpty()) {
        QMessageBox::information(m_pParent, qApp->applicationName(),
                                 tr("Could not download the article."));
        return;
      }

      m_sArticleText = sTmpArticle;
      downloadImages();
      // --------------------------------------------------------------------
    } else {
      // Download article images metadata
      QStringList sListTmp;
      QStringList sListMetadata;
      QStringList sListSaveFolder;

      // Site does not exist etc.
      if (sTmpArticle.isEmpty()) {
        QMessageBox::information(m_pParent, qApp->applicationName(),
                                 tr("Could not find meta data."));
        return;
      }

      // Copy metadata line by line in list
      sListTmp << sTmpArticle.split(QStringLiteral("\n"));
      // qDebug() << "META files:" << sListTmp;

      // Get only attachments article metadata
      for (int i = 0; i < sListTmp.size(); i++) {
        if (sListTmp[i].startsWith("X-Attach: " + m_sSitename + "/",
                                   Qt::CaseInsensitive)) {
          // Remove "X-Attach: "
          sListMetadata << sListTmp[i].remove(QStringLiteral("X-Attach: "));
          // Remove windows specific newline \r
          sListMetadata.last().remove(QStringLiteral("\r"));
          sListMetadata.last() =
              m_sInyokaUrl + "/_image?target=" + sListMetadata.last();
          sListSaveFolder << m_sImgDir;

          // qDebug() << sListMetadata.last();
        }
      }

      // If attachments exist
      if (!sListMetadata.isEmpty()) {
        int iRet = 0;
        // Ask if images should be downloaded,
        // if not enabled by default in settings
        if (!m_bAutomaticImageDownload) {
          iRet = QMessageBox::question(m_pParent, qApp->applicationName(),
                                       tr("Do you want to download "
                                          "the images which are "
                                          "attached to the article?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        } else {
          iRet = QMessageBox::Yes;
        }

        if (iRet != QMessageBox::No) {
          qDebug() << "Starting image download...";
          m_DlImages->setDLs(sListMetadata, sListSaveFolder);
          QTimer::singleShot(0, m_DlImages, &DownloadImg::startDownloads);
        } else {
          this->showArticle();
        }
      } else {
        this->showArticle();
      }
    }
  }
  m_listDownloadReplies.removeAll(pReply);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Download::redirectUrl(const QUrl &possibleRedirectUrl,
                           const QUrl &oldRedirectUrl) -> QUrl {
  QUrl redirectUrl;
  if (!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl) {
    redirectUrl = possibleRedirectUrl;
    m_sSitename = redirectUrl.toString().mid(m_sInyokaUrl.size() + 1);
    if (m_sSitename.startsWith('/')) {
      m_sSitename.remove(0, 1);
    }
    if (m_sSitename.endsWith('/')) {
      m_sSitename.remove(m_sSitename.length() - 1, 1);
    }
    qDebug() << "Set new sitename:" << m_sSitename;
  } else {
    m_urlRedirectedTo.clear();
  }
  return redirectUrl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Download::showArticle() {
  emit sendArticleText(m_sArticleText, m_sSitename);
}
