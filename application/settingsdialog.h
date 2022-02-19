/**
 * \file settingsdialog.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
 * Class definition of settings gui.
 */

#ifndef APPLICATION_SETTINGSDIALOG_H_
#define APPLICATION_SETTINGSDIALOG_H_

#include <QDialog>

#include "./ieditorplugin.h"  // Cannot use forward declaration (since Qt 6)

class Settings;

namespace Ui {
class SettingsDialog;
}

/**
 * \class SettingsDialog
 * \brief Graphcal interface to settings
 */
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

    QList<IEditorPlugin *> m_listPLugins;
    QList<QPushButton *> m_listPluginSettingsButtons;
    QList<QPushButton *> m_listPluginInfoButtons;
};

#endif  // APPLICATION_SETTINGSDIALOG_H_
