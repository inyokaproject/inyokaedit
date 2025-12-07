// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

// clazy:excludeall=copyable-polymorphic

#ifndef APPLICATION_IEDITORPLUGIN_H_
#define APPLICATION_IEDITORPLUGIN_H_

#include <QTranslator>
#include <QtPlugin>

class QDir;

class TextEditor;

class IEditorPlugin {
 public:
  virtual ~IEditorPlugin() {}

  // ALL FUNCTIONS PURE VIRTUAL !!!
  virtual void initPlugin(QWidget *pParent, TextEditor *pEditor,
                          const QDir &userDataDir, const QString &sSharePath,
                          const bool &bIsDarkTheme) = 0;
  virtual QString getPluginName() const = 0;
  virtual QString getPluginVersion() const = 0;
  virtual QTranslator *getTranslator(const QString &sLang) = 0;
  virtual QString getCaption() const = 0;
  virtual QIcon getIcon() const = 0;
  virtual bool includeMenu() const = 0;
  virtual bool includeToolbar() const = 0;
  virtual bool hasSettings() const = 0;
  virtual void setCurrentEditor(TextEditor *pEditor) = 0;
  virtual void setEditorlist(const QList<TextEditor *> &listEditors) = 0;

 public slots:
  virtual void callPlugin() = 0;     // Execute manually
  virtual void executePlugin() = 0;  // Execute DIRECTLY after loading it!
  virtual void changeLanguage() = 0;
  virtual void showSettings() = 0;
  virtual void showAbout() = 0;
};

Q_DECLARE_INTERFACE(IEditorPlugin, "InyokaEdit.PluginInterface")

#endif  // APPLICATION_IEDITORPLUGIN_H_
