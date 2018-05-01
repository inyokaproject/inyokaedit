/**
 * \file utils.cpp
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
 * Utile functions, which can be used by different classes.
 */

#include "./utils.h"

#include <QDate>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QNetworkConfigurationManager>
#include <QNetworkProxy>
#include <QMessageBox>
#include <QPushButton>
#include <QNetworkRequest>
#include <QNetworkReply>

Utils::Utils(QWidget *pParent)
  : m_pParent(pParent) {
  m_NwManager = new QNetworkAccessManager(this);
  connect(m_NwManager, SIGNAL(finished(QNetworkReply*)),
          this, SLOT(replyFinished(QNetworkReply*)));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool Utils::getOnlineState() {
  QNetworkConfigurationManager mgr;
  if (!mgr.isOnline()) {
    return false;
  }
  return true;
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
  QString sDownloadUrl("https://github.com/inyokaproject/inyokaedit/releases");
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
    QRegExp regExp("\\bInyokaEdit-\\d+.\\d+.\\d+-\\d+-Windows_x64.zip\\b");
    QString sReply = QString::fromUtf8(pData->readAll());

    int nIndex = regExp.indexIn(sReply);
    if (nIndex >= 0) {
      QStringList sListCurrentVer;
      QStringList sListLatestVer;
      QString sLatestVersion(regExp.cap());
      sLatestVersion.remove("InyokaEdit-");
      sLatestVersion.remove("-Windows_x64.zip");
      sLatestVersion = sLatestVersion.left(sLatestVersion.lastIndexOf("-"));
      sLatestVersion = sLatestVersion.trimmed();
      qDebug() << "Latest version on server:" << sLatestVersion;

      sListCurrentVer = qApp->applicationVersion().split(".");
      sListLatestVer = sLatestVersion.split(".");

      if (sListCurrentVer.size() > 2 && sListLatestVer.size() > 2) {
        quint8 nMainVer1 = sListCurrentVer[0].toUInt();
        quint8 nMinorVer1 = sListCurrentVer[1].toUInt();
        quint8 nRevision1 = sListCurrentVer[2].toUInt();
        quint8 nMainVer2 = sListLatestVer[0].toUInt();
        quint8 nMinorVer2 = sListLatestVer[1].toUInt();
        quint8 nRevision2 = sListLatestVer[2].toUInt();

        if (nMainVer2 > nMainVer1
            ||nMinorVer2 > nMinorVer1
            || nRevision2 > nRevision1) {
          QMessageBox *msgBox = new QMessageBox(
                                  QMessageBox::Question,
                                  tr("Update found"),
                                  tr("Found a new version of %1.<br>"
                                     "Do you want to download the latest "
                                     "version?")
                                  .arg(qApp->applicationName()), 0, m_pParent);
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
          } else if (msgBox->clickedButton() == yesButton) {
            qDebug() << "Calling download page.";
            QDesktopServices::openUrl(
                  QUrl("https://github.com/inyokaproject/inyokaedit/releases"));
          } else if (msgBox->clickedButton() == noButton) {
            qDebug() << "Don't want to download an update.";
          }
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Utils::reportBug() {
    QDesktopServices::openUrl(
          QUrl("https://github.com/inyokaproject/inyokaedit/issues"));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Utils::showAbout() {
  QMessageBox::about(
        m_pParent, tr("About")+ " " + qApp->applicationName(),
        QString("<big><b>%1 %2</b></big><br />"
                "%3<br />"
                "<small>%4</small><br /><br />"
                "%5<br />"
                "%6<br />"
                "<small>%7</small><br /><br />"
                "%8")
        .arg(qApp->applicationName(),
             qApp->applicationVersion(),
             tr("Editor for Inyoka-based portals"),
             APP_COPY,
             "URL: <a href=\"https://github.com/inyokaproject/inyokaedit\">"
             "https://github.com/inyokaproject/inyokaedit</a>",
             tr("License") +
             ": <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
             "GNU General Public License Version 3</a>",
             tr("This application uses icons from "
                "<a href=\"http://tango.freedesktop.org\">"
                "Tango project</a>."),
             tr("Special thanks to djcj, bubi97, Lasall, Vistaus, "
                "Shakesbier and all testers from "
                "<a href=\"http://ubuntuusers.de\"> "
                "ubuntuusers.de</a>.")));
}
