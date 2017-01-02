/**
 * \file CSettingsDialog.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2017 The InyokaEdit developers
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
 * Settings gui.
 */

#include <QDebug>
#include <QKeyEvent>
#include <QPushButton>

#include "./CSettingsDialog.h"
#include "ui_CSettingsDialog.h"

CSettingsDialog::CSettingsDialog(CSettings *pSettings,
                                 const QString &sSharePath,
                                 QWidget *pParent)
    : QDialog(pParent),
      m_pSettings(pSettings),
      m_sSharePath(sSharePath) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi = new Ui::CSettingsDialog();
    m_pUi->setupUi(this);
    this->setWindowFlags(this->windowFlags()
                         & ~Qt::WindowContextHelpButtonHint);
    this->setModal(true);
    m_pUi->tabWidget->setCurrentIndex(0);  // Load tab "general" at first start

#if defined _WIN32
#else
    m_pUi->WindowsUpdateCheck->setEnabled(false);
#endif

    // Load Settings
    // General
    m_pUi->codeCompletionCheck->setChecked(m_pSettings->m_bCodeCompletion);
    m_pUi->splitHorizontalRadio->setChecked(m_pSettings->m_bPreviewSplitHorizontal);
    m_pUi->splitVerticalRadio->setChecked(!m_pSettings->m_bPreviewSplitHorizontal);
    m_pUi->inyokaUrlEdit->setText(m_pSettings->getInyokaUrl());
    m_pUi->articleImageDownloadCheck->setChecked(m_pSettings->m_bAutomaticImageDownload);
    m_pUi->linkCheckingCheck->setChecked(m_pSettings->m_bCheckLinks);
    m_pUi->autosaveEdit->setValue(m_pSettings->m_nAutosave);
    m_pUi->reloadPreviewKeyEdit->setText(
                "0x" + QString::number(m_pSettings->getReloadPreviewKey(), 16));
    m_pUi->timedPreviewsEdit->setValue(m_pSettings->m_nTimedPreview);
    m_pUi->scrollbarSyncCheck->setChecked(m_pSettings->m_bSyncScrollbars);
#if QT_VERSION >= 0x050600
    m_pUi->scrollbarSyncCheck->setEnabled(false);
