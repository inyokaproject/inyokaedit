/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CInyokaEdit.h
* Purpose:    Class definition main application
***************************************************************************/

#ifndef CINYOKAEDIT_H
#define CINYOKAEDIT_H

#include <QMainWindow>
#include <QWebHistory>
#include <iostream>

#include "CInterwiki.h"
#include "CSettings.h"
#include "CHighlighter.h"
#include "CParser.h"
#include "CTextEditor.h"
#include "CInsertSyntaxElement.h"
#include "CProgressDialog.h"
#include "qtfindreplacedialog/finddialog.h"
#include "qtfindreplacedialog/findreplacedialog.h"

// Spell checker currently not under windows
#if not defined _WIN32
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
class CDownloadArticle;
class FindDialog;
class FindReplaceDialog;
class CProgressDialog;
class CSettings;


namespace Ui {
    class CInyokaEdit;
}

class CInyokaEdit : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit CInyokaEdit(QApplication *ptrApp, QWidget *parent = 0);   // Constructor
    ~CInyokaEdit();  // Desstructor
    
public slots:
    void showHtmlPreview(const QString &filename);

protected:
    // Event when program will close
    void closeEvent(QCloseEvent *event);

protected slots:
    void checkSpelling();

private slots:
    // Functions file menu/toolbar
    void newFile();
    void open();
    void openRecentFile(int iEntry);
    void clearRecentFiles();
    bool save();
    bool saveAs();
    void reportBug();
    void about();

    // Called when text in editor was changed
    void documentWasModified();

    // Functions in inyoka toolbar
    void insertDropDownHeadline(const int iSelection);
    void insertDropDownTextmacro(const int iSelection);
    void insertDropDownTextformat(const int iSelection);

    // Preview / download toolbar
    void previewInyokaPage(const int iIndex = 999);
    void loadPreviewFinished(bool bSuccess);
    void downloadArticle();
    void downloadImages(const QString &sArticlename);

    // Insert text sample menu
    void insertTextSample(const QString &sMenuEntry);

    // Insert interwiki-link menu
    void insertInterwikiLink(const QString &sMenuEntry);

private:
    Ui::CInyokaEdit *ui;
    QApplication *pApp;

    void createObjects();

    // Build gui
    void setupEditor();
    void createActions();
    void createMenus();
    void createToolBars();

    // Load / save application settings
    void readSettings();
    void writeSettings();

    bool maybeSave();

    // Load / save file
    void loadFile(const QString &sFileName);
    bool saveFile(const QString &sFileName);

    void updateRecentFiles(const QString &sFileName);

    void setCurrentFile(const QString &sFileName);
    QString strippedName(const QString &sFullFileName);

    void DownloadStyles(const QDir myDirectory);

    // Objects
    CTextEditor *myEditor;
    QCompleter *myCompleter;
    CHighlighter *myHighlighter;  // Syntax highlighting
    CParser *myParser;            // Parser text to HTML
    CDownloadArticle *myDownloadModule; // Download of exsiting inyoka articles
    CInsertSyntaxElement *myInsertSyntaxElement;
    CProgressDialog *myArticleDownloadProgress;
    CProgressDialog *myImageDownloadProgress;
    CSettings *mySettings;
    CInterWiki *myInterWikiLinks;

    //QTabWidget *myTabwidgetDocuments;
    QTabWidget *myTabwidgetRawPreview;
    QWebView *myWebview;

    FindDialog *m_findDialog;
    FindReplaceDialog *m_findReplaceDialog;

    QString sCurFile;      // Current file

    QDir StylesAndImagesDir;

    QStringList sListCompleter;

    bool bLogging;

    // File menu: Last opened files list and clear list
    QList<QAction *> LastOpenedFilesAct;
    QSignalMapper *mySigMapLastOpenedFiles;
    QAction *clearRecentFilesAct;

    // InterWiki links menu group list
    QList<QMenu *> iWikiGroups;

    // Comboboxes for samplesmacrosBar toolbar
    QComboBox *headlineBox;
    QComboBox *textmacrosBox;
    QComboBox *textformatBox;

    // Text samples signal mapper
    QSignalMapper *mySigMapTextSamples;

    // Inter-Wiki Links
    QList<QList<QAction *> > iWikiLinksActions;
    QSignalMapper *mySigMapInterWikiLinks;

};

#endif // CINYOKAEDIT_H
