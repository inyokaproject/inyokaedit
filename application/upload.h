/**
 * \file upload.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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
 * Class definition for upload functions.
 */

#ifndef APPLICATION_UPLOAD_H_
#define APPLICATION_UPLOAD_H_

#include <QNetworkReply>
#include <QTextEdit>

#include "./session.h"

/**
 * \class Upload
 * \brief Upload an article to Inyoka
 */
class Upload : public QObject {
  Q_OBJECT

 public:
    explicit Upload(QWidget *pParent, Session *pSession,
                    const QString &sInyokaUrl, const QString &sConstArea);

    void setEditor(QTextEdit *pEditor, const QString &sArticlename);

 public slots:
    void clickUploadArticle();

 private:
    void requestRevision(QString sUrl = "");
    void getRevisionReply(const QString &sNWReply);
    QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                     const QUrl& oldRedirectUrl);
    void requestUpload();
    void getUploadReply(const QString &sNWReply);
    void replyFinished(QNetworkReply *pReply);

    enum REQUESTSTATE {REQUREVISION, RECREVISION, REQUPLOAD, RECUPLOAD};

    QWidget *m_pParent;
    Session *m_pSession;
    QString m_sInyokaUrl;
    QNetworkReply *m_pReply;

    REQUESTSTATE m_State;
    QString m_sSitename;
    QUrl m_urlRedirectedTo;
    QString m_sRevision;
    QString m_sConstructionArea;
    QTextEdit *m_pEditor;
    QString m_sArticlename;
};

#endif  // APPLICATION_UPLOAD_H_
