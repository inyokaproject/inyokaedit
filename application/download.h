/**
 * \file download.h
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
 * Class definition for download functions.
 */

#ifndef APPLICATION_DOWNLOAD_H_
#define APPLICATION_DOWNLOAD_H_

#include <QObject>
#include <QString>
#include <QUrl>

class QNetworkReply;
class QWidget;

class DownloadImg;
class Session;

/**
 * \class Download
 * \brief Download functions (articles, article images)
 */
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
