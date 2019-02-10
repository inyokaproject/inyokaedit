/**
 * \file settings.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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

#include "./settings.h"

#include <QApplication>
#include <QDebug>
#include <QStandardPaths>

Settings::Settings(QWidget *pParent, const QString &sSharePath) {
#if defined _WIN32
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  this->readSettings(sSharePath);

  m_pSettingsDialog = new SettingsDialog(this, sSharePath, pParent);

  connect(this, &Settings::availablePlugins,
          m_pSettingsDialog, &SettingsDialog::getAvailablePlugins);

  connect(this, &Settings::showSettingsDialog,
          m_pSettingsDialog, &SettingsDialog::show);

  connect(m_pSettingsDialog, &SettingsDialog::changeLang,
          this, &Settings::changeLang);
  connect(this, &Settings::updateUiLang,
          m_pSettingsDialog, &SettingsDialog::updateUiLang);
  connect(m_pSettingsDialog, &SettingsDialog::updatedSettings,
          this, &Settings::updateEditorSettings);
}

Settings::~Settings() {
  if (NULL != m_pSettingsDialog) {
    delete m_pSettingsDialog;
  }
  m_pSettingsDialog = NULL;

  if (NULL != m_pSettings) {
    delete m_pSettings;
    m_pSettings = NULL;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::readSettings(const QString &sSharePath) {
  // General settings
  m_sGuiLanguage = m_pSettings->value("GuiLanguage", "auto").toString();
  m_bCodeCompletion = m_pSettings->value("CodeCompletion", true).toBool();
  m_bSyntaxCheck = m_pSettings->value("InyokaSyntaxCheck", true).toBool();
  m_bPreviewSplitHorizontal = m_pSettings->value("PreviewSplitHorizontal",
                                                 true).toBool();
  m_sInyokaCommunity = m_pSettings->value("InyokaCommunity",
                                          "ubuntuusers_de").toString();
  // Community settings
  QFile communityFile(sSharePath + "/community/" +
                      m_sInyokaCommunity + "/community.conf");
  QSettings communityConfig(communityFile.fileName(), QSettings::IniFormat);
  communityConfig.setIniCodec("UTF-8");

  m_sInyokaUrl = m_pSettings->value("InyokaUrl", "").toString();
  if (m_sInyokaUrl.isEmpty()) {
    QString sValue(communityConfig.value("InyokaUrl", "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka construction area not found!";
    } else {
      m_sInyokaUrl = sValue;
    }
  }
  if (m_sInyokaUrl.endsWith("/")) {
    m_sInyokaUrl = m_sInyokaUrl.remove(m_sInyokaUrl.length() - 1, 1);
  }

  m_sInyokaConstArea = m_pSettings->value("ConstructionArea", "").toString();
  if (m_sInyokaConstArea.isEmpty()) {
    QString sValue(communityConfig.value("ConstructionArea", "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka construction area not found!";
    } else {
      m_sInyokaConstArea = sValue;
    }
  }

  m_sInyokaHash = m_pSettings->value("Hash", "").toString();
  if (m_sInyokaHash.isEmpty()) {
    QString sValue(communityConfig.value("Hash", "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka hash is empty!";
    } else {
      m_sInyokaHash = sValue;
    }
  }

  QStringList sListPaths = QStandardPaths::standardLocations(
                             QStandardPaths::DocumentsLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting documents standard path.";
    sListPaths << "";
  }
  m_LastOpenedDir = m_pSettings->value("LastOpenedDir",
                                       sListPaths[0]).toString();

  m_bAutomaticImageDownload = m_pSettings->value("AutomaticImageDownload",
                                                 false).toBool();
  m_bCheckLinks = m_pSettings->value("CheckLinks", false).toBool();
  m_nAutosave = m_pSettings->value("AutoSave", 300).toUInt();
  // 0x01000004 = Qt::Key_Return
  m_sReloadPreviewKey = m_pSettings->value("ReloadPreviewKey",
                                           "0x01000004").toString();
  m_nTimedPreview = m_pSettings->value("TimedPreview", 15).toUInt();

  // TODO(volunteer): Check again as soon as QWebEngine scrolling is available
#ifdef USEQTWEBKIT
  m_bSyncScrollbars = m_pSettings->value("SyncScrollbars", true).toBool();
#else
  m_bSyncScrollbars = false;
#endif

  m_bWinCheckUpdate = m_pSettings->value("WindowsCheckForUpdate",
                                         false).toBool();

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

void Settings::writeSettings(const QByteArray &WinGeometry,
                             const QByteArray &WinState,
                             const QByteArray &SplitterState) {
  this->removeObsolete();

  // General settings
  m_pSettings->setValue("GuiLanguage", m_sGuiLanguage);
  m_pSettings->setValue("CodeCompletion", m_bCodeCompletion);
  m_pSettings->setValue("InyokaSyntaxCheck", m_bSyntaxCheck);
  m_pSettings->setValue("PreviewSplitHorizontal", m_bPreviewSplitHorizontal);
  m_pSettings->setValue("InyokaCommunity", m_sInyokaCommunity);
  m_pSettings->setValue("InyokaUrl", m_sInyokaUrl);
  m_pSettings->setValue("ConstructionArea", m_sInyokaConstArea);
  m_pSettings->setValue("Hash", m_sInyokaHash);
  m_pSettings->setValue("LastOpenedDir", m_LastOpenedDir.absolutePath());
  m_pSettings->setValue("AutomaticImageDownload", m_bAutomaticImageDownload);
  m_pSettings->setValue("CheckLinks", m_bCheckLinks);
  m_pSettings->setValue("AutoSave", m_nAutosave);
  m_pSettings->setValue("ReloadPreviewKey", m_sReloadPreviewKey);
  m_pSettings->setValue("TimedPreview", m_nTimedPreview);
  m_pSettings->setValue("SyncScrollbars", m_bSyncScrollbars);
#if defined _WIN32
  m_pSettings->setValue("WindowsCheckForUpdate", m_bWinCheckUpdate);
#endif

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
  m_pSettings->setValue("SplitterState", SplitterState);
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Remove obsolete entries

void Settings::removeObsolete() {
  m_pSettings->remove("ConfVersion");
  m_pSettings->remove("ShowStatusbar");
  m_pSettings->remove("SpellCheckerLanguage");
  m_pSettings->remove("PreviewAlongside");
  m_pSettings->remove("PreviewInEditor");
  m_pSettings->remove("TemplateLanguage");
  m_pSettings->beginGroup("FindDialog");
  m_pSettings->remove("");
  m_pSettings->endGroup();
  m_pSettings->beginGroup("FindReplaceDialog");
  m_pSettings->remove("");
  m_pSettings->endGroup();
  m_pSettings->beginGroup("FindReplace");
  m_pSettings->remove("");
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Get / set methods

QString Settings::getGuiLanguage() const {
  QString sLang;
  if ("auto" == m_sGuiLanguage) {
#ifdef Q_OS_UNIX
    QByteArray lang = qgetenv("LANG");
    if (!lang.isEmpty()) {
      sLang = QLocale(lang).name();
      return sLang.mid(0, sLang.indexOf('_'));
    }
#endif
    sLang = QLocale::system().name();
    return sLang.mid(0, sLang.indexOf('_'));
  } else {
    return m_sGuiLanguage;
  }
}

QString Settings::getInyokaCommunity() const {
  return m_sInyokaCommunity;
}
QString Settings::getInyokaUrl() const {
  return m_sInyokaUrl;
}
QString Settings::getInyokaConstructionArea() const {
  return m_sInyokaConstArea;
}
QString Settings::getInyokaHash() const {
  return m_sInyokaHash;
}

bool Settings::getCodeCompletion() const {
  return m_bCodeCompletion;
}

bool Settings::getSyntaxCheck() const {
  return m_bSyntaxCheck;
}

bool Settings::getAutomaticImageDownload() const {
  return m_bAutomaticImageDownload;
}

bool Settings::getPreviewHorizontal() const {
  return m_bPreviewSplitHorizontal;
}

QDir Settings::getLastOpenedDir() const {
  return m_LastOpenedDir;
}

void Settings::setLastOpenedDir(const QDir &LastDir) {
  m_LastOpenedDir = LastDir;
}

bool Settings::getCheckLinks() const {
  return m_bCheckLinks;
}

quint32 Settings::getAutoSave() const {
  return m_nAutosave;
}

qint32 Settings::getReloadPreviewKey() const {
  QString sTmp = m_sReloadPreviewKey;
  return sTmp.remove("0x", Qt::CaseInsensitive).toInt(0, 16);
}

quint32 Settings::getTimedPreview() const {
  return m_nTimedPreview;
}

bool Settings::getSyncScrollbars() const {
  return m_bSyncScrollbars;
}

// ----------------------------------------------------

QFont Settings::getEditorFont() const {
  return m_EditorFont;
}

// ----------------------------------------------------

quint16 Settings::getNumOfRecentFiles() const {
  return (quint16)m_nMaxLastOpenedFiles;
}

quint16 Settings::getMaxNumOfRecentFiles() const {
  return (quint16)m_cMAXFILES;
}

QStringList Settings::getRecentFiles() const {
  return m_sListRecentFiles;
}

void Settings::setRecentFiles(const QStringList &sListNewRecent) {
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

bool Settings::getWindowsCheckUpdate() const {
  return m_bWinCheckUpdate;
}
void Settings::setWindowsCheckUpdate(const bool bValue) {
  m_bWinCheckUpdate = bValue;
}

// ----------------------------------------------------

QByteArray Settings::getWindowGeometry() const {
  return m_aWindowGeometry;
}
QByteArray Settings::getWindowState() const {
  return m_aWindowState;
}
QByteArray Settings::getSplitterState() const {
  return m_aSplitterState;
}

// ----------------------------------------------------

QString Settings::getProxyHostName() const {
  return m_sProxyHostName;
}
quint16 Settings::getProxyPort() const {
  return m_nProxyPort;
}
QString Settings::getProxyUserName() const {
  return m_sProxyUserName;
}
QString Settings::getProxyPassword() const {
  return m_sProxyPassword;
}

// ----------------------------------------------------

QStringList Settings::getDisabledPlugins() const {
  return m_sListDisabledPlugins;
}
