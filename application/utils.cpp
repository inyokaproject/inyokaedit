/**
 * \file utils.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
#include <QNetworkProxy>
#include <QMessageBox>
#include <QPushButton>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>

Utils::Utils(QWidget *pParent, QObject *pParentObj)
  : m_pParent(pParent) {
  Q_UNUSED(pParentObj)
  m_NwManager = new QNetworkAccessManager(this);
  connect(m_NwManager, &QNetworkAccessManager::finished,
          this, &Utils::replyFinished);
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
  QString sDownloadUrl(
        QStringLiteral("https://github.com/inyokaproject/inyokaedit/releases"));
  qDebug() << "Looking for updates...";
  m_NwManager->get(QNetworkRequest(QUrl(sDownloadUrl)));
}

// ----------------------------------------------------------------------------

void Utils::replyFinished(QNetworkReply *pReply) {
  QIODevice *pData(pReply);

  if (QNetworkReply::NoError != pReply->error()) {
    qWarning() << "Error (#" << pReply->error() << ")while update check:"
               << pData->errorString();
  } else {
    QRegularExpression regExp(
          QStringLiteral(
            "\\bInyokaEdit-(\\d+.\\d+.\\d+)-\\d+-Windows_x64.zip\\b"));
    QString sReply = QString::fromUtf8(pData->readAll());
    QRegularExpressionMatch match = regExp.match(sReply);

    if (match.hasMatch()) {
      QStringList sListCurrentVer;
      QStringList sListLatestVer;
      QString sLatestVersion(match.captured(1));
      qDebug() << "Latest version on server:" << sLatestVersion;

      sListCurrentVer = qApp->applicationVersion().split(QStringLiteral("."));
      sListLatestVer = sLatestVersion.split(QStringLiteral("."));

      if (sListCurrentVer.size() > 2 && sListLatestVer.size() > 2) {
        auto nMainVer1 = static_cast<quint8>(sListCurrentVer[0].toUInt());
        auto nMinorVer1 = static_cast<quint8>(sListCurrentVer[1].toUInt());
        auto nRevision1 = static_cast<quint8>(sListCurrentVer[2].toUInt());
        auto nMainVer2 = static_cast<quint8>(sListLatestVer[0].toUInt());
        auto nMinorVer2 = static_cast<quint8>(sListLatestVer[1].toUInt());
        auto nRevision2 = static_cast<quint8>(sListLatestVer[2].toUInt());

        if (nMainVer2 > nMainVer1
            ||nMinorVer2 > nMinorVer1
            || nRevision2 > nRevision1) {
          auto *msgBox = new QMessageBox(
                           QMessageBox::Question,
                           tr("Update found"),
                           tr("Found a new version of %1.<br>"
                           "Do you want to download the latest version?")
                           .arg(qApp->applicationName()),
                           QMessageBox::NoButton, m_pParent);
          QPushButton *noDontAskAgainButton = msgBox->addButton(
                                                tr("No, don't ask again!"),
                                                QMessageBox::NoRole);
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
            QDesktopServices::openUrl(
                  QUrl(
                    QStringLiteral(
                      "https://github.com/inyokaproject/inyokaedit/releases")));
          } else if (msgBox->clickedButton() == noButton) {
            qDebug() << "Don't want to download an update.";
          }
        }
      }
    }
  }
}
