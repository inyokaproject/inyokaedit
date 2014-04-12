/**
 * \file CSettings.cpp
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
 * Read and write settings.
 */

#include <QApplication>
#include <QDebug>

#if QT_VERSION >= 0x050000
    #include <QStandardPaths>
#else
    #include <QDesktopServices>
#endif

#include "./CSettings.h"

CSettings::CSettings(QWidget *pParent)
    : m_pParent(pParent) {
    qDebug() << "Calling" << Q_FUNC_INFO;

#if defined _WIN32
    m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                qApp->applicationName().toLower(),
                                qApp->applicationName().toLower());
#else
    m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                qApp->applicationName().toLower(),
                                qApp->applicationName().toLower());
#endif

    this->readSettings();
}

CSettings::~CSettings() {
    if (NULL != m_pSettingsDialog) {
        delete m_pSettingsDialog;
    }
    m_pSettingsDialog = NULL;
    if (NULL != m_pHighlighter) {
        delete m_pHighlighter;
    }
    m_pHighlighter = NULL;

    if (NULL != m_pSettings) {
        delete m_pSettings;
        m_pSettings = NULL;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettings::init(CTemplates *pTemplates, QTextDocument *pDoc) {
    m_pHighlighter = new CHighlighter(pTemplates, m_sStyleFile, pDoc);

    m_pSettingsDialog = new CSettingsDialog(this, m_pHighlighter, m_pParent);

    connect(this, SIGNAL(availablePlugins(QList<IEditorPlugin*>,QList<QObject*>)),
            m_pSettingsDialog, SLOT(getAvailablePlugins(QList<IEditorPlugin*>,QList<QObject*>)));

    connect(this, SIGNAL(showSettingsDialog()),
            m_pSettingsDialog, SLOT(show()));

    connect (m_pSettingsDialog, SIGNAL(updatedSettings()),
             this, SIGNAL(updateEditorSettings()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettings::readSettings() {
    // General settings
    m_sGuiLanguage = m_pSettings->value("GuiLanguage", "auto").toString();
    m_bCodeCompletion = m_pSettings->value("CodeCompletion",
                                           true).toBool();
    m_bPreviewInEditor = m_pSettings->value("PreviewInEditor",
                                            true).toBool();
    m_bPreviewAlongside = m_pSettings->value("PreviewAlongside",
                                             false).toBool();
    m_bTmpPreviewInEditor = m_bPreviewInEditor;
    m_bTmpPreviewAlongside = m_bPreviewAlongside;
    m_sInyokaUrl = m_pSettings->value("InyokaUrl",
                                      "http://wiki.ubuntuusers.de").toString();
    if (m_sInyokaUrl.endsWith("/")) {
        m_sInyokaUrl.remove(m_sInyokaUrl.length() - 1, 1);
    }

#if QT_VERSION >= 0x050000
    QStringList sListPaths = QStandardPaths::standardLocations(
                QStandardPaths::DocumentsLocation);
    if (sListPaths.isEmpty()) {
        qCritical() << "Error while getting documents standard path.";
        sListPaths << "";
    }
    m_LastOpenedDir = m_pSettings->value("LastOpenedDir", sListPaths[0]).toString();
#else
    m_LastOpenedDir = m_pSettings->value("LastOpenedDir",
                                         QDesktopServices::storageLocation(
                                             QDesktopServices::DocumentsLocation))
                                         .toString();
#endif

    m_bAutomaticImageDownload = m_pSettings->value("AutomaticImageDownload",
                                                   false).toBool();
    m_bCheckLinks = m_pSettings->value("CheckLinks",
                                       false).toBool();
    m_sTemplateLang = m_pSettings->value("TemplateLanguage",
                                         "de").toString();
    m_nAutosave = m_pSettings->value("AutoSave",
                                     300).toUInt();
    // 0x01000004 = Qt::Key_Return
    m_sReloadPreviewKey = m_pSettings->value("ReloadPreviewKey",
                                             "0x01000004").toString();
    m_nTimedPreview = m_pSettings->value("TimedPreview",
                                         15).toUInt();
    m_bSyncScrollbars = m_pSettings->value("SyncScrollbars",
                                           true).toBool();
    m_bWinCheckUpdate = m_pSettings->value("WindowsCheckForUpdate",
                                           false).toBool();

    QString sStyle = m_pSettings->value("Style", "standard-style").toString();
    QFileInfo fiStylePath(m_pSettings->fileName());
#if defined _WIN32
    m_sStyleFile = fiStylePath.absolutePath() + "/" + sStyle + ".ini";
#else
    m_sStyleFile = fiStylePath.absolutePath() + "/" + sStyle + ".conf";
#endif

    // Font settings
    m_pSettings->beginGroup("Font");
    m_sFontFamily = m_pSettings->value("FontFamily",
                                       "Monospace").toString();
    // Used string for font size because float isn't saved human readable...
    m_nFontsize = m_pSettings->value("FontSize",
                                     "10.5").toFloat();
    if (m_nFontsize <= 0) {
        m_nFontsize = 10.5;
    }

    m_EditorFont.setFamily(m_sFontFamily);
    m_EditorFont.setFixedPitch(true);
    // Font matcher prefers fixed pitch fonts
    m_EditorFont.setStyleHint(QFont::TypeWriter);
    m_EditorFont.setPointSizeF(m_nFontsize);

    m_pSettings->endGroup();

    // Recent files
    m_pSettings->beginGroup("RecentFiles");
    m_nMaxLastOpenedFiles = m_pSettings->value("NumberOfRecentFiles",
                                               5).toInt();
    if (m_nMaxLastOpenedFiles < 0) {
        m_nMaxLastOpenedFiles = 0;
    }
    if (m_nMaxLastOpenedFiles > m_cMAXFILES) {
        m_nMaxLastOpenedFiles = m_cMAXFILES;
    }
    QString sTmpFile;
    for (int i = 0; i < m_nMaxLastOpenedFiles; i++) {
        sTmpFile = m_pSettings->value("File_" + QString::number(i)
                                      , "").toString();
        if (!sTmpFile.isEmpty()) {
            m_sListRecentFiles << sTmpFile;
        }
    }
    m_sListRecentFiles.removeDuplicates();
    m_pSettings->endGroup();

    // Find/replace dialogs
    m_pSettings->beginGroup("FindReplace");
    m_sTextFind = m_pSettings->value("TextFind", "").toString();
    m_sTextReplace = m_pSettings->value("TextReplace", "").toString();
    m_bSearchForward = m_pSettings->value("SearchForward", true).toBool();
    m_bCheckCase = m_pSettings->value("CaseSensitive", false).toBool();
    m_bCheckWholeWord = m_pSettings->value("MatchCases", false).toBool();
    m_bUseRegExp = m_pSettings->value("UseRegExp", false).toBool();
    m_pSettings->endGroup();

    // Proxy
    m_pSettings->beginGroup("Proxy");
    m_sProxyHostName = m_pSettings->value("HostName", "").toString();
    m_nProxyPort = m_pSettings->value("Port", "").toUInt();
    m_sProxyUserName = m_pSettings->value("UserName", "").toString();
    m_sProxyPassword = m_pSettings->value("Password", "").toString();
    m_pSettings->endGroup();

    // Plugins
    m_pSettings->beginGroup("Plugins");
    m_sListDisabledPlugins = m_pSettings->value("Disabled", "").toStringList();
    m_pSettings->endGroup();

    // Window state
    m_pSettings->beginGroup("Window");
    m_aWindowGeometry = m_pSettings->value("Geometry").toByteArray();
    // Restore toolbar position etc.
    m_aWindowState = m_pSettings->value("WindowState").toByteArray();
    m_aSplitterState = m_pSettings->value("SplitterState").toByteArray();
    m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettings::writeSettings(const QByteArray WinGeometry,
                              const QByteArray WinState,
                              const QByteArray SplitterState) {
    this->removeObsolete();

    // General settings
    m_pSettings->setValue("GuiLanguage", m_sGuiLanguage);
    m_pSettings->setValue("CodeCompletion", m_bCodeCompletion);
    m_pSettings->setValue("PreviewInEditor", m_bTmpPreviewInEditor);
    m_pSettings->setValue("PreviewAlongside", m_bTmpPreviewAlongside);
    m_pSettings->setValue("InyokaUrl", m_sInyokaUrl);
    m_pSettings->setValue("LastOpenedDir", m_LastOpenedDir.absolutePath());
    m_pSettings->setValue("AutomaticImageDownload", m_bAutomaticImageDownload);
    m_pSettings->setValue("CheckLinks", m_bCheckLinks);
    m_pSettings->setValue("TemplateLanguage", m_sTemplateLang);
    m_pSettings->setValue("AutoSave", m_nAutosave);
    m_pSettings->setValue("ReloadPreviewKey", m_sReloadPreviewKey);
    m_pSettings->setValue("TimedPreview", m_nTimedPreview);
    m_pSettings->setValue("SyncScrollbars", m_bSyncScrollbars);
#if defined _WIN32
    m_pSettings->setValue("WindowsCheckForUpdate", m_bWinCheckUpdate);
#endif

    QFileInfo fiStylePath(m_sStyleFile);
    m_pSettings->setValue("Style", fiStylePath.baseName());

    // Font settings
    m_pSettings->beginGroup("Font");
    m_pSettings->setValue("FontFamily", m_sFontFamily);
    m_pSettings->setValue("FontSize", m_nFontsize);
    m_pSettings->endGroup();

    // Recent files
    m_pSettings->beginGroup("RecentFiles");
    m_pSettings->setValue("NumberOfRecentFiles", m_nMaxLastOpenedFiles);
    for (int i = 0; i < m_cMAXFILES; i++) {
        if (i < m_sListRecentFiles.size()) {
            m_pSettings->setValue("File_" + QString::number(i),
                                  m_sListRecentFiles[i]);
        } else {
            m_pSettings->setValue("File_" + QString::number(i), "");
        }
    }
    m_pSettings->endGroup();

    // Find / replace
    m_pSettings->beginGroup("FindReplace");
    m_pSettings->setValue("TextFind", m_sTextFind);
    m_pSettings->setValue("TextReplace", m_sTextReplace);
    m_pSettings->setValue("SearchForward", m_bSearchForward);
    m_pSettings->setValue("CaseSensitive", m_bCheckCase);
    m_pSettings->setValue("MatchCases", m_bCheckWholeWord);
    m_pSettings->setValue("UseRegExp", m_bUseRegExp);
    m_pSettings->endGroup();

    // Proxy
    m_pSettings->beginGroup("Proxy");
    m_pSettings->setValue("HostName", m_sProxyHostName);
    if (0 == m_nProxyPort) {
        m_pSettings->setValue("Port", "");
    } else {
        m_pSettings->setValue("Port", m_nProxyPort);
    }
    m_pSettings->setValue("UserName", m_sProxyUserName);
    m_pSettings->setValue("Password", m_sProxyPassword);
    m_pSettings->endGroup();

    // Plugins
    m_pSettings->beginGroup("Plugins");
    if (m_sListDisabledPlugins.isEmpty()) {
        m_pSettings->setValue("Disabled", "");
    } else {
        m_pSettings->setValue("Disabled", m_sListDisabledPlugins);
    }
    m_pSettings->endGroup();

    // Save toolbar position etc.
    m_pSettings->beginGroup("Window");
    m_pSettings->setValue("Geometry", WinGeometry);
    m_pSettings->setValue("WindowState", WinState);
    if (true == m_bPreviewAlongside && true == m_bPreviewInEditor) {
        m_pSettings->setValue("SplitterState", SplitterState);
    } else {
        m_pSettings->setValue("SplitterState", m_aSplitterState);
    }
    m_pSettings->endGroup();

    // Save syntax highlighting
    m_pHighlighter->saveStyle();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Remove obsolete entries

void CSettings::removeObsolete() {
    m_pSettings->remove("ConfVersion");
    m_pSettings->remove("ShowStatusbar");
    m_pSettings->remove("SpellCheckerLanguage");
    m_pSettings->beginGroup("FindDialog");
    m_pSettings->remove("");
    m_pSettings->endGroup();
    m_pSettings->beginGroup("FindReplaceDialog");
    m_pSettings->remove("");
    m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Get / set methods

QString CSettings::getGuiLanguage() const {
    if ("auto" == m_sGuiLanguage) {
        #ifdef Q_OS_UNIX
        QByteArray lang = qgetenv("LANG");
        if (!lang.isEmpty()) {
            return QLocale(lang).name();
        }
        #endif
        return QLocale::system().name();
    } else {
        return m_sGuiLanguage;
    }
}

QString CSettings::getInyokaUrl() const {
    return m_sInyokaUrl;
}

bool CSettings::getCodeCompletion() const {
    return m_bCodeCompletion;
}

bool CSettings::getAutomaticImageDownload() const {
    return m_bAutomaticImageDownload;
}

bool CSettings::getPreviewInEditor() const {
    return m_bPreviewInEditor;
}

bool CSettings::getPreviewAlongside() const {
    return m_bPreviewAlongside;
}

QDir CSettings::getLastOpenedDir() const {
    return m_LastOpenedDir;
}

void CSettings::setLastOpenedDir(const QDir &LastDir) {
    m_LastOpenedDir = LastDir;
}

bool CSettings::getCheckLinks() const {
    return m_bCheckLinks;
}

QString CSettings::getTemplateLanguage() const {
    return m_sTemplateLang;
}

quint32 CSettings::getAutoSave() const {
    return m_nAutosave;
}

qint32 CSettings::getReloadPreviewKey() const {
    QString sTmp = m_sReloadPreviewKey;
    return sTmp.remove("0x", Qt::CaseInsensitive).toInt(0, 16);
}

quint32 CSettings::getTimedPreview() const {
    return m_nTimedPreview;
}

bool CSettings::getSyncScrollbars() const {
    return m_bSyncScrollbars;
}

QString CSettings::getStyleFile() const {
    return m_sStyleFile;
}

// ----------------------------------------------------

QFont CSettings::getEditorFont() const {
    return m_EditorFont;
}

// ----------------------------------------------------

quint16 CSettings::getNumOfRecentFiles() const {
    return (quint16)m_nMaxLastOpenedFiles;
}

quint16 CSettings::getMaxNumOfRecentFiles() const {
    return (quint16)m_cMAXFILES;
}

QStringList CSettings::getRecentFiles() const {
    return m_sListRecentFiles;
}

void CSettings::setRecentFiles(const QStringList &sListNewRecent) {
    quint16 iCnt;
    if (sListNewRecent.size() > m_cMAXFILES) {
        iCnt = m_cMAXFILES;
    } else {
        iCnt = sListNewRecent.size();
    }

    m_sListRecentFiles.clear();
    for (int i = 0; i < iCnt; i++) {
        m_sListRecentFiles << sListNewRecent[i];
    }
}

// ----------------------------------------------------

bool CSettings::getWindowsCheckUpdate() const {
    return m_bWinCheckUpdate;
}

void CSettings::setWindowsCheckUpdate(const bool bValue) {
    m_bWinCheckUpdate = bValue;
}

// ----------------------------------------------------

QString CSettings::getTextFind() const {
    return m_sTextFind;
}
QString CSettings::getTextReplace() const {
    return m_sTextReplace;
}
void CSettings::setTextFind(const QString sNewText) {
    m_sTextFind = sNewText;
}
void CSettings::setTextReplace(const QString sNewText) {
    m_sTextReplace = sNewText;
}

bool CSettings::getSearchForwardState() const {
    return m_bSearchForward;
}
void CSettings::setSearchForwardState(const bool bForward) {
    m_bSearchForward = bForward;
}

bool CSettings::getCaseState() const {
    return m_bCheckCase;
}
void CSettings::setCaseState(const bool bNewState) {
    m_bCheckCase = bNewState;
}

bool CSettings::getWholeWordState() const {
    return m_bCheckWholeWord;
}
void CSettings::setWholeWordState(const bool bNewState) {
    m_bCheckWholeWord = bNewState;
}

bool CSettings::getUseRegExpState() const {
    return m_bUseRegExp;
}
void CSettings::setUseRegExpState(const bool bNewState) {
    m_bUseRegExp = bNewState;
}

// ----------------------------------------------------

QByteArray CSettings::getWindowGeometry() const {
    return m_aWindowGeometry;
}
QByteArray CSettings::getWindowState() const {
    return m_aWindowState;
}
QByteArray CSettings::getSplitterState() const {
    return m_aSplitterState;
}

// ----------------------------------------------------

QString CSettings::getProxyHostName() const {
    return m_sProxyHostName;
}
quint16 CSettings::getProxyPort() const {
    return m_nProxyPort;
}
QString CSettings::getProxyUserName() const {
    return m_sProxyUserName;
}
QString CSettings::getProxyPassword() const {
    return m_sProxyPassword;
}

// ----------------------------------------------------

QStringList CSettings::getDisabledPlugins() const {
    return m_sListDisabledPlugins;
}

// ----------------------------------------------------

QString CSettings::getFileName() const {
    return m_pSettings->fileName();
}

// ----------------------------------------------------

QString CSettings::getHighlightBG() const {
    return m_pHighlighter->getHighlightBG();
}
QString CSettings::getHighlightFG() const {
    return m_pHighlighter->getHighlightFG();
}
