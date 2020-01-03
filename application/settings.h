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
 * along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * \section DESCRIPTION
 * Class definition for reading and writing settings.
 */

#ifndef APPLICATION_SETTINGS_H_
#define APPLICATION_SETTINGS_H_

#include <QDir>
#include <QSettings>
#include <QMessageBox>

#include "./settingsdialog.h"
#include "./ieditorplugin.h"

class SettingsDialog;

/**
 * \class Settings
 * \brief Load and save settings.
 */
class Settings : public QObject {
  Q_OBJECT

 public:
    Settings(QWidget *pParent, const QString &sSharePath);
    ~Settings();

    // Load / save application settings
    void readSettings(const QString &sSharePath);
    void writeSettings(const QByteArray &WinGeometry,
                       const QByteArray &WinState,
                       const QByteArray &SplitterState = 0);

    void setLastOpenedDir(const QDir &LastDir);

    // General
    QString getGuiLanguage() const;
    bool getCodeCompletion() const;
    bool getSyntaxCheck() const;
    QString getInyokaUrl() const;
    QString getInyokaCommunity() const;
    QString getInyokaConstructionArea() const;
    QString getInyokaHash() const;
    bool getAutomaticImageDownload() const;
    bool getPreviewHorizontal() const;
    QDir getLastOpenedDir() const;
    bool getCheckLinks() const;
    quint32 getAutoSave() const;
    qint32 getReloadPreviewKey() const;
    quint32 getTimedPreview() const;
    bool getSyncScrollbars() const;
    bool getWindowsCheckUpdate() const;
    QString getPygmentize() const;

    // Font
    QFont getEditorFont() const;

    // Window state
    QByteArray getWindowState() const;
    QByteArray getWindowGeometry() const;
    QByteArray getSplitterState() const;

    // Recent files
    quint16 getNumOfRecentFiles() const;
    quint16 getMaxNumOfRecentFiles() const;
    QStringList getRecentFiles() const;
    void setRecentFiles(const QStringList &sListNewRecent);

    // Proxy
    QString getProxyHostName() const;
    quint16 getProxyPort() const;
    QString getProxyUserName() const;
    QString getProxyPassword() const;

    // Plugins
    QStringList getDisabledPlugins() const;

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
    bool m_bCodeCompletion;   // Enable / disable code completion
    bool m_bSyntaxCheck;
    bool m_bPreviewSplitHorizontal;
    QString m_sInyokaCommunity;
    QString m_sInyokaUrl;     // Url to inyoka
    QString m_sInyokaConstArea;
    QString m_sInyokaHash;
    QDir m_LastOpenedDir;
    bool m_bAutomaticImageDownload;  // Enable/disable download article images
    bool m_bCheckLinks;
    quint32 m_nAutosave;
    QString m_sReloadPreviewKey;
    quint32 m_nTimedPreview;
    bool m_bSyncScrollbars;
    bool m_bWinCheckUpdate;
    QString m_sPygmentize;

    // Font
    QFont m_EditorFont;
    QString m_sFontFamily;
    qreal m_nFontsize;

    // Window state
    QByteArray m_aWindowState;
    QByteArray m_aWindowGeometry;
    QByteArray m_aSplitterState;

    // Last opened
    static const quint16 m_cMAXFILES = 10;
    qint32 m_nMaxLastOpenedFiles;
    QStringList m_sListRecentFiles;

    // Proxy
    QString m_sProxyHostName;
    quint16 m_nProxyPort;
    QString m_sProxyUserName;
    QString m_sProxyPassword;

    // Plugins
    QStringList m_sListDisabledPlugins;
};

#endif  // APPLICATION_SETTINGS_H_
