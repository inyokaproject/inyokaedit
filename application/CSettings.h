/**
 * \file CSettings.h
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
 * Class definition for reading and writing settings.
 */

#ifndef INYOKAEDIT_CSETTINGS_H_
#define INYOKAEDIT_CSETTINGS_H_

#include <QDir>
#include <QSettings>
#include <QMessageBox>

#include "./CSettingsDialog.h"
#include "./IEditorPlugin.h"

class CSettingsDialog;

/**
 * \class CSettings
 * \brief Load and save settings.
 */
class CSettings : public QObject {
    Q_OBJECT

  public:
    CSettings(QWidget *pParent, const QString &sSharePath);
    ~CSettings();

    // Load / save application settings
    void readSettings();
    void writeSettings(const QByteArray WinGeometry, const QByteArray WinState,
                       const QByteArray SplitterState = 0);

    void setLastOpenedDir(const QDir &LastDir);

    // General
    QString getGuiLanguage() const;
    bool getCodeCompletion() const;
    QString getInyokaUrl() const;
    bool getAutomaticImageDownload() const;
    bool getPreviewInEditor() const;
    bool getPreviewAlongside() const;
    QDir getLastOpenedDir() const;
    bool getCheckLinks() const;
    QString getTemplateLanguage() const;
    quint32 getAutoSave() const;
    qint32 getReloadPreviewKey() const;
    quint32 getTimedPreview() const;
    bool getSyncScrollbars() const;
    bool getWindowsCheckUpdate() const;

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

    // Allow CSettingsDialog to access private members
    friend class CSettingsDialog;

  public slots:
    void setWindowsCheckUpdate(const bool bValue);

  signals:
    void showSettingsDialog();
    void updateEditorSettings();
    void availablePlugins(const QList<IEditorPlugin *> PluginList,
                          const QList<QObject *> PluginObjList);

  private:
    void removeObsolete();

    QSettings *m_pSettings;
    CSettingsDialog *m_pSettingsDialog;

    // General
    QString m_sGuiLanguage;
    bool m_bCodeCompletion;   // Enable / disable code completion
    bool m_bPreviewInEditor;  // Enable / disable preview in edior
    bool m_bPreviewAlongside;
    bool m_bTmpPreviewInEditor;   // Temp. variables needed, changes may only be
    bool m_bTmpPreviewAlongside;  // saved when app will be closed!
    QString m_sInyokaUrl;     // Url to inyoka
    QDir m_LastOpenedDir;
    bool m_bAutomaticImageDownload;  // Enable/disable download article images
    bool m_bCheckLinks;
    QString m_sTemplateLang;
    quint32 m_nAutosave;
    QString m_sReloadPreviewKey;
    quint32 m_nTimedPreview;
    bool m_bSyncScrollbars;
    bool m_bWinCheckUpdate;

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

#endif  // INYOKAEDIT_CSETTINGS_H_
