// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_SETTINGS_H_
#define APPLICATION_SETTINGS_H_

#include <QDir>
#include <QFont>
#include <QSettings>
#include <QWidget>

#include "./ieditorplugin.h"  // Cannot use forward declaration (since Qt 6)

class Settings : public QObject {
  Q_OBJECT

 public:
  static Settings *instance();
  auto getSharePath() const -> QString;
  void setSharePath(const QString &sPath);

  // General
  auto getGuiLanguage() -> QString;
  void setGuiLanguage(const QString &sGuiLanguage);
  auto getCodeCompletion() const -> bool;
  void setCodeCompletion(const bool bCodeCompletion);
  auto getSyntaxCheck() const -> bool;
  void setSyntaxCheck(const bool bSyntaxCheck);
  auto getAutomaticImageDownload() const -> bool;
  void setAutomaticImageDownload(const bool bAutoImageDownload);
  auto getPreviewHorizontal() const -> bool;
  void setPreviewHorizontal(const bool bPreviewHorizontal);
  auto getLastOpenedDir() const -> QDir;
  void setLastOpenedDir(const QString &sLastDir);
  auto getCheckLinks() const -> bool;
  void setCheckLinks(const bool bCheckLinks);
  auto getAutoSave() const -> quint32;
  void setAutoSave(const quint32 nAutosave);
  auto getReloadPreviewKey() const -> qint32;
  void setReloadPreviewKey(const QString &sReloadPreviewKey);
  auto getTimedPreview() const -> quint32;
  void setTimedPreview(const quint32 nTimedPreview);
  auto getSyncScrollbars() const -> bool;
  void setSyncScrollbars(const bool bSyncScrollbars);
  auto getWindowsCheckUpdate() const -> bool;
  auto getPygmentize() const -> QString;
  void setPygmentize(const QString &sPygmentize);

  // Inyoka community
  auto getInyokaCommunity() -> QString;
  void setInyokaCommunity(const QString &sCommunity);
  auto getInyokaUrl() const -> QString;
  auto getInyokaConstructionArea() const -> QString;
  auto getInyokaCookieDomain() const -> QString;
  auto getInyokaLoginUrl() const -> QString;
  auto getInyokaUser() const -> QString;
  void setInyokaUser(const QString &sInyokaUser);
  auto getInyokaPassword() const -> QString;
  void setInyokaPassword(const QString &sInyokaPassword);

  // Font
  auto getEditorFont() -> QFont;
  void setEditorFont(const QFont EditorFont);

  // Window state
  auto getWindowState() const -> QByteArray;
  auto getWindowGeometry() const -> QByteArray;
  auto getSplitterState() const -> QByteArray;
  void saveWindowStates(const QByteArray &WinGeometry,
                        const QByteArray &WinState,
                        const QByteArray &SplitterState = nullptr);
  auto isDarkScheme() const -> bool;

  // Recent files
  auto getNumOfRecentFiles() const -> quint16;
  void setNumOfRecentFiles(const quint16 nNumOfRecentFiles);
  auto getRecentFiles() const -> QStringList;
  void setRecentFiles(const QStringList &sListNewRecent);

  // Proxy
  auto getProxyHostName() const -> QString;
  void setProxyHostName(const QString &sProxyHostName);
  auto getProxyPort() const -> quint16;
  void setProxyPort(const quint16 nProxyPort);
  auto getProxyUserName() const -> QString;
  void setProxyUserName(const QString &sProxyUserName);
  auto getProxyPassword() const -> QString;
  void setProxyPassword(const QString &sProxyPassword);

  // Plugins
  auto getDisabledPlugins() const -> QStringList;
  void setDisabledPlugins(const QStringList &sListDisabledPlugins);

  static const quint16 MAX_RECENT_FILES = 10;

 public slots:
  void setWindowsCheckUpdate(const bool bWinCheckUpdate);

 private:
  void removeObsolete();
  void readInyokaCommunityFile(const QString &sCommunity);

  explicit Settings(QWidget *pParent = nullptr);
  QWidget *m_pParent;
  QSettings m_settings;
  QString m_sSharePath;

  // Inyoka community
  QString m_sInyokaUrl;
  QString m_sInyokaLoginUrl;
  QString m_sInyokaConstArea;
  QString m_sInyokaCookieDomain;
};

#endif  // APPLICATION_SETTINGS_H_
