/**
 * \file main.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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

// For global translation (Qt MessageBox etc.)
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QtGlobal>
#include <QTime>
#include <fstream>

#include "CInyokaEdit.h"

std::ofstream logfile;

void LoggingHandler( QtMsgType type, const char *msg );


int main( int argc, char *argv[] )
{
    const QString sDebugFile("Debug.log");

    // Resource file (images, icons)
    Q_INIT_RESOURCE( inyokaedit_resources );

    // New application
    QApplication app(argc, argv);
    app.setApplicationName("InyokaEdit");
    app.setApplicationVersion( sVERSION );

    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF-8") );
    QTextCodec::setCodecForTr( QTextCodec::codecForName("UTF-8") );

    // Set config and styles/images path
    QDir userAppDir = QDir::homePath() + "/." + app.applicationName();

    if ( QFile(userAppDir.absolutePath() + "/" + sDebugFile).exists() )
    {
        QFile(userAppDir.absolutePath() + "/" + sDebugFile).remove();
    }
    logfile.open( QString(userAppDir.absolutePath() + "/" + sDebugFile).toStdString().c_str(), std::ios::app);
    qInstallMsgHandler( LoggingHandler );
    qDebug() << app.applicationName() << app.applicationVersion();
    qDebug() << "Compiled with Qt" << QT_VERSION_STR;
    qDebug() << "Qt runtime" <<  qVersion();

    // Load global translation
    QTranslator qtTranslator;
    // Try to load Qt translation
    if ( !qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
    {
        // If it fails search in application dircetory
        qtTranslator.load( "qt_" + QLocale::system().name(), app.applicationDirPath() + "/lang" );
    }
    app.installTranslator( &qtTranslator );

    // Load app translation if it exists
    QTranslator myAppTranslator;
    // Try to load app translation (normal installation)
    if ( !myAppTranslator.load(app.applicationName().toLower() + "_" + QLocale::system().name(), "/usr/share/" + app.applicationName().toLower() + "/lang") )
    {
        // If it fails search in application dircetory
        qtTranslator.load( app.applicationName().toLower() + "_"  + QLocale::system().name(), app.applicationDirPath() + "/lang" );
    }
    app.installTranslator( &myAppTranslator );

    // New object of InyokaEdit
    CInyokaEdit myInyokaEdit( &app, userAppDir );
    myInyokaEdit.show();

    int nRet = app.exec();

    logfile.close();
    return nRet;
}

// ----------------------------------------------------------------------------
// Source: http://www.developer.nokia.com/Community/Wiki/File_based_logging_in_Qt_for_debugging

void LoggingHandler( QtMsgType type, const char *sMsg )
{
    switch (type)
    {
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
