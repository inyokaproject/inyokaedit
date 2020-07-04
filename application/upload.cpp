/**
 * \file upload.cpp
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
 * Upload raw text of an article to Inyoka.
 */

#include "./upload.h"

#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QInputDialog>
#include <QMessageBox>
#include <QNetworkCookie>
#include <QRegularExpression>
#include <QUrl>
#include <QUrlQuery>

#include "./utils.h"

Upload::Upload(QWidget *pParent, Session *pSession,
               const QString &sInyokaUrl, const QString &sConstArea)
  : m_pParent(pParent),
    m_pSession(pSession),
    m_sInyokaUrl(sInyokaUrl),
    m_pReply(nullptr),
    m_State(REQUREVISION),
    m_sSitename(QLatin1String("")),
    m_sRevision(QLatin1String("")),
    m_sConstructionArea(sConstArea),
    m_pEditor(nullptr),
    m_sArticlename(QLatin1String("")) {
  this->setParent(m_pParent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Upload::setEditor(QTextEdit *pEditor, const QString &sArticlename) {
  m_pEditor = pEditor;
  m_sArticlename = tr("Untitled");
  if (!sArticlename.isEmpty() && !sArticlename.contains(m_sArticlename)) {
    m_sArticlename = sArticlename;
  }
  m_sArticlename.remove(m_sConstructionArea + "/");
  QFileInfo fi(m_sArticlename);
  m_sArticlename = fi.baseName();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Upload::clickUploadArticle() {
  if (m_pEditor->toPlainText().trimmed().isEmpty()) {
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Please insert article text first!"));
    return;
  }

  // Check for internet connection
  if (!Utils::getOnlineState()) {
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Upload not possible, no active internet "
                            "connection found!"));
    return;
  }

  bool bOk = false;
  m_sSitename = QInputDialog::getText(
                  m_pParent, tr("Upload"),
                  tr("Please insert name of the article which should be "
                     "uploaded.\nIt is only possible to upload into "
                     "the \"%1\"!").arg(m_sConstructionArea),
                  QLineEdit::Normal, m_sConstructionArea + "/" + m_sArticlename,
                  &bOk);
  m_sSitename = m_sSitename.trimmed();

  if (!bOk || m_sSitename.isEmpty()) {
    return;
  }

  // Only allowed to upload into the "Baustelle"
  if (!m_sSitename.startsWith(m_sConstructionArea + "/")) {
    m_sSitename = m_sConstructionArea + "/" + m_sSitename;
  }

  // Replace non valid characters
  m_sSitename.replace(QStringLiteral("ä"), QLatin1String("a"),
                      Qt::CaseInsensitive);
  m_sSitename.replace(QStringLiteral("ö"), QLatin1String("o"),
                      Qt::CaseInsensitive);
  m_sSitename.replace(QStringLiteral("ü"), QLatin1String("u"),
                      Qt::CaseInsensitive);
  m_sSitename.replace(QLatin1String(" "), QLatin1String("_"));
  qDebug() << "UPLOAD site name:" << m_sSitename;

  m_pSession->checkSession();
  if (!m_pSession->isLoggedIn()) {
    qWarning() << "Upload failed - user not logged in!";
    return;
  }

  switch (m_State) {
    case RECREVISION:
      this->requestUpload();
      break;
    case REQUREVISION:
    case REQUPLOAD:
    case RECUPLOAD:
      this->requestRevision();
      break;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Upload::requestRevision(QString sUrl) {
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
  m_pReply = m_pSession->getNwManager()->get(request);
  QEventLoop loop;
  connect(m_pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  this->replyFinished(m_pReply);
}

// ----------------------------------------------------------------------------

void Upload::getRevisionReply(const QString &sNWReply) {
  QString sURL(m_sInyokaUrl);
  sURL.remove(QStringLiteral("https://"));
  sURL.remove(QStringLiteral("http://"));
  QRegularExpression findRevision(sURL + "/" + m_sSitename +
                                  "/a/revision/" + "\\d+",
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatch match = findRevision.match(sNWReply);

  if (match.hasMatch()) {
    m_sRevision = match.captured(0);
    m_sRevision.remove(0, m_sRevision.lastIndexOf('/') + 1);
    qDebug() << "Last revision of" << m_sSitename << "=" << m_sRevision;
  } else {
    m_sRevision = QLatin1String("");
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Last article revision not found!"));
    qWarning() << "Article revision not found!";
    qDebug() << "Reply:" << sNWReply;
    return;
  }

  m_State = RECREVISION;
  this->requestUpload();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Upload::requestUpload() {
  const int nMAXINPUT = 510;  // Max length = 512 in Inyoka input form
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
  listCookies << m_pSession->getNwManager()->cookieJar()->cookiesForUrl(
                   QUrl(sUrl));
  // qDebug() << "COOKIES FOR URL:" << listCookies;

  QString sCookie(QLatin1String(""));
  foreach (QNetworkCookie cookie, listCookies) {
    if (!cookie.isSessionCookie() && sCookie.isEmpty()) {
      // Use first cookie
      sCookie = cookie.toRawForm();
      break;
    }
  }
  // qDebug() << "COOKIE:" << sCookie;

  QString sToken(QStringLiteral("csrftoken="));
  int nInd = sCookie.indexOf(sToken) + sToken.length();
  sToken = sCookie.mid(nInd, sCookie.indexOf(';', nInd) - nInd);
  if (sToken.isEmpty()) {
    qWarning() << "Upload failed! Empty CSRFTOKEN.";
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("Upload failed! No CSRFTOKEN received."));
    return;
  }

  QString sNote(QLatin1String(""));
  bool bOk(false);
  sNote = QInputDialog::getText(m_pParent, tr("Change note"),
                                tr("Please insert a change message:"),
                                QLineEdit::Normal, QLatin1String(""), &bOk);
  sNote = sNote.trimmed();
  if (sNote.length() > nMAXINPUT) {
    sNote.resize(nMAXINPUT);
  }
  if (!bOk || sNote.isEmpty()) {
    qWarning() << "Change note is empty.";
    QMessageBox::warning(m_pParent, tr("Error"),
                         tr("It is not allowed to upload an article "
                            "without change message!"));
    return;
  }

#ifndef QT_NO_CURSOR
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  auto *pMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  QHttpPart tokenPart;
  tokenPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                      QVariant("form-data; name=\"csrfmiddlewaretoken\""));
  tokenPart.setBody(sToken.toLatin1());

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
                     QStringLiteral("yyyy-MM-dd hh:mm:ss.zzzzzz")).toLatin1());

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
  m_pReply = m_pSession->getNwManager()->post(request, pMultiPart);
  QEventLoop loop;
  connect(m_pReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  this->replyFinished(m_pReply);

  pMultiPart->setParent(m_pReply);
}

// ----------------------------------------------------------------------------

void Upload::getUploadReply(const QString &sNWReply) {
  m_State = RECUPLOAD;

  if (sNWReply.isEmpty()) {
    qDebug() << "UPLOAD SUCCESSFUL!";
    QMessageBox::information(m_pParent, QStringLiteral("Upload"),
                             tr("Upload successful!"));
  } else {
    if (sNWReply.contains(
          QStringLiteral("Du hast die Seite nicht verändert."))) {
      qDebug() << "UPLOAD REPLY: Page was not changed.";
      QMessageBox::warning(m_pParent, tr("Upload failed"),
                           tr("The page content was not changed!"));
    } else {
      qDebug() << "UPLOAD REPLY:" << sNWReply;
      QMessageBox::warning(m_pParent, tr("Error"), tr("Upload failed!"));
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Upload::replyFinished(QNetworkReply *pReply) {
#ifndef QT_NO_CURSOR
  QApplication::restoreOverrideCursor();
#endif

  QIODevice *pData(pReply);

  if (QNetworkReply::NoError != pReply->error()) {
    QMessageBox::critical(m_pParent, QStringLiteral("Error"),
                          pData->errorString());
    qCritical() << "Error (#" << pReply->error() << ") while NW reply:"
                << pData->errorString();
    qDebug() << "Reply content:" << pReply->readAll();
    return;
  }

  if (m_State == REQUREVISION) {
    // Check for redirection
    QVariant varRedirectUrl = pReply->attribute(
                                QNetworkRequest::RedirectionTargetAttribute);
    m_urlRedirectedTo = this->redirectUrl(varRedirectUrl.toUrl(),
                                          m_urlRedirectedTo);
  }
  if (!m_urlRedirectedTo.isEmpty() && m_State == REQUREVISION) {
    qDebug() << "Redirected to: " + m_urlRedirectedTo.toString();
    this->requestRevision(m_urlRedirectedTo.toString() + "a/log/");
  } else {
    QString sReply = QString::fromUtf8(pData->readAll());
    sReply.replace(QLatin1String("\r\r\n"), QLatin1String("\n"));
    m_pReply->deleteLater();

    if (sReply.isEmpty()) {
      qDebug() << "Upload NW reply is empty.";
    }

    switch (m_State) {
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Upload::redirectUrl(const QUrl &possibleRedirectUrl,
                         const QUrl &oldRedirectUrl) -> QUrl {
  QUrl redirectUrl;
  if (!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl) {
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
