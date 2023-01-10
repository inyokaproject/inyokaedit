/**
 * \file plugins.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * Class definition for plugin management.
 */

#ifndef APPLICATION_PLUGINS_H_
#define APPLICATION_PLUGINS_H_

#include <QAction>  // Cannot use forward declaration (since Qt 6)
#include <QDir>
#include <QList>

#include "./ieditorplugin.h"  // Cannot use forward declaration (since Qt 6)

class TextEditor;

class Plugins : public QObject {
  Q_OBJECT

 public:
  Plugins(QWidget *pParent, TextEditor *pEditor,
          const QStringList &sListDisabledPlugins, const QDir &userDataDir,
          const QString &sSharePath, QObject *pObj = nullptr);
  void loadPlugins(const QString &sLang);
  void setCurrentEditor(TextEditor *pEditor);
  void setEditorlist(const QList<TextEditor *> &listEditors);

 public slots:
  void changeLang(const QString &sLang);

 signals:
  void availablePlugins(const QList<IEditorPlugin *> &PluginList,
                        const QList<QObject *> &PluginObjList);
  void addMenuToolbarEntries(const QList<QAction *> &ToolbarEntries,
                             QList<QAction *> MenueEntries);

 private:
  QWidget *m_pParent;
  TextEditor *m_pEditor;
  QStringList m_sListDisabledPlugins;
  const QDir m_userDataDir;
  const QString m_sSharePath;

  QList<IEditorPlugin *> m_listPlugins;
  QList<QObject *> m_listPluginObjects;
  QList<QAction *> m_PluginMenuEntries;
  QList<QAction *> m_PluginToolbarEntries;
};

#endif  // APPLICATION_PLUGINS_H_
