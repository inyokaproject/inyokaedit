/**
 * \file main.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2016 The InyokaEdit developers
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
 * InyokaEdit is a small markup editor for Inyoka articles (http://ubuntuusers.de/inyoka/).<br />
 * <br />Some features:<br />
 * \li Syntax highlighting
 * \li Offline preview
 * \li Downloading raw text of existing articles and corresponding images.
 * \li Spell checker
 *
 * \section Launchpad
 * InyokaEdit on Launchpad: https://launchpad.net/inyokaedit
 */

#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QTranslator>
#include <QTextStream>

#include "./CInyokaEdit.h"

QFile logfile;
QTextStream out(&logfile);

QString getLanguage(const QString &sSharePath);
void setupLogger(const QString &sDebugFilePath);

#if QT_VERSION >= 0x050000
    void LoggingHandler(QtMsgType type,
                        const QMessageLogContext &context,
                        const QString &sMsg);
#else
    void LoggingHandler(QtMsgType type, const char *sMsg);
#endif

// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);

    if (app.arguments().contains("-v")
            || app.arguments().contains("--version")) {
        qDebug() << app.arguments()[0] << "\t"
                 << app.applicationVersion() << "\n";
        exit(0);
    }

    QTranslator qtTranslator;
    QTranslator AppTranslator;

    // User data directory
#if QT_VERSION >= 0x050000
    QStringList sListPaths = QStandardPaths::standardLocations(
                QStandardPaths::DataLocation);
    if (sListPaths.isEmpty()) {
        qCritical() << "Error while getting data standard path.";
        sListPaths << "";
    }
    const QDir userDataDir(sListPaths[0].toLower());
#else
    const QDir userDataDir(
                QDesktopServices::storageLocation(
                    QDesktopServices::DataLocation).toLower());
#endif

    // Default share data path (Windows and debugging)
    QString sSharePath = app.applicationDirPath();
    // Standard installation path (Linux)
    QDir tmpDir(app.applicationDirPath() + "/../share/"
                + app.applicationName().toLower());
    if (!app.arguments().contains("--debug") && tmpDir.exists()) {
        sSharePath = app.applicationDirPath() + "/../share/"
                       + app.applicationName().toLower();
    }

    const QString sLang(getLanguage(sSharePath));
    const QString sDebugFile("debug.log");

    // Resource file (images, icons)
    Q_INIT_RESOURCE(inyokaedit_resources);

    if (!userDataDir.exists()) {
        // Create folder including possible parent directories (mkPATH)!
        userDataDir.mkpath(userDataDir.absolutePath());
    }
    setupLogger(userDataDir.absolutePath() + "/" + sDebugFile);

    // Setup gui translation (Qt)
    if (!qtTranslator.load("qt_" + sLang,
                           QLibraryInfo::location(
                               QLibraryInfo::TranslationsPath))) {
        // If it fails, search in application directory
        if (!qtTranslator.load("qt_" + sLang, sSharePath + "/lang")) {
            qWarning() << "Could not load Qt translations:" << "qt_" + sLang;
        }
    }
    app.installTranslator(&qtTranslator);

    // Setup gui translation (app)
    if (!AppTranslator.load(app.applicationName().toLower() + "_" + sLang,
                            sSharePath + "/lang")) {
        qWarning() << "Could not load application translation:"
                   << qAppName() + "_" + sLang;
    }
    app.installTranslator(&AppTranslator);

    CInyokaEdit InyokaEdit(userDataDir, sSharePath);
    InyokaEdit.show();
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
#if QT_VERSION >= 0x050000
        qInstallMessageHandler(LoggingHandler);
#else
        qInstallMsgHandler(LoggingHandler);
#endif
    }

    qDebug() << qApp->applicationName() << qApp->applicationVersion();
    qDebug() << "Compiled with Qt" << QT_VERSION_STR;
    qDebug() << "Qt runtime" << qVersion();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#if QT_VERSION >= 0x050000
void LoggingHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &sMsg) {
    QString sMsg2(sMsg);
    QString sContext = sMsg + " (" + QString(context.file) + ":"
                       + QString::number(context.line) + ", "
                       + QString(context.function) + ")";
#else
void LoggingHandler(QtMsgType type, const char *sMsg) {
    QString sMsg2(QString::fromUtf8(sMsg));
    QString sContext(sMsg);
#endif
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
        break;
    default:
        out << sTime << " OTHER INFO: " << sContext << "\n";
        out.flush();
        break;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString getLanguage(const QString &sSharePath) {
#if defined _WIN32
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       qAppName().toLower(), qAppName().toLower());
#else
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
                       qApp->applicationName().toLower(),
                       qApp->applicationName().toLower());
#endif

    QString sLang = settings.value("GuiLanguage", "auto").toString();
    if ("auto" == sLang) {
        #ifdef Q_OS_UNIX
        QByteArray lang = qgetenv("LANG");
        if (!lang.isEmpty()) {
            return QLocale(lang).name();
        }
        #endif
        return QLocale::system().name();
    } else if (!QFile(sSharePath + "/lang/"
                      + qAppName() + "_" + sLang + ".qm").exists()) {
        settings.setValue("GuiLanguage", "en");
        return "en";
    }
    return sLang;
}
