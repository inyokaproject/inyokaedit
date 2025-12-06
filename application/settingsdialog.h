// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLICATION_SETTINGSDIALOG_H_
#define APPLICATION_SETTINGSDIALOG_H_

#include <QDialog>

#include "./ieditorplugin.h"  // Cannot use forward declaration (since Qt 6)

class Settings;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  SettingsDialog(Settings *pSettings, const QString &sSharePath,
                 QWidget *pParent = nullptr);
  virtual ~SettingsDialog();

 public slots:
  void accept() override;
  void reject() override;
  void updateUiLang();
  void getAvailablePlugins(const QList<IEditorPlugin *> &Plugins,
                           const QList<QObject *> &PluginObjList);

 signals:
  void changeLang(const QString &sLang);
  void updatedSettings();

 protected:
  auto eventFilter(QObject *obj, QEvent *event) -> bool override;

 private slots:
  void changedCommunity(int nIndex);

 private:
  auto searchTranslations() -> QStringList;

  Ui::SettingsDialog *m_pUi;
  Settings *m_pSettings;
  const QString m_sSharePath;
  QString m_sGuiLang;
  QString m_sCommunity;

  QList<IEditorPlugin *> m_listPlugins;
  QList<QPushButton *> m_listPluginSettingsButtons;
  QList<QPushButton *> m_listPluginInfoButtons;
};

#endif  // APPLICATION_SETTINGSDIALOG_H_
