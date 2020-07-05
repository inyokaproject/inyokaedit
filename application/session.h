/**
 * \file session.h
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Class definition for seesion functions.
 */

#ifndef APPLICATION_SESSION_H_
#define APPLICATION_SESSION_H_

#include <QNetworkCookie>
#include <QNetworkCookieJar>

class QNetworkAccessManager;
class QNetworkReply;

/**
 * \class Session
 * \brief Login/session handling
 */
class Session : public QNetworkCookieJar {
  Q_OBJECT

 public:
    explicit Session(QWidget *pParent, const QString &sInyokaUrl,
                     const QString &sHash);

    void checkSession();
    auto isLoggedIn() const -> bool;
    auto getNwManager() -> QNetworkAccessManager*;

 private:
    void requestToken();
    void getTokenReply(const QString &sNWReply);
    void requestLogin();
    void getLoginReply(const QString &sNWReply);
    void replyFinished(QNetworkReply *pReply);

    enum REQUESTSTATE {REQUTOKEN, RECTOKEN, REQULOGIN, RECLOGIN};

    QWidget *m_pParent;
    QString m_sInyokaUrl;
    QNetworkAccessManager *m_pNwManager;

    REQUESTSTATE m_State;
    QString m_sToken;
    QString m_sHash;
    QNetworkCookie m_SessionCookie;
    QList<QNetworkCookie> m_ListCookies;
};

#endif  // APPLICATION_SESSION_H_
