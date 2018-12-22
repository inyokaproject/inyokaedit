/**
 * \file session.cpp
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
 * Login and session handling
 */

#include "./session.h"

#include <QApplication>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

Session::Session(QWidget *pParent, const QString &sInyokaUrl,
                 const QString &sHash)
  : m_pParent(pParent),
    m_sInyokaUrl(sInyokaUrl),
    m_State(REQUTOKEN),
    m_sToken(""),
    m_sHash(sHash) {
  m_pNwManager = new QNetworkAccessManager(m_pParent);
  m_pNwManager->setCookieJar(this);
  this->setParent(m_pParent);

  if (m_sHash.isEmpty()) {
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Inyoka community hash not defined!"));
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
      this->requestLogin();
      break;
    case REQULOGIN:
      this->requestLogin();
      break;
    case RECLOGIN:
      break;
    default:
      qWarning() << "Ran into unexpected state at session check:" << m_State;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::requestToken() {
  qDebug() << "Calling" << Q_FUNC_INFO;
#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  QString sLoginUrl(m_sInyokaUrl);
  sLoginUrl = sLoginUrl.remove("wiki.") + "/login/";
  QNetworkRequest request(sLoginUrl);
  request.setRawHeader("User-Agent",
                       QString(qApp->applicationName() + "/"
                               + qApp->applicationVersion()).toLatin1());
  request.setAttribute(QNetworkRequest::User, QVariant("ReqestToken"));

  m_State = REQUTOKEN;
  QNetworkReply *pReply = m_pNwManager->get(request);
  QEventLoop loop;
  connect(m_pNwManager, &QNetworkAccessManager::finished,
          &loop, &QEventLoop::quit);
  loop.exec();
  this->replyFinished(pReply);
}

// ----------------------------------------------------------------------------

void Session::getTokenReply(const QString &sNWReply) {
  qDebug() << "Calling" << Q_FUNC_INFO;

  QString sSessionCookie("");

  if (m_ListCookies.size() > 0) {
    // qDebug() << "COOKIES:" << m_ListCookies;

    QString sCookie("");
    foreach (QNetworkCookie cookie, m_ListCookies) {
      if (cookie.isSessionCookie()) {
        sSessionCookie = cookie.toRawForm();
        m_SessionCookie = cookie;
      } else if (sCookie.isEmpty()) {  // Use first found cookie
        sCookie = cookie.toRawForm();
      }
    }

    m_sToken = "csrftoken=";
    int nInd = sCookie.indexOf(m_sToken) + m_sToken.length();
    m_sToken = sCookie.mid(nInd, sCookie.indexOf(';', nInd) - nInd);

    if (m_sToken.isEmpty()) {
      qWarning() << "Token request failed. No CSRFTOKEN received.";
      qWarning() << "COOKIES" << m_ListCookies;
      QMessageBox::warning(m_pParent, tr("Error"),
                           tr("Upload failed! No CSRFTOKEN received."));
      m_State = REQUTOKEN;
      return;
    } else if (sSessionCookie.isEmpty()) {
      qWarning() << "No session cookie received.";
      qWarning() << "COOKIES" << m_ListCookies;
      QMessageBox::warning(
            m_pParent, tr("Error"),
            tr("Upload failed! No session cookie received."));
      m_State = REQUTOKEN;
      return;
    } else {  // SUCCESS
      // qDebug() << "CSRFTOKEN:" << m_sToken;
      // qDebug() << "SESSION COOKIE:" << sSessionCookie;
      m_State = RECTOKEN;
      this->requestLogin();
    }
  } else {
    qWarning() << "Calling login page failed! No cookies received.";
    qWarning() << "NW REPLY:" << sNWReply;
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Upload failed! No cookies received."));
    m_State = REQUTOKEN;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Session::requestLogin() {
  qDebug() << "Calling" << Q_FUNC_INFO;
  m_State = REQULOGIN;

  QString sUrl(m_sInyokaUrl);
  bool bOk = false;
  QString sUsername("");
  QString sPassword("");

  sUsername = QInputDialog::getText(
                m_pParent, tr("Login user"),
                tr("Please insert your Inyoka user name:"),
                QLineEdit::Normal, "", &bOk).trimmed();
  if (true != bOk || sUsername.isEmpty()) {
    return;
  }

  sPassword = QInputDialog::getText(
                m_pParent, tr("Login password"),
                tr("Please insert your Inyoka password:"),
                QLineEdit::Password, "", &bOk).trimmed();
  if (true != bOk || sPassword.isEmpty()) {
    return;
  }

  sUrl = sUrl.remove("wiki.") + "/login/?next=" + m_sInyokaUrl;

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
  QNetworkRequest request(sUrl);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  // Referer needed with POST request + https in Django
  QString sReferer(m_sInyokaUrl);
  sReferer = sReferer.remove("wiki.");
  request.setRawHeader("Referer", sReferer.toLatin1());
  request.setRawHeader("User-Agent",
                       QString(qApp->applicationName() + "/"
                               + qApp->applicationVersion()).toLatin1());
  request.setAttribute(QNetworkRequest::User, QVariant("ReqestLogin"));
  m_State = REQULOGIN;

  QUrlQuery params;
  params.addQueryItem("csrfmiddlewaretoken", m_sToken);
  params.addQueryItem("username", sUsername);
  sPassword.replace(QChar('+'), QString("%2B"));
  params.addQueryItem("password", sPassword);
  params.addQueryItem("redirect", "");

  QNetworkReply *pReply = m_pNwManager->post(
                            request, params.query(QUrl::FullyEncoded).toUtf8());
  QEventLoop loop;
  connect(m_pNwManager, &QNetworkAccessManager::finished,
          &loop, &QEventLoop::quit);
  loop.exec();
  this->replyFinished(pReply);
}

// ----------------------------------------------------------------------------

void Session::getLoginReply(const QString &sNWReply) {
  // If "$IS_LOGGED_IN = false" is found in reply --> login failed
  if (-1 != sNWReply.indexOf("$IS_LOGGED_IN = false")) {
    m_State = REQUTOKEN;
    qWarning() << "LOGIN FAILED! Wrong credentials?";
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Login at Inyoka failed. Wrong credentials?"));
    return;
  } else {
    foreach (QNetworkCookie cookie, m_ListCookies) {
      if (cookie.isSessionCookie()) {
        // E.g. uu includes message "153cae855e0ae527d6dc2434f3eb8ef60b782570"
        // --> "Du hast dich erfolgreich angemeldet"
        // See raw debug output:
        // qDebug() << "RawSessionCookie:" << cookie.toRawForm();
        if (cookie.toRawForm().contains(m_sHash.toLatin1())) {
          m_State = RECLOGIN;
          qDebug() << "LOGIN SUCCESSFUL!";
          break;
        }
      }
    }

    if (RECLOGIN != m_State) {
      m_State = REQUTOKEN;
      qWarning() << "LOGIN FAILED! No success message cookie.";
      QMessageBox::warning(m_pParent, tr("Error"),
                           tr("Login at Inyoka failed."));
      return;
    }
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
    QMessageBox::critical(m_pParent, "Error", pData->errorString());
    qCritical() << "Error (#" << pReply->error() << ") while NW reply:"
                << pData->errorString();
    qDebug() << "Reply content:" << pReply->readAll();
    pReply->close();
    pReply->deleteLater();
    return;
  } else {
    m_ListCookies = this->allCookies();
    QString sReply = QString::fromUtf8(pData->readAll());
    sReply.replace("\r\r\n", "\n");

    if (sReply.isEmpty()) {
      qDebug() << "Upload NW reply is empty.";
    }

    QString sAttribute(pReply->request().attribute(
                         QNetworkRequest::User).toString());
    if (sAttribute.contains("ReqestToken")) {
      this->getTokenReply(sReply);
    } else if (sAttribute.contains("ReqestLogin")) {
      this->getLoginReply(sReply);
    } else {
        qWarning() << "Ran into unexpected state!";
        qWarning() << "REPLY:" << sReply;
    }
  }

  pReply->close();
  pReply->deleteLater();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool Session::isLoggedIn() const {
  if (RECLOGIN == m_State) {
    return true;
  }
  return false;
}

// ----------------------------------------------------------------------------

QNetworkAccessManager* Session::getNwManager() {
  return m_pNwManager;
}
