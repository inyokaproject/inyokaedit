/**
 * \file settings.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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
 * Class definition for reading and writing settings.
 */

#ifndef APPLICATION_SETTINGS_H_
#define APPLICATION_SETTINGS_H_

#include <QDir>
#include <QFont>

class QSettings;

class SettingsDialog;
class IEditorPlugin;

/**
 * \class Settings
 * \brief Load and save settings.
 */
class Settings : public QObject {
  Q_OBJECT

 public:
    Settings(QWidget *pParent, const QString &sSharePath,
             QObject *pObj = nullptr);
    ~Settings();

    // Load / save application settings
    void readSettings(const QString &sSharePath);
    void writeSettings(const QByteArray &WinGeometry,
                       const QByteArray &WinState,
                       const QByteArray &SplitterState = nullptr);

    void setLastOpenedDir(const QDir &LastDir);

    // General
    auto getGuiLanguage() const -> QString;
    auto getCodeCompletion() const -> bool;
    auto getSyntaxCheck() const -> bool;
    auto getInyokaUrl() const-> QString;
    auto getInyokaCommunity() const-> QString;
    auto getInyokaConstructionArea() const-> QString;
    auto getInyokaHash() const-> QString;
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

    // Font
    auto getEditorFont() const -> QFont;

    // Window state
    auto getWindowState() const -> QByteArray;
    auto getWindowGeometry() const -> QByteArray;
    auto getSplitterState() const -> QByteArray;
    auto getDarkThreshold() const -> double;

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

    // General
    QString m_sGuiLanguage;
    bool m_bCodeCompletion{};   // Enable / disable code completion
    bool m_bSyntaxCheck{};
    bool m_bPreviewSplitHorizontal{};
    QString m_sInyokaCommunity;
    QString m_sInyokaUrl;     // Url to inyoka
    QString m_sInyokaConstArea;
    QString m_sInyokaHash;
    QDir m_LastOpenedDir;
    bool m_bAutomaticImageDownload{};  // Enable/disable download article images
    bool m_bCheckLinks{};
    quint32 m_nAutosave{};
    QString m_sReloadPreviewKey;
    quint32 m_nTimedPreview{};
    bool m_bSyncScrollbars{};
    bool m_bWinCheckUpdate{};
    QString m_sPygmentize;

    // Font
    QFont m_EditorFont;
    QString m_sFontFamily;
    qreal m_nFontsize{};

    // Window state
    QByteArray m_aWindowState;
    QByteArray m_aWindowGeometry;
    QByteArray m_aSplitterState;
    double m_nDarkThreshold;

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
