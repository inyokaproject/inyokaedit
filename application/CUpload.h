/**
 * \file CUpload.h
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
 * Class definition for upload functions.
 */

#ifndef INYOKAEDIT_CUPLOAD_H_
#define INYOKAEDIT_CUPLOAD_H_

#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QTextEdit>

/**
 * \class CUpload
 * \brief Upload an article to Inyoka
 */
class CUpload : public QNetworkCookieJar {
    Q_OBJECT

  public:
    explicit CUpload(QWidget *pParent, const QString &sInyokaUrl);

    void setEditor(QTextEdit *pEditor, const QString &sArticlename);

  public slots:
    void clickUploadArticle();

  private slots:
    void replyFinished(QNetworkReply *pReply);

  private:
    void requestToken();
    void getTokenReply(QString sNWReply);
    void requestLogin();
    void getLoginReply(QString sNWReply);
    void requestRevision(QString sUrl = "");
    void getRevisionReply(QString sNWReply);
    QUrl redirectUrl(const QUrl& possibleRedirectUrl,
                     const QUrl& oldRedirectUrl);
    void requestUpload();
    void getUploadReply(QString sNWReply);

    enum REQUESTSTATE {REQUTOKEN, RECTOKEN, REQULOGIN, RECLOGIN,
                       REQUREVISION, RECREVISION, REQUPLOAD, RECUPLOAD};

    QWidget *m_pParent;    /**< Pointer to parent window */
    QString m_sInyokaUrl;
    QNetworkAccessManager *m_pNwManager;
    QNetworkReply *m_pReply;

    REQUESTSTATE m_State;
    QString m_sToken;
    QNetworkCookie m_SessionCookie;
    QList<QNetworkCookie> m_ListCookies;
    QString m_sSitename;
    QUrl m_urlRedirectedTo;
    QString m_sRevision;
    QString m_sConstructionArea;
    QTextEdit *m_pEditor;
    QString m_sArticlename;
};

#endif  // INYOKAEDIT_CUPLOAD_H_
