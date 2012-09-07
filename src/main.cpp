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

#include <fstream>

#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QLibraryInfo>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>
#include <QSettings>

#include "CInyokaEdit.h"

std::ofstream logfile;

void setupLogger( const QString sDebugFilePath, const QString sAppName, const QString sVersion );
void LoggingHandler( QtMsgType type, const char *msg );
QString getLanguage( const QString sConfigDir, const QString sAppName );

// ----------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    app.setApplicationName( "InyokaEdit" );
    app.setApplicationVersion( sVERSION );

    QTranslator qtTranslator;
    QTranslator AppTranslator;

    const QDir userAppDir( QDir::homePath() + "/." + app.applicationName() );
    const QString sLang( getLanguage( userAppDir.absolutePath(), app.applicationName() ) );
    const QString sDebugFile( "Debug.log" );

    // Resource file (images, icons)
    Q_INIT_RESOURCE( inyokaedit_resources );

    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF-8") );
    QTextCodec::setCodecForTr( QTextCodec::codecForName("UTF-8") );

    setupLogger( userAppDir.absolutePath() + "/" + sDebugFile,
                 app.applicationName(), app.applicationVersion() );

    // Setup gui translation (Qt)
    if ( !qtTranslator.load("qt_" + sLang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
    {
        // If it fails search in application dircetory
        qtTranslator.load( "qt_" + sLang, app.applicationName() + "/lang" );
    }
    app.installTranslator( &qtTranslator );

    // Setup gui translation (app)
    if ( !AppTranslator.load(app.applicationName().toLower() + "_" + sLang,
                             "/usr/share/" + app.applicationName().toLower() + "/lang") )
    {
        // If it fails search in application dircetory
        qtTranslator.load( app.applicationName().toLower() + "_"  + sLang,
                           app.applicationDirPath() + "/lang" );
    }
    app.installTranslator( &AppTranslator );

    CInyokaEdit InyokaEdit( &app, userAppDir );
    InyokaEdit.show();
    int nRet = app.exec();

    logfile.close();
    return nRet;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void setupLogger( const QString sDebugFilePath, const QString sAppName, const QString sVersion )
{
    // Remove old debug file
    if ( QFile(sDebugFilePath).exists() )
    {
        QFile(sDebugFilePath).remove();
    }

    // Create new file
    logfile.open( sDebugFilePath.toStdString().c_str(), std::ios::app );
    qInstallMsgHandler( LoggingHandler );
    qDebug() << sAppName << sVersion;
    qDebug() << "Compiled with Qt" << QT_VERSION_STR;
    qDebug() << "Qt runtime" <<  qVersion();
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString getLanguage(const QString sConfigDir, const QString sAppName)
{
    #if defined _WIN32
    QSettings::setPath( QSettings::IniFormat, QSettings::UserScope, sConfigDir );
    QSettings mySet( QSettings::IniFormat, QSettings::UserScope, sAppName );
    #else
    QSettings::setPath( QSettings::NativeFormat, QSettings::UserScope, sConfigDir );
    QSettings mySet( QSettings::NativeFormat, QSettings::UserScope, sAppName );
    #endif

    QString sLang = mySet.value("GuiLanguage", "auto").toString();
    if( "auto" == sLang )
    {
        #ifdef Q_OS_UNIX
        QByteArray lang = qgetenv("LANG");
        if( !lang.isEmpty() )
        {
            return QLocale(lang).name();
        }
        #endif
        return QLocale::system().name();
    }
    else
    {
        return sLang;
    }
}
