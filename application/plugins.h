// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

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
          const QString &sSharePath, const bool bDarkScheme,
          QObject *pObj = nullptr);
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
  const bool m_bDarkScheme;

  QList<IEditorPlugin *> m_listPlugins;
  QList<QObject *> m_listPluginObjects;
  QList<QAction *> m_PluginMenuEntries;
  QList<QAction *> m_PluginToolbarEntries;
};

#endif  // APPLICATION_PLUGINS_H_
