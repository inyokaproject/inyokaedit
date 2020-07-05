/**
 * \file plugins.cpp
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
 * Plugin management.
 */

#include "./plugins.h"

#include <QApplication>
#include <QAction>
#include <QDebug>
#include <QIcon>
#include <QPluginLoader>

#include "./texteditor.h"
#include "./ieditorplugin.h"

Plugins::Plugins(QWidget *pParent, TextEditor *pEditor,
                 const QStringList &sListDisabledPlugins,
                 const QDir &userDataDir, const QString &sSharePath)
  : m_pParent(pParent),
    m_pEditor(pEditor),
    m_sListDisabledPlugins(sListDisabledPlugins),
    m_userDataDir(userDataDir),
    m_sSharePath(sSharePath) {
  QStringList sListAvailablePlugins;
  QList<QDir> listPluginsDir;

  // If share folder start parameter is used
  QDir pluginsDir = sSharePath;
  if (qApp->arguments().contains(QStringLiteral("-s")) ||
      qApp->arguments().contains(QStringLiteral("--share"))) {
    if (pluginsDir.cd(QStringLiteral("plugins"))) {
      listPluginsDir << pluginsDir;
    }
  }
  // Plugins in user folder
  pluginsDir = m_userDataDir;
  if (pluginsDir.cd(QStringLiteral("plugins"))) {
    if (!listPluginsDir.contains(pluginsDir)) {
      listPluginsDir << pluginsDir;
    }
  }
  // Plugins in app folder (Windows and debugging)
  pluginsDir.setPath(qApp->applicationDirPath());
  if (pluginsDir.cd(QStringLiteral("plugins"))) {
    if (!listPluginsDir.contains(pluginsDir)) {
      listPluginsDir << pluginsDir;
    }
  }
  // Plugins in standard installation folder (Linux)
  pluginsDir.setPath(qApp->applicationDirPath() + "/../lib/"
                     + qApp->applicationName().toLower());
  if (pluginsDir.cd(QStringLiteral("plugins"))) {
    if (!listPluginsDir.contains(pluginsDir)) {
      listPluginsDir << pluginsDir;
    }
  }

  // Look for available plugins
  foreach (QDir dir, listPluginsDir) {
    qDebug() << "Plugins folder:" << dir.absolutePath();

    foreach (QString sFile, dir.entryList(QDir::Files)) {
      qDebug() << "Plugin file:" << sFile;
      QPluginLoader loader(dir.absoluteFilePath(sFile));
      QObject *pPlugin = loader.instance();
      if (pPlugin) {
        IEditorPlugin *piPlugin = qobject_cast<IEditorPlugin *>(pPlugin);

        if (piPlugin) {
          // Check for duplicates
          if (sListAvailablePlugins.contains(piPlugin->getPluginName())) {
            qDebug() << "             ... skipping duplicate file!";
            continue;
          }
          sListAvailablePlugins << piPlugin->getPluginName();
          m_listPlugins << piPlugin;
          m_listPluginObjects << pPlugin;
        } else {
          qWarning() << "           ... invalid IEditorPlugin file!";
        }
      } else {
        qWarning() << "           ... plugin cannot be loaded!";
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Plugins::loadPlugins(const QString &sLang) {
  m_PluginMenuEntries.clear();
  m_PluginToolbarEntries.clear();

  for (int i = 0; i < m_listPlugins.size(); i++) {
    qDebug() << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -";
    if (m_sListDisabledPlugins.contains(m_listPlugins.at(i)->getPluginName())) {
      qDebug() << "Disabled plugin:" << m_listPlugins.at(i)->getPluginName();
      continue;
    }

    m_listPlugins.at(i)->initPlugin(m_pParent, m_pEditor,
                                    m_userDataDir, m_sSharePath);
    m_listPlugins.at(i)->installTranslator(sLang);

    QString sMenu(m_listPlugins.at(i)->getCaption());
    if (!sMenu.isEmpty() && m_listPlugins.at(i)->includeMenu()) {
      m_PluginMenuEntries << new QAction(m_listPlugins.at(i)->getIcon(),
                                         m_listPlugins.at(i)->getCaption(),
                                         m_pParent);
      connect(m_PluginMenuEntries.last(), &QAction::triggered,
              m_listPluginObjects.at(i), [=]() {
        qobject_cast<IEditorPlugin *>(
              m_listPluginObjects.at(i))->callPlugin(); });
    }
    if (m_listPlugins.at(i)->includeToolbar()) {
      m_PluginToolbarEntries << new QAction(m_listPlugins.at(i)->getIcon(),
                                            m_listPlugins.at(i)->getCaption(),
                                            m_pParent);
      connect(m_PluginToolbarEntries.last(), &QAction::triggered,
              m_listPluginObjects.at(i), [=]() {
        qobject_cast<IEditorPlugin *>(
              m_listPluginObjects.at(i))->callPlugin(); });
    }

    m_listPlugins.at(i)->executePlugin();

    if (i == m_listPlugins.size() - 1) {
      qDebug() << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -";
    }
  }

  emit addMenuToolbarEntries(m_PluginToolbarEntries, m_PluginMenuEntries);
  emit availablePlugins(m_listPlugins, m_listPluginObjects);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Plugins::setCurrentEditor(TextEditor *pEditor) {
  for (auto &plugin : m_listPlugins) {
    if (!m_sListDisabledPlugins.contains(plugin->getPluginName())) {
      plugin->setCurrentEditor(pEditor);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Plugins::setEditorlist(const QList<TextEditor *> &listEditors) {
  for (auto &plugin : m_listPlugins) {
    if (!m_sListDisabledPlugins.contains(plugin->getPluginName())) {
      plugin->setEditorlist(listEditors);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Plugins::changeLang(const QString &sLang) {
  for (auto &plugin : m_listPlugins) {
    if (!m_sListDisabledPlugins.contains(plugin->getPluginName())) {
      plugin->installTranslator(sLang);
    }
  }
}
