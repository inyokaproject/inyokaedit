/**
 * \file CUpload.cpp
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
 * Upload raw text of an article to Inyoka.
 */

#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QInputDialog>
#include <QMessageBox>
#include <QUrl>

#if QT_VERSION >= 0x050000
#include <QUrlQuery>
#endif

#include "./CUpload.h"
#include "./CUtils.h"

CUpload::CUpload(QWidget *pParent, const QString &sInyokaUrl)
    : m_pParent(pParent),
      m_sInyokaUrl(sInyokaUrl),
      m_State(REQUTOKEN),
      m_sToken(""),
      m_sSitename(""),
      m_sRevision(""),
      m_sConstructionArea(trUtf8("ConstructionArea")),
      m_pEditor(NULL),
      m_sArticlename("") {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pNwManager = new QNetworkAccessManager(m_pParent);
    connect(m_pNwManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    m_pNwManager->setCookieJar(this);
    this->setParent(m_pParent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::setEditor(QTextEdit *pEditor, const QString &sArticlename) {
    m_pEditor = pEditor;
    m_sArticlename = trUtf8("Untitled");
    if (!sArticlename.isEmpty() && !sArticlename.contains(m_sArticlename)) {
        m_sArticlename = sArticlename;
    }
    m_sArticlename.remove(m_sConstructionArea + "/");
    QFileInfo fi(m_sArticlename);
    m_sArticlename = fi.baseName();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::clickUploadArticle() {
    qDebug() << "Calling" << Q_FUNC_INFO;

    if (m_pEditor->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("Please insert article text first!"));
        return;
    }

    // Check for internet connection
    if (!CUtils::getOnlineState()) {
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("Upload not possible, no active internet "
                                    "connection found!"));
        return;
    }

    bool bOk = false;
    m_sSitename = QInputDialog::getText(m_pParent, trUtf8("Upload"),
                                        trUtf8("Please insert name of the article "
                                               "which should be uploaded.\n"
                                               "It is only possible to upload "
                                               "into the \"%1\"!")
                                        .arg(m_sConstructionArea),
                                        QLineEdit::Normal,
                                        m_sConstructionArea + "/" + m_sArticlename,
                                        &bOk);
    m_sSitename = m_sSitename.trimmed();

    // Click on "cancel" or string is empty
    if (true != bOk || m_sSitename.isEmpty()) {
        return;
    }

    // Only allowed to upload into the "Baustelle"
    if (!m_sSitename.startsWith(m_sConstructionArea + "/")) {
        m_sSitename = m_sConstructionArea + "/" + m_sSitename;
    }

    // Replace non valid characters
    m_sSitename.replace(QString::fromUtf8("ä"), "a", Qt::CaseInsensitive);
    m_sSitename.replace(QString::fromUtf8("ö"), "o", Qt::CaseInsensitive);
    m_sSitename.replace(QString::fromUtf8("ü"), "u", Qt::CaseInsensitive);
    m_sSitename.replace(" ", "_");
    qDebug() << "Upload site name:" << m_sSitename;

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
        this->requestRevision();
        break;
    case REQUREVISION:
        this->requestRevision();
        break;
    case RECREVISION:
        this->requestUpload();
        break;
    case REQUPLOAD:
        this->requestRevision();
        break;
    case RECUPLOAD:
        this->requestRevision();
        break;
    default:
        qWarning() << "Ran into unexpected state at upload:" << m_State;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::requestToken() {
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

    m_State = REQUTOKEN;
    m_pReply = m_pNwManager->get(request);
}

// ----------------------------------------------------------------------------

void CUpload::getTokenReply(QString sNWReply) {
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
            QMessageBox::warning(m_pParent, trUtf8("Error"),
                                 trUtf8("Upload failed! No CSRFTOKEN received."));
            m_State = REQUTOKEN;
            return;
        } else if (sSessionCookie.isEmpty()) {
            qWarning() << "No session cookie received.";
            qWarning() << "COOKIES" << m_ListCookies;
            QMessageBox::warning(m_pParent, trUtf8("Error"),
                                 trUtf8("Upload failed! No session cookie received."));
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
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("Upload failed! No cookies received."));
        m_State = REQUTOKEN;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::requestLogin() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_State = REQULOGIN;

    QString sUrl(m_sInyokaUrl);
    bool bOk = false;
    QString sUsername("");
    QString sPassword("");

    sUsername = QInputDialog::getText(m_pParent, trUtf8("Login user"),
                                      trUtf8("Please insert your Inyoka user name:"),
                                      QLineEdit::Normal, "", &bOk).trimmed();
    if (true != bOk || sUsername.isEmpty()) {
        return;
    }

    sPassword = QInputDialog::getText(m_pParent, trUtf8("Login password"),
                                      trUtf8("Please insert your Inyoka password:"),
                                      QLineEdit::Password, "", &bOk).trimmed();
    if (true != bOk || sPassword.isEmpty()) {
        return;
    }

    sUrl = sUrl.remove("wiki.") + "/login/?next=" + m_sInyokaUrl + "/" + m_sSitename;

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

#if QT_VERSION < 0x050000
    QUrl params;
#else
    QUrlQuery params;
#endif
    params.addQueryItem("csrfmiddlewaretoken", m_sToken);
    params.addQueryItem("username", sUsername);
    params.addQueryItem("password", sPassword);
    params.addQueryItem("redirect", "");

    m_State = REQULOGIN;
#if QT_VERSION < 0x050000
    m_pReply = m_pNwManager->post(request, params.encodedQuery());
#else
    m_pReply = m_pNwManager->post(request, params.toString().toUtf8());
#endif
}

// ----------------------------------------------------------------------------

void CUpload::getLoginReply(QString sNWReply) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    // If "$IS_LOGGED_IN = false" is found in reply --> login failed
    if (-1 != sNWReply.indexOf("$IS_LOGGED_IN = false")) {
        m_State = REQUTOKEN;
        qWarning() << "LOGIN FAILED! Wrong credentials?";
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("Login at Inyoka failed. Wrong credentials?"));
        return;
    } else {
        foreach (QNetworkCookie cookie, m_ListCookies) {
            // Includes cookie message "153cae855e0ae527d6dc2434f3eb8ef60b782570"
            // --> "Du hast dich erfolgreich angemeldet"
            if (-1 != cookie.toRawForm().indexOf(
                        "153cae855e0ae527d6dc2434f3eb8ef60b782570")) {
                m_State = RECLOGIN;
                qDebug() << "LOGIN SUCCESSFUL!";
                break;
            }
        }

        if (RECLOGIN != m_State) {
            m_State = REQUTOKEN;
            qWarning() << "LOGIN FAILED! No success message cookie.";
            QMessageBox::warning(m_pParent, trUtf8("Error"),
                                 trUtf8("Login at Inyoka failed."));
            return;
        }
    }

    this->requestRevision();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::requestRevision(QString sUrl) {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_State = REQUREVISION;

    if (sUrl.isEmpty()) {
        sUrl = m_sInyokaUrl + "/" + m_sSitename + "/a/log/";
    }

    QNetworkRequest request(sUrl);
    m_urlRedirectedTo = sUrl;
    request.setRawHeader("User-Agent",
                         QString(qApp->applicationName() + "/"
                                 + qApp->applicationVersion()).toLatin1());
    m_pReply = m_pNwManager->get(request);
}

// ----------------------------------------------------------------------------

void CUpload::getRevisionReply(QString sNWReply) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QString sURL(m_sInyokaUrl);
    sURL.remove("https://");
    sURL.remove("http://");
    QRegExp findRevision(sURL + "/" + m_sSitename + "/a/revision/" + "\\d+",
                         Qt::CaseInsensitive);

    int nPos = findRevision.indexIn(sNWReply);
    if (nPos > -1) {
        m_sRevision = findRevision.cap();
        m_sRevision.remove(0, m_sRevision.lastIndexOf('/') + 1);
        qDebug() << "Last revision of" << m_sSitename << "=" << m_sRevision;
    } else {
        m_sRevision = "";
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("Last article revision not found!"));
        qWarning() << "Article revision not found!";
        qDebug() << "Reply:" << sNWReply;
        return;
    }

    m_State = RECREVISION;
    this->requestUpload();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::requestUpload() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_State = REQUPLOAD;

    QString sUrl(m_sInyokaUrl + "/" + m_sSitename + "/a/edit/");
    qDebug() << "UPLOADING article:" << sUrl;
    QNetworkRequest request;
    // Referer needed with POST request + https in Django
    request.setRawHeader("Referer", m_sInyokaUrl.toLatin1());
    request.setRawHeader("User-Agent",
                         QString(qApp->applicationName() + "/"
                                 + qApp->applicationVersion()).toLatin1());
    request.setUrl(QUrl(sUrl));

    QList<QNetworkCookie> listCookies;
    listCookies << m_pNwManager->cookieJar()->cookiesForUrl(QUrl(sUrl));
    // qDebug() << "COOKIES FOR URL:" << listCookies;

    QString sCookie("");
    foreach (QNetworkCookie cookie, listCookies) {
        if (!cookie.isSessionCookie() && sCookie.isEmpty()) {
            // Use first cookie
            sCookie = cookie.toRawForm();
            break;
        }
    }
    // qDebug() << "COOKIE:" << sCookie;

    m_sToken = "csrftoken=";
    int nInd = sCookie.indexOf(m_sToken) + m_sToken.length();
    m_sToken = sCookie.mid(nInd, sCookie.indexOf(';', nInd) - nInd);
    if (m_sToken.isEmpty()) {
        qWarning() << "Upload failed! Empty CSRFTOKEN.";
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("Upload failed! No CSRFTOKEN received."));
        return;
    }
    // qDebug() << "TOKEN:" << m_sToken;

    QString sNote("");
    bool bOk(false);
    sNote = QInputDialog::getText(m_pParent, trUtf8("Change note"),
                                      trUtf8("Please insert a change message:"),
                                      QLineEdit::Normal, "", &bOk);
    sNote = sNote.trimmed();
    // Click on "cancel" or string is empty
    if (true != bOk || sNote.isEmpty()) {
        qWarning() << "Change note is empty.";
        QMessageBox::warning(m_pParent, trUtf8("Error"),
                             trUtf8("It is not allowed to upload an article "
                                    "without change message!"));
        return;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QHttpMultiPart *pMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart tokenPart;
    tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"csrfmiddlewaretoken\""));
    tokenPart.setBody(m_sToken.toLatin1());

    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"text\""));
    textPart.setBody(QString(m_pEditor->toPlainText()).toUtf8());

    QHttpPart notePart;
    notePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"note\""));
    notePart.setBody(sNote.toUtf8());

    QHttpPart timePart;
    timePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"edit_time\""));
    timePart.setBody(QDateTime::currentDateTimeUtc().toString(
                         "yyyy-MM-dd hh:mm:ss.zzzzzz").toLatin1());

    QHttpPart revPart;
    revPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"revision\""));
    revPart.setBody(m_sRevision.toLatin1());

    pMultiPart->append(tokenPart);
    pMultiPart->append(textPart);
    pMultiPart->append(notePart);
    pMultiPart->append(timePart);
    pMultiPart->append(revPart);

    m_State = REQUPLOAD;
    m_pReply = m_pNwManager->post(request, pMultiPart);
    pMultiPart->setParent(m_pReply);
}

