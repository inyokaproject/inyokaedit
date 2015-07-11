/**
 * \file CFileOperations.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2015 The InyokaEdit developers
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

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QScrollBar>

#if QT_VERSION >= 0x050000
    #include <QtWebKitWidgets/QWebView>
#else
    #include <QWebView>
#endif

#include "./CFileOperations.h"

CFileOperations::CFileOperations(QWidget *pParent, QTabWidget *pTabWidget,
                                 CSettings *pSettings, const QString &sPreviewFile,
                                 QString sUserDataDir, QStringList sListTplMacros)
    : m_pParent(pParent),
      m_pDocumentTabs(pTabWidget),
      m_pCurrentEditor(NULL),
      m_pSettings(pSettings),
      m_sPreviewFile(sPreviewFile),
      m_sFileFilter(trUtf8("Inyoka document") + " (*.iny *.inyoka);;"
                    + trUtf8("All files") + " (*)"),
      m_bLoadPreview(false),
      m_bCloseApp(false),
      m_sUserDataDir(sUserDataDir),
      m_sListTplMacros(sListTplMacros) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pFindReplace = new CFindReplace();
    connect(this, SIGNAL(triggeredFind()),
            m_pFindReplace, SLOT(callFind()));
    connect(this, SIGNAL(triggeredReplace()),
            m_pFindReplace, SLOT(callReplace()));
    connect(this, SIGNAL(triggeredFindNext()),
            m_pFindReplace, SLOT(findNext()));
    connect(this, SIGNAL(triggeredFindPrevious()),
            m_pFindReplace, SLOT(findPrevious()));

    // Install auto save timer
    m_pTimerAutosave = new QTimer(this);
    connect(m_pTimerAutosave, SIGNAL(timeout()),
            this, SLOT(saveDocumentAuto()));

    this->newFile();

    connect(m_pDocumentTabs, SIGNAL(currentChanged(int)),
            this, SLOT(changedDocTab(int)));
    connect(m_pDocumentTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(closeDocument(int)));

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

    // Clear recent files list
    m_LastOpenedFilesAct.append(new QAction(this));
    m_LastOpenedFilesAct.last()->setSeparator(true);
    m_pClearRecentFilesAct = new QAction(trUtf8("Clear list"), this);
    m_LastOpenedFilesAct << m_pClearRecentFilesAct;

    connect(m_pClearRecentFilesAct, SIGNAL(triggered()),
            this, SLOT(clearRecentFiles()));

    m_pSigMapOpenTemplate = new QSignalMapper(this);
    connect(m_pSigMapOpenTemplate, SIGNAL(mapped(QString)),
            this, SLOT(loadFile(QString)));
    connect(m_pSettings, SIGNAL(updateEditorSettings()),
            this, SLOT(updateEditorSettings()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::newFile() {
    this->newFile("");
}

void CFileOperations::newFile(QString sFileName) {
    static quint8 nCntDocs = 0;

    m_pCurrentEditor = new CTextEditor(m_sListTplMacros, m_pParent);
    m_pListEditors << m_pCurrentEditor;
    m_pCurrentEditor->installEventFilter(m_pParent);

    if (sFileName.isEmpty() || "!_TPL_!" == sFileName) {
        if ("!_TPL_!" == sFileName) {
            m_bLoadPreview = false;
        } else {
            m_bLoadPreview = true;
        }
        nCntDocs++;
        sFileName = trUtf8("Untitled");
        if (nCntDocs > 1 && nCntDocs < 254) {
            sFileName += " (" + QString::number(nCntDocs) + ")";
        }
    } else {
        m_bLoadPreview = false;
    }

    QFileInfo file(sFileName);
    m_pCurrentEditor->setFileName(sFileName);
    this->setCurrentEditor();
    m_pDocumentTabs->addTab(m_pCurrentEditor, file.fileName());
    m_pDocumentTabs->setTabToolTip(m_pDocumentTabs->count() - 1,
                                   m_pCurrentEditor->getFileName());

    connect(m_pCurrentEditor, SIGNAL(documentChanged(bool)),
            this, SIGNAL(modifiedDoc(bool)));
    connect(m_pCurrentEditor, SIGNAL(copyAvailable(bool)),
            this, SIGNAL(copyAvailable(bool)));
    connect(m_pCurrentEditor, SIGNAL(undoAvailable(bool)),
            this, SIGNAL(undoAvailable(bool)));
    connect(m_pCurrentEditor, SIGNAL(redoAvailable(bool)),
            this, SIGNAL(redoAvailable(bool)));
    connect(m_pCurrentEditor->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SIGNAL(movedEditorScrollbar()));

    m_pDocumentTabs->setCurrentIndex(m_pDocumentTabs->count() - 1);
    this->updateEditorSettings();
    emit this->newEditor();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::open() {
    // File dialog opens last used folder
    QString sFileName = QFileDialog::getOpenFileName(
                m_pParent, trUtf8("Open file"),
                m_pSettings->getLastOpenedDir().absolutePath(),
                m_sFileFilter);
    if (!sFileName.isEmpty()) {
        QFileInfo tmpFI(sFileName);
        m_pSettings->setLastOpenedDir(tmpFI.absoluteDir());
        this->loadFile(sFileName, true);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::openRecentFile(const int nEntry) {
    this->loadFile(m_pSettings->getRecentFiles()[nEntry], true);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::save() {
    if (m_pCurrentEditor->getFileName().isEmpty() ||
            m_pCurrentEditor->getFileName().contains(trUtf8("Untitled"))) {
        return this->saveAs();
    } else {
        return this->saveFile(m_pCurrentEditor->getFileName());
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::saveAs() {
    QString sCurFileName(trUtf8("Untitled"));
    if (!m_pCurrentEditor->getFileName().isEmpty() &&
            !m_pCurrentEditor->getFileName().contains(sCurFileName)) {
        sCurFileName = m_pCurrentEditor->getFileName();
    } else {
        sCurFileName = m_pSettings->getLastOpenedDir().absolutePath();
    }

    QFileDialog saveDialog(m_pParent);
    saveDialog.setDefaultSuffix("iny");
    QString sFileName = saveDialog.getSaveFileName(m_pParent,
                                                   trUtf8("Save file"),
                                                   sCurFileName,
                                                   m_sFileFilter);
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
    if (m_pCurrentEditor->document()->isModified()) {
        QMessageBox::StandardButton ret;
        QString sTempCurFileName;
        if (m_pCurrentEditor->getFileName().isEmpty()) {
            sTempCurFileName = trUtf8("Untitled");
        } else {
            QFileInfo tempCurFile(m_pCurrentEditor->getFileName());
            sTempCurFileName = tempCurFile.fileName();
        }

        ret = QMessageBox::warning(m_pParent, qApp->applicationName(),
                                   trUtf8("The document \"%1\" has been modified.\n"
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
                               const bool bUpdateRecent) {
    QString sTmpName(sFileName);
    QFile file(sTmpName);
    // No permission to read
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(m_pParent, qApp->applicationName(),
                             trUtf8("The file \"%1\" could not be opened:\n%2.")
                             .arg(sTmpName)
                             .arg(file.errorString()));
        qWarning() << "File" << sTmpName << "could not be opened:"
                   << file.errorString();
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    in.setAutoDetectUnicode(true);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    m_bLoadPreview = false;
    if (sTmpName.endsWith(".tpl")) {
        sTmpName = "!_TPL_!";
    }
    this->newFile(sTmpName);
    m_pCurrentEditor->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    // Do not update recent files if template is loaded
    if (bUpdateRecent) {
        this->updateRecentFiles(sTmpName);
    }

    emit this->callPreview();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::saveFile(const QString &sFileName) {
    QFile file(sFileName);
    // No write permission
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(m_pParent, qApp->applicationName(),
                             trUtf8("The file \"%1\" could not be saved:\n%2.")
                             .arg(sFileName)
                             .arg(file.errorString()));
        qWarning() << "File" << sFileName << "could not be saved:"
                   << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out.setAutoDetectUnicode(true);

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << m_pCurrentEditor->toPlainText();
    file.close();

    m_pCurrentEditor->setFileName(sFileName);
    QFileInfo fi(m_pCurrentEditor->getFileName());
    m_pDocumentTabs->setTabText(m_pDocumentTabs->indexOf(m_pCurrentEditor),
                               fi.fileName());
    m_pDocumentTabs->setTabToolTip(m_pDocumentTabs->indexOf(m_pCurrentEditor),
                                   m_pCurrentEditor->getFileName());
    m_pCurrentEditor->document()->setModified(false);
    this->setCurrentEditor();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    this->updateRecentFiles(sFileName);
    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::saveDocumentAuto() {
    if (!m_bCloseApp) {
        qDebug() << "Calling" << Q_FUNC_INFO;
        QFile fAutoSave;
        for (int i = 0; i < m_pListEditors.count(); i++) {
            if (NULL != m_pListEditors[i]) {
                if (m_pListEditors[i]->getFileName().contains(
                            trUtf8("Untitled"))) {
                    fAutoSave.setFileName(m_sUserDataDir + "/AutoSave" +
                                          QString::number(i) + ".bak~");
                } else {
                    fAutoSave.setFileName(
                                m_pListEditors[i]->getFileName() + ".bak~");
                }
                QTextStream outStream(&fAutoSave);
                outStream.setCodec("UTF-8");
                outStream.setAutoDetectUnicode(true);

                // No write permission
                if (!fAutoSave.open(QFile::WriteOnly | QFile::Text)) {
                    qWarning() << "Could not open auto backup"
                               << fAutoSave.fileName() << "file!";
                    return;
                }

                outStream << m_pListEditors[i]->toPlainText();
                fAutoSave.close();
            }
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::printPreview() {
    QWebView previewWebView;
    QPrinter printer;
    QFile previewFile(m_sPreviewFile);
    QString sHtml("");

    QList <QPrinterInfo> listPrinters = QPrinterInfo::availablePrinters();
    if (0 == listPrinters.size()) {
        QMessageBox::warning(m_pParent, qApp->applicationName(),
                             trUtf8("No supported printer found."));
        return;
    } else {
        foreach (QPrinterInfo info, listPrinters) {
            qDebug() << "Found printers" << info.printerName();
        }
    }

    // Configure printer: format A4, PDF
    printer.setPaperSize(QPrinter::A4);
    printer.setFullPage(true);
    printer.setOrientation(QPrinter::Portrait);
    printer.setPrintRange(QPrinter::AllPages);
#ifndef _WIN32
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(m_pSettings->getLastOpenedDir().absolutePath()
                                + "/Preview.pdf");
#else
    printer.setOutputFormat(QPrinter::NativeFormat);
#endif

    if (!previewFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(0, trUtf8("Warning"),
                             trUtf8("Could not open preview file for printing!"));
        qWarning() << "Could not open text html preview file for printing:"
                   << m_sPreviewFile;
        return;
    } else {
        QTextStream in(&previewFile);
        QString sTmpLine1("");
        QString sTmpLine2("");
        while (!in.atEnd()) {
            sTmpLine1 = in.readLine() + "\n";
            sTmpLine2 = in.readLine() + "\n";
            // If line == </body> skip previous line
            // See below: <div class=\"wrap\">...</div>\n</body>)
            if ("</body>" == sTmpLine2.trimmed()) {
                sHtml += sTmpLine2;
            } else {
                sHtml += sTmpLine1;
                sHtml += sTmpLine2;
            }
        }
        previewFile.close();
    }

    /*
    // Load preview from url
    previewWebView.load(QUrl::fromLocalFile(m_sPreviewFile));
    */

    // Add style format; remove unwanted div for printing
    sHtml.replace("</style>",
                  "html{background-color:#ffffff; margin:40px;}\n"
                  "body{background-color:#ffffff;\n</style>");
    sHtml.remove("<div class=\"wrap\">");

    previewWebView.setHtml(sHtml, QUrl::fromLocalFile(
                                 QFileInfo(m_sPreviewFile)
                                 .absoluteDir().absolutePath() + "/"));

    /*
    previewWebView.setContent(sHtml.toLocal8Bit(), "application/xhtml+xml",
                                QUrl::fromLocalFile(QFileInfo(m_sPreviewFile)
                                                    .absoluteDir()
                                                    .absolutePath() + "/"));
    */

    QPrintDialog printDialog(&printer);
    if (QDialog::Accepted == printDialog.exec()) {
        previewWebView.print(&printer);
    }
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::changedDocTab(int nIndex) {
    if (nIndex < m_pListEditors.size() && nIndex >= 0) {
        m_pCurrentEditor = m_pListEditors[nIndex];
        this->setCurrentEditor();

        if (!m_bCloseApp && m_bLoadPreview) {
            emit this->callPreview();
        }
        m_bLoadPreview = true;
        emit this->modifiedDoc(m_pCurrentEditor->document()->isModified());
        emit this->undoAvailable2(m_pCurrentEditor->document()->isUndoAvailable());
        emit this->redoAvailable2(m_pCurrentEditor->document()->isRedoAvailable());
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::copy() {
    m_pCurrentEditor->copy();
}
void CFileOperations::cut() {
    m_pCurrentEditor->cut();
}
void CFileOperations::paste() {
    m_pCurrentEditor->paste();
}
void CFileOperations::undo() {
    m_pCurrentEditor->undo();
}
void CFileOperations::redo() {
    m_pCurrentEditor->redo();
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CFileOperations::updateEditorSettings() {
    foreach (CTextEditor *pEditor, m_pListEditors) {
        pEditor->setFont(m_pSettings->getEditorFont());
        pEditor->updateTextEditorSettings(m_pSettings->getCodeCompletion());
    }

    m_pTimerAutosave->stop();
    if (0 != m_pSettings->getAutoSave()) {
        m_pTimerAutosave->setInterval(m_pSettings->getAutoSave() * 1000);
        m_pTimerAutosave->start();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::closeDocument(int nIndex) {
    m_pDocumentTabs->setCurrentIndex(nIndex);
    m_pCurrentEditor = m_pListEditors[nIndex];
    m_bLoadPreview = false;
    this->setCurrentEditor();

    if (this->maybeSave()) {
        m_pDocumentTabs->removeTab(nIndex);
        m_pListEditors[nIndex]->deleteLater();
        m_pListEditors[nIndex] = NULL;
        m_pListEditors.removeAt(nIndex);

        if (m_pDocumentTabs->count() > 0) {
            m_pCurrentEditor = m_pListEditors.last();
            this->setCurrentEditor();
            m_pDocumentTabs->setCurrentIndex(m_pDocumentTabs->count() - 1);
            if (nIndex == m_pDocumentTabs->count() && !m_bCloseApp) {
                emit this->callPreview();
            }
        } else if (!m_bCloseApp) {
            this->newFile();
        }
    } else {
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CFileOperations::closeAllmaybeSave() {
    m_bCloseApp = true;
    for (int i = m_pDocumentTabs->count() - 1; i >= 0; i--) {
        if (!this->closeDocument(i)) {
            m_bCloseApp = false;
            return false;
        }
    }
    m_bCloseApp = false;
    return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CFileOperations::getCurrentFile() const {
    QFileInfo file(m_pCurrentEditor->getFileName());
    return file.baseName();
}

void CFileOperations::setCurrentEditor() {
    QFileInfo file(m_pCurrentEditor->getFileName());
    m_pParent->setWindowFilePath(file.fileName());
    emit this->changedCurrentEditor();
    m_pFindReplace->setEditor(m_pCurrentEditor);
}

CTextEditor* CFileOperations::getCurrentEditor() {
    return m_pCurrentEditor;
}

QList<CTextEditor *> CFileOperations::getEditors() const {
    return m_pListEditors;
}
