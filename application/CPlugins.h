/**
 * \file CPlugins.h
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
 * Class definition for plugin management.
 */

#ifndef INYOKAEDIT_CPLUGINS_H_
#define INYOKAEDIT_CPLUGINS_H_

#include <QAction>
#include <QDir>
#include <QList>

#include "./CTextEditor.h"
#include "./IEditorPlugin.h"

class CPlugins : public QObject {
    Q_OBJECT

  public:
    CPlugins(QWidget *pParent, CTextEditor *pEditor, const QString &sGuiLang,
             const QStringList &sListDisabledPlugins, const QDir userDataDir,
             const QString &sSharePath);
    void loadPlugins();

  signals:
    void availablePlugins(const QList<IEditorPlugin *> PluginList,
                          const QList<QObject *> PluginObjList);
    void addMenuToolbarEntries(const QList<QAction *> ToolbarEntries,
                               const QList<QAction *> MenueEntries);

  private:
    QWidget *m_pParent;
    CTextEditor *m_pEditor;
    QString m_sGuiLanguage;
    QStringList m_sListDisabledPlugins;
    QDir m_userDataDir;
    QString m_sSharePath;

    QList<IEditorPlugin *> m_listPlugins;
    QList<QObject *> m_listPluginObjects;
    QList<QAction *> m_PluginMenuEntries;
    QList<QAction *> m_PluginToolbarEntries;
};

#endif  // INYOKAEDIT_CPLUGINS_H_
