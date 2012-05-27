/**
 * \file CInyokaEdit.h
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
 * Class definition main application.
 */

#ifndef CINYOKAEDIT_H
#define CINYOKAEDIT_H

#include <QMainWindow>
#include <QWebHistory>

#include "CInterwiki.h"
#include "CSettings.h"
#include "CHighlighter.h"
#include "CParser.h"
#include "CTextEditor.h"
#include "CInsertSyntaxElement.h"
#include "CProgressDialog.h"
#include "qtfindreplacedialog/finddialog.h"
#include "qtfindreplacedialog/findreplacedialog.h"
#include "CDownload.h"
#include "CFileOperations.h"

#ifndef DISABLE_SPELLCHECKER
#include "CSpellChecker.h"
#include "CSpellCheckDialog.h"
#endif

// Qt classes
class QComboBox;
class QSignalMapper;
class QWebView;
class QFile;
class QDir;

// Own classes
class CParser;
class FindDialog;
class FindReplaceDialog;
class CProgressDialog;
class CSettings;
class CDownload;
class CFileOperations;

#define sVERSION "0.5.0"

namespace Ui {
    class CInyokaEdit;
}

/**
 * \class CInyokaEdit
 * \brief Main application definition (gui, objects, etc.)
 */
class CInyokaEdit : public QMainWindow
{
    Q_OBJECT
    
public:
    /**
    * \brief Constructor
    * \param ptrApp Pointer to main application
    * \param parent pointer to parent window
    */
    explicit CInyokaEdit( QApplication *ptrApp, QDir userAppDir, QWidget *parent = 0 );   // Constructor

    /** \brief Destructor */
    ~CInyokaEdit();
    
public slots:
    void newDocumentTab();

    /** \brief Called when text in editor was changed */
    void documentWasModified();

    /**
    * \brief Call preview
    * \param sFilename Name of temporarily generated html file
    */
    void showHtmlPreview( const QString &sFilename );

    /**
    * \brief Update text in editor with downloaded article
    * \param sArticleText Raw text
    * \param sSitename Name of downloaded site
    */
    void displayArticleText( const QString &sArticleText, const QString &sSitename );

protected:
    /**
    * \brief Close event handler
    * \param event Close event
    */
    void closeEvent( QCloseEvent *event );

    bool eventFilter( QObject *obj, QEvent *event );

protected slots:
    /** \brief Call spell checker */
    void checkSpelling();

private slots:
    /** \brief Call Apport for bug report */
    void reportBug();

    /** \brief Call about dialog */
    void about();

    /**
    * \brief Insert headline from drop down box
    * \param nSelection Selected entry
    */
    void insertDropDownHeadline( const int nSelection );

    /**
    * \brief Insert text macro from drop down box
    * \param nSelection Selected entry
    */
    void insertDropDownTextmacro( const int nSelection );

    /**
    * \brief Insert text format from drop down box
    * \param nSelection Selected entry
    */
    void insertDropDownTextformat( const int nSelection );

    /** \brief Download an existing article */
    void downloadArticle();

    /** \brief Delete downloaded images which had been attached to an article */
    void deleteTempImages();

    // Preview / download toolbar
    void previewInyokaPage( const int nIndex = 999 );
    void loadPreviewFinished( bool bSuccess );

    void clickedLink();

    /**
    * \brief Insert text sample from menu
    * \param sMenuEntry Selected entry
    */
    void insertTextSample( const QString &sMenuEntry);

    /**
    * \brief Insert interwiki link from menu
    * \param sMenuEntry Selected entry
    */
    void insertInterwikiLink( const QString &sMenuEntry );

private:
    Ui::CInyokaEdit *m_pUi;
    QApplication *m_pApp;

    void createObjects();

    // Build gui
    void setupEditor();
    void createActions();
    void createMenus();
    void createToolBars();

    // Load / save application settings
    void readSettings();
    void writeSettings();

    // Objects
    CTextEditor *m_pEditor;
    QCompleter *m_pCompleter;
    CFileOperations *m_pFileOperations;
    CHighlighter *m_pHighlighter;  // Syntax highlighting
    CParser *m_pParser;            // Parser text to HTML
    CInsertSyntaxElement *m_pInsertSyntaxElement;
    CSettings *m_pSettings;
    CInterWiki *m_pInterWikiLinks;
    CDownload *m_pDownloadModule;
    QSplitter *m_pWidgetSplitter;
    QBoxLayout *m_pFrameLayout;
    QFrame *m_pWebviewFrame;

    //QTabWidget *myTabwidgetDocuments;
    QTabWidget *m_pTabwidgetRawPreview;
    QWebView *m_pWebview;

    FindDialog *m_findDialog;
    FindReplaceDialog *m_findReplaceDialog;

    QDir m_UserAppDir;
    QDir m_tmpPreviewImgDir;

    QStringList m_sListCompleter;

    // File menu: Clear recent opened files list
    QAction *m_pClearRecentFilesAct;

    // InterWiki links menu group list
    QList<QMenu *> m_iWikiGroups;

    // Comboboxes for samplesmacrosBar toolbar
    QComboBox *m_pHeadlineBox;
    QComboBox *m_pTextmacrosBox;
    QComboBox *m_pTextformatBox;

    // Text samples signal mapper
    QSignalMapper *mySigMapTextSamples;

    // Inter-Wiki Links
    QList<QList<QAction *> > m_iWikiLinksActions;
    QSignalMapper *m_pSigMapInterWikiLinks;

    QList<QAction *> m_OpenTemplateFilesActions;
    QSignalMapper *m_pSigMapOpenTemplate;
};

#endif // CINYOKAEDIT_H
