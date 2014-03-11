/**
 * \file CSettingsDialog.h
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
 * Class definition of settings gui.
 */

#ifndef INYOKAEDIT_CSETTINGSDIALOG_H_
#define INYOKAEDIT_CSETTINGSDIALOG_H_

#include <QDialog>
#include "./CHighlighter.h"
#include "./CSettings.h"

namespace Ui {
    class CSettingsDialog;
}

class CSettings;
class CHighlighter;

/**
 * \class CSettingsDialog
 * \brief Graphcal interface to settings
 */
class CSettingsDialog : public QDialog {
    Q_OBJECT

  public:
    CSettingsDialog(CSettings *pSettings, CHighlighter *pHighlighter,
                    QWidget *pParent = 0);
    virtual ~CSettingsDialog();

  public slots:
    void accept();

  signals:
    void updatedSettings();

  protected:
    bool eventFilter(QObject *obj, QEvent *event);

  private slots:
    void changedPreviewAlongside(bool bState);
    void changedPreviewInEditor(bool bState);
    void changedStyle(int nIndex);
    void clickedStyleCell(int nRow, int nCol);

  private:
    void loadHighlighting(const QString &sStyleFile);
    void readValue(const quint16 nRow,
                   const QTextCharFormat &charFormat);
    void saveHighlighting();
    QString createValues(const quint16 nRow);
    Ui::CSettingsDialog *m_pUi;
    CSettings *m_pSettings;
    CHighlighter *m_pHighlighter;

    bool m_bTmpPreviewInEditor;
    bool m_bTmpPreviewAlongside;
    QString m_sProxyHostName;
    quint16 m_nProxyPort;
    QString m_sProxyUserName;
    QString m_sProxyPassword;
    QString m_sGuiLang;
    QString m_sStyleFile;
    QString m_sExt;

    QStringList m_sListStyleFiles;
};

#endif  // INYOKAEDIT_CSETTINGSDIALOG_H_
