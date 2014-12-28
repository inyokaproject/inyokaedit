/**
 * \file CSettingsDialog.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2015 The InyokaEdit developers
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
 * Class definition of settings gui.
 */

#ifndef INYOKAEDIT_CSETTINGSDIALOG_H_
#define INYOKAEDIT_CSETTINGSDIALOG_H_

#include <QDialog>

#include "./CSettings.h"
#include "./IEditorPlugin.h"

namespace Ui {
    class CSettingsDialog;
}

class CSettings;

/**
 * \class CSettingsDialog
 * \brief Graphcal interface to settings
 */
class CSettingsDialog : public QDialog {
    Q_OBJECT

  public:
    CSettingsDialog(CSettings *pSettings, const QString &sSharePath,
                    QWidget *pParent = 0);
    virtual ~CSettingsDialog();

  public slots:
    void accept();
    void reject();

  signals:
    void updatedSettings();

  protected:
    bool eventFilter(QObject *obj, QEvent *event);

  private slots:
    void changedPreviewAlongside(bool bState);
    void changedPreviewInEditor(bool bState);
    void getAvailablePlugins(const QList<IEditorPlugin *> PluginList,
                             const QList<QObject *> PluginObjList);

  private:
    Ui::CSettingsDialog *m_pUi;
    CSettings *m_pSettings;
    QString m_sSharePath;

    bool m_bTmpPreviewInEditor;
    bool m_bTmpPreviewAlongside;
    QString m_sProxyHostName;
    quint16 m_nProxyPort;
    QString m_sProxyUserName;
    QString m_sProxyPassword;
    QString m_sGuiLang;

    QList<IEditorPlugin *> m_listPLugins;
    QList<QPushButton *> m_listPluginSettingsButtons;
    QList<QPushButton *> m_listPluginInfoButtons;
};

#endif  // INYOKAEDIT_CSETTINGSDIALOG_H_
