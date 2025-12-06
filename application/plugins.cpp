// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./plugins.h"

#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QPluginLoader>

#include "./texteditor.h"

Plugins::Plugins(QWidget *pParent, TextEditor *pEditor,
                 const QStringList &sListDisabledPlugins,
                 const QDir &userDataDir, const QString &sSharePath,
                 const bool bDarkScheme, QObject *pObj)
    : m_pParent(pParent),
      m_pEditor(pEditor),
      m_sListDisabledPlugins(sListDisabledPlugins),
      m_userDataDir(userDataDir),
      m_sSharePath(sSharePath),
      m_bDarkScheme(bDarkScheme) {
  Q_UNUSED(pObj)
  QStringList sListAvailablePlugins;
  QList<QDir> listPluginsDir;
  QStringList sListSearchPaths;

  // If share folder start parameter is used
  QDir pluginsDir = sSharePath;
  if (qApp->arguments().contains(QStringLiteral("-s")) ||
      qApp->arguments().contains(QStringLiteral("--share"))) {
    if (pluginsDir.cd(QStringLiteral("plugins"))) {
      listPluginsDir << pluginsDir;
    }
  }

  // Plugins in user folder
  sListSearchPaths << m_userDataDir.absolutePath();
  // Plugins in app folder (Windows and debugging)
  sListSearchPaths << qApp->applicationDirPath();
  // Plugins in standard installation folder (Linux)
  sListSearchPaths << qApp->applicationDirPath() + "/../lib/" +
                          qApp->applicationName().toLower();
  // Plugins in standard installation folder (Linux / Debian / Ubuntu)
  sListSearchPaths << qApp->applicationDirPath() + "/../lib/x86_64-linux-gnu/" +
                          qApp->applicationName().toLower();
  for (const auto &searchDir : std::as_const(sListSearchPaths)) {
    pluginsDir.setPath(searchDir);
    if (pluginsDir.cd(QStringLiteral("plugins"))) {
      if (!listPluginsDir.contains(pluginsDir)) {
        listPluginsDir << pluginsDir;
      }
    }
  }

  // Look for available plugins
  QStringList filters;
  filters << "*.dll"
          << "*.so"
          << "*.dylib";
  for (const auto &dir : std::as_const(listPluginsDir)) {
    qDebug() << "Plugins folder:" << dir.absolutePath();
    const QStringList entryList(dir.entryList(filters, QDir::Files));
    for (const auto &sFile : entryList) {
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

    m_listPlugins.at(i)->initPlugin(m_pParent, m_pEditor, m_userDataDir,
                                    m_sSharePath, m_bDarkScheme);
    m_listPlugins.at(i)->installTranslator(sLang);

    QString sMenu(m_listPlugins.at(i)->getCaption());
    QIcon ico(m_listPlugins.at(i)->getIcon());

    if (!sMenu.isEmpty() && m_listPlugins.at(i)->includeMenu()) {
      m_PluginMenuEntries << new QAction(ico, m_listPlugins.at(i)->getCaption(),
                                         m_pParent);
      connect(m_PluginMenuEntries.last(), &QAction::triggered,
              m_listPluginObjects.at(i), [=]() {
                qobject_cast<IEditorPlugin *>(m_listPluginObjects.at(i))
                    ->callPlugin();
              });
    }
    if (m_listPlugins.at(i)->includeToolbar()) {
      m_PluginToolbarEntries
          << new QAction(ico, m_listPlugins.at(i)->getCaption(), m_pParent);
      connect(m_PluginToolbarEntries.last(), &QAction::triggered,
              m_listPluginObjects.at(i), [=]() {
                qobject_cast<IEditorPlugin *>(m_listPluginObjects.at(i))
                    ->callPlugin();
              });
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
