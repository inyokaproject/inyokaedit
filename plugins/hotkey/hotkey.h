// SPDX-FileCopyrightText: 2015-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_HOTKEY_HOTKEY_H_
#define PLUGINS_HOTKEY_HOTKEY_H_

#include <QAction>  // Cannot use forward declaration (since Qt 6)
#include <QString>
#include <QTranslator>
#include <QtPlugin>

#include "../../application/ieditorplugin.h"

class QDialog;
class QDir;
class QKeySequenceEdit;
class QPushButton;
class QSettings;

class TextEditor;

namespace Ui {
class HotkeyClass;
}

class Hotkey : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.hotkey")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir &userDataDir, const QString &sSharePath,
                  const bool &bIsDarkTheme) override;
  auto getPluginName() const -> QString override;
  auto getPluginVersion() const -> QString override;
  auto getTranslator(const QString &sLang) -> QTranslator * override;
  auto getCaption() const -> QString override;
  auto getIcon() const -> QIcon override;
  auto includeMenu() const -> bool override;
  auto includeToolbar() const -> bool override;
  bool hasSettings() const override;
  void setCurrentEditor(TextEditor *pEditor) override;
  void setEditorlist(const QList<TextEditor *> &listEditors) override;

 public slots:
  void callPlugin() override;
  void executePlugin() override;
  void changeLanguage() override;
  void showSettings() override;
  void showAbout() override;

 private slots:
  void accept();
  void reject();
  void addRow();
  void deleteRow();
  void insertElement(const QString &sId);

 private:
  void loadHotkeyEntries();
  void buildUi(QWidget *pParent);
  void registerHotkeys();
  void writeSettings();
  void createRow(QKeySequenceEdit *sequenceEdit, const QString &sText);

  QWidget *m_pParent;
  Ui::HotkeyClass *m_pUi;
  QDialog *m_pDialog;
  QSettings *m_pSettings;
  QSettings *m_pSettingsApp;
  TextEditor *m_pEditor;
  QTranslator m_translator;
  QString m_sSharePath;
  bool m_bIsDarkTheme;
  QList<QKeySequenceEdit *> m_listSequenceEdit;
  QStringList m_sListEntries;
  QList<QAction *> m_listActions;
  QList<QAction *> m_listActionsOld;
  QList<QPushButton *> m_listDelRowButtons;
};

#endif  // PLUGINS_HOTKEY_HOTKEY_H_
