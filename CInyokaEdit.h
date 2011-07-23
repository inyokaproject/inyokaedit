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
* File Name:  InyokaEdit.h
* Purpose:    Class definition main application
***************************************************************************/

#ifndef CINYOKAEDIT_H
#define CINYOKAEDIT_H

#include <QMainWindow>

#include "CHighlighter.h"
#include "CParser.h"
#include "CDownloadArticle.h"
#include "CTextEditor.h"
#include "CInsertSyntaxElement.h"
#include "qtfindreplacedialog/finddialog.h"
#include "qtfindreplacedialog/findreplacedialog.h"

// Qt classes
class QAction;
class QMenu;
class QComboBox;
class QSignalMapper;
class QWebView;
class QFile;

// Own classes
class CParser;
class CDownloadArticle;
class FindDialog;
class FindReplaceDialog;

class CInyokaEdit : public QMainWindow
{
    Q_OBJECT

public:
    CInyokaEdit(const QString &sAppName);   // Constructor
    ~CInyokaEdit();  // Desstructor

public slots:
    void showHtmlPreview(const QString &filename);
    void showMessageBox(const QString &messagetext, const QString &type);

protected:
    // Event when program will close
    void closeEvent(QCloseEvent *event);

private slots:
    // Functions file menu/toolbar
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();

    void findDialog();
    void findReplaceDialog();

    // Called when text in editor was changed
    void documentWasModified();

    // Functions in inyoka toolbar
    void insertDropDownHeading(const int iSelection);
    void insertDropDownTextmacro(const int iSelection);

    // Preview / Download toolbar
    void previewInyokaPage();
    void downloadArticle();

    // Insert text sample menu
    void insertTextSample(const QString &sMenuEntry);

    // Insert interwiki-link menu
    void insertInterwikiLink(const QString &sMenuEntry);

private:
    // Build gui
    void setupEditor();
    void createActions();
    void createMenus();
    void createToolBars();

    // Load / save application settings
    void readSettings();
    void writeSettings();

    bool codecompletion; // Code completion ist active inactive
    bool ineditorpreview;

    bool maybeSave();

    // Load / save file
    void loadFile(const QString &sFileName);
    bool saveFile(const QString &sFileName);

    void setCurrentFile(const QString &sFileName);
    QString strippedName(const QString &sFullFileName);

    // Objects
    CTextEditor *myeditor;
    QCompleter *mycompleter;
    CHighlighter *myhighlighter;  // Syntax highlighting
    CParser *myparser;            // Parser text to HTML
    CDownloadArticle *myDownloadModule; // Download of exsiting inyoka articles
    CInsertSyntaxElement *myInsertSyntaxElement;

    QTabWidget *mytabwidget;
    QWebView *mywebview;

    FindDialog *m_findDialog;
    FindReplaceDialog *m_findReplaceDialog;

    const QString sAppName;
    QString sCurFile;  // Current file
    QString sInyokaUrl;

    // Menus and toolbars
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QMenu *insertTextSampleMenu;
    QMenu *insertBeginningMenu;
    QMenu *insertFormatingHelpMenu;
    QMenu *insertPacketinstallationMenu;
    QMenu *insertMiscelementMenu;
    QMenu *insertImageMenu;
    QMenu *iWikiMenu;
    QMenu *iWikiMenuInyoka;
    QMenu *iWikiMenuCanonical;
    QMenu *iWikiMenuWikimedia;
    QMenu *iWikiMenuHoster;
    QMenu *iWikiMenuMisc;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *inyokaeditorBar;
    QToolBar *comboboxBar;
    QToolBar *previewDlBar;

    // File menu / file toolbar
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *searchAct;
    QAction *replaceAct;
    QAction *findNextAct;
    QAction *findPreviousAct;
    QAction *aboutAct;

    // Inyoka editor toolbar
    QAction *boldAct;
    QAction *italicAct;
    QAction *monotypeAct;
    QAction *wikilinkAct;
    QAction *externLinkAct;
    QAction *imageAct;
    QAction *codeblockAct;

    // Preview / download toolbar
    QAction *previewAct;
    QAction *downloadArticleAct;

    // Combobox toolbar
    QComboBox *headingsBox;
    QComboBox *textmacrosBox;

    // Text samples menu
    QAction *insertUnderConstructionAct;
    QAction *insertTestedAct;
    QAction *insertKnowledgeAct;
    QAction *insertTableOfContentAct;
    QAction *insertExpertboxAct;
    QAction *insertAwardboxAct;

    QAction *insertCommandAct;
    QAction *insertHintAct;
    QAction *insertWarningAct;
    QAction *insertExpertinfoAct;

    QAction *insertImageAct;
    QAction *insertImageSubscriptionAct;
    QAction *insertImageCollectionAct;
    QAction *insertImageCollectionInTextAct;

    QAction *insertPackageMacroAct;
    QAction *insertPackageInstallAct;
    QAction *insertPpaSampleAct;
    QAction *insertForeignSourceSampleAct;
    QAction *insertForeignAuthSampleAct;
    QAction *insertForeignPackageSampleAct;

    QAction *insertExpandableAct;
    QAction *insertFaultyAct;
    QAction *insertLeftAct;
    QAction *insertPackageWarningAct;
    QAction *insertSourceWaringAct;
    QAction *insertSoftwareWarningAct;

    QSignalMapper *mySigMapTextSamples;

    // Inter-Wiki Links
    // Inyoka
    QAction *iWikiForum;
    QAction *iWikiTopic;
    QAction *iWikiPost;
    QAction *iWikiUser;
    QAction *iWikiIkhaya;
    QAction *iWikiCalendar;
    QAction *iWikiTicket;
    QAction *iWikiBehind;
    QAction *iWikiPaste;
    QAction *iWikiPlanet;

    // Canonical, Ubuntu, Derivate
    QAction *iWikiCanonical;
    QAction *iWikiPackages;
    QAction *iWikiApt;
    QAction *iWikiUbuntu;
    QAction *iWikiUbuntu_doc;
    QAction *iWikiAskubuntu;
    QAction *iWikiUbuntu_fr;
    QAction *iWikiUbuntuone;
    QAction *iWikiKubuntu;
    QAction *iWikiKubuntuDe;
    QAction *iWikiXubuntu;
    QAction *iWikiUbuntustudio;
    QAction *iWikiEdubuntu;
    QAction *iWikiDebian;
    QAction *iWikiDebian_de;

    // Wikimedia
    QAction *iWikiWikipedia;
    QAction *iWikiWikipedia_en;
    QAction *iWikiWikibooks;
    QAction *iWikiWikimedia;

    // Source & project hoster, bugtracker
    QAction *iWikiFreshmeat;
    QAction *iWikiGetdeb;
    QAction *iWikiGooglecode;
    QAction *iWikiLaunchpad;
    QAction *iWikiBug;
    QAction *iWikiLpuser;
    QAction *iWikiPlaydeb;
    QAction *iWikiSourceforge;

    // Misc
    QAction *iWikiEan;
    QAction *iWikiFb;
    QAction *iWikiGoogle;
    QAction *iWikiHolarse;
    QAction *iWikiIdentica;
    QAction *iWikiIsbn;
    QAction *iWikiLinuxgaming;
    QAction *iWikiOsm;
    QAction *iWikiTwitter;
    QAction *iWikiYoutube;

    QSignalMapper *mySigMapInterWikiLinks;

signals:
    void sendCodeCompState(bool);
};

#endif // CINYOKAEDIT_H
