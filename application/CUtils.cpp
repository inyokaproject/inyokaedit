/**
 * \file CUtils.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2014 The InyokaEdit developers
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

#include <QDate>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QNetworkConfigurationManager>
#include <QNetworkProxy>
#include <QMessageBox>
#include <QPushButton>
#include <QProcess>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "./CUtils.h"

CUtils::CUtils(QWidget *pParent)
    : m_pParent(pParent) {
    qDebug() << "Calling" << Q_FUNC_INFO;
    NwManager = new QNetworkAccessManager(this);
    connect(NwManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CUtils::getOnlineState() {
#if QT_VERSION >= 0x040700
    QNetworkConfigurationManager mgr;
    if (!mgr.isOnline()) {
        return false;
    }
#endif
    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUtils::setProxy(const QString &sHostName, const quint16 nPort,
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
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUtils::checkWindowsUpdate() {
    QString sDownloadUrl("http://bazaar.launchpad.net/~elthoro/inyokaedit/windows_files/view/head:/LatestVersion.txt");
    qDebug() << "Looking for updates...";
    NwManager->get(QNetworkRequest(QUrl(sDownloadUrl)));
}

// ----------------------------------------------------------------------------

void CUtils::replyFinished(QNetworkReply *pReply) {
    QIODevice *pData(pReply);

    if (QNetworkReply::NoError != pReply->error()) {
        qWarning() << "Error while update check:" << pData->errorString();
    } else {
        QRegExp regExp("\\b(Latest InyokaEdit version: )\\b\\d+.\\d+.\\d+");
        QString sReply = QString::fromUtf8(pData->readAll());

        int nIndex = regExp.indexIn(sReply);
        if (nIndex >= 0) {
            QStringList sListCurrentVer;
            QStringList sListLatestVer;
            QString sLatestVersion(regExp.cap());
            sLatestVersion.remove("Latest InyokaEdit version:");
            sLatestVersion = sLatestVersion.trimmed();
            qDebug() << "Latest version:" << sLatestVersion;

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
                    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question,
                                                          trUtf8("Update found"),
                                                          trUtf8("Found a new version of %1.<br>Do you want to download the latest version?")
                                                          .arg(qApp->applicationName()),
                                                          0,
                                                          m_pParent);
                    QPushButton *noDontAskAgainButton = msgBox->addButton(trUtf8("No, don't ask again!"), QMessageBox::NoRole);
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
                        QDesktopServices::openUrl(QUrl("https://launchpad.net/inyokaedit/+download"));
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

void CUtils::reportBug() {
    // Ubuntu: Using Apport, if needed files exist
    if (QFile::exists("/usr/bin/ubuntu-bug")
            && QFile::exists("/etc/apport/crashdb.conf.d/inyokaedit-crashdb.conf")
            && QFile::exists(qApp->applicationDirPath()
                             + "/../../share/apport/package-hooks/source_inyokaedit.py")) {
        // Start apport
        QProcess procApport;
        procApport.start("ubuntu-bug",
                         QStringList() << qApp->applicationName().toLower());

        if (!procApport.waitForStarted()) {
            QMessageBox::critical(m_pParent, qApp->applicationName(),
                                  trUtf8("Error while starting Apport."));
            qCritical() << "Error while starting Apport - waitForStarted()";
            return;
        }
        if (!procApport.waitForFinished()) {
            QMessageBox::critical(m_pParent, qApp->applicationName(),
                                  trUtf8("Error while executing Apport."));
            qCritical() << "Error while executing Apport - waitForFinished()";
            return;
        }
    } else {
        // Not Ubuntu or apport files not found: Load Launchpad bug tracker
        QDesktopServices::openUrl(QUrl("https://bugs.launchpad.net/inyokaedit"));
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CUtils::showAbout() {
    QDate nDate = QDate::currentDate();
    QMessageBox::about(m_pParent,
                       trUtf8("About") + " " + qApp->applicationName(),
                       "<p><b>" + qApp->applicationName() + "</b> - "
                       + trUtf8("Editor for Inyoka-based portals") + "<br />"
                       + trUtf8("Version") + ": "
                       + qApp->applicationVersion() + "</p>"
                       "<p>&copy; 2011-" + QString::number(nDate.year()) + ", "
                       + trUtf8("The %1 developers").arg(qApp->applicationName())
                       + "<br />" + trUtf8("Licence") + ": "
                       "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
                       "GNU General Public License Version 3</a></p>"
                       "<p>" + trUtf8("Special thanks to djcj, bubi97, Lasall, "
                                      "Shakesbier and all testers from "
                                      "<a href=\"http://ubuntuusers.de\">"
                                      "ubuntuusers.de</a>.")
                       + "</p>""<p>" + trUtf8("This application uses icons from "
                                             "<a href=\"http://tango.freedesktop.org\">"
                                             "Tango project</a>.") + "</p>");
}
