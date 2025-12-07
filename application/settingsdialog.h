// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_SETTINGSDIALOG_H_
#define APPLICATION_SETTINGSDIALOG_H_

#include <QDialog>

#include "./ieditorplugin.h"  // Cannot use forward declaration (since Qt 6)
#include "./settings.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  SettingsDialog(QWidget *pParent = nullptr);
  virtual ~SettingsDialog();

 public slots:
  void accept() override;
  void reject() override;
  void getAvailablePlugins(const QList<IEditorPlugin *> &Plugins,
                           const QList<QObject *> &PluginObjList);

 signals:
  void changeGuiLanguage(const QString &sLanguage);
  void updateEditorSettings();

 protected:
  auto eventFilter(QObject *obj, QEvent *event) -> bool override;
  void changeEvent(QEvent *pEvent) override;

 private:
  auto searchTranslations() -> QStringList;
  void readSettings();

  Ui::SettingsDialog *m_pUi;
  Settings *m_pSettings;
  const QString m_sSharePath;

  QList<IEditorPlugin *> m_listPlugins;
  QList<QPushButton *> m_listPluginSettingsButtons;
  QList<QPushButton *> m_listPluginInfoButtons;
};

#endif  // APPLICATION_SETTINGSDIALOG_H_
