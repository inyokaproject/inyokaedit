/**
 * \file downloadimg.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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
 * Class definition for download manager for images.
 */

#ifndef APPLICATION_DOWNLOADIMG_H_
#define APPLICATION_DOWNLOADIMG_H_

#include <QNetworkReply>
#include <QProgressDialog>
#include <QStringList>

class DownloadImg : public QObject {
  Q_OBJECT

 public:
    explicit DownloadImg(QObject *pParent, QNetworkAccessManager *pNwManager);
    void setDLs(const QStringList &sListUrls,
                const QStringList &sListSavePath);

 public slots:
    void startDownloads();

 private slots:
    void downloadFinished(QNetworkReply *reply);
    void cancelDownloads();

 signals:
    void finsihedImageDownload();

 private:
    void doDownload(const QUrl &url,
                    const QString &sSavePath,
                    const QString &sBase = "");
    QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                     const QUrl& oldRedirectUrl) const;

    QNetworkAccessManager* m_pNwManager;
    QList<QNetworkReply *> m_listDownloadReplies;

    QProgressDialog *m_pProgessDialog;
    quint16 m_nProgress;
    QString m_sDownloadError;

    QUrl m_urlRedirectedTo;
    QStringList m_sListUrls;
    QStringList m_sListSavePath;

    QStringList m_sListRepliesPath;
    QStringList m_sListBasename;
};

#endif  // APPLICATION_DOWNLOADIMG_H_
