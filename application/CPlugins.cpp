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
#include "./IEditorPlugin.h"

CPlugins::CPlugins(QWidget *pParent, CTextEditor *pEditor,
                   const QString &sGuiLang, const QDir userDataDir)
    : m_pParent(pParent),
      m_pEditor(pEditor),
      m_sGuiLanguage(sGuiLang),
      m_userDataDir(userDataDir) {
    qDebug() << "Calling" << Q_FUNC_INFO;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CPlugins::loadPlugins() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_sListPlugins.clear();
    m_PluginMenuEntries.clear();
    m_PluginToolbarEntries.clear();

    QList<QDir> listPluginsDir;
    QDir pluginsDir = m_userDataDir;
    if (pluginsDir.cd("plugins")) {
        listPluginsDir << pluginsDir;
    }
    pluginsDir = qApp->applicationDirPath();
    pluginsDir.cd("plugins");
    listPluginsDir << pluginsDir;

    foreach (QDir dir, listPluginsDir) {
        qDebug() << "Plugins folder:" << dir.absolutePath();

        foreach (QString sFile, dir.entryList(QDir::Files)) {
            QPluginLoader loader(dir.absoluteFilePath(sFile));
            QObject *pPlugin = loader.instance();
            if (pPlugin) {
                IEditorPlugin *piPlugin = qobject_cast<IEditorPlugin *>(pPlugin);

                if (piPlugin) {
                    if (m_sListPlugins.contains(piPlugin->getPluginName())) {
                        continue;
                    }
                    qDebug() << "Loaded plugin:" << sFile;
                    piPlugin->initPlugin(m_pParent, m_pEditor, m_userDataDir);
                    qApp->installTranslator(piPlugin->getPluginTranslator(
                                                m_sGuiLanguage));
                    m_sListPlugins << piPlugin->getPluginName();

                    QIcon icon(piPlugin->getMenuIcon());
                    QString sMenu(piPlugin->getMenuEntry());

                    if (!sMenu.isEmpty()) {  // Add to menue if entry available
                        m_PluginMenuEntries << new QAction(piPlugin->getMenuIcon(),
                                                           piPlugin->getMenuEntry(),
                                                           m_pParent);
                        connect(m_PluginMenuEntries.last(), SIGNAL(triggered()),
                                pPlugin, SLOT(executePlugin()));
                    }
                    if (!icon.isNull()) {  // Add to toolbar if icon available
                        m_PluginToolbarEntries << new QAction(piPlugin->getMenuIcon(),
                                                              piPlugin->getMenuEntry(),
                                                              m_pParent);
                        connect(m_PluginToolbarEntries.last(), SIGNAL(triggered()),
                                pPlugin, SLOT(executePlugin()));
                    }
                }
            }
        }
    }

    emit addMenuToolbarEntries(m_PluginToolbarEntries, m_PluginMenuEntries);
}
