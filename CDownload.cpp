/**
 * \file CDownload.cpp
 * \author See AUTHORS
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 by the respective authors (see AUTHORS)
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

#include "CDownload.h"

CDownload::CDownload(QWidget *pParent, const QString &sAppName, const QString &sAppDir, const QDir StylesDir) :
    m_pParent(pParent),
    m_sAppName(sAppName),
    m_sAppDir(sAppDir),
    m_StylesDir(StylesDir)
{
#if defined _WIN32
    m_sWinBashFolder = m_sAppDir + "/win-bash/";
    m_sWget = "\"" + m_sWinBashFolder + "wget\"";
#else
    m_sWinBashFolder = "";
    m_sWget = "wget";
#endif
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// Call download script for Inyoka styles

bool CDownload::loadInyokaStyles()
{
    int iRet = QMessageBox::question(m_pParent, tr("Download styles"), tr("In order to preview articles correctly, Inyoka resources have to be downloaded. This process may take a few minutes.\n\nDo you want to download these files now?"), QMessageBox::Yes | QMessageBox::No);

    if ( QMessageBox::Yes== iRet )
    {
        CProgressDialog *myArticleDownloadProgress;

        try
        {
            // Path from normal installation
            if ( QFile::exists("/usr/share/" + m_sAppName.toLower() + "/GetInyokaStyles") )
            {
                myArticleDownloadProgress = new CProgressDialog("/usr/share/" + m_sAppName.toLower() + "/GetInyokaStyles", QStringList() << m_StylesDir.absolutePath(), m_sAppName, m_pParent);
            }
            // No installation: Use app path
            else
            {
#if defined _WIN32
                if ( QFile::exists(m_StylesDir.absolutePath() + "/GetInyokaStyles") )
                {
                    QFile::remove(m_StylesDir.absolutePath() + "/GetInyokaStyles");
                    QFile::copy(m_sAppDir + "/GetInyokaStyles", m_StylesDir.absolutePath() + "/GetInyokaStyles");
                }
                myArticleDownloadProgress = new CProgressDialog(m_sWinBashFolder + "bash.exe ", QStringList() << m_StylesDir.absolutePath() + "/GetInyokaStyles" << m_StylesDir.absolutePath(), m_sAppName, m_pParent);
#else
                myArticleDownloadProgress = new CProgressDialog(m_sAppDir + "/GetInyokaStyles", QStringList() << m_StylesDir.absolutePath(), m_sAppName, m_pParent);
#endif
            }
        }
        catch ( std::bad_alloc& ba )
        {
            std::cerr << "ERROR: myArticleDownloadProgress - bad_alloc caught: " << ba.what() << std::endl;
            QMessageBox::critical(m_pParent, m_sAppName, "Error while memory allocation: bad_alloc - myArticleDownloadProgress");
            exit (-8);
        }
        myArticleDownloadProgress->open();

        return true;
    }
    return false;
}
// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// DOWNLOAD EXISTING INYOKA WIKI ARTICLE

void CDownload::downloadArticle( const QDir ImgDir, const QString &sInyokaUrl, const bool bAutomaticImageDownload )
{
    QString sTmpArticle("");
    QString sSitename("");
    QByteArray tempResult;

    bool ok; // Buttons of input dialog (click on "OK" -> ok = true, click on "Cancel" -> ok = false)

    // Show input dialog
    sSitename = QInputDialog::getText(m_pParent, m_sAppName,
                                      tr("Please insert name of the article which should be downloaded:"), QLineEdit::Normal,
                                      tr("Category/Article", "Msg: Input dialog DL article example text"), &ok);

    // Click on "cancel" or string is empty
    if ( false == ok || sSitename.isEmpty() )
    {
        return;
    }

    // Replace non valid characters
    sSitename.replace(QString::fromUtf8("ä"), "a", Qt::CaseInsensitive);
    sSitename.replace(QString::fromUtf8("ö"), "o", Qt::CaseInsensitive);
    sSitename.replace(QString::fromUtf8("ü"), "u", Qt::CaseInsensitive);
    sSitename.replace(" ", "_");

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    // Start article download
    QProcess procDownloadRawtext;
    procDownloadRawtext.start(m_sWget, QStringList() << "-O" << "-" << sInyokaUrl + "/" + sSitename + "?action=export&format=raw");

    if ( !procDownloadRawtext.waitForStarted() )
    {
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(m_pParent, m_sAppName, tr("Could not start the download of the raw format of article."));
        procDownloadRawtext.kill();
        return;
    }
    if ( !procDownloadRawtext.waitForFinished() )
    {
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(m_pParent, m_sAppName, tr("Error while downloading raw format of article."));
        procDownloadRawtext.kill();
        return;
    }

    tempResult = procDownloadRawtext.readAll();
    sTmpArticle = QString::fromUtf8(tempResult);

    sTmpArticle.replace("\r\r\n", "\n");  // Replace windows specific newlines

    // Site does not exist etc.
    if ( "" == sTmpArticle )
    {
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
        QMessageBox::information(m_pParent, m_sAppName, tr("Could not download the article."));
        return;
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    emit sendArticleText(sTmpArticle);
    this->downloadImages(sSitename, ImgDir, sInyokaUrl, bAutomaticImageDownload);
}

// -----------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------
// DOWNLOAD IN ARTICLES INCLUDED IMAGES

void CDownload::downloadImages( const QString &sArticlename, const QDir ImgDir, const QString &sInyokaUrl, const bool bAutomaticImageDownload )
{
    int iRet = 0;
    QByteArray tempResult;
    QString sMetadata("");
    QStringList sListTmp, sListMetadata;
    const QString sScriptName("tmpDlScript");

    // Start metadata download
    QProcess procDownloadMetadata;
    procDownloadMetadata.start(m_sWget, QStringList() << "-O" << "-" << sInyokaUrl + "/" + sArticlename + "?action=metaexport");

    if ( !procDownloadMetadata.waitForStarted() )
    {
        QMessageBox::critical(m_pParent, m_sAppName, tr("Could not start download of the meta data."));
        procDownloadMetadata.kill();
        return;
    }
    if ( !procDownloadMetadata.waitForFinished() )
    {
        QMessageBox::critical(m_pParent, m_sAppName, tr("Error while downloading meta data."));
        procDownloadMetadata.kill();
        return;
    }

    tempResult = procDownloadMetadata.readAll();
    sMetadata = QString::fromLocal8Bit(tempResult);

    // Site does not exist etc.
    if ( "" == sMetadata )
    {
        QMessageBox::information(m_pParent, m_sAppName, tr("Could not find meta data."));
        return;
    }

    // Copy metadata line by line in list
    sListTmp << sMetadata.split("\n");

    // Get only attachments article metadata
    for ( int i = 0; i < sListTmp.size(); i++ )
    {
        if ( sListTmp[i].startsWith("X-Attach: " + sArticlename + "/", Qt::CaseInsensitive) )
        {
            sListMetadata << sListTmp[i];
            //qDebug() << sListTmp[i];
        }
    }

    // If attachments exist
    if ( sListMetadata.size() > 0 )
    {
        // Ask if images should be downloaded (if not enabled by default in settings)
        if ( false == bAutomaticImageDownload )
        {
            iRet = QMessageBox::question(m_pParent, m_sAppName, tr("Do you want to download the images which are attached to the article?"), QMessageBox::Yes, QMessageBox::No);
        }
        else
        {
            iRet = QMessageBox::Yes;
        }

        if ( QMessageBox::Yes == iRet )
        {
            // File for download script
            QFile tmpScriptfile(ImgDir.absolutePath() + "/" + sScriptName);

            // No write permission
            if ( !tmpScriptfile.open(QFile::WriteOnly | QFile::Text) )
            {
                QMessageBox::warning(m_pParent, m_sAppName, tr("Could not create temporary download file!"));
                return;
            }

            // Stream for output in file
            QTextStream scriptOutputstream(&tmpScriptfile);
            scriptOutputstream << "#!/bin/bash\n"
                                  "# Temporary script for downloading images from an article\n"
                                  "#\n\necho \"Downloading images...\"\n"
                                  "cd \"" << ImgDir.absolutePath() << "\"" << endl;  // "..." impoartant for windows because folder can contain white spaces!

            // Write wget download lines
            QString sTmp("");
            for ( int j = 0; j < sListMetadata.size(); j++ )
            {
                // Trim image infos lines
                sListMetadata[j].remove("X-Attach: ");  // Remove "X-Attach: "
                sListMetadata[j].remove("\r");  // Remove windows specific newline \r

                // http://wiki.ubuntuusers.de/_image?target=Kontact/uebersicht.png
                sTmp = sListMetadata[j];
                sTmp = sTmp.remove(sArticlename + "/", Qt::CaseInsensitive);
                scriptOutputstream << "wget -nv " << sInyokaUrl << "/_image?target=" << sListMetadata[j].toLower() << " -O " << sTmp << endl;
            }
            scriptOutputstream << "sleep 2\n"
                                  "echo \"Finished image download.\"\n" << endl;

            tmpScriptfile.close();

            // Make script executable
            tmpScriptfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther);

            CProgressDialog *myImageDownloadProgress;
            // Start download script
            try
            {
                QString sTmpFilePath = ImgDir.absolutePath() + "/" + sScriptName;
#if defined _WIN32
                myImageDownloadProgress = new CProgressDialog(m_sWinBashFolder + "bash.exe ", QStringList() << sTmpFilePath << ImgDir.absolutePath(), m_sAppName, m_pParent);
#else
                myImageDownloadProgress = new CProgressDialog(sTmpFilePath, QStringList() << ImgDir.absolutePath(), m_sAppName, m_pParent);
#endif
            }
            catch ( std::bad_alloc& ba )
            {
                std::cerr << "ERROR: Caught bad_alloc in \"downloadImages()\": " << ba.what() << std::endl;
                QMessageBox::critical(m_pParent, m_sAppName, "Error while memory allocation: bad_alloc - myImageDownloadProgress");
                exit (-9);
            }

            myImageDownloadProgress->open();
        }
    }
}
