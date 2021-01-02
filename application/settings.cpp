/**
 * \file settings.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2021 The InyokaEdit developers
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
 * along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Read and write settings.
 */

#include "./settings.h"

#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

#include "./settingsdialog.h"
#include "./ieditorplugin.h"

Settings::Settings(QWidget *pParent, const QString &sSharePath, QObject *pObj) {
  Q_UNUSED(pObj)
#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
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
  delete m_pSettingsDialog;
  m_pSettingsDialog = nullptr;
  delete m_pSettings;
  m_pSettings = nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::readSettings(const QString &sSharePath) {
  // General settings
  m_sGuiLanguage = m_pSettings->value(QStringLiteral("GuiLanguage"),
                                      "auto").toString();
  m_bCodeCompletion = m_pSettings->value(QStringLiteral("CodeCompletion"),
                                         true).toBool();
  m_bSyntaxCheck = m_pSettings->value(QStringLiteral("InyokaSyntaxCheck"),
                                      true).toBool();
  m_bPreviewSplitHorizontal = m_pSettings->value(
                                QStringLiteral("PreviewSplitHorizontal"),
                                false).toBool();

  QStringList sListPaths = QStandardPaths::standardLocations(
                             QStandardPaths::DocumentsLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting documents standard path.";
    sListPaths << QLatin1String("");
  }
  m_LastOpenedDir.setPath(m_pSettings->value(QStringLiteral("LastOpenedDir"),
                                             sListPaths[0]).toString());

  m_bAutomaticImageDownload = m_pSettings->value(
                                QStringLiteral("AutomaticImageDownload"),
                                false).toBool();
  m_bCheckLinks = m_pSettings->value(QStringLiteral("CheckLinks"),
                                     false).toBool();
  m_nAutosave = m_pSettings->value(QStringLiteral("AutoSave"), 300).toUInt();
  // 0x01000004 = Qt::Key_Return
  m_sReloadPreviewKey = m_pSettings->value(QStringLiteral("ReloadPreviewKey"),
                                           "0x01000004").toString();
  m_nTimedPreview = m_pSettings->value(QStringLiteral("TimedPreview"),
                                       15).toUInt();

  // TODO(volunteer): Check again as soon as QWebEngine scrolling is available
#ifdef USEQTWEBKIT
  m_bSyncScrollbars = m_pSettings->value(QStringLiteral("SyncScrollbars"),
                                         true).toBool();
#else
  m_bSyncScrollbars = false;
#endif

  m_sPygmentize = m_pSettings->value(QStringLiteral("Pygmentize"),
                                     "/usr/bin/pygmentize").toString();

  m_bWinCheckUpdate = m_pSettings->value(
                        QStringLiteral("WindowsCheckForUpdate"),
                        false).toBool();

  // Inyoka community settings
  m_pSettings->beginGroup(QStringLiteral("Inyoka"));
  m_sInyokaCommunity = m_pSettings->value(QStringLiteral("Community"),
                                          "ubuntuusers_de").toString();
  // Community settings
  QFile communityFile(sSharePath + "/community/" +
                      m_sInyokaCommunity + "/community.conf");
  QSettings communityConfig(communityFile.fileName(), QSettings::IniFormat);
  communityConfig.setIniCodec("UTF-8");

  m_sInyokaUrl = m_pSettings->value(
                   QStringLiteral("WikiUrl"), "").toString();
  if (m_sInyokaUrl.isEmpty()) {
    QString sValue(communityConfig.value(
                     QStringLiteral("WikiUrl"), "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka wiki URL not found!";
    } else {
      m_sInyokaUrl = sValue;
    }
  }
  if (m_sInyokaUrl.endsWith(QLatin1String("/"))) {
    m_sInyokaUrl = m_sInyokaUrl.remove(m_sInyokaUrl.length() - 1, 1);
  }

  m_sInyokaConstArea = m_pSettings->value(
                         QStringLiteral("ConstructionArea"), "").toString();
  if (m_sInyokaConstArea.isEmpty()) {
    QString sValue(communityConfig.value(
                     QStringLiteral("ConstructionArea"), "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka construction area not found!";
    } else {
      m_sInyokaConstArea = sValue;
    }
  }

  m_sInyokaHash = m_pSettings->value(
                    QStringLiteral("Hash"), "").toString();
  if (m_sInyokaHash.isEmpty()) {
    QString sValue(communityConfig.value(
                     QStringLiteral("Hash"), "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka hash is empty!";
    } else {
      m_sInyokaHash = sValue;
    }
  }

  m_sInyokaUser = m_pSettings->value(QStringLiteral("UserName"),
                                          "").toString();
  m_sInyokaPassword = QByteArray::fromBase64(
        m_pSettings->value(QStringLiteral("Password"), "").toByteArray());
  m_pSettings->endGroup();

  // Font settings
  m_pSettings->beginGroup(QStringLiteral("Font"));
  m_sFontFamily = m_pSettings->value(QStringLiteral("FontFamily"),
                                     "Monospace").toString();
  // Used string for font size because float isn't saved human readable...
  m_nFontsize = m_pSettings->value(QStringLiteral("FontSize"),
                                   "10.5").toReal();
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
  m_pSettings->beginGroup(QStringLiteral("RecentFiles"));
  m_nMaxLastOpenedFiles = m_pSettings->value(
                            QStringLiteral("NumberOfRecentFiles"), 5).toInt();
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
  m_pSettings->beginGroup(QStringLiteral("Proxy"));
  m_sProxyHostName = m_pSettings->value(
                       QStringLiteral("HostName"), "").toString();
  m_nProxyPort = static_cast<quint16>(m_pSettings->value(
                                        QStringLiteral("Port"), "").toUInt());
  m_sProxyUserName = m_pSettings->value(
                       QStringLiteral("UserName"), "").toString();
  m_sProxyPassword = QByteArray::fromBase64(
        m_pSettings->value(QStringLiteral("Password"), "").toByteArray());
  m_pSettings->endGroup();

  // Plugins
  m_pSettings->beginGroup(QStringLiteral("Plugins"));
  m_sListDisabledPlugins = m_pSettings->value(QStringLiteral("Disabled"),
                                              "").toStringList();
  m_pSettings->endGroup();

  // Window state
  m_pSettings->beginGroup(QStringLiteral("Window"));
  m_aWindowGeometry = m_pSettings->value(
                        QStringLiteral("Geometry")).toByteArray();
  // Restore toolbar position etc.
  m_aWindowState = m_pSettings->value(
                     QStringLiteral("WindowState")).toByteArray();
  m_aSplitterState = m_pSettings->value(
                       QStringLiteral("SplitterState")).toByteArray();
  m_nDarkThreshold = m_pSettings->value(
        QStringLiteral("DarkThreshold"), 0.5).toDouble();
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Settings::writeSettings(const QByteArray &WinGeometry,
                             const QByteArray &WinState,
                             const QByteArray &SplitterState) {
  this->removeObsolete();

  // General settings
  m_pSettings->setValue(QStringLiteral("GuiLanguage"), m_sGuiLanguage);
  m_pSettings->setValue(QStringLiteral("CodeCompletion"), m_bCodeCompletion);
  m_pSettings->setValue(QStringLiteral("InyokaSyntaxCheck"), m_bSyntaxCheck);
  m_pSettings->setValue(QStringLiteral("PreviewSplitHorizontal"),
                        m_bPreviewSplitHorizontal);

  m_pSettings->setValue(QStringLiteral("LastOpenedDir"),
                        m_LastOpenedDir.absolutePath());
  m_pSettings->setValue(QStringLiteral("AutomaticImageDownload"),
                        m_bAutomaticImageDownload);
  m_pSettings->setValue(QStringLiteral("CheckLinks"), m_bCheckLinks);
  m_pSettings->setValue(QStringLiteral("AutoSave"), m_nAutosave);
  m_pSettings->setValue(QStringLiteral("ReloadPreviewKey"),
                        m_sReloadPreviewKey);
  m_pSettings->setValue(QStringLiteral("TimedPreview"), m_nTimedPreview);
  m_pSettings->setValue(QStringLiteral("SyncScrollbars"), m_bSyncScrollbars);
  m_pSettings->setValue(QStringLiteral("Pygmentize"), m_sPygmentize);
#if defined _WIN32
  m_pSettings->setValue(QStringLiteral("WindowsCheckForUpdate"),
                        m_bWinCheckUpdate);
#endif

  // Inyoka community settings
  m_pSettings->beginGroup(QStringLiteral("Inyoka"));
  m_pSettings->setValue(QStringLiteral("Community"), m_sInyokaCommunity);
  m_pSettings->setValue(QStringLiteral("WikiUrl"), m_sInyokaUrl);
  m_pSettings->setValue(QStringLiteral("ConstructionArea"), m_sInyokaConstArea);
  m_pSettings->setValue(QStringLiteral("Hash"), m_sInyokaHash);
  m_pSettings->setValue(QStringLiteral("UserName"), m_sInyokaUser);
  QByteArray ba;
  ba.append(m_sInyokaPassword.toUtf8());
  m_pSettings->setValue(QStringLiteral("Password"), ba.toBase64());
  m_pSettings->endGroup();

  // Font settings
  m_pSettings->beginGroup(QStringLiteral("Font"));
  m_pSettings->setValue(QStringLiteral("FontFamily"), m_sFontFamily);
  m_pSettings->setValue(QStringLiteral("FontSize"), m_nFontsize);
  m_pSettings->endGroup();

  // Recent files
  m_pSettings->beginGroup(QStringLiteral("RecentFiles"));
  m_pSettings->setValue(QStringLiteral("NumberOfRecentFiles"),
                        m_nMaxLastOpenedFiles);
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
  m_pSettings->beginGroup(QStringLiteral("Proxy"));
  m_pSettings->setValue(QStringLiteral("HostName"), m_sProxyHostName);
  if (0 == m_nProxyPort) {
    m_pSettings->setValue(QStringLiteral("Port"), "");
  } else {
    m_pSettings->setValue(QStringLiteral("Port"), m_nProxyPort);
  }
  m_pSettings->setValue(QStringLiteral("UserName"), m_sProxyUserName);
  ba.clear();
  ba.append(m_sProxyPassword.toUtf8());
  m_pSettings->setValue(QStringLiteral("Password"), ba.toBase64());
  m_pSettings->endGroup();

  // Plugins
  m_pSettings->beginGroup(QStringLiteral("Plugins"));
  if (m_sListDisabledPlugins.isEmpty()) {
    m_pSettings->setValue(QStringLiteral("Disabled"), "");
  } else {
    m_pSettings->setValue(QStringLiteral("Disabled"), m_sListDisabledPlugins);
  }
  m_pSettings->endGroup();

  // Save toolbar position etc.
  m_pSettings->beginGroup(QStringLiteral("Window"));
  m_pSettings->setValue(QStringLiteral("Geometry"), WinGeometry);
  m_pSettings->setValue(QStringLiteral("WindowState"), WinState);
  m_pSettings->setValue(QStringLiteral("SplitterState"), SplitterState);
  // m_pSettings->setValue(QStringLiteral("DarkThreshold"), m_nDarkThreshold);
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Remove obsolete entries

void Settings::removeObsolete() {
  m_pSettings->remove(QStringLiteral("ConfVersion"));
  m_pSettings->remove(QStringLiteral("ShowStatusbar"));
  m_pSettings->remove(QStringLiteral("SpellCheckerLanguage"));
  m_pSettings->remove(QStringLiteral("PreviewAlongside"));
  m_pSettings->remove(QStringLiteral("PreviewInEditor"));
  m_pSettings->remove(QStringLiteral("TemplateLanguage"));
  m_pSettings->remove(QStringLiteral("InyokaCommunity"));
  m_pSettings->remove(QStringLiteral("Hash"));
  m_pSettings->remove(QStringLiteral("InyokaUrl"));
  m_pSettings->remove(QStringLiteral("ConstructionArea"));
  m_pSettings->beginGroup(QStringLiteral("FindDialog"));
  m_pSettings->remove(QLatin1String(""));
  m_pSettings->endGroup();
  m_pSettings->beginGroup(QStringLiteral("FindReplaceDialog"));
  m_pSettings->remove(QLatin1String(""));
  m_pSettings->endGroup();
  m_pSettings->beginGroup(QStringLiteral("FindReplace"));
  m_pSettings->remove(QLatin1String(""));
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Get / set methods

auto Settings::getGuiLanguage() const -> QString {
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
  }
  return m_sGuiLanguage;
}

auto Settings::getCodeCompletion() const -> bool {
  return m_bCodeCompletion;
}

auto Settings::getSyntaxCheck() const -> bool {
  return m_bSyntaxCheck;
}

auto Settings::getAutomaticImageDownload() const -> bool {
  return m_bAutomaticImageDownload;
}

auto Settings::getPreviewHorizontal() const -> bool {
  return m_bPreviewSplitHorizontal;
}

auto Settings::getLastOpenedDir() const -> QDir {
  return m_LastOpenedDir;
}

void Settings::setLastOpenedDir(const QDir &LastDir) {
  m_LastOpenedDir = LastDir;
}

auto Settings::getCheckLinks() const -> bool {
  return m_bCheckLinks;
}

auto Settings::getAutoSave() const -> quint32 {
  return m_nAutosave;
}

auto Settings::getReloadPreviewKey() const -> qint32 {
  QString sTmp = m_sReloadPreviewKey;
  return sTmp.remove(QStringLiteral("0x"),
                     Qt::CaseInsensitive).toInt(nullptr, 16);
}

auto Settings::getTimedPreview() const -> quint32 {
  return m_nTimedPreview;
}

auto Settings::getSyncScrollbars() const -> bool {
  return m_bSyncScrollbars;
}

auto Settings::getPygmentize() const -> QString {
  return m_sPygmentize;
}

// ----------------------------------------------------

auto Settings::getInyokaCommunity() const -> QString {
  return m_sInyokaCommunity;
}

auto Settings::getInyokaUrl() const -> QString {
  return m_sInyokaUrl;
}

auto Settings::getInyokaConstructionArea() const -> QString {
  return m_sInyokaConstArea;
}

auto Settings::getInyokaHash() const -> QString {
  return m_sInyokaHash;
}

auto Settings::getInyokaUser() const -> QString {
  return m_sInyokaUser;
}

auto Settings::getInyokaPassword() const -> QString {
  return m_sInyokaPassword;
}

// ----------------------------------------------------

auto Settings::getEditorFont() const -> QFont {
  return m_EditorFont;
}

// ----------------------------------------------------

auto Settings::getNumOfRecentFiles() const -> quint16 {
  return static_cast<quint16>(m_nMaxLastOpenedFiles);
}

auto Settings::getMaxNumOfRecentFiles() -> quint16 {
  return static_cast<quint16>(m_cMAXFILES);
}

auto Settings::getRecentFiles() const -> QStringList {
  return m_sListRecentFiles;
}

void Settings::setRecentFiles(const QStringList &sListNewRecent) {
  quint16 iCnt;
  if (sListNewRecent.size() > m_cMAXFILES) {
    iCnt = m_cMAXFILES;
  } else {
    iCnt = static_cast<quint16>(sListNewRecent.size());
  }

  m_sListRecentFiles.clear();
  for (int i = 0; i < iCnt; i++) {
    m_sListRecentFiles << sListNewRecent[i];
  }
}

// ----------------------------------------------------

auto Settings::getWindowsCheckUpdate() const -> bool {
  return m_bWinCheckUpdate;
}
void Settings::setWindowsCheckUpdate(const bool bValue) {
  m_bWinCheckUpdate = bValue;
}

// ----------------------------------------------------

auto Settings::getWindowGeometry() const -> QByteArray {
  return m_aWindowGeometry;
}
auto Settings::getWindowState() const -> QByteArray {
  return m_aWindowState;
}
auto Settings::getSplitterState() const -> QByteArray {
  return m_aSplitterState;
}
auto Settings::getDarkThreshold() const -> double {
  return m_nDarkThreshold;
}

// ----------------------------------------------------

auto Settings::getProxyHostName() const -> QString {
  return m_sProxyHostName;
}
auto Settings::getProxyPort() const -> quint16 {
  return m_nProxyPort;
}
auto Settings::getProxyUserName() const -> QString {
  return m_sProxyUserName;
}
auto Settings::getProxyPassword() const -> QString {
  return m_sProxyPassword;
}

// ----------------------------------------------------

auto Settings::getDisabledPlugins() const -> QStringList {
  return m_sListDisabledPlugins;
}
