/**
 * \file download.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Class definition for download functions.
 */

#ifndef INYOKAEDIT_DOWNLOAD_H_
#define INYOKAEDIT_DOWNLOAD_H_

#include <QNetworkReply>

#include "./inyokaedit.h"
#include "./downloadimg.h"

/**
 * \class Download
 * \brief Download functions (articles, article images)
 */
class Download : public QObject {
  Q_OBJECT

 public:
  Download(QWidget *pParent, const QString &sStylesDir,
           const QString &sImgDir, const QString &sSharePath);

 public slots:
  void downloadArticle(QString sUrl = "");
  void showArticle();
  void updateSettings(const bool bCompleter,
                      const QString &sInyokaUrl,
                      const QString &sConstArea);

 private slots:
  void replyFinished(QNetworkReply *pReply);

 signals:
  void sendArticleText(const QString &, const QString &);

 private:
  QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                   const QUrl& oldRedirectUrl);
  void downloadImages();

  QWidget *m_pParent;
  const QString m_sStylesDir;
  const QString m_sImgDir;

  QNetworkAccessManager *m_pNwManager;
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

#endif  // INYOKAEDIT_DOWNLOAD_H_
