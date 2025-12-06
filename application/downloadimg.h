// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_DOWNLOADIMG_H_
#define APPLICATION_DOWNLOADIMG_H_

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class QProgressDialog;

class DownloadImg : public QObject {
  Q_OBJECT

 public:
  explicit DownloadImg(QNetworkAccessManager *pNwManager,
                       QObject *pObj = nullptr);
  void setDLs(const QStringList &sListUrls, const QString &sSavePath);

 public slots:
  void startDownloads();

 private slots:
  void downloadFinished(QNetworkReply *reply);
  void cancelDownloads();

 signals:
  void finsihedImageDownload();

 private:
  void doDownload(const QUrl &url, const QString &sBase);
  static auto redirectUrl(const QUrl &possibleRedirectUrl,
                          const QUrl &oldRedirectUrl) -> QUrl;

  QNetworkAccessManager *m_pNwManager;
  QHash<QNetworkReply *, QString> m_listDownloadReplies;

  QProgressDialog *m_pProgessDialog;
  quint16 m_nProgress;
  QString m_sDownloadError;

  QUrl m_urlRedirectedTo;
  QStringList m_sListUrls;
  QString m_sSavePath;
};

#endif  // APPLICATION_DOWNLOADIMG_H_
