/**
 * \file main.cpp
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
#include <QTranslator>
#include <QSettings>

#include <iostream>
#include <fstream>

#include "./CInyokaEdit.h"

std::ofstream logfile;

QString getLanguage(const QString &sAppName);
void setupLogger(const QString &sDebugFilePath,
                 const QString &sAppName,
                 const QString &sVersion);

#if QT_VERSION >= 0x050000
    void LoggingHandler(QtMsgType type,
                        const QMessageLogContext &context,
                        const QString &sMsg);
#else
    void LoggingHandler(QtMsgType type, const char *sMsg);
#endif

// Don't change this value! Use "--debug" command line option instead.
bool bDEBUG = false;

// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("InyokaEdit");
    app.setApplicationVersion(sVERSION);

    if (app.arguments().size() >= 2) {
        QString sTmp = QString(app.arguments()[1]).toLower();
        if ("-v" == sTmp || "--version" == sTmp) {
            std::cout << app.arguments()[0].toStdString() << "\t v"
                      << app.applicationVersion().toStdString() << std::endl;
            exit(0);
        } else if ("--debug" == sTmp) {
            bDEBUG = true;
        }
    }

    QTranslator qtTranslator;
    QTranslator AppTranslator;

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

    const QString sLang(getLanguage(app.applicationName()));
    const QString sDebugFile("debug.log");

    // Resource file (images, icons)
    Q_INIT_RESOURCE(inyokaedit_resources);

    setupLogger(userDataDir.absolutePath() + "/" + sDebugFile,
                app.applicationName(), app.applicationVersion());

    // Setup gui translation (Qt)
    if (!qtTranslator.load("qt_" + sLang,
                           QLibraryInfo::location(
                               QLibraryInfo::TranslationsPath))) {
        // If it fails search in application dircetory
        if (!qtTranslator.load("qt_" + sLang, app.applicationDirPath()
                               + "/lang")) {
            qWarning() << "Could not load any Qt translations.";
        }
    }
    app.installTranslator(&qtTranslator);

    // Setup gui translation (app)
    if (bDEBUG ||
            !AppTranslator.load(app.applicationName().toLower() + "_" + sLang,
                                "/usr/share/" + app.applicationName().toLower()
                                + "/lang")) {
        // If it fails search in application dircetory
        if (!AppTranslator.load(app.applicationName().toLower() + "_"  + sLang,
                          app.applicationDirPath() + "/lang")) {
            qWarning() << "Could not load any application translations.";
        }
    }
    app.installTranslator(&AppTranslator);

    CInyokaEdit InyokaEdit(&app, userDataDir);
    InyokaEdit.show();
    int nRet = app.exec();

    qDebug() << "Closing" << app.applicationName();
    logfile.close();
    return nRet;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void setupLogger(const QString &sDebugFilePath, const QString &sAppName,
                 const QString &sVersion) {
    // Remove old debug file
    if (QFile(sDebugFilePath).exists()) {
        QFile(sDebugFilePath).remove();
    }

    // Create new file
    logfile.open(sDebugFilePath.toStdString().c_str(), std::ios::app);
#if QT_VERSION >= 0x050000
    qInstallMessageHandler(LoggingHandler);
    //qInstallMessageHandler(0);
#else
    qInstallMsgHandler(LoggingHandler);
#endif
    qDebug() << sAppName << sVersion;
    qDebug() << "Compiled with Qt" << QT_VERSION_STR;
    qDebug() << "Qt runtime" <<  qVersion();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#if QT_VERSION >= 0x050000
void LoggingHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &sMsg) {
    QString sMsg2(sMsg);
    QString sContext = sMsg + " (" +
            QString(context.file) + ":" +
            QString::number(context.line) + ", " +
            QString(context.function) + ")";
#else
void LoggingHandler(QtMsgType type, const char *sMsg) {
    QString sMsg2(sMsg);
    QString sContext(sMsg);
#endif
    char* sTime(QTime::currentTime().toString().toLatin1().data());

    switch (type) {
    case QtDebugMsg:
        logfile << sTime << " Debug: " << sMsg2.toStdString().c_str() << "\n";
        logfile.flush();
        break;
    case QtWarningMsg:
        logfile << sTime << " Warning: " << sContext.toStdString().c_str() << "\n";
        logfile.flush();
        break;
    case QtCriticalMsg:
        logfile << sTime << " Critical: " << sContext.toStdString().c_str() << "\n";
        logfile.flush();
        break;
    case QtFatalMsg:
        logfile << sTime << " Fatal: " << sContext.toStdString().c_str() << "\n";
        logfile.flush();
        logfile.close();
        abort();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString getLanguage(const QString &sAppName) {
#if defined _WIN32
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       sAppName.toLower(), sAppName.toLower());
#else
    QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
                       sAppName.toLower(), sAppName.toLower());
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
    } else {
        return sLang;
    }
}
