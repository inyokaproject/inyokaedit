/**
 * \file fileoperations.cpp
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
 * File operations: New, load, save...
 */

#include "./fileoperations.h"

#include <QApplication>
#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTabWidget>
#include <QTimer>

#ifdef USEQTWEBKIT
#include <QtWebKitWidgets/QWebView>
#else
#include <QWebEngineView>
#endif

#include "./findreplace.h"
#include "./settings.h"
#include "./texteditor.h"
#include "./3rdparty/miniz/miniz.c"

FileOperations::FileOperations(QWidget *pParent, QTabWidget *pTabWidget,
                               Settings *pSettings, const QString &sPreviewFile,
                               const QString &sUserDataDir,
                               const QStringList &sListTplMacros)
  : m_pParent(pParent),
    m_pDocumentTabs(pTabWidget),
    m_pCurrentEditor(nullptr),
    m_pSettings(pSettings),
    m_sPreviewFile(sPreviewFile),
    m_sFileFilter(tr("Inyoka article") + " (*.iny *.inyoka);;" +
                  tr("Inyoka article + images") + " (*.inyzip);;" +
                  tr("All files") + " (*)"),
    m_bLoadPreview(false),
    m_bCloseApp(false),
    m_sUserDataDir(sUserDataDir),
    m_sExtractDir(m_sUserDataDir + "/tmpImages"),
    m_sListTplMacros(sListTplMacros) {
    qDebug() << "Using miniz version:" << MZ_VERSION;
  m_pFindReplace = new FindReplace();
  connect(this, &FileOperations::triggeredFind,
          m_pFindReplace, &FindReplace::callFind);
  connect(this, &FileOperations::triggeredReplace,
          m_pFindReplace, &FindReplace::callReplace);
  connect(this, &FileOperations::triggeredFindNext,
          m_pFindReplace, &FindReplace::findNext);
  connect(this, &FileOperations::triggeredFindPrevious,
          m_pFindReplace, &FindReplace::findPrevious);

  // Install auto save timer
  m_pTimerAutosave = new QTimer(this);
  connect(m_pTimerAutosave, &QTimer::timeout,
          this, &FileOperations::saveDocumentAuto);

  this->newFile(QString());

  connect(m_pDocumentTabs, &QTabWidget::currentChanged,
          this, &FileOperations::changedDocTab);
  connect(m_pDocumentTabs, &QTabWidget::tabCloseRequested,
          this, &FileOperations::closeDocument);

  // Generate recent files list
  for (int i = 0; i < m_pSettings->getMaxNumOfRecentFiles(); i++) {
    if (i < m_pSettings->getRecentFiles().size()) {
      m_LastOpenedFilesAct << new QAction(
                                m_pSettings->getRecentFiles().at(i), this);
    } else {
      m_LastOpenedFilesAct << new QAction(QStringLiteral("EMPTY"), this);
      m_LastOpenedFilesAct.at(i)->setVisible(false);
    }
    connect(m_LastOpenedFilesAct.at(i), &QAction::triggered,
            this, [this, i]() { openRecentFile(i); });
  }

  // Clear recent files list
  m_LastOpenedFilesAct.append(new QAction(this));
  m_LastOpenedFilesAct.last()->setSeparator(true);
  m_pClearRecentFilesAct = new QAction(tr("Clear list"), this);
  m_LastOpenedFilesAct << m_pClearRecentFilesAct;

  connect(m_pClearRecentFilesAct, &QAction::triggered,
          this, &FileOperations::clearRecentFiles);

  connect(m_pSettings, &Settings::updateEditorSettings,
          this, &FileOperations::updateEditorSettings);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileOperations::newFile(QString sFileName) {
  m_pCurrentEditor = new TextEditor(m_sListTplMacros, tr("Template"),
                                    m_pParent);
  m_pListEditors << m_pCurrentEditor;
  m_pCurrentEditor->installEventFilter(m_pParent);

  if (sFileName.isEmpty() || "!_TPL_!" == sFileName) {
    static quint8 nCntDocs = 0;
    m_bLoadPreview = ("!_TPL_!" != sFileName);

    nCntDocs++;
    sFileName = tr("Untitled");
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

  connect(m_pCurrentEditor, &TextEditor::documentChanged,
          this, &FileOperations::modifiedDoc);
  connect(m_pCurrentEditor, &TextEditor::copyAvailable,
          this, &FileOperations::copyAvailable);
  connect(m_pCurrentEditor, &TextEditor::undoAvailable,
          this, &FileOperations::undoAvailable);
  connect(m_pCurrentEditor, &TextEditor::redoAvailable,
          this, &FileOperations::redoAvailable);
  connect(m_pCurrentEditor->verticalScrollBar(), &QScrollBar::valueChanged,
          this, &FileOperations::movedEditorScrollbar);

  m_pDocumentTabs->setCurrentIndex(m_pDocumentTabs->count() - 1);
  this->updateEditorSettings();
  emit this->newEditor();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileOperations::open() {
  // File dialog opens last used folder
  QString sFileName = QFileDialog::getOpenFileName(
                        m_pParent, tr("Open file"),
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

void FileOperations::openRecentFile(const int nEntry) {
  this->loadFile(m_pSettings->getRecentFiles().at(nEntry), true);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::save() -> bool {
  if (m_pCurrentEditor->getFileName().isEmpty() ||
      m_pCurrentEditor->getFileName().contains(tr("Untitled")) ||
      !QFile::exists(m_pCurrentEditor->getFileName())) {
    return this->saveAs();
  }
  return this->saveFile(m_pCurrentEditor->getFileName());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::saveAs() -> bool {
  QString sCurFileName(tr("Untitled"));
  if (!m_pCurrentEditor->getFileName().isEmpty() &&
      !m_pCurrentEditor->getFileName().contains(sCurFileName)) {
    sCurFileName = m_pCurrentEditor->getFileName();
  } else {
    sCurFileName = m_pSettings->getLastOpenedDir().absolutePath();
  }

  QFileDialog saveDialog(m_pParent);
  saveDialog.setDefaultSuffix(QStringLiteral("iny"));
  QString sFileName = saveDialog.getSaveFileName(m_pParent,
                                                 tr("Save file"),
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
auto FileOperations::maybeSave() -> bool {
  if (m_pCurrentEditor->document()->isModified()) {
    QMessageBox::StandardButton ret;
    QString sTempCurFileName;
    if (m_pCurrentEditor->getFileName().isEmpty()) {
      sTempCurFileName = tr("Untitled");
    } else {
      QFileInfo tempCurFile(m_pCurrentEditor->getFileName());
      sTempCurFileName = tempCurFile.fileName();
    }

    ret = QMessageBox::warning(m_pParent, qApp->applicationName(),
                               tr("The document \"%1\" has been modified.\n"
                                  "Do you want to save your changes or "
                                  "discard them?").arg(sTempCurFileName),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);

    if (QMessageBox::Save == ret) {
      return save();
    }
    if (QMessageBox::Cancel == ret) {
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileOperations::loadFile(const QString &sFileName,
                              const bool bUpdateRecent,
                              const bool bArchive) {
  QString sTmpName(sFileName);
  if (sTmpName.endsWith(QLatin1String(".inyzip"))) {
    this->loadInyArchive(sTmpName);
    return;
  }

  QFile file(sTmpName);
  // No permission to read
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("The file \"%1\" could not be opened:\n%2.")
                         .arg(sTmpName, file.errorString()));
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
  if (sTmpName.endsWith(QLatin1String(".tpl"))) {
    sTmpName = QStringLiteral("!_TPL_!");
  }
  if (bArchive) {
    sTmpName.replace(QLatin1String(".iny"), QLatin1String(".inyzip"));
    sTmpName.replace(QLatin1String(".inyoka"), QLatin1String(".inyzip"));
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

void FileOperations::loadInyArchive(const QString &sArchive) {
  QString sArticle(QLatin1String(""));
  QString sOutput;
  QFileInfo file(sArchive);

  mz_zip_archive archive;
  memset(&archive, 0, sizeof(archive));
  mz_zip_archive_file_stat file_stat;

  if (!mz_zip_reader_init_file(&archive, sArchive.toLatin1(), 0)) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("The file \"%1\" could not be opened.")
                         .arg(sArchive));
    qWarning() << "mz_zip_reader_init_file() failed:" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    return;
  }

  int nFileCount = static_cast<int>(mz_zip_reader_get_num_files(&archive));
  if (0 == nFileCount) {
    mz_zip_reader_end(&archive);
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("The file \"%1\" is empty!").arg(sArchive));
    qWarning() << "Archive is empty!";
    return;
  }

  if (!mz_zip_reader_file_stat(&archive, 0, &file_stat)) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("Error reading \"%1\"").arg(sArchive));
    qWarning() << "Error reading archive!" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    mz_zip_reader_end(&archive);
    return;
  }

  // Go through each file from archive
  for (int i = 0; i < nFileCount; i++) {
    if (!mz_zip_reader_file_stat(
          &archive, static_cast<mz_uint>(i), &file_stat)) {
      QMessageBox::critical(m_pParent, qApp->applicationName(),
                            tr("Something went wrong while reading \"%1\"")
                            .arg(sArchive));
      qWarning() << "mz_zip_reader_file_stat() failed:" <<
                    mz_zip_get_error_string(mz_zip_get_last_error(&archive));
      mz_zip_reader_end(&archive);
      return;
    }
    // qDebug() << "Extracting:" << file_stat.m_filename;

    sOutput = file_stat.m_filename;

    if (sOutput.endsWith(QLatin1String(".iny")) ||
        sOutput.endsWith(QLatin1String(".inyoka"))) {
      // Extract article into same folder as archive
      sArticle = file.absolutePath() + "/" + sOutput;
      sOutput = sArticle;
    } else {
      // Extract images into tmpImages folder in user directory
      sOutput = m_sExtractDir + "/" + sOutput;
    }

    if (!mz_zip_reader_extract_to_file(
          &archive, static_cast<mz_uint>(i), sOutput.toLatin1(), 0)) {
      QMessageBox::critical(
            m_pParent, qApp->applicationName(),
            tr("Error while extracting \"%1\" from archive!")
            .arg(file_stat.m_filename));
      qWarning() << "mz_zip_reader_extract_to_file() failed:" <<
                    mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    }
  }

  // Close the archive, freeing any resources it was using
  if (!mz_zip_reader_end(&archive)) {
    QMessageBox::critical(m_pParent, qApp->applicationName(),
                          tr("Error while extracting archive!"));
    qWarning() << "mz_zip_reader_end() failed:" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
  }

  this->loadFile(sArticle, true, true);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::saveFile(QString sFileName) -> bool {
  QFile file;
  if (sFileName.endsWith(QLatin1String(".inyzip"))) {
    // Special characters not allowed for miniz achives
    sFileName.replace(QStringLiteral("Ä"), QLatin1String("Ae"));
    sFileName.replace(QStringLiteral("Ü"), QLatin1String("Ue"));
    sFileName.replace(QStringLiteral("Ö"), QLatin1String("Oe"));
    sFileName.replace(QStringLiteral("ä"), QLatin1String("ae"));
    sFileName.replace(QStringLiteral("ü"), QLatin1String("ue"));
    sFileName.replace(QStringLiteral("ö"), QLatin1String("oe"));

    QString sFile(sFileName);
    file.setFileName(sFile.replace(QLatin1String(".inyzip"),
                                   QLatin1String(".iny")));
  } else {
    file.setFileName(sFileName);
  }

  // No write permission
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("The file \"%1\" could not be saved:\n%2.")
                         .arg(sFileName, file.errorString()));
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

  if (sFileName.endsWith(QLatin1String(".inyzip"))) {
    if (!this->saveInyArchive(sFileName)) {
      return false;
    }
  }

  this->updateRecentFiles(sFileName);
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::saveInyArchive(const QString &sArchive) -> bool {
  QFileInfo file(sArchive);
  QString sArticle(file.baseName() + ".iny");
  QByteArray baComment("");

  mz_zip_archive archive;
  memset(&archive, 0, sizeof(archive));

  if (!mz_zip_writer_init_file(&archive, sArchive.toLatin1(), 65537)) {
    QMessageBox::critical(m_pParent, qApp->applicationName(),
                          tr("Error while creating archive \"%1\"")
                          .arg(sArchive));
    qWarning() << "mz_zip_writer_init_file() failed:" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    return false;
  }

  // Grab images from html preview
  QFile html(m_sPreviewFile);
  // No permission to read
  if (!html.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("Error while packing image files:\n%1.")
                         .arg(html.errorString()));
    qWarning() << "Error while packing image files:" << html.errorString();
    return false;
  }

  QTextStream in(&html);
  in.setCodec("UTF-8");
  in.setAutoDetectUnicode(true);
  QString sHtml(in.readAll());

  QRegularExpression imgTagRegex(
        QStringLiteral("\\<img[^\\>]*src\\s*=\\s*\"([^\"]*)\"[^\\>]*\\>"),
        QRegularExpression::InvertedGreedinessOption |
        QRegularExpression::DotMatchesEverythingOption |
        QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatchIterator it = imgTagRegex.globalMatch(sHtml);

  QFileInfo img;
  QStringList sListFiles;

  while (it.hasNext()) {
    QRegularExpressionMatch match = it.next();
    img.setFile(match.captured(1).remove(
                  QStringLiteral("file:///")));  // (Windows file:///)
    if (sListFiles.contains(img.fileName()) ||
        img.absoluteFilePath().contains(QLatin1String("/community/"),
                                        Qt::CaseInsensitive)) {
      continue;  // Filter duplicates or community images
    }
    sListFiles << img.fileName();

    if (!mz_zip_writer_add_file(&archive, img.fileName().toLatin1(),
                                img.absoluteFilePath().toLatin1(),
                                baComment,
                                static_cast<mz_uint16>(baComment.size()),
                                MZ_BEST_COMPRESSION)) {
      QMessageBox::critical(m_pParent, qApp->applicationName(),
                            tr("Error while adding \"%1\" to archive!")
                            .arg(img.absoluteFilePath()));
      qWarning() << "Error while adding" <<
                    img.absoluteFilePath() << "to archive!:" <<
                    mz_zip_get_error_string(mz_zip_get_last_error(&archive));
      return false;
    }
  }

  if (!mz_zip_writer_add_file(&archive, sArticle.toLatin1(),
                              QString(file.absolutePath() + "/" +
                                      sArticle.toLatin1()).toLatin1(),
                              baComment,
                              static_cast<mz_uint16>(baComment.size()),
                              MZ_BEST_COMPRESSION)) {
    QMessageBox::critical(m_pParent, qApp->applicationName(),
                          tr("Error while adding \"%1\" to archive!")
                          .arg(sArticle));
    qWarning() << "Error while adding" <<
                  img.absoluteFilePath() << "to archive!:" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    return false;
  }

  if (!mz_zip_writer_finalize_archive(&archive)) {
    mz_zip_writer_end(&archive);
    QFile::remove(sArchive);
    QMessageBox::critical(m_pParent, qApp->applicationName(),
                          tr("Error while finalizing archive!"));
    qWarning() << "mz_zip_writer_finalize_archive() failed:" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    return false;
  }

  // Close the archive, freeing any resources it was using
  if (!mz_zip_writer_end(&archive)) {
    QMessageBox::critical(m_pParent, qApp->applicationName(),
                          tr("Error while creating archive!"));
    qWarning() << "mz_zip_writer_end() failed:" <<
                  mz_zip_get_error_string(mz_zip_get_last_error(&archive));
    return false;
  }

  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileOperations::saveDocumentAuto() {
  if (!m_bCloseApp) {
    qDebug() << "Calling" << Q_FUNC_INFO;
    QFile fAutoSave;
    for (int i = 0; i < m_pListEditors.count(); i++) {
      if (nullptr != m_pListEditors.at(i)) {
        if (m_pListEditors.at(i)->getFileName().contains(tr("Untitled"))) {
          fAutoSave.setFileName(m_sUserDataDir + "/AutoSave" +
                                QString::number(i) + ".bak~");
        } else if (m_pListEditors.at(i)->getFileName().endsWith(
                     QLatin1String(".inyzip"))) {
          QString sName(m_pListEditors.at(i)->getFileName().replace(
                          QLatin1String(".inyzip"),
                          QLatin1String(".iny.bak~")));
          fAutoSave.setFileName(sName);
        } else {
          fAutoSave.setFileName(
                m_pListEditors.at(i)->getFileName() + ".bak~");
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

void FileOperations::printPreview() {
#ifdef USEQTWEBKIT
  QWebView previewWebView;
#else
  QWebEngineView previewWebView;
#endif
  QPrinter printer;
  QFile previewFile(m_sPreviewFile);
  QString sHtml(QLatin1String(""));

  QList <QPrinterInfo> listPrinters = QPrinterInfo::availablePrinters();
  if (listPrinters.isEmpty()) {
    QMessageBox::warning(m_pParent, qApp->applicationName(),
                         tr("No supported printer found."));
    return;
  }

  foreach (QPrinterInfo info, listPrinters) {
      qDebug() << "Found printers" << info.printerName();
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
    QMessageBox::warning(nullptr, tr("Warning"),
                         tr("Could not open preview file for printing!"));
    qWarning() << "Could not open text html preview file for printing:"
               << m_sPreviewFile;
    return;
  }

  QTextStream in(&previewFile);
  QString sTmpLine1;
  QString sTmpLine2;
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

  /*
  // Load preview from url
  previewWebView.load(QUrl::fromLocalFile(m_sPreviewFile));
  */

  // Add style format; remove unwanted div for printing
  sHtml.replace(QLatin1String("</style>"),
                "html{background-color:#ffffff; margin:40px;}\n"
                "body{background-color:#ffffff;\n</style>");
  sHtml.remove(QStringLiteral("<div class=\"wrap\">"));

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
#ifdef USEQTWEBKIT
    previewWebView.print(&printer);
#else
  #if QT_VERSION >= 0x050800
    previewWebView.page()->print(&printer, [=](bool){});
  #else
    QMessageBox::warning(
          0, tr("Warning"),
          "Printing not supported with Qt < 5.8.0 and QWebEngineView.");
    qWarning() << "Printing not supported with Qt < 5.8.0 and QWebEngineView.";
  #endif
#endif
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::getLastOpenedFiles() const -> QList<QAction *>{
  return m_LastOpenedFilesAct;
}

// ----------------------------------------------------------------------------

void FileOperations::updateRecentFiles(const QString &sFileName) {
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
        m_LastOpenedFilesAct.at(i)->setText(sListTmp.at(i));
        m_LastOpenedFilesAct.at(i)->setVisible(true);
      } else {
        m_LastOpenedFilesAct.at(i)->setVisible(false);
      }
    }
    if (!sListTmp.isEmpty()) {
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

void FileOperations::clearRecentFiles() {
  this->updateRecentFiles(QStringLiteral("_-CL3AR#R3C3NT#F!L35-_"));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileOperations::changedDocTab(int nIndex) {
  if (nIndex < m_pListEditors.size() && nIndex >= 0) {
    m_pCurrentEditor = m_pListEditors.at(nIndex);
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

void FileOperations::copy() {
  m_pCurrentEditor->copy();
}
void FileOperations::cut() {
  m_pCurrentEditor->cut();
}
void FileOperations::paste() {
  m_pCurrentEditor->paste();
}
void FileOperations::undo() {
  m_pCurrentEditor->undo();
}
void FileOperations::redo() {
  m_pCurrentEditor->redo();
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void FileOperations::updateEditorSettings() {
  foreach (TextEditor *pEditor, m_pListEditors) {
    pEditor->setFont(m_pSettings->getEditorFont());
    pEditor->updateTextEditorSettings(m_pSettings->getCodeCompletion());
  }

  m_pTimerAutosave->stop();
  if (0 != m_pSettings->getAutoSave()) {
    m_pTimerAutosave->setInterval(
          static_cast<int>(m_pSettings->getAutoSave() * 1000));
    m_pTimerAutosave->start();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::closeDocument(int nIndex) -> bool {
  m_pDocumentTabs->setCurrentIndex(nIndex);
  m_pCurrentEditor = m_pListEditors.at(nIndex);
  m_bLoadPreview = false;
  this->setCurrentEditor();

  if (this->maybeSave()) {
    if (m_pCurrentEditor->getFileName().endsWith(QLatin1String(".inyzip"))) {
      if (!QFile::remove(
            m_pCurrentEditor->getFileName().replace(QLatin1String(".inyzip"),
                                                    QLatin1String(".iny")))) {
        qWarning() << "Couldn't remove temp iny from archive" <<
                      m_pCurrentEditor->getFileName();
      }
    }
    m_pDocumentTabs->removeTab(nIndex);
    m_pListEditors.at(nIndex)->deleteLater();
    m_pListEditors[nIndex] = nullptr;
    m_pListEditors.removeAt(nIndex);

    if (m_pDocumentTabs->count() > 0) {
      m_pCurrentEditor = m_pListEditors.last();
      this->setCurrentEditor();
      m_pDocumentTabs->setCurrentIndex(m_pDocumentTabs->count() - 1);
      if (nIndex == m_pDocumentTabs->count() && !m_bCloseApp) {
        emit this->callPreview();
      }
    } else if (!m_bCloseApp) {
      this->newFile(QString());
    }
  } else {
    return false;
  }
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto FileOperations::closeAllmaybeSave() -> bool {
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

auto FileOperations::getCurrentFile() const -> QString {
  QFileInfo file(m_pCurrentEditor->getFileName());
  return file.absoluteFilePath();
}

void FileOperations::setCurrentEditor() {
  QFileInfo file(m_pCurrentEditor->getFileName());
  m_pParent->setWindowFilePath(file.fileName());
  emit this->changedCurrentEditor();
  m_pFindReplace->setEditor(m_pCurrentEditor);
}

auto FileOperations::getCurrentEditor() -> TextEditor* {
  return m_pCurrentEditor;
}

auto FileOperations::getEditors() const -> QList<TextEditor *> {
  return m_pListEditors;
}