// ----------------------------------------------------------------------------

void CUpload::getUploadReply(QString sNWReply) {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_State = RECUPLOAD;

    if (sNWReply.isEmpty()) {
        QMessageBox::information(m_pParent, "Upload",
                                 trUtf8("Upload successful!"));
    } else {
        if (sNWReply.contains("Du hast die Seite nicht verändert.")) {
          qDebug() << "UPLOAD REPLY: Page was not changed.";
          QMessageBox::warning(m_pParent, trUtf8("Upload failed"),
                               trUtf8("The page content was not changed!"));
        } else {
          qDebug() << "UPLOAD REPLY:" << sNWReply;
          QMessageBox::warning(m_pParent, trUtf8("Error"),
                               trUtf8("Upload failed!"));
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUpload::replyFinished(QNetworkReply *pReply) {
    qDebug() << "Calling" << Q_FUNC_INFO;
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    QIODevice *pData(pReply);

    if (QNetworkReply::NoError != pReply->error()) {
        QMessageBox::critical(m_pParent, "Error", pData->errorString());
        qCritical() << "Error (#" << pReply->error() << ") while NW reply:"
                    << pData->errorString();
        qDebug() << "Reply content:" << pReply->readAll();
        return;
    } else {
        if (m_State == REQUREVISION) {
            // Check for redirection
            QVariant possibleRedirectUrl = pReply->attribute(
                        QNetworkRequest::RedirectionTargetAttribute);
            m_urlRedirectedTo = this->redirectUrl(possibleRedirectUrl.toUrl(),
                                                  m_urlRedirectedTo);
        }
        if (!m_urlRedirectedTo.isEmpty() && m_State == REQUREVISION) {
            qDebug() << "Redirected to: " + m_urlRedirectedTo.toString();
            this->requestRevision(m_urlRedirectedTo.toString() + "a/log/");
        } else {
            m_ListCookies = this->allCookies();
            QString sReply = QString::fromUtf8(pData->readAll());
            sReply.replace("\r\r\n", "\n");
            m_pReply->deleteLater();

            if (sReply.isEmpty()) {
                qDebug() << "Upload NW reply is empty.";
            }

            switch (m_State) {
            case REQUTOKEN:
                this->getTokenReply(sReply);
                break;
            case REQULOGIN:
                this->getLoginReply(sReply);
                break;
            case REQUREVISION:
                this->getRevisionReply(sReply);
                break;
            case REQUPLOAD:
                this->getUploadReply(sReply);
                break;
            default:
                qWarning() << "Ran into unexpected state:" << m_State;
                qWarning() << "REPLY:" << sReply;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QUrl CUpload::redirectUrl(const QUrl &possibleRedirectUrl,
                          const QUrl &oldRedirectUrl) {
    QUrl redirectUrl;
    if (!possibleRedirectUrl.isEmpty()
            && possibleRedirectUrl != oldRedirectUrl) {
        redirectUrl = possibleRedirectUrl;
        m_sSitename = redirectUrl.toString().mid(m_sInyokaUrl.size() + 1);
        if (m_sSitename.startsWith('/')) {
            m_sSitename.remove(0, 1);
        }
        if (m_sSitename.endsWith('/')) {
            m_sSitename.remove(m_sSitename.length() - 1, 1);
        }
        qDebug() << "Set new sitename:" << m_sSitename;
    } else {
        m_urlRedirectedTo.clear();
    }
    return redirectUrl;
}
