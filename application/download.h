// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_DOWNLOAD_H_
#define APPLICATION_DOWNLOAD_H_

#include <QObject>
#include <QString>
#include <QUrl>

class QNetworkReply;
class QWidget;

class DownloadImg;
class Session;

class Download : public QObject {
  Q_OBJECT

 public:
  Download(QWidget *pParent, Session *pSession, const QString &sStylesDir,
           const QString &sImgDir, const QString &sSharePath,
           const QString &sInyokaUrl, const QString &sConstArea,
           QObject *pObj = nullptr);

 public slots:
  void downloadArticle(QString sUrl = QLatin1String(""));
  void showArticle();
  void updateSettings(const bool bDownloadImages);

 private slots:
  void replyFinished(QNetworkReply *pReply);

 signals:
  void sendArticleText(const QString &, const QString &);

 private:
  auto redirectUrl(const QUrl &possibleRedirectUrl, const QUrl &oldRedirectUrl)
      -> QUrl;
  void downloadImages();

  QWidget *m_pParent;
  Session *m_pSession;
  const QString m_sStylesDir;
  const QString m_sImgDir;

  QList<QNetworkReply *> m_listDownloadReplies;
  QUrl m_urlRedirectedTo;
  QString m_sArticleText;
  QString m_sSitename;
  QString m_sRevision;
  QString m_sSource;
  QString m_sInyokaUrl;
  QString m_sConstructionArea;
  bool m_bAutomaticImageDownload;
  const QString m_sSharePath;

  DownloadImg *m_DlImages;
  bool m_bDownloadArticle;
};

#endif  // APPLICATION_DOWNLOAD_H_
