/**
 * \file utils.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * Utile functions, which can be used by different classes.
 */

#include "./utils.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QVersionNumber>

Utils::Utils(QWidget *pParent, QObject *pParentObj) : m_pParent(pParent) {
  Q_UNUSED(pParentObj)
  m_NwManager = new QNetworkAccessManager(this);
  connect(m_NwManager, &QNetworkAccessManager::finished, this,
          &Utils::replyFinished);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Utils::getOnlineState() -> bool {
  QNetworkAccessManager nam;
  QNetworkRequest req(
      QUrl(QStringLiteral("https://github.com/inyokaproject/inyokaedit")));
  QNetworkReply *reply = nam.get(req);
  QEventLoop loop;
  connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
  connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  if (!reply->isFinished()) {
    loop.exec();
  }
  if (reply->bytesAvailable()) {
    return true;
  } else {
    qDebug() << "NO internet connection available!";
    return false;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Utils::setProxy(const QString &sHostName, const quint16 nPort,
                     const QString &sUser, const QString &sPassword) {
  if (!sHostName.isEmpty() && 0 != nPort) {
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(sHostName);
    proxy.setPort(nPort);
    if (!sUser.isEmpty() && !sPassword.isEmpty()) {
      proxy.setUser(sUser);
      proxy.setPassword(sPassword);
    }
    QNetworkProxy::setApplicationProxy(proxy);
  } else {
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Utils::checkWindowsUpdate() {
  QUrl url(QStringLiteral(
      "https://api.github.com/repos/inyokaproject/inyokaedit/releases/latest"));
  qDebug() << "Looking for updates...";
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  m_NwManager->get(request);
}

// ----------------------------------------------------------------------------

void Utils::replyFinished(QNetworkReply *pReply) {
  QIODevice *pData(pReply);

  if (QNetworkReply::NoError != pReply->error()) {
    qWarning() << "Error (#" << pReply->error()
               << ")while update check:" << pData->errorString();
  } else {
    QByteArray response_data = pData->readAll();
    QJsonDocument jsondoc = QJsonDocument::fromJson(response_data);
    QJsonObject jsonobj = jsondoc.object();
    QString sLatestVersion(jsonobj[QStringLiteral("tag_name")].toString());
    sLatestVersion = sLatestVersion.remove('v');

    if (sLatestVersion.isEmpty()) {
      qWarning() << "Couldn't get latest version from GitHub API:"
                 << pReply->url().toString();
    } else {
      qDebug() << "Latest version on server:" << sLatestVersion;
      QVersionNumber currentVer(
          QVersionNumber::fromString(qApp->applicationVersion()));
      QVersionNumber latestVer(QVersionNumber::fromString(sLatestVersion));

      if (latestVer > currentVer) {
        auto *msgBox =
            new QMessageBox(QMessageBox::Question, tr("Update found"),
                            tr("Found a new version of %1.<br>"
                               "Do you want to download the latest version?")
                                .arg(qApp->applicationName()),
                            QMessageBox::NoButton, m_pParent);
        QPushButton *noDontAskAgainButton =
            msgBox->addButton(tr("No, don't ask again!"), QMessageBox::NoRole);
        QPushButton *noButton = msgBox->addButton(QMessageBox::No);
        QPushButton *yesButton = msgBox->addButton(QMessageBox::Yes);

        msgBox->setDefaultButton(noButton);
        msgBox->exec();

        if (msgBox->clickedButton() == noDontAskAgainButton) {
          qDebug() << "Don't want to download an update and DON'T ASK AGAIN!";
          emit this->setWindowsUpdateCheck(false);
          return;
        }
        if (msgBox->clickedButton() == yesButton) {
          qDebug() << "Calling download page.";
          QDesktopServices::openUrl(QUrl(QStringLiteral(
              "https://github.com/inyokaproject/inyokaedit/releases")));
        } else if (msgBox->clickedButton() == noButton) {
          qDebug() << "Don't want to download an update.";
        }
      }
    }
  }
}
