// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_SESSION_H_
#define APPLICATION_SESSION_H_

#include <QNetworkCookie>
#include <QNetworkCookieJar>

class QNetworkAccessManager;
class QNetworkReply;

class Session : public QNetworkCookieJar {
  Q_OBJECT

 public:
  explicit Session(QWidget *pParent, const QString &sLoginUrl,
                   const QString &sInyokaUrl, const QString &sCookieDomain,
                   QObject *pObj = nullptr);

  void checkSession();
  auto isLoggedIn() const -> bool;
  auto getNwManager() -> QNetworkAccessManager *;

 public slots:
  void updateSettings(const QString &sUsername, const QString &sPassword);

 private:
  void requestToken();
  void getTokenReply(const QString &sNWReply);
  void requestLogin();
  void getLoginReply(const QString &sNWReply);
  void replyFinished(QNetworkReply *pReply);

  enum REQUESTSTATE { REQUTOKEN, RECTOKEN, REQULOGIN, RECLOGIN };

  QWidget *m_pParent;
  QString m_sInyokaUrl;
  QString m_sInyokaLoginUrl;
  QString m_sUsername;
  QString m_sPassword;
  QNetworkAccessManager *m_pNwManager;

  REQUESTSTATE m_State;
  QString m_sToken;
  QString m_sCookieDomain;
  QList<QNetworkCookie> m_ListCookies;
};

#endif  // APPLICATION_SESSION_H_
