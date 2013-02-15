/**
 * \file CFileOperations.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * File operations: New, load, save...
 */

#include <QDebug>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QWebView>

#include "./CFileOperations.h"

CFileOperations::CFileOperations(QWidget *pParent, CTextEditor *pEditor,
                                 CSettings *pSettings,
                                 const QString &sAppName,
                                 const QString &sPreviewFile)
    : m_pParent(pParent),
      m_pEditor(pEditor),
      m_pSettings(pSettings),
      m_sAppName(sAppName),
      m_sPreviewFile(sPreviewFile) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    // Generate recent files list
    m_pSigMapLastOpenedFiles = new QSignalMapper(this);
    for (int i = 0; i < m_pSettings->getMaxNumOfRecentFiles(); i++) {
        if (i < m_pSettings->getRecentFiles().size()) {
            m_LastOpenedFilesAct << new QAction(
                                        m_pSettings->getRecentFiles()[i], this);
        } else {
            m_LastOpenedFilesAct << new QAction("EMPTY", this);
            m_LastOpenedFilesAct[i]->setVisible(false);
        }
        m_pSigMapLastOpenedFiles->setMapping(m_LastOpenedFilesAct[i], i);
        connect(m_LastOpenedFilesAct[i], SIGNAL(triggered()),
                m_pSigMapLastOpenedFiles, SLOT(map()));
    }
    connect(m_pSigMapLastOpenedFiles, SIGNAL(mapped(int)),
            this, SLOT(openRecentFile(int)));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::newFile() {
    if (this->maybeSave()) {
        m_pEditor->clear();
        this->setCurrentFile("");
        emit this->loadedFile();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::open() {
    if (this->maybeSave()) {
        // File dialog opens last used folder
        QString sFileName = QFileDialog::getOpenFileName(
                    m_pParent, tr("Open file"),
                    m_pSettings->getLastOpenedDir().absolutePath());
        if (!sFileName.isEmpty()) {
            QFileInfo tmpFI(sFileName);
            m_pSettings->setLastOpenedDir(tmpFI.absoluteDir());
            this->loadFile(sFileName, true, false);
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::openRecentFile(const int nEntry) {
    this->loadFile(m_pSettings->getRecentFiles()[nEntry], true, true);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::save() {
    if (m_sCurFile.isEmpty()) {
        return this->saveAs();
    } else {
        return this->saveFile(m_sCurFile);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::saveAs() {
    QString sCurFileName("");
    if ("" != m_sCurFile) {
        sCurFileName = m_pSettings->getLastOpenedDir().absolutePath()
                + "/" + m_sCurFile;
    } else {
        sCurFileName = m_pSettings->getLastOpenedDir().absolutePath();
    }

    // File dialog opens last used folder
    QString sFileName = QFileDialog::getSaveFileName(
                m_pParent, tr("Save file"), sCurFileName);
    if (sFileName.isEmpty()) {
        return false;
    }

    QFileInfo tmpFI(sFileName);
    m_pSettings->setLastOpenedDir(tmpFI.absoluteDir());

    return this->saveFile(sFileName);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Handle unsaved files
bool CFileOperations::maybeSave() {
    if (m_pEditor->document()->isModified()) {
        QMessageBox::StandardButton ret;
        QString sTempCurFileName;
        if ("" == m_sCurFile) {
            sTempCurFileName = tr("Untitled", "No file name set");
        } else {
            QFileInfo tempCurFile(m_sCurFile);
            sTempCurFileName = tempCurFile.fileName();
        }

        ret = QMessageBox::warning(m_pParent, m_sAppName,
                                   tr("The document \"%1\" has been modified.\n"
                                      "Do you want to save your changes or "
                                      "discard them?").arg(sTempCurFileName),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);

        if (QMessageBox::Save == ret) {
            return save();
        } else if (QMessageBox::Cancel == ret) {
            return false;
        }
    }
    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::loadFile(const QString &sFileName,
                               const bool bUpdateRecent,
                               const bool bCheckSave) {
    if (bCheckSave) {
        if (!this->maybeSave()) {
            return;
        }
    }

    QFile file(sFileName);
    // No permission to read
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(m_pParent, m_sAppName,
                             tr("The file \"%1\" could not be opened:\n%2.")
                             .arg(sFileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    in.setAutoDetectUnicode(true);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    m_pEditor->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    // Do not update recent files if template is loaded
    if (bUpdateRecent) {
        this->updateRecentFiles(sFileName);
        this->setCurrentFile(sFileName);
    } else {
        this->setCurrentFile("");
    }

    emit this->loadedFile();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::saveFile(const QString &sFileName) {
    QFile file(sFileName);
    // No write permission
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(m_pParent, m_sAppName,
                             tr("The file \"%1\" could not be saved:\n%2.")
                             .arg(sFileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out.setAutoDetectUnicode(true);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << m_pEditor->toPlainText();
    file.close();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    this->updateRecentFiles(sFileName);
    this->setCurrentFile(sFileName);

    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::printPreview() {
#ifndef _WIN32
    QWebView myPreviewWebView;
    QPrinter myPrinter;

    // Configure printer : format A4, PDF
    myPrinter.setPageSize(QPrinter::A4);
    myPrinter.setFullPage(true);
    myPrinter.setOrientation(QPrinter::Portrait);
    myPrinter.setPrintRange(QPrinter::AllPages);
    myPrinter.setOutputFormat(QPrinter::PdfFormat);
    myPrinter.setOutputFileName("Preview.pdf");  // Default name

    // Load preview from url
    myPreviewWebView.load(QUrl::fromLocalFile(m_sPreviewFile));

    QPrintDialog myPrintDialog(&myPrinter);
    if (QDialog::Accepted == myPrintDialog.exec()) {
        myPreviewWebView.print(&myPrinter);
    }
#else
    QMessageBox::information(m_pParent, m_sAppName,
                             tr("Printing not supported under Windows, yet."));
#endif
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CFileOperations::getCurrentFile() const {
    return m_sCurFile;
}

// ----------------------------------------------------------------------------

void CFileOperations::setCurrentFile(const QString &sFileName) {
    m_sCurFile = sFileName;
    m_pEditor->document()->setModified(false);
    m_pParent->setWindowModified(false);

    QString sShownName = m_sCurFile;
    if (m_sCurFile.isEmpty()) {
        sShownName = tr("Untitled");
    }
    m_pParent->setWindowFilePath(sShownName);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QList<QAction *> CFileOperations::getLastOpenedFiles() const {
    return m_LastOpenedFilesAct;
}

// ----------------------------------------------------------------------------

void CFileOperations::updateRecentFiles(const QString &sFileName) {
    QStringList sListTmp;

    if ("_-CL3AR#R3C3NT#F!L35-_" != sFileName) {
        sListTmp = m_pSettings->getRecentFiles();

        // Remove entry if exists
        if (sListTmp.contains(sFileName)) {
            sListTmp.removeAll(sFileName);
        }
        // Add file name to list
        sListTmp.push_front(sFileName);

        // Remove all entries from end, if list is too long
        while (sListTmp.size() > m_pSettings->getMaxNumOfRecentFiles()
               || sListTmp.size() > m_pSettings->getNumOfRecentFiles()) {
            sListTmp.removeLast();
        }

        for (int i = 0; i < m_pSettings->getMaxNumOfRecentFiles(); i++) {
            // Set list menu entries
            if (i < sListTmp.size()) {
                m_LastOpenedFilesAct[i]->setText(sListTmp[i]);
                m_LastOpenedFilesAct[i]->setVisible(true);
            } else {
                m_LastOpenedFilesAct[i]->setVisible(false);
            }
        }
        if (sListTmp.size() > 0) {
            emit this->setMenuLastOpenedEnabled(true);
        }
    } else {
        // Clear list
        sListTmp.clear();
        emit this->setMenuLastOpenedEnabled(false);
    }

    m_pSettings->setRecentFiles(sListTmp);
}

// ----------------------------------------------------------------------------

void CFileOperations::clearRecentFiles() {
    this->updateRecentFiles("_-CL3AR#R3C3NT#F!L35-_");
}
