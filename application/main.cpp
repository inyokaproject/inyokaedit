// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QSslSocket>
#include <QStandardPaths>
#include <QTextStream>
#include <QTime>
#include <QtGlobal>

#include "./inyokaedit.h"

static QFile logfile;
static QTextStream out(&logfile);

void setupLogger(const QString &sDebugFilePath);
void LoggingHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &sMsg);

// ----------------------------------------------------------------------------

auto main(int argc, char *argv[]) -> int {
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  QApplication::setStyle("Fusion");  // Supports dark scheme on Win 10/11
#endif

  QApplication app(argc, argv);
  app.setApplicationName(QStringLiteral(APP_NAME));
  app.setApplicationVersion(QStringLiteral(APP_VERSION));
  app.setApplicationDisplayName(QStringLiteral(APP_NAME));
#if !defined(Q_OS_WIN)
  app.setWindowIcon(QIcon::fromTheme(
      QStringLiteral("inyokaedit"), QIcon(QStringLiteral(":/inyokaedit.png"))));
  app.setDesktopFileName(QStringLiteral("org.inyokaproject.inyokaedit"));
#endif

  QCommandLineParser cmdparser;
  cmdparser.setApplicationDescription(QStringLiteral(APP_DESC));
  cmdparser.addHelpOption();
  cmdparser.addVersionOption();
  QCommandLineOption enableDebug(QStringLiteral("debug"),
                                 QStringLiteral("Enable debug mode"));
  cmdparser.addOption(enableDebug);
  QCommandLineOption cmdShare(
      QStringList() << QStringLiteral("s") << QStringLiteral("share"),
      QString::fromLatin1("User defined share folder (folder containing "
                          "community files, plugins, etc.)"),
      QStringLiteral("Path to folder"));
  cmdparser.addOption(cmdShare);
  cmdparser.addPositionalArgument(QStringLiteral("file"),
                                  QStringLiteral("File to be opened"));
  cmdparser.process(app);

  // User data directory
  QStringList sListPaths =
      QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting data standard path.";
    sListPaths << QLatin1String("");
  }
  const QDir userDataDir(sListPaths[0].toLower());

  // Default share data path (Windows and debugging)
  QString sSharePath = app.applicationDirPath();
  // Standard installation path (Linux)
  QDir tmpDir(app.applicationDirPath() + "/../share/" +
              app.applicationName().toLower());
  if (cmdparser.isSet(cmdShare)) {  // -s overwrites debug path (app folder)
    sSharePath = cmdparser.value(cmdShare);
  } else if (!cmdparser.isSet(enableDebug) && tmpDir.exists()) {
    sSharePath = app.applicationDirPath() + "/../share/" +
                 app.applicationName().toLower();
  }

  const QString sDebugFile(QStringLiteral("debug.log"));
  if (!userDataDir.exists()) {
    // Create folder including possible parent directories (mkPATH)!
    userDataDir.mkpath(userDataDir.absolutePath());
  }
  setupLogger(userDataDir.absolutePath() + "/" + sDebugFile);
  if (cmdparser.isSet(cmdShare)) {
    qDebug() << "User defined share:" << cmdparser.value(cmdShare);
  } else if (cmdparser.isSet(enableDebug)) {  // -s overwrites debug path!
    qWarning() << "DEBUG is enabled!";
  }

  const QStringList sListArgs = cmdparser.positionalArguments();
  QString sArg(QLatin1String(""));
  if (!sListArgs.isEmpty()) {
    sArg = sListArgs.at(0);
  }

  InyokaEdit myInyokaEdit(userDataDir, sSharePath, sArg);
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

#ifdef USEQTWEBENGINE
  qDebug() << "Compiled with Qt" << QT_VERSION_STR << "+ webenginewidgets";
#endif
#ifdef NOPREVIEW
  qDebug() << "Compiled with Qt" << QT_VERSION_STR << "WITHOUT preview!";
#endif
  qDebug() << "Qt runtime" << qVersion();
  qDebug() << "Compiled with:" << QSslSocket::sslLibraryBuildVersionString();
  qDebug() << "Run-time:" << QSslSocket::sslLibraryVersionString();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void LoggingHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &sMsg) {
  QString sContext = sMsg + " (" + QString::fromLatin1(context.file) + ":" +
                     QString::number(context.line) + ", " +
                     QString::fromLatin1(context.function) + ")";
  QString sTime(QTime::currentTime().toString());

  switch (type) {
    case QtDebugMsg:
      out << sTime << " Debug: " << sMsg << "\n";
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
