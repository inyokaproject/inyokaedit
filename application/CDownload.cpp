/**
 * \file CDownload.cpp
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
 * Download functions: Styles, article, images
 */

#include <QApplication>
#include <QDebug>
#include <QInputDialog>

#include "./CDownload.h"
#include "./CProgressDialog.h"
#include "./CUtils.h"

extern bool bDEBUG;


// TODO: Up-/download as plugin?

CDownload::CDownload(QWidget *pParent, const QString &sStylesDir, const QString &sImgDir)
    : m_pParent(pParent),
      m_sStylesDir(sStylesDir),
      m_sImgDir(sImgDir),
      m_sInyokaUrl("http://wiki.ubuntuusers.de"),
      m_bAutomaticImageDownload(false) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pNwManager = new QNetworkAccessManager(m_pParent);
    connect(m_pNwManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    m_DlImages = new CDownloadImg();
    connect(m_DlImages, SIGNAL(finsihedImageDownload()),
            this, SLOT(showArticle()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownload::updateSettings(const bool bCompleter,
                               const QString &sInyokaUrl) {
    m_bAutomaticImageDownload = bCompleter;
    m_sInyokaUrl = sInyokaUrl;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownload::loadInyokaStyles() {
    int iRet = QMessageBox::question(m_pParent, trUtf8("Download styles"),
                                     trUtf8("In order to preview articles "
                                            "correctly, Inyoka resources have "
                                            "to be downloaded. This process may "
                                            "take a few minutes.\n\nDo you want "
                                            "to download these files now?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);

    if (iRet != QMessageBox::No) {
        this->callDownloadScript("GetInyokaStyles");
    }
}

void CDownload::updateIWLs() {
    int iRet = QMessageBox::question(m_pParent, trUtf8("Update IWLs"),
                                     trUtf8("Do you want update the Inyoka "
                                            "InterWiki-Links now?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);

    if (iRet != QMessageBox::No) {
        this->callDownloadScript("GetIWLs");
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownload::callDownloadScript(const QString &sScript) {
    qDebug() << "Calling" << Q_FUNC_INFO << sScript;

    // Check for internet connection
    if (!CUtils::getOnlineState()) {
        QMessageBox::warning(m_pParent, qApp->applicationName(),
                             trUtf8("Download not possible, no active internet "
                                    "connection found!"));
        return;
    }

    CProgressDialog *pDownloadProgress;

    // Path from normal installation
    if (QFile::exists(qApp->applicationDirPath() + "/../../share/" + qApp->applicationName().toLower()
                      + "/" + sScript) && !bDEBUG) {
        pDownloadProgress =
                new CProgressDialog(qApp->applicationDirPath() + "/../../share/"
                                    + qApp->applicationName().toLower() + "/"
                                    + sScript,
                                    QStringList() << m_sStylesDir);
    } else if (QFile::exists(qApp->applicationDirPath() + "/" + sScript)) {
        // No installation: Use app path
        pDownloadProgress =
                new CProgressDialog(qApp->applicationDirPath() + "/" + sScript,
                                    QStringList() << m_sStylesDir);
    } else {
        qWarning() << "Download script could not be found:"
                   << "\n\t"+ qApp->applicationDirPath() + "/../../share/" + qApp->applicationName().toLower()
                      + "/" + sScript
                   << "\n\t" + qApp->applicationDirPath() + "/" + sScript;
        QMessageBox::warning(m_pParent, qApp->applicationName(),
                             trUtf8("Download script could not be found."));
        return;
    }

    pDownloadProgress->show();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// DOWNLOAD EXISTING INYOKA WIKI ARTICLE

void CDownload::downloadArticle() {
    // Check for internet connection
    if (!CUtils::getOnlineState()) {
        QMessageBox::warning(m_pParent, qApp->applicationName(),
                             trUtf8("Download not possible, no active internet "
                                    "connection found!"));
        return;
    }

    QString sUrl("");
    // Buttons of input dialog (click on "OK" -> ok = true,
    // click on "Cancel" -> ok = false)
    bool bOk;

    // Show input dialog
    m_sSitename = QInputDialog::getText(m_pParent, qApp->applicationName(),
                                        trUtf8("Please insert name of the article "
                                               "which should be downloaded:"),
                                        QLineEdit::Normal,
                                        trUtf8("Category/Article"),
                                        &bOk);

    // Click on "cancel" or string is empty
    if (true != bOk || m_sSitename.isEmpty()) {
        return;
    }
    m_sSitename.replace(" ", "_");

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    sUrl = m_sInyokaUrl + "/" + m_sSitename + "?action=export&format=raw";

    // Download specific revision
    if (m_sSitename.contains("@rev=")) {
        QString sRevision = m_sSitename.mid(m_sSitename.indexOf("@rev="));
        sRevision.remove("@rev=");
        m_sSitename.remove(m_sSitename.indexOf("@rev="), m_sSitename.length());
        sUrl += "&rev=" + sRevision;
    }

    m_bDownloadArticle = true;
    qDebug() << "DOWNLOADING article:" << sUrl;
    QNetworkRequest request(sUrl);
    m_pReply = m_pNwManager->get(request);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// DOWNLOAD IN ARTICLES INCLUDED IMAGES

void CDownload::downloadImages() {
    m_bDownloadArticle = false;

    QString sUrl(m_sInyokaUrl + "/" + m_sSitename +"?action=metaexport");
    qDebug() << "DOWNLOADING meta data:" << sUrl;
    QNetworkRequest request(sUrl);
    m_pReply = m_pNwManager->get(request);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownload::replyFinished(QNetworkReply *pReply) {
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    QIODevice *pData(pReply);

    if (QNetworkReply::NoError != pReply->error()) {
        QMessageBox::critical(m_pParent, qApp->applicationName(), pData->errorString());
        qCritical() << "Error while NW reply:" << pData->errorString();
        return;
    } else {
        QString sTmpArticle = QString::fromUtf8(pData->readAll());

        if (m_bDownloadArticle) {
            // Replace windows specific newlines
            sTmpArticle.replace("\r\r\n", "\n");

            m_pReply->deleteLater();

            // Site does not exist etc.
            if (sTmpArticle.isEmpty()) {
                QMessageBox::information(m_pParent, qApp->applicationName(),
                                         trUtf8("Could not download the article."));
                return;
            }

            m_sArticleText = sTmpArticle;
            downloadImages();
        // --------------------------------------------------------------------
        } else {
            // Download article images metadata
            int iRet = 0;
            QStringList sListTmp, sListMetadata, sListSaveFolder;

            // Site does not exist etc.
            if (sTmpArticle.isEmpty()) {
                QMessageBox::information(m_pParent, qApp->applicationName(),
                                         trUtf8("Could not find meta data."));
                return;
            }

            // Copy metadata line by line in list
            sListTmp << sTmpArticle.split("\n");
            // qDebug() << "META files:" << sListTmp;

            m_pReply->deleteLater();

            // Get only attachments article metadata
            for (int i = 0; i < sListTmp.size(); i++) {
                if (sListTmp[i].startsWith("X-Attach: " + m_sSitename + "/",
                                           Qt::CaseInsensitive)) {
                    // Remove "X-Attach: "
                    sListMetadata << sListTmp[i].remove("X-Attach: ");
                    // Remove windows specific newline \r
                    sListMetadata.last().remove("\r");
                    sListMetadata.last() = m_sInyokaUrl + "/_image?target="
                            + sListMetadata.last();
                    sListSaveFolder << m_sImgDir;

                    // qDebug() << sListMetadata.last();
                }
            }

            // If attachments exist
            if (sListMetadata.size() > 0) {
                // Ask if images should be downloaded,
                // if not enabled by default in settings
                if (true != m_bAutomaticImageDownload) {
                    iRet = QMessageBox::question(m_pParent, qApp->applicationName(),
                                                 trUtf8("Do you want to download "
                                                        "the images which are "
                                                        "attached to the article?"),
                                                 QMessageBox::Yes
                                                 | QMessageBox::No,
                                                 QMessageBox::No);
                } else {
                    iRet = QMessageBox::Yes;
                }

                if (iRet != QMessageBox::No) {
                    qDebug() << "Starting image download...";
                    m_DlImages->setDLs(sListMetadata, sListSaveFolder);
                    QTimer::singleShot(0, m_DlImages, SLOT(startDownloads()));
                } else {
                    this->showArticle();
                }
            } else {
                this->showArticle();
            }
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CDownload::showArticle() {
    emit sendArticleText(m_sArticleText, m_sSitename);
}
