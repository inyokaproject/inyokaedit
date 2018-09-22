/**
 * \file settingsdialog.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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

#include "./settingsdialog.h"

#include <QDebug>
#include <QDirIterator>
#include <QKeyEvent>
#include <QPushButton>

#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(Settings *pSettings,
                               const QString &sSharePath,
                               QWidget *pParent)
  : QDialog(pParent),
    m_pSettings(pSettings),
    m_sSharePath(sSharePath) {
  m_pUi = new Ui::SettingsDialog();
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
  m_pUi->syntaxCheck->setChecked(m_pSettings->m_bSyntaxCheck);
  m_pUi->splitHorizontalRadio->setChecked(
        m_pSettings->m_bPreviewSplitHorizontal);
  m_pUi->splitVerticalRadio->setChecked(
        !m_pSettings->m_bPreviewSplitHorizontal);
  m_pUi->inyokaUrlEdit->setText(m_pSettings->getInyokaUrl());
  m_pUi->articleImageDownloadCheck->setChecked(
        m_pSettings->m_bAutomaticImageDownload);
  m_pUi->linkCheckingCheck->setChecked(m_pSettings->m_bCheckLinks);
  m_pUi->autosaveEdit->setValue(m_pSettings->m_nAutosave);
  m_pUi->reloadPreviewKeyEdit->setText(
        "0x" + QString::number(m_pSettings->getReloadPreviewKey(), 16));
  m_pUi->timedPreviewsEdit->setValue(m_pSettings->m_nTimedPreview);
  m_pUi->scrollbarSyncCheck->setChecked(m_pSettings->m_bSyncScrollbars);
#ifndef USEQTWEBKIT
  m_pUi->scrollbarSyncCheck->setEnabled(false);
#endif
  m_pUi->WindowsUpdateCheck->setChecked(m_pSettings->m_bWinCheckUpdate);

  m_pUi->GuiLangCombo->addItems(this->searchTranslations());
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
  m_pUi->numberRecentFilesEdit->setValue(
        (quint16)m_pSettings->m_nMaxLastOpenedFiles);
  m_pUi->numberRecentFilesEdit->setMaximum(
        (quint16)m_pSettings->m_cMAXFILES);

  QStringList sListCommunities;
  QDir extendedShareDir(m_sSharePath + "/community");
  QFileInfoList fiListFiles = extendedShareDir.entryInfoList(
                                QDir::NoDotAndDotDot | QDir::Dirs);
  foreach (QFileInfo fi, fiListFiles) {
    sListCommunities << fi.fileName();
  }

  m_pUi->CommunityCombo->blockSignals(true);  // No change index signal emited
  m_pUi->CommunityCombo->addItems(sListCommunities);
  if (-1 != m_pUi->CommunityCombo->findText(
        m_pSettings->getInyokaCommunity())) {
    m_pUi->CommunityCombo->setCurrentIndex(
          m_pUi->CommunityCombo->findText(m_pSettings->getInyokaCommunity()));
  } else if (sListCommunities.size() > 0) {
    m_pUi->CommunityCombo->setCurrentIndex(0);
  }
  m_sCommunity = m_pUi->CommunityCombo->currentText();
  m_pSettings->m_sInyokaCommunity = m_pUi->CommunityCombo->currentText();
  m_pUi->CommunityCombo->blockSignals(false);

  // Proxy
  m_pUi->proxyHostNameEdit->setText(m_pSettings->m_sProxyHostName);
  m_pUi->proxyPortSpinBox->setValue(m_pSettings->m_nProxyPort);
  m_pUi->proxyUserNameEdit->setText(m_pSettings->m_sProxyUserName);
  m_pUi->proxyPasswordEdit->setText(m_pSettings->m_sProxyPassword);

  connect(m_pUi->CommunityCombo,
          static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
          this, &SettingsDialog::changedCommunity);

  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted,
          this, &SettingsDialog::accept);
  connect(m_pUi->buttonBox, &QDialogButtonBox::rejected,
          this, &SettingsDialog::reject);
}

SettingsDialog::~SettingsDialog() {
  if (m_pUi) {
    delete m_pUi;
    m_pUi = NULL;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SettingsDialog::accept() {
  QString tmpReloadPreviewKey(m_pUi->reloadPreviewKeyEdit->text());
  if (!tmpReloadPreviewKey.startsWith("0x")) {
    tmpReloadPreviewKey = "0x" + tmpReloadPreviewKey;
  }

  // General
  m_pSettings->m_bCodeCompletion = m_pUi->codeCompletionCheck->isChecked();
  m_pSettings->m_bSyntaxCheck = m_pUi->syntaxCheck->isChecked();
  m_pSettings->m_bPreviewSplitHorizontal =
      m_pUi->splitHorizontalRadio->isChecked();
  m_pSettings->m_sInyokaCommunity = m_pUi->CommunityCombo->currentText();
  m_pSettings->m_sInyokaUrl = m_pUi->inyokaUrlEdit->text();
  m_pSettings->m_bAutomaticImageDownload =
      m_pUi->articleImageDownloadCheck->isChecked();
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

  if (m_pUi->GuiLangCombo->currentText() != m_sGuiLang) {
    emit changeLang(m_pSettings->getGuiLanguage());
  }

  // If the following settings have been changed, a restart is needed
  if (m_pUi->CommunityCombo->currentText() != m_sCommunity ||
      oldDisabledPlugins != m_pSettings->m_sListDisabledPlugins) {
    QFile communityFile(
          m_sSharePath + "/community/" +
          m_pUi->CommunityCombo->currentText() + "/community.conf");
    QSettings communityConfig(communityFile.fileName(), QSettings::IniFormat);
    communityConfig.setIniCodec("UTF-8");
    QString sValue(communityConfig.value("ConstructionArea", "").toString());
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka construction area not found!";
    } else {
      m_pSettings->m_sInyokaConstArea = sValue;
    }
    sValue = communityConfig.value("Hash", "").toString();
    if (sValue.isEmpty()) {
      qWarning() << "Inyoka hash is empty!";
    } else {
      m_pSettings->m_sInyokaHash = sValue;
    }

    QMessageBox::information(0, this->windowTitle(),
                             tr("The editor has to be restarted for "
                                "applying the changes."));
  }

  m_pUi->tabWidget->setCurrentIndex(0);  // Reset first tab after start
  QDialog::accept();
  emit updatedSettings();
}

// ----------------------------------------------------------------------------

void SettingsDialog::reject() {
  m_pUi->tabWidget->setCurrentIndex(0);  // Reset first tab after start
  QDialog::reject();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SettingsDialog::eventFilter(QObject *obj, QEvent *event) {
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

void SettingsDialog::changedCommunity(const QString &sCommunity) {
  QFile communityFile(m_sSharePath + "/community/" +
                      sCommunity + "/community.conf");

  if (!communityFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(0, tr("Error"),
                          tr("Could not open/find community file!"));
    qCritical() << "Could not open/find community file:"
                << communityFile.fileName();
  }

  QSettings communityConfig(communityFile.fileName(), QSettings::IniFormat);
  communityConfig.setIniCodec("UTF-8");

  QString sUrl(communityConfig.value("InyokaUrl", "").toString());
  if (sUrl.isEmpty()) {
    qWarning() << "Community Url not found!";
    QMessageBox::warning(0, tr("Warning"), tr("No community url defined!"));
  } else {
    m_pUi->inyokaUrlEdit->setText(sUrl);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SettingsDialog::updateUiLang() {
  m_pUi->retranslateUi(this);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SettingsDialog::getAvailablePlugins(const QList<IEditorPlugin *> &Plugins,
                                         const QList<QObject *> &PluginObjList) {
  m_listPLugins = Plugins;
  const quint8 nNUMCOLS = 5;

  m_pUi->pluginsTable->setColumnCount(nNUMCOLS);
  m_pUi->pluginsTable->setRowCount(m_listPLugins.size());

  m_pUi->pluginsTable->setColumnWidth(0, 40);
  m_pUi->pluginsTable->setColumnWidth(1, 40);
  m_pUi->pluginsTable->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Stretch);
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
          m_listPLugins.at(nRow)->getPluginName())) {
      m_pUi->pluginsTable->item(nRow, 0)->setCheckState(Qt::Unchecked);
    } else {
      m_pUi->pluginsTable->item(nRow, 0)->setCheckState(Qt::Checked);
    }

    // Icon
    m_pUi->pluginsTable->setIconSize(QSize(22, 22));
    m_pUi->pluginsTable->item(nRow, 1)->setIcon(
          m_listPLugins.at(nRow)->getIcon());
    // Caption
    m_pUi->pluginsTable->item(nRow, 2)->setText(
          m_listPLugins.at(nRow)->getCaption());

    // Settings
    if (m_listPLugins.at(nRow)->hasSettings()) {
      m_listPluginInfoButtons << new QPushButton(
                                   QIcon::fromTheme(
                                     "preferences-system",
                                     QIcon(":/images/preferences-system.png")),
                                   "");
      connect(m_listPluginInfoButtons.last(), &QPushButton::pressed,
              PluginObjList.at(nRow), [=]() {
        qobject_cast<IEditorPlugin *>(PluginObjList.at(nRow))->showSettings(); });

      m_pUi->pluginsTable->setCellWidget(nRow, 3,
                                         m_listPluginInfoButtons.last());

      if (m_pSettings->m_sListDisabledPlugins.contains(
            m_listPLugins[nRow]->getPluginName())) {
        m_listPluginInfoButtons.last()->setEnabled(false);
      }
    }

    // Info
    m_listPluginInfoButtons << new QPushButton(
                                 QIcon::fromTheme(
                                   "help-about",
                                   QIcon(":/images/help-browser.png")), "");
    connect(m_listPluginInfoButtons.last(), &QPushButton::pressed,
            PluginObjList[nRow], [=]() {
      qobject_cast<IEditorPlugin *>(PluginObjList[nRow])->showAbout(); });
    m_pUi->pluginsTable->setCellWidget(nRow, 4,
                                       m_listPluginInfoButtons.last());
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QStringList SettingsDialog::searchTranslations() {
  QStringList sList;
  QString sTmp;

  // Translations build in resources
  QDirIterator it(":", QStringList() << "*.qm",
                  QDir::NoDotAndDotDot | QDir::Files);
  while (it.hasNext()) {
    it.next();
    sTmp = it.fileName();
    // qDebug() << sTmp;

    if (sTmp.startsWith(qApp->applicationName().toLower() + "_") &&
        sTmp.endsWith(".qm")) {
      sList << sTmp.remove(
                 qApp->applicationName().toLower() + "_").remove(".qm");
    }
  }

  // Check for additional translation files in share folder
  QDirIterator it2(m_sSharePath + "/lang", QStringList() << "*.qm",
                   QDir::NoDotAndDotDot | QDir::Files);
  while (it2.hasNext()) {
    it2.next();
    sTmp = it2.fileName();
    // qDebug() << sTmp;

    if (sTmp.startsWith(qApp->applicationName().toLower() + "_")) {
      sTmp = sTmp.remove(
               qApp->applicationName().toLower() + "_") .remove(".qm");
      if (!sList.contains(sTmp)) {
        sList << sTmp;
      }
    }
  }

  sList << "en";
  sList.sort();
  sList.push_front("auto");
  return sList;
}
