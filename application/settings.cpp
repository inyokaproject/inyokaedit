// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./settings.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QStandardPaths>
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#include <QStyleHints>
#endif

Settings *Settings::instance() {
  static Settings _instance;
  return &_instance;
}

Settings::Settings(QWidget *pParent)
    : m_pParent(pParent),
#if defined __linux__
      m_settings(QSettings::NativeFormat, QSettings::UserScope,
                 qApp->applicationName().toLower(),
                 qApp->applicationName().toLower())
#else
      m_settings(QSettings::IniFormat, QSettings::UserScope,
                 qApp->applicationName().toLower(),
                 qApp->applicationName().toLower())
#endif
{
  this->removeObsolete();
}

// ----------------------------------------------------------------------------

auto Settings::getSharePath() const -> QString { return m_sSharePath; }

void Settings::setSharePath(const QString &sPath) { m_sSharePath = sPath; }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Remove obsolete entries

void Settings::removeObsolete() {
  m_settings.remove(QStringLiteral("ConfVersion"));
  m_settings.remove(QStringLiteral("ShowStatusbar"));
  m_settings.remove(QStringLiteral("SpellCheckerLanguage"));
  m_settings.remove(QStringLiteral("Style"));
  m_settings.remove(QStringLiteral("PreviewAlongside"));
  m_settings.remove(QStringLiteral("PreviewInEditor"));
  m_settings.remove(QStringLiteral("TemplateLanguage"));
  m_settings.remove(QStringLiteral("InyokaCommunity"));
  m_settings.remove(QStringLiteral("Hash"));
  m_settings.remove(QStringLiteral("Inyoka/Hash"));
  m_settings.remove(QStringLiteral("InyokaUrl"));
  m_settings.remove(QStringLiteral("Inyoka/WikiUrl"));
  m_settings.remove(QStringLiteral("ConstructionArea"));
  m_settings.remove(QStringLiteral("Inyoka/ConstructionArea"));
  m_settings.remove(QStringLiteral("Window/DarkThreshold"));
  m_settings.beginGroup(QStringLiteral("FindDialog"));
  m_settings.remove(QLatin1String(""));
  m_settings.endGroup();
  m_settings.beginGroup(QStringLiteral("FindReplaceDialog"));
  m_settings.remove(QLatin1String(""));
  m_settings.endGroup();
  m_settings.beginGroup(QStringLiteral("FindReplace"));
  m_settings.remove(QLatin1String(""));
  m_settings.endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// General

auto Settings::getGuiLanguage() -> QString {
  QString sGuiLanguage =
      m_settings.value(QStringLiteral("GuiLanguage"), QStringLiteral("auto"))
          .toString();

  // Automatically detected language
  if ("auto" == sGuiLanguage) {
#ifdef Q_OS_UNIX
    QByteArray lang = qgetenv("LANG");
    if (!lang.isEmpty()) {
      return QLocale(QString::fromLatin1(lang)).name();
    }
#endif
    return QLocale::system().name();
  }

  // Specific language selected
  if (!QFile(":/" + qApp->applicationName().toLower() + "_" + sGuiLanguage +
             ".qm")
           .exists() &&
      !QFile(m_sSharePath + "/lang/" + qApp->applicationName().toLower() + "_" +
             sGuiLanguage + ".qm")
           .exists()) {
    sGuiLanguage = QStringLiteral("en");
    this->setGuiLanguage(sGuiLanguage);
  }

  return sGuiLanguage;
}
void Settings::setGuiLanguage(const QString &sGuiLanguage) {
  m_settings.setValue(QStringLiteral("GuiLanguage"), sGuiLanguage);
}

auto Settings::getCodeCompletion() const -> bool {
  return m_settings.value(QStringLiteral("CodeCompletion"), true).toBool();
}
void Settings::setCodeCompletion(const bool bCodeCompletion) {
  m_settings.setValue(QStringLiteral("CodeCompletion"), bCodeCompletion);
}

auto Settings::getSyntaxCheck() const -> bool {
  return m_settings.value(QStringLiteral("InyokaSyntaxCheck"), true).toBool();
}
void Settings::setSyntaxCheck(const bool bSyntaxCheck) {
  m_settings.setValue(QStringLiteral("InyokaSyntaxCheck"), bSyntaxCheck);
}

auto Settings::getAutomaticImageDownload() const -> bool {
  return m_settings.value(QStringLiteral("AutomaticImageDownload"), false)
      .toBool();
}
void Settings::setAutomaticImageDownload(const bool bAutoImageDownload) {
  m_settings.setValue(QStringLiteral("AutomaticImageDownload"),
                      bAutoImageDownload);
}

auto Settings::getPreviewHorizontal() const -> bool {
  return m_settings.value(QStringLiteral("PreviewSplitHorizontal"), false)
      .toBool();
}
void Settings::setPreviewHorizontal(const bool bPreviewHorizontal) {
  m_settings.setValue(QStringLiteral("PreviewSplitHorizontal"),
                      bPreviewHorizontal);
}

auto Settings::getLastOpenedDir() const -> QDir {
  QStringList sListPaths =
      QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
  if (sListPaths.isEmpty()) {
    qCritical() << "Error while getting documents standard path.";
    sListPaths << QLatin1String("");
  }

  return QDir(m_settings.value(QStringLiteral("LastOpenedDir"), sListPaths[0])
                  .toString());
}
void Settings::setLastOpenedDir(const QString &sLastDir) {
  m_settings.setValue(QStringLiteral("LastOpenedDir"), sLastDir);
}

auto Settings::getCheckLinks() const -> bool {
  return m_settings.value(QStringLiteral("CheckLinks"), false).toBool();
}
void Settings::setCheckLinks(const bool bCheckLinks) {
  m_settings.setValue(QStringLiteral("CheckLinks"), bCheckLinks);
}

auto Settings::getAutoSave() const -> quint32 {
  return m_settings.value(QStringLiteral("AutoSave"), 300).toUInt();
}
void Settings::setAutoSave(const quint32 nAutosave) {
  m_settings.setValue(QStringLiteral("AutoSave"), nAutosave);
}

auto Settings::getReloadPreviewKey() const -> qint32 {
#ifdef NOPREVIEW
  return QStringLiteral("0x0");
#else
  // 0x01000004 = Qt::Key_Return
  QString sReloadKey(
      m_settings.value(QStringLiteral("ReloadPreviewKey"), "0x01000004")
          .toString());
  return sReloadKey.remove(QStringLiteral("0x"), Qt::CaseInsensitive)
      .toInt(nullptr, 16);
#endif
}
void Settings::setReloadPreviewKey(const QString &sReloadPreviewKey) {
  QString sReloadKey = sReloadPreviewKey;
  if (!sReloadKey.startsWith(QLatin1String("0x"))) {
    sReloadKey = "0x" + sReloadKey;
  }
  m_settings.setValue(QStringLiteral("ReloadPreviewKey"), sReloadKey);
}

auto Settings::getTimedPreview() const -> quint32 {
  return m_settings.value(QStringLiteral("TimedPreview"), 15).toUInt();
}
void Settings::setTimedPreview(const quint32 nTimedPreview) {
  m_settings.setValue(QStringLiteral("TimedPreview"), nTimedPreview);
}

auto Settings::getSyncScrollbars() const -> bool {
#ifdef NOPREVIEW
  return false;
#else
  return m_settings.value(QStringLiteral("SyncScrollbars"), true).toBool();
#endif
}
void Settings::setSyncScrollbars(const bool bSyncScrollbars) {
  m_settings.setValue(QStringLiteral("SyncScrollbars"), bSyncScrollbars);
}

auto Settings::getWindowsCheckUpdate() const -> bool {
  return m_settings.value(QStringLiteral("WindowsCheckForUpdate"), false)
      .toBool();
}
void Settings::setWindowsCheckUpdate(const bool bWinCheckUpdate) {
  m_settings.setValue(QStringLiteral("WindowsCheckForUpdate"), bWinCheckUpdate);
}

auto Settings::getPygmentize() const -> QString {
  return m_settings.value(QStringLiteral("Pygmentize"), "/usr/bin/pygmentize")
      .toString();
}
void Settings::setPygmentize(const QString &sPygmentize) {
  m_settings.setValue(QStringLiteral("Pygmentize"), sPygmentize);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Inyoka community settings

auto Settings::getInyokaCommunity() -> QString {
  QString sCommunity = m_settings
                           .value(QStringLiteral("Inyoka/Community"),
                                  QStringLiteral("ubuntuusers_de"))
                           .toString();
  if (sCommunity.isEmpty()) {
    sCommunity = QStringLiteral("ubuntuusers_de");
  }
  this->readInyokaCommunityFile(sCommunity);
  return sCommunity;
}
void Settings::setInyokaCommunity(const QString &sInyokaCommunity) {
  if (this->getInyokaCommunity() != sInyokaCommunity) {
    this->readInyokaCommunityFile(sInyokaCommunity);
  }
  m_settings.setValue(QStringLiteral("Inyoka/Community"), sInyokaCommunity);
}

auto Settings::getInyokaUser() const -> QString {
  return m_settings.value(QStringLiteral("Inyoka/UserName"), "").toString();
}
void Settings::setInyokaUser(const QString &sInyokaUser) {
  m_settings.setValue(QStringLiteral("Inyoka/UserName"), sInyokaUser);
}

auto Settings::getInyokaPassword() const -> QString {
  return QString::fromLatin1(QByteArray::fromBase64(
      m_settings.value(QStringLiteral("Inyoka/Password"), "").toByteArray()));
}
void Settings::setInyokaPassword(const QString &sInyokaPassword) {
  QByteArray ba;
  ba.append(sInyokaPassword.toUtf8());
  m_settings.setValue(QStringLiteral("Inyoka/Password"), ba.toBase64());
}

// ----------------------------------------------------------------------------

void Settings::readInyokaCommunityFile(const QString &sCommunity) {
  QFile communityFile(m_sSharePath + "/community/" + sCommunity +
                      "/community.conf");
  if (!communityFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(m_pParent, tr("Error"),
                          tr("Could not open/find community file!"));
    qCritical() << "Could not open/find community file:"
                << communityFile.fileName();
  }

  m_sInyokaUrl.clear();
  m_sInyokaLoginUrl.clear();
  m_sInyokaConstArea.clear();
  m_sInyokaCookieDomain.clear();

  QSettings communityConfig(communityFile.fileName(), QSettings::IniFormat);
  QString sValue(
      communityConfig.value(QStringLiteral("WikiUrl"), "").toString());
  if (sValue.isEmpty()) {
    qWarning() << "Inyoka wiki URL not found!";
  } else {
    m_sInyokaUrl = sValue;
  }

  if (m_sInyokaUrl.endsWith(QLatin1String("/"))) {
    m_sInyokaUrl = m_sInyokaUrl.remove(m_sInyokaUrl.length() - 1, 1);
  }

  sValue = communityConfig.value(QStringLiteral("LoginUrl"), "").toString();
  if (sValue.isEmpty()) {
    qWarning() << "Inyoka login URL not found!";
  } else {
    m_sInyokaLoginUrl = sValue;
  }

  sValue =
      communityConfig.value(QStringLiteral("ConstructionArea"), "").toString();
  if (sValue.isEmpty()) {
    qWarning() << "Inyoka construction area not found!";
  } else {
    m_sInyokaConstArea = sValue;
  }

  sValue = communityConfig.value(QStringLiteral("CookieDomain"), "").toString();
  if (sValue.isEmpty()) {
    qWarning() << "Inyoka session cookie domain not found!";
  } else {
    m_sInyokaCookieDomain = sValue;
  }

  if (m_sInyokaUrl.isEmpty() || m_sInyokaLoginUrl.isEmpty() ||
      m_sInyokaConstArea.isEmpty() || m_sInyokaCookieDomain.isEmpty()) {
    qWarning() << "Community.conf not complete!";
    QMessageBox::warning(m_pParent, tr("Warning"),
                         tr("Community.conf not complete!"));
  }
}

auto Settings::getInyokaUrl() const -> QString { return m_sInyokaUrl; }

auto Settings::getInyokaLoginUrl() const -> QString {
  return m_sInyokaLoginUrl;
}

auto Settings::getInyokaConstructionArea() const -> QString {
  return m_sInyokaConstArea;
}

auto Settings::getInyokaCookieDomain() const -> QString {
  return m_sInyokaCookieDomain;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Font settings

auto Settings::getEditorFont() -> QFont {
  QFont EditorFont;
  m_settings.beginGroup(QStringLiteral("Font"));
  EditorFont.setFamily(
      m_settings.value(QStringLiteral("FontFamily"), "Monospace").toString());
  // Used string for font size because float isn't saved human readable...
  qreal nTmpFontsize =
      m_settings.value(QStringLiteral("FontSize"), "10.5").toReal();
  if (nTmpFontsize <= 0) {
    nTmpFontsize = 10.5;
  }
  EditorFont.setPointSizeF(nTmpFontsize);
  EditorFont.setFixedPitch(true);
  // Font matcher prefers fixed pitch fonts
  EditorFont.setStyleHint(QFont::TypeWriter);
  m_settings.endGroup();

  return EditorFont;
}
void Settings::setEditorFont(const QFont EditorFont) {
  m_settings.beginGroup(QStringLiteral("Font"));
  m_settings.setValue(QStringLiteral("FontFamily"), EditorFont.family());
  m_settings.setValue(QStringLiteral("FontSize"), EditorFont.pointSizeF());
  m_settings.endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Recent files

auto Settings::getNumOfRecentFiles() const -> quint16 {
  qint32 nNumberOfRecentFiles =
      m_settings.value(QStringLiteral("RecentFiles/NumberOfRecentFiles"), 5)
          .toInt();
  if (nNumberOfRecentFiles < 0) {
    nNumberOfRecentFiles = 0;
  }
  if (nNumberOfRecentFiles > MAX_RECENT_FILES) {
    nNumberOfRecentFiles = MAX_RECENT_FILES;
  }
  return static_cast<quint16>(nNumberOfRecentFiles);
}
void Settings::setNumOfRecentFiles(const quint16 nNumOfRecentFiles) {
  m_settings.setValue(QStringLiteral("RecentFiles/NumberOfRecentFiles"),
                      nNumOfRecentFiles);
}

auto Settings::getRecentFiles() const -> QStringList {
  QStringList sListRecentFiles;

  for (int i = 0; i < this->getNumOfRecentFiles(); i++) {
    QString sTmpFile =
        m_settings.value("RecentFiles/File_" + QString::number(i), "")
            .toString();
    if (!sTmpFile.isEmpty()) {
      sListRecentFiles << sTmpFile;
    }
  }
  sListRecentFiles.removeDuplicates();

  return sListRecentFiles;
}

void Settings::setRecentFiles(const QStringList &sListNewRecent) {
  for (int i = 0; i < MAX_RECENT_FILES; i++) {
    if (i < sListNewRecent.size()) {
      m_settings.setValue("RecentFiles/File_" + QString::number(i),
                          sListNewRecent[i]);
    } else {
      m_settings.setValue("RecentFiles/File_" + QString::number(i), "");
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Proxy

auto Settings::getProxyHostName() const -> QString {
  return m_settings.value(QStringLiteral("Proxy/HostName"), "").toString();
}
void Settings::setProxyHostName(const QString &sProxyHostName) {
  m_settings.setValue(QStringLiteral("Proxy/HostName"), sProxyHostName);
}

auto Settings::getProxyPort() const -> quint16 {
  return static_cast<quint16>(
      m_settings.value(QStringLiteral("Proxy/Port"), "").toUInt());
}
void Settings::setProxyPort(const quint16 nProxyPort) {
  if (0 == nProxyPort) {
    m_settings.setValue(QStringLiteral("Proxy/Port"), "");
  } else {
    m_settings.setValue(QStringLiteral("Proxy/Port"), nProxyPort);
  }
}

auto Settings::getProxyUserName() const -> QString {
  return m_settings.value(QStringLiteral("Proxy/UserName"), "").toString();
}
void Settings::setProxyUserName(const QString &sProxyUserName) {
  m_settings.setValue(QStringLiteral("Proxy/UserName"), sProxyUserName);
}

auto Settings::getProxyPassword() const -> QString {
  return QString::fromLatin1(QByteArray::fromBase64(
      m_settings.value(QStringLiteral("Proxy/Password"), "").toByteArray()));
}
void Settings::setProxyPassword(const QString &sProxyPassword) {
  QByteArray ba;
  ba.append(sProxyPassword.toUtf8());
  m_settings.setValue(QStringLiteral("Proxy/Password"), ba.toBase64());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Plugins

auto Settings::getDisabledPlugins() const -> QStringList {
  return m_settings.value(QStringLiteral("Plugins/Disabled"), "")
      .toStringList();
}
void Settings::setDisabledPlugins(const QStringList &sListDisabledPlugins) {
  if (sListDisabledPlugins.isEmpty()) {
    m_settings.setValue(QStringLiteral("Plugins/Disabled"), "");
  } else {
    m_settings.setValue(QStringLiteral("Plugins/Disabled"),
                        sListDisabledPlugins);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Window state/geometry/splitter

auto Settings::getWindowGeometry() const -> QByteArray {
  return m_settings.value(QStringLiteral("Window/Geometry")).toByteArray();
}

auto Settings::getWindowState() const -> QByteArray {
  return m_settings.value(QStringLiteral("Window/WindowState")).toByteArray();
}

auto Settings::getSplitterState() const -> QByteArray {
  return m_settings.value(QStringLiteral("Window/SplitterState")).toByteArray();
}

void Settings::saveWindowStates(const QByteArray &WinGeometry,
                                const QByteArray &WinState,
                                const QByteArray &SplitterState) {
  // Save toolbar position etc.
  m_settings.beginGroup(QStringLiteral("Window"));
  m_settings.setValue(QStringLiteral("Geometry"), WinGeometry);
  m_settings.setValue(QStringLiteral("WindowState"), WinState);
  m_settings.setValue(QStringLiteral("SplitterState"), SplitterState);
  m_settings.endGroup();
}

// ----------------------------------------------------------------------------

auto Settings::isDarkScheme() const -> bool {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  if (Qt::ColorScheme::Dark == QGuiApplication::styleHints()->colorScheme()) {
    return true;
  }
#endif

  // Fallback: If window is darker than text
  if (m_pParent->window()->palette().window().color().lightnessF() <
      m_pParent->window()->palette().windowText().color().lightnessF()) {
    return true;
  }

  return false;
}
