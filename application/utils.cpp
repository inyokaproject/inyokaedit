// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

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

#include "./settings.h"

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

void Utils::setProxy() {
  Settings *pSettings(Settings::instance());
  if (!pSettings->getProxyHostName().isEmpty() &&
      0 != pSettings->getProxyPort()) {
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(pSettings->getProxyHostName());
    proxy.setPort(pSettings->getProxyPort());
    if (!pSettings->getProxyUserName().isEmpty() &&
        !pSettings->getProxyPassword().isEmpty()) {
      proxy.setUser(pSettings->getProxyUserName());
      proxy.setPassword(pSettings->getProxyPassword());
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
          Settings::instance()->setWindowsCheckUpdate(false);
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
