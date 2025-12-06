// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./downloadimg.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>

DownloadImg::DownloadImg(QNetworkAccessManager *pNwManager, QObject *pObj)
    : m_pNwManager(pNwManager), m_pProgessDialog(nullptr), m_nProgress(0) {
  Q_UNUSED(pObj)
  connect(m_pNwManager, &QNetworkAccessManager::finished, this,
          &DownloadImg::downloadFinished);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::setDLs(const QStringList &sListUrls,
                         const QString &sSavePath) {
  m_sListUrls = sListUrls;
  m_sSavePath = sSavePath;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::startDownloads() {
  const int MINDURATION = 100;
  m_nProgress = 0;
  m_sDownloadError.clear();

  // Create progress dialog
  m_pProgessDialog =
      new QProgressDialog(tr("Downloading images..."), tr("Cancel"),
                          m_nProgress, m_sListUrls.size(), nullptr,
                          Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
  m_pProgessDialog->setWindowTitle(qApp->applicationName());
  m_pProgessDialog->setMinimumDuration(MINDURATION);
  m_pProgessDialog->setModal(true);
  m_pProgessDialog->setMaximumSize(m_pProgessDialog->size());
  m_pProgessDialog->setMinimumSize(m_pProgessDialog->size());
  connect(m_pProgessDialog, &QProgressDialog::canceled, this,
          &DownloadImg::cancelDownloads);

  if (!m_sListUrls.isEmpty()) {
    for (int i = 0; i < m_sListUrls.size(); i++) {
      QUrl url = QUrl::fromEncoded(m_sListUrls[i].toLocal8Bit());
      this->doDownload(url,
                       url.toString().mid(
                           url.toString().lastIndexOf(QLatin1String("/")) + 1));
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::doDownload(const QUrl &url, const QString &sBase) {
  QNetworkRequest request(url);
  qDebug() << "Image DL request:" << url.toString();
  request.setOriginatingObject(this);
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                       QNetworkRequest::NoLessSafeRedirectPolicy);
  QNetworkReply *reply = m_pNwManager->get(request);

  m_listDownloadReplies.insert(reply, sBase);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::downloadFinished(QNetworkReply *pReply) {
  if (this != pReply->request().originatingObject()) {
    // Handle only requests from Download class
    return;
  }

  QIODevice *pData(pReply);
  m_pProgessDialog->setValue(m_nProgress);

  // Check for redirection
  QVariant possibleRedirectUrl =
      pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  m_urlRedirectedTo =
      DownloadImg::redirectUrl(possibleRedirectUrl.toUrl(), m_urlRedirectedTo);

  // Error
  if (QNetworkReply::NoError != pReply->error()) {
    if (QNetworkReply::OperationCanceledError != pReply->error()) {
      m_sDownloadError += pData->errorString() + "\n\n";
    }
    qWarning() << "Image download error (#" << pReply->error()
               << "): " + pData->errorString();

    m_pProgessDialog->setValue(m_nProgress);
    m_nProgress++;
  } else {
    // No error
    QUrl url = pReply->url();
    qDebug() << "Downloading URL:" << url.toString();

    // If the URL is not empty, we're being redirected
    if (!m_urlRedirectedTo.isEmpty()) {
      url = m_urlRedirectedTo;
      qDebug() << "Redirected to: " + url.toString();

      // New request with redirected url
      this->doDownload(url, m_listDownloadReplies.value(pReply));
    } else {
      m_urlRedirectedTo.clear();
      m_pProgessDialog->setValue(m_nProgress);
      m_nProgress++;

      // Save file
      QFile file(m_sSavePath + "/" + m_listDownloadReplies.value(pReply));
      if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open " + file.fileName() +
                          " for writing: " + file.errorString();
      } else {
        file.write(pData->readAll());
        file.close();

        qDebug() << "Download of " + m_listDownloadReplies.value(pReply) +
                        " succeeded - saved to " + m_sSavePath;
      }
    }
  }

  m_listDownloadReplies.remove(pReply);
  pReply->deleteLater();

  // All downloads finished
  if (m_listDownloadReplies.isEmpty()) {
    m_pProgessDialog->setValue(m_pProgessDialog->maximum());
    m_pProgessDialog->deleteLater();

    qDebug() << "All downloads finished...";

    // Show error messages
    if (!m_sDownloadError.isEmpty()) {
      QMessageBox::warning(nullptr, tr("Download error"), m_sDownloadError);
    }

    emit finsihedImageDownload();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto DownloadImg::redirectUrl(const QUrl &possibleRedirectUrl,
                              const QUrl &oldRedirectUrl) -> QUrl {
  QUrl redirectUrl;
  if (!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl) {
    redirectUrl = possibleRedirectUrl;
  }
  return redirectUrl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DownloadImg::cancelDownloads() {
  for (auto i = m_listDownloadReplies.cbegin(),
            end = m_listDownloadReplies.cend();
       i != end; ++i) {
    i.key()->abort();
  }
  m_listDownloadReplies.clear();

  qDebug() << "Canceled downloads...";
  emit finsihedImageDownload();
}