#endif
    m_pUi->WindowsUpdateCheck->setChecked(m_pSettings->m_bWinCheckUpdate);

    QStringList sListGuiLanguages;
    sListGuiLanguages << "auto" << "en";
    QDir appDir(m_sSharePath + "/lang");
    QFileInfoList fiListFiles = appDir.entryInfoList(
                QDir::NoDotAndDotDot | QDir::Files);
    foreach (QFileInfo fi, fiListFiles) {
        if ("qm" == fi.suffix() && fi.baseName().startsWith(qAppName().toLower() + "_")) {
            sListGuiLanguages << fi.baseName().remove(qAppName().toLower() + "_");
        }
    }

    m_pUi->GuiLangCombo->addItems(sListGuiLanguages);
    if (-1 != m_pUi->GuiLangCombo->findText(m_pSettings->getGuiLanguage())) {
        m_pUi->GuiLangCombo->setCurrentIndex(
                    m_pUi->GuiLangCombo->findText(m_pSettings->getGuiLanguage()));
    } else {
        m_pUi->GuiLangCombo->setCurrentIndex(
                    m_pUi->GuiLangCombo->findText("auto"));
    }
    m_sGuiLang = m_pUi->GuiLangCombo->currentText();

    // Enter Qt keycode automatically in text box
    m_pUi->reloadPreviewKeyEdit->installEventFilter(this);

    // Font
    m_pUi->fontComboBox->setCurrentFont(QFont(m_pSettings->m_sFontFamily));
    m_pUi->fontSizeEdit->setValue(m_pSettings->m_nFontsize);

    // Recent files
    m_pUi->numberRecentFilesEdit->setValue((quint16)m_pSettings->m_nMaxLastOpenedFiles);
    m_pUi->numberRecentFilesEdit->setMaximum((quint16)m_pSettings->m_cMAXFILES);

    // Proxy
    m_pUi->proxyHostNameEdit->setText(m_pSettings->m_sProxyHostName);
    m_pUi->proxyPortSpinBox->setValue(m_pSettings->m_nProxyPort);
    m_pUi->proxyUserNameEdit->setText(m_pSettings->m_sProxyUserName);
    m_pUi->proxyPasswordEdit->setText(m_pSettings->m_sProxyPassword);

    connect(m_pUi->buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(m_pUi->buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
}

CSettingsDialog::~CSettingsDialog() {
    if (m_pUi) {
        delete m_pUi;
        m_pUi = NULL;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettingsDialog::accept() {
    QString tmpReloadPreviewKey(m_pUi->reloadPreviewKeyEdit->text());
    if (!tmpReloadPreviewKey.startsWith("0x")) {
        tmpReloadPreviewKey = "0x" + tmpReloadPreviewKey;
    }

    // General
    m_pSettings->m_bCodeCompletion = m_pUi->codeCompletionCheck->isChecked();
    m_pSettings->m_bPreviewSplitHorizontal = m_pUi->splitHorizontalRadio->isChecked();
    m_pSettings->m_sInyokaUrl = m_pUi->inyokaUrlEdit->text();
    m_pSettings->m_bAutomaticImageDownload = m_pUi->articleImageDownloadCheck->isChecked();
    m_pSettings->m_bCheckLinks = m_pUi->linkCheckingCheck->isChecked();
    m_pSettings->m_nAutosave = m_pUi->autosaveEdit->value();
    m_pSettings->m_sReloadPreviewKey = tmpReloadPreviewKey;
    m_pSettings->m_nTimedPreview = m_pUi->timedPreviewsEdit->value();
    m_pSettings->m_bSyncScrollbars = m_pUi->scrollbarSyncCheck->isChecked();
    m_pSettings->m_bWinCheckUpdate = m_pUi->WindowsUpdateCheck->isChecked();
    m_pSettings->m_sGuiLanguage = m_pUi->GuiLangCombo->currentText();

    // Font
    m_pSettings->m_sFontFamily = m_pUi->fontComboBox->currentFont().family();
    m_pSettings->m_nFontsize = m_pUi->fontSizeEdit->value();
    m_pSettings->m_EditorFont.setFamily(m_pSettings->m_sFontFamily);
    m_pSettings->m_EditorFont.setPointSizeF(m_pSettings->m_nFontsize);

    // Recent files
    m_pSettings->m_nMaxLastOpenedFiles = m_pUi->numberRecentFilesEdit->value();

    // Proxy
    m_pSettings->m_sProxyHostName = m_pUi->proxyHostNameEdit->text();
    m_pSettings->m_nProxyPort = m_pUi->proxyPortSpinBox->value();
    m_pSettings->m_sProxyUserName = m_pUi->proxyUserNameEdit->text();
    m_pSettings->m_sProxyPassword = m_pUi->proxyPasswordEdit->text();

    // Plugins
    QStringList oldDisabledPlugins;
    oldDisabledPlugins = m_pSettings->m_sListDisabledPlugins;
    m_pSettings->m_sListDisabledPlugins.clear();
    for (int i = 0; i < m_listPLugins.size(); i ++) {
        if (m_pUi->pluginsTable->item(i, 0)->checkState() != Qt::Checked) {
            m_pSettings->m_sListDisabledPlugins << m_listPLugins[i]->getPluginName();
        }
    }
    oldDisabledPlugins.sort();  // Sort for comparison
    m_pSettings->m_sListDisabledPlugins.sort();
    oldDisabledPlugins.removeAll("");
    m_pSettings->m_sListDisabledPlugins.removeAll("");

    // If the following settings have been changed, a restart is needed
    if (m_pUi->GuiLangCombo->currentText() != m_sGuiLang
            || oldDisabledPlugins != m_pSettings->m_sListDisabledPlugins) {
        QMessageBox::information(0, this->windowTitle(),
                                 trUtf8("The editor has to be restarted for "
                                        "applying the changes."));
    }

    m_pUi->tabWidget->setCurrentIndex(0);  // Reset first tab after start
    QDialog::accept();
    emit updatedSettings();
}

// ----------------------------------------------------------------------------

void CSettingsDialog::reject() {
    m_pUi->tabWidget->setCurrentIndex(0);  // Reset first tab after start
    QDialog::reject();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CSettingsDialog::eventFilter(QObject *obj, QEvent *event) {
    // Enter Qt keycode automatically in text box
    if (m_pUi->reloadPreviewKeyEdit == obj) {
        if (QEvent::KeyPress == event->type()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            // Remove key with DEL
            if (Qt::Key_Delete != keyEvent->key()) {
                m_pUi->reloadPreviewKeyEdit->setText(
                            "0x" + QString::number(keyEvent->key(), 16));
                return true;
            } else {
                m_pUi->reloadPreviewKeyEdit->setText("0x0");
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettingsDialog::getAvailablePlugins(const QList<IEditorPlugin *> PluginList,
                                          const QList<QObject *> PluginObjList) {
    m_listPLugins = PluginList;
    const quint8 nNUMCOLS = 5;

    m_pUi->pluginsTable->setColumnCount(nNUMCOLS);
    m_pUi->pluginsTable->setRowCount(m_listPLugins.size());

    m_pUi->pluginsTable->setColumnWidth(0, 40);
    m_pUi->pluginsTable->setColumnWidth(1, 40);
#if QT_VERSION >= 0x050000
    m_pUi->pluginsTable->horizontalHeader()->setSectionResizeMode(
                2, QHeaderView::Stretch);
#else
    m_pUi->pluginsTable->horizontalHeader()->setResizeMode(
                2, QHeaderView::Stretch);
#endif
    m_pUi->pluginsTable->setColumnWidth(3, 40);
    m_pUi->pluginsTable->setColumnWidth(4, 40);

    for (int nRow = 0; nRow < m_listPLugins.size(); nRow++) {
        for (int nCol = 0; nCol < nNUMCOLS; nCol++) {
            m_pUi->pluginsTable->setItem(nRow, nCol, new QTableWidgetItem());
        }

        // Checkbox
        m_pUi->pluginsTable->item(nRow, 0)->setFlags(
                    Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (m_pSettings->m_sListDisabledPlugins.contains(
                    m_listPLugins[nRow]->getPluginName())) {
            m_pUi->pluginsTable->item(nRow, 0)->setCheckState(Qt::Unchecked);
        } else {
            m_pUi->pluginsTable->item(nRow, 0)->setCheckState(Qt::Checked);
        }

        // Icon
        m_pUi->pluginsTable->setIconSize(QSize(22, 22));
        m_pUi->pluginsTable->item(nRow, 1)->setIcon(
                    m_listPLugins[nRow]->getIcon());
        // Caption
        m_pUi->pluginsTable->item(nRow, 2)->setText(
                    m_listPLugins[nRow]->getCaption());

        // Settings
        if (m_listPLugins[nRow]->hasSettings()) {
            m_listPluginInfoButtons << new QPushButton(
                                           QIcon::fromTheme("preferences-system",
                                                            QIcon(":/images/preferences-system.png")), "");
            connect(m_listPluginInfoButtons.last(), SIGNAL(pressed()),
                    PluginObjList[nRow], SLOT(showSettings()));

            m_pUi->pluginsTable->setCellWidget(nRow, 3,
                                               m_listPluginInfoButtons.last());

            if (m_pSettings->m_sListDisabledPlugins.contains(
                        m_listPLugins[nRow]->getPluginName())) {
                m_listPluginInfoButtons.last()->setEnabled(false);
            }
        }

        // Info
        m_listPluginInfoButtons << new QPushButton(
                                       QIcon::fromTheme("help-about",
                                                        QIcon(":/images/help-browser.png")), "");
        connect(m_listPluginInfoButtons.last(), SIGNAL(pressed()),
                PluginObjList[nRow], SLOT(showAbout()));
        m_pUi->pluginsTable->setCellWidget(nRow, 4,
                                           m_listPluginInfoButtons.last());
    }
}
