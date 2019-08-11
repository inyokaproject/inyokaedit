/**
 * \file main.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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
 * Main function, start application, loading translation.
 */

/** \mainpage
 * \section Introduction
 * InyokaEdit is a small markup editor for Inyoka articles
 * (http://ubuntuusers.de/inyoka/).<br /><br />Some features:<br />
 * \li Syntax highlighting
 * \li Offline preview
 * \li Downloading raw text of existing articles and corresponding images.
 * \li Spell checker
 *
 * \section GitHub
 * InyokaEdit on GitHub: https://github.com/inyokaproject/inyokaedit
 */

#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QTextStream>

#include "./inyokaedit.h"

static QFile logfile;
static QTextStream out(&logfile);

void setupLogger(const QString &sDebugFilePath);
void LoggingHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &sMsg);

// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName(APP_NAME);
  app.setApplicationVersion(APP_VERSION);

  if (app.arguments().contains("-v") ||
      app.arguments().contains("--version")) {
    qDebug() << app.arguments().at(0) << "\t" <<
                app.applicationVersion() << "\n";
    exit(0);
  }

  // User data directory
  QStringList sListPaths = QStandardPaths::standardLocations(
                             QStandardPaths::DataLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting data standard path.";
    sListPaths << "";
  }
  const QDir userDataDir(sListPaths[0].toLower());

  // Default share data path (Windows and debugging)
  QString sSharePath = app.applicationDirPath();
  // Standard installation path (Linux)
  QDir tmpDir(app.applicationDirPath() + "/../share/"
              + app.applicationName().toLower());
  if (!app.arguments().contains("--debug") && tmpDir.exists()) {
    sSharePath = app.applicationDirPath() + "/../share/"
                 + app.applicationName().toLower();
  }

  const QString sDebugFile("debug.log");
  if (!userDataDir.exists()) {
    // Create folder including possible parent directories (mkPATH)!
    userDataDir.mkpath(userDataDir.absolutePath());
  }
  setupLogger(userDataDir.absolutePath() + "/" + sDebugFile);

  InyokaEdit myInyokaEdit(userDataDir, sSharePath);
  myInyokaEdit.show();
  int nRet = app.exec();

  qDebug() << "Closing" << app.applicationName();
  out.flush();
  logfile.close();
  return nRet;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void setupLogger(const QString &sDebugFilePath) {
  // Remove old debug file
  if (QFile(sDebugFilePath).exists()) {
    QFile(sDebugFilePath).remove();
  }

  // Create new file
  logfile.setFileName(sDebugFilePath);
  if (!logfile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't create logging file: " << sDebugFilePath;
  } else {
    qInstallMessageHandler(LoggingHandler);
  }

  qDebug() << qApp->applicationName() << qApp->applicationVersion();

#ifdef USEQTWEBKIT
  qDebug() << "Compiled with Qt" << QT_VERSION_STR << "+ webkitwidgets";
#else
  qDebug() << "Compiled with Qt" << QT_VERSION_STR << "+ webenginewidgets";
#endif
  qDebug() << "Qt runtime" << qVersion();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LoggingHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &sMsg) {
  QString sMsg2(sMsg);
  QString sContext = sMsg + " (" + QString(context.file) + ":"
                     + QString::number(context.line) + ", "
                     + QString(context.function) + ")";
  QString sTime(QTime::currentTime().toString());

  switch (type) {
    case QtDebugMsg:
      out << sTime << " Debug: " << sMsg2 << "\n";
      out.flush();
      break;
    case QtWarningMsg:
      out << sTime << " Warning: " << sContext << "\n";
      out.flush();
      break;
    case QtCriticalMsg:
      out << sTime << " Critical: " << sContext << "\n";
      out.flush();
      break;
    case QtFatalMsg:
      out << sTime << " Fatal: " << sContext << "\n";
      out.flush();
      logfile.close();
      abort();
    default:
      out << sTime << " OTHER INFO: " << sContext << "\n";
      out.flush();
      break;
  }
}
