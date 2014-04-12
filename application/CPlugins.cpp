/**
 * \file Plugins.cpp
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
 * Plugin management.
 */

#include <QApplication>
#include <QDebug>
#include <QPluginLoader>

#include "./CPlugins.h"

CPlugins::CPlugins(QWidget *pParent, CTextEditor *pEditor, const QString &sGuiLang,
                   const QStringList &sListDisabledPlugins, const QDir userDataDir)
    : m_pParent(pParent),
      m_pEditor(pEditor),
      m_sGuiLanguage(sGuiLang),
      m_sListDisabledPlugins(sListDisabledPlugins),
      m_userDataDir(userDataDir) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    QStringList sListAvailablePlugins;
    QList<QDir> listPluginsDir;
    QDir pluginsDir = m_userDataDir;
    if (pluginsDir.cd("plugins")) {
        listPluginsDir << pluginsDir;
    }
    pluginsDir = qApp->applicationDirPath();
    pluginsDir.cd("plugins");
    listPluginsDir << pluginsDir;

    // Look for available plugins
    foreach (QDir dir, listPluginsDir) {
        qDebug() << "Plugins folder:" << dir.absolutePath();

        foreach (QString sFile, dir.entryList(QDir::Files)) {
            QPluginLoader loader(dir.absoluteFilePath(sFile));
            QObject *pPlugin = loader.instance();
            if (pPlugin) {
                IEditorPlugin *piPlugin = qobject_cast<IEditorPlugin *>(pPlugin);

                if (piPlugin) {
                    if (sListAvailablePlugins.contains(piPlugin->getPluginName())) {
                        continue;
                    }
                    sListAvailablePlugins << piPlugin->getPluginName();
                    m_listPlugins << piPlugin;
                    m_listPluginObjects << pPlugin;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CPlugins::loadPlugins() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_PluginMenuEntries.clear();
    m_PluginToolbarEntries.clear();

    for (int i = 0; i < m_listPlugins.size(); i++) {
        if (m_sListDisabledPlugins.contains(m_listPlugins[i]->getPluginName())) {
            continue;
        }

        qDebug() << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -";
        m_listPlugins[i]->initPlugin(m_pParent, m_pEditor, m_userDataDir);

        qApp->installTranslator(m_listPlugins[i]->getPluginTranslator(m_sGuiLanguage));

        QIcon icon(m_listPlugins[i]->getIcon());
        QString sMenu(m_listPlugins[i]->getCaption());

        if (!sMenu.isEmpty()) {  // Add to menue if entry available
            m_PluginMenuEntries << new QAction(m_listPlugins[i]->getIcon(),
                                               m_listPlugins[i]->getCaption(),
                                               m_pParent);
            connect(m_PluginMenuEntries.last(), SIGNAL(triggered()),
                    m_listPluginObjects[i], SLOT(executePlugin()));
        }
        if (!icon.isNull()) {  // Add to toolbar if icon available
            m_PluginToolbarEntries << new QAction(m_listPlugins[i]->getIcon(),
                                                  m_listPlugins[i]->getCaption(),
                                                  m_pParent);
            connect(m_PluginToolbarEntries.last(), SIGNAL(triggered()),
                    m_listPluginObjects[i], SLOT(executePlugin()));
        }
        if (i == m_listPlugins.size() - 1) {
            qDebug() << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -";
        }
    }

    emit addMenuToolbarEntries(m_PluginToolbarEntries, m_PluginMenuEntries);
    emit availablePlugins(m_listPlugins, m_listPluginObjects);
}
