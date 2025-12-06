// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_SETTINGS_H_
#define APPLICATION_SETTINGS_H_

#include <QDir>
#include <QFont>

#include "./ieditorplugin.h"  // Cannot use forward declaration (since Qt 6)

class QSettings;

class SettingsDialog;

class Settings : public QObject {
  Q_OBJECT

 public:
  Settings(QWidget *pParent, const QString &sSharePath,
           QObject *pObj = nullptr);
  ~Settings();

  // Load / save application settings
  void readSettings(const QString &sSharePath);
  void writeSettings(const QByteArray &WinGeometry, const QByteArray &WinState,
                     const QByteArray &SplitterState = nullptr);

  void setLastOpenedDir(const QString &sLastDir);

  // General
  auto getGuiLanguage() const -> QString;
  auto getCodeCompletion() const -> bool;
  auto getSyntaxCheck() const -> bool;
  auto getAutomaticImageDownload() const -> bool;
  auto getPreviewHorizontal() const -> bool;
  auto getLastOpenedDir() const -> QDir;
  auto getCheckLinks() const -> bool;
  auto getAutoSave() const -> quint32;
  auto getReloadPreviewKey() const -> qint32;
  auto getTimedPreview() const -> quint32;
  auto getSyncScrollbars() const -> bool;
  auto getWindowsCheckUpdate() const -> bool;
  auto getPygmentize() const -> QString;

  // Inyoka community
  auto getInyokaCommunity() const -> QString;
  auto getInyokaUrl() const -> QString;
  auto getInyokaConstructionArea() const -> QString;
  auto getInyokaCookieDomain() const -> QString;
  auto getInyokaLoginUrl() const -> QString;
  auto getInyokaUser() const -> QString;
  auto getInyokaPassword() const -> QString;

  // Font
  auto getEditorFont() const -> QFont;

  // Window state
  auto getWindowState() const -> QByteArray;
  auto getWindowGeometry() const -> QByteArray;
  auto getSplitterState() const -> QByteArray;
  auto isDarkScheme() const -> bool;

  // Recent files
  auto getNumOfRecentFiles() const -> quint16;
  static auto getMaxNumOfRecentFiles() -> quint16;
  auto getRecentFiles() const -> QStringList;
  void setRecentFiles(const QStringList &sListNewRecent);

  // Proxy
  auto getProxyHostName() const -> QString;
  auto getProxyPort() const -> quint16;
  auto getProxyUserName() const -> QString;
  auto getProxyPassword() const -> QString;

  // Plugins
  auto getDisabledPlugins() const -> QStringList;

  // Allow SettingsDialog to access private members
  friend class SettingsDialog;

 public slots:
  void setWindowsCheckUpdate(const bool bValue);

 signals:
  void showSettingsDialog();
  void changeLang(const QString &sLang);
  void updateUiLang();
  void updateEditorSettings();
  void availablePlugins(const QList<IEditorPlugin *> &PluginList,
                        const QList<QObject *> &PluginObjList);

 private:
  void removeObsolete();

  QSettings *m_pSettings;
  SettingsDialog *m_pSettingsDialog;
  QWidget *m_pParent;

  // General
  QString m_sGuiLanguage;
  bool m_bCodeCompletion{};
  bool m_bSyntaxCheck{};
  bool m_bPreviewSplitHorizontal{};
  QDir m_LastOpenedDir;
  bool m_bAutomaticImageDownload{};
  bool m_bCheckLinks{};
  quint32 m_nAutosave{};
  QString m_sReloadPreviewKey;
  quint32 m_nTimedPreview{};
  bool m_bSyncScrollbars{};
  bool m_bWinCheckUpdate{};
  QString m_sPygmentize;

  // Inyoka community
  QString m_sInyokaCommunity;
  QString m_sInyokaUrl;
  QString m_sInyokaLoginUrl;
  QString m_sInyokaConstArea;
  QString m_sInyokaCookieDomain;
  QString m_sInyokaUser;
  QString m_sInyokaPassword;

  // Font
  QFont m_EditorFont;
  QString m_sFontFamily;
  qreal m_nFontsize{};

  // Window state
  QByteArray m_aWindowState;
  QByteArray m_aWindowGeometry;
  QByteArray m_aSplitterState;

  // Last opened
  static const quint16 m_cMAXFILES = 10;
  qint32 m_nMaxLastOpenedFiles{};
  QStringList m_sListRecentFiles;

  // Proxy
  QString m_sProxyHostName;
  quint16 m_nProxyPort{};
  QString m_sProxyUserName;
  QString m_sProxyPassword;

  // Plugins
  QStringList m_sListDisabledPlugins;
};

#endif  // APPLICATION_SETTINGS_H_
