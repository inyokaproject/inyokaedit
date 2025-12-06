// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./session.h"

#include <QApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

Session::Session(QWidget *pParent, const QString &sLoginUrl,
                 const QString &sInyokaUrl, const QString &sCookieDomain,
                 QObject *pObj)
    : m_pParent(pParent),
      m_sInyokaUrl(sInyokaUrl),
      m_sInyokaLoginUrl(sLoginUrl),
      m_State(REQUTOKEN),
      m_sToken(QLatin1String("")),
      m_sCookieDomain(sCookieDomain) {
  Q_UNUSED(pObj)
  m_pNwManager = new QNetworkAccessManager(m_pParent);
  m_pNwManager->setCookieJar(this);
  this->setParent(m_pParent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::updateSettings(const QString &sUsername,
                             const QString &sPassword) {
  if (m_sUsername != sUsername || m_sPassword != sPassword) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_State = REQUTOKEN;
    for (auto &cookie : this->allCookies()) {
      m_pNwManager->cookieJar()->deleteCookie(cookie);
    }
    m_ListCookies.clear();
    m_sToken.clear();

    m_sUsername = sUsername;
    m_sPassword = sPassword;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::checkSession() {
  switch (m_State) {
    case REQUTOKEN:
      this->requestToken();
      break;
    case RECTOKEN:
    case REQULOGIN:
      this->requestLogin();
      break;
    case RECLOGIN:
      break;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::requestToken() {
  qDebug() << "Calling" << Q_FUNC_INFO;
#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  QNetworkRequest request(m_sInyokaLoginUrl);
  if (!m_sInyokaLoginUrl.endsWith('/')) {
    request.setUrl(m_sInyokaLoginUrl + "/");
  }
  request.setRawHeader("User-Agent", QString(qApp->applicationName() + "/" +
                                             qApp->applicationVersion())
                                         .toLatin1());
  request.setAttribute(QNetworkRequest::User, QVariant("ReqestToken"));

  m_State = REQUTOKEN;
  QNetworkReply *pReply = m_pNwManager->get(request);
  QEventLoop loop;
  connect(m_pNwManager, &QNetworkAccessManager::finished, &loop,
          &QEventLoop::quit);
  loop.exec();
  this->replyFinished(pReply);
}

// ----------------------------------------------------------------------------

void Session::getTokenReply(const QString &sNWReply) {
  qDebug() << "Calling" << Q_FUNC_INFO;

  QString sSessionCookie(QLatin1String(""));

  if (!m_ListCookies.isEmpty()) {
    // qDebug() << "COOKIES:" << m_ListCookies;

    for (const auto &cookie : std::as_const(m_ListCookies)) {
      if (cookie.isSessionCookie() && cookie.isSecure() &&
          m_sCookieDomain == cookie.domain()) {
        sSessionCookie = QString::fromLatin1(cookie.toRawForm());
      } else if (!cookie.isSessionCookie() && "csrftoken" == cookie.name()) {
        m_sToken = cookie.value();
      }
    }

    if (m_sToken.isEmpty()) {
      qWarning() << "Token request failed. No CSRFTOKEN received.";
      qWarning() << "COOKIES" << m_ListCookies;
      QMessageBox::warning(m_pParent, tr("Error"),
                           tr("Login failed! No CSRFTOKEN received."));
      m_State = REQUTOKEN;
      return;
    }
    if (sSessionCookie.isEmpty()) {
      qWarning() << "No session cookie received.";
      qWarning() << "COOKIES" << m_ListCookies;
      QMessageBox::warning(m_pParent, tr("Error"),
                           tr("Login failed! No session cookie received."));
      m_State = REQUTOKEN;
      return;
    }

    // SUCCESS
    // qDebug() << "CSRFTOKEN:" << m_sToken;
    // qDebug() << "SESSION COOKIE:" << sSessionCookie;
    m_State = RECTOKEN;
    this->requestLogin();
  } else {
    qWarning() << "Calling login page failed! No cookies received.";
    qWarning() << "NW REPLY:" << sNWReply;
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Login failed! No cookies received."));
    m_State = REQUTOKEN;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::requestLogin() {
  qDebug() << "Calling" << Q_FUNC_INFO;
  m_State = REQULOGIN;

  bool bOk = false;
  QString sUsername(QLatin1String(""));
  QString sPassword(QLatin1String(""));

  if (m_sUsername.isEmpty()) {
    sUsername =
        QInputDialog::getText(m_pParent, tr("Login user"),
                              tr("Please insert your Inyoka user name:"),
                              QLineEdit::Normal, QLatin1String(""), &bOk)
            .trimmed();
    if (!bOk || sUsername.isEmpty()) {
      return;
    }
  } else {
    sUsername = m_sUsername;
  }

  if (m_sPassword.isEmpty()) {
    sPassword =
        QInputDialog::getText(m_pParent, tr("Login password"),
                              tr("Please insert your Inyoka password:"),
                              QLineEdit::Password, QLatin1String(""), &bOk)
            .trimmed();
    if (!bOk || sPassword.isEmpty()) {
      return;
    }
  } else {
    sPassword = m_sPassword;
  }

  QString sUrl(m_sInyokaLoginUrl);
  sUrl = sUrl + "/?next=" + m_sInyokaUrl;

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
  QNetworkRequest request(sUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  // Referer needed with POST request + https in Django
  QString sReferer(m_sInyokaUrl);
  sReferer = sReferer.remove(QStringLiteral("wiki."));
  request.setRawHeader("Referer", sReferer.toLatin1());
  request.setRawHeader("User-Agent", QString(qApp->applicationName() + "/" +
                                             qApp->applicationVersion())
                                         .toLatin1());
  request.setAttribute(QNetworkRequest::User, QVariant("ReqestLogin"));
  m_State = REQULOGIN;

  QUrlQuery params;
  params.addQueryItem(QStringLiteral("csrfmiddlewaretoken"), m_sToken);
  params.addQueryItem(QStringLiteral("username"), sUsername);
  sPassword.replace(QChar('+'), QStringLiteral("%2B"));
  params.addQueryItem(QStringLiteral("password"), sPassword);
  params.addQueryItem(QStringLiteral("redirect"), QLatin1String(""));

  QNetworkReply *pReply =
      m_pNwManager->post(request, params.query(QUrl::FullyEncoded).toUtf8());
  QEventLoop loop;
  connect(m_pNwManager, &QNetworkAccessManager::finished, &loop,
          &QEventLoop::quit);
  loop.exec();
  this->replyFinished(pReply);
}

// ----------------------------------------------------------------------------

void Session::getLoginReply(const QString &sNWReply) {
  // If "$IS_LOGGED_IN = false" is found in reply --> login failed
  if (sNWReply.contains(QLatin1String("$IS_LOGGED_IN = false"))) {
    m_State = REQUTOKEN;
    qWarning() << "LOGIN FAILED! Wrong credentials?";
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Login at Inyoka failed. Wrong credentials?"));
    return;
  }

  for (const auto &cookie : std::as_const(m_ListCookies)) {
    // qDebug() << "RawSessionCookie:" << cookie.toRawForm();
    if (cookie.isSessionCookie() && cookie.isSecure() &&
        m_sCookieDomain == cookie.domain()) {
      m_State = RECLOGIN;
      qDebug() << "LOGIN SUCCESSFUL!";
      break;
    }
  }

  if (RECLOGIN != m_State) {
    m_State = REQUTOKEN;
    qWarning() << "LOGIN FAILED! No success message cookie.";
    QMessageBox::warning(m_pParent, tr("Error"), tr("Login at Inyoka failed."));
    return;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::replyFinished(QNetworkReply *pReply) {
#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  QIODevice *pData(pReply);

  if (QNetworkReply::NoError != pReply->error()) {
    QMessageBox::critical(m_pParent, QStringLiteral("Error"),
                          pData->errorString());
    qCritical() << "Error (#" << pReply->error()
                << ") while NW reply:" << pData->errorString();
    qDebug() << "Reply content:" << pReply->readAll();
    pReply->close();
    pReply->deleteLater();
    return;
  }

  m_ListCookies = this->allCookies();
  QString sReply = QString::fromUtf8(pData->readAll());
  sReply.replace(QLatin1String("\r\r\n"), QLatin1String("\n"));

  if (sReply.isEmpty()) {
    qDebug() << "Login NW reply is empty.";
  }

  QString sAttribute(
      pReply->request().attribute(QNetworkRequest::User).toString());
  if (sAttribute.contains(QLatin1String("ReqestToken"))) {
    this->getTokenReply(sReply);
  } else if (sAttribute.contains(QLatin1String("ReqestLogin"))) {
    this->getLoginReply(sReply);
  } else {
    qWarning() << "Ran into unexpected state!";
    qWarning() << "REPLY:" << sReply;
  }

  pReply->close();
  pReply->deleteLater();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Session::isLoggedIn() const -> bool { return (RECLOGIN == m_State); }

// ----------------------------------------------------------------------------

auto Session::getNwManager() -> QNetworkAccessManager * { return m_pNwManager; }
