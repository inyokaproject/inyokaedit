/**
 * \file CEditorInterfaces.h
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
 * Editor interfaces definition for additional plugins.
 */

#ifndef INYOKAEDIT_IEDITORPLUGIN_H_
#define INYOKAEDIT_IEDITORPLUGIN_H_

#include <QAction>
#include <QDir>
#include <QTranslator>
#include <QtPlugin>

class CTextEditor;

class IEditorPlugin {
  public:
    virtual ~IEditorPlugin() {}

    // ALL FUNCTIONS PURE VIRTUAL !!!
    virtual void initPlugin(QWidget *pParent, CTextEditor *pEditor,
                            QDir userDataDir, bool bDebug) = 0;
    virtual QString getPluginName() const = 0;
    virtual QString getPluginVersion() const = 0;
    virtual QTranslator* getPluginTranslator(const QString &sLocale) = 0;
    virtual QString getCaption() const = 0;
    virtual QIcon getIcon() const = 0;
    virtual bool hasSettings() const = 0;

  public slots:
    virtual void executePlugin() = 0;
    virtual void showSettings() = 0;
    virtual void showAbout() = 0;
};

Q_DECLARE_INTERFACE(IEditorPlugin, "InyokaEdit.PluginInterface")

#endif  // INYOKAEDIT_IEDITORPLUGIN_H_
