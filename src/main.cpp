/**
 * \file main.cpp
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
#include <QTextCodec>
#include <QTranslator>
#include <QSettings>

#include <iostream>
#include <fstream>

#include "./CInyokaEdit.h"

std::ofstream logfile;

void setupLogger(const QString &sDebugFilePath,
                 const QString &sAppName,
                 const QString &sVersion);
void LoggingHandler(QtMsgType type, const char *sMsg);
QString getLanguage(const QString &sAppName);

// ----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("InyokaEdit");
    app.setApplicationVersion(sVERSION);

    if (app.arguments().size() >= 2) {
        QString sTmp = app.argv()[1];
        if ("-v" == sTmp || "--version" == sTmp) {
            std::cout << app.argv()[0] << "\t v"
                      << app.applicationVersion().toStdString() << std::endl;
            exit(0);
        }
    }

    QTranslator qtTranslator;
    QTranslator AppTranslator;

    const QDir userDataDir(
                QDesktopServices::storageLocation(
                    QDesktopServices::DataLocation).toLower());

    const QString sLang(getLanguage(app.applicationName()));
    const QString sDebugFile("debug.log");

    // Resource file (images, icons)
    Q_INIT_RESOURCE(inyokaedit_resources);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    setupLogger(userDataDir.absolutePath() + "/" + sDebugFile,
                app.applicationName(), app.applicationVersion());

    // Setup gui translation (Qt)
    if (!qtTranslator.load("qt_" + sLang,
                           QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        // If it fails search in application dircetory
        if (!qtTranslator.load("qt_" + sLang, app.applicationDirPath() + "/lang")) {
            qWarning() << "Could not load any Qt translations.";
        }
    }
    app.installTranslator(&qtTranslator);

    // Setup gui translation (app)
    if (!AppTranslator.load(app.applicationName().toLower() + "_" + sLang,
                            "/usr/share/" + app.applicationName().toLower()
                            + "/lang")) {
        // If it fails search in application dircetory
        if (!qtTranslator.load(app.applicationName().toLower() + "_"  + sLang,
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
    qInstallMsgHandler(LoggingHandler);
    qDebug() << sAppName << sVersion;
    qDebug() << "Compiled with Qt" << QT_VERSION_STR;
    qDebug() << "Qt runtime" <<  qVersion();
}

// ----------------------------------------------------------------------------
// Source: http://www.developer.nokia.com/Community/Wiki/File_based_logging_in_Qt_for_debugging

void LoggingHandler(QtMsgType type, const char *sMsg) {
    switch (type) {
        case QtDebugMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<
                       " Debug: " << sMsg << "\n";
            logfile.flush();
            break;
        case QtCriticalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<
                       " Critical: " << sMsg << "\n";
            logfile.flush();
            break;
        case QtWarningMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<
                       " Warning: " << sMsg << "\n";
            logfile.flush();
            break;
        case QtFatalMsg:
            logfile << QTime::currentTime().toString().toAscii().data() <<
                       " Fatal: " << sMsg << "\n";
            logfile.flush();
            logfile.close();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString getLanguage(const QString &sAppName) {
#if defined _WIN32
    QSettings mySet(QSettings::IniFormat, QSettings::UserScope,
                    sAppName.toLower(), sAppName.toLower());
#else
    QSettings mySet(QSettings::NativeFormat, QSettings::UserScope,
                    sAppName.toLower(), sAppName.toLower());
#endif

    QString sLang = mySet.value("GuiLanguage", "auto").toString();
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
