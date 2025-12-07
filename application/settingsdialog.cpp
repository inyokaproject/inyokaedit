// SPDX-FileCopyrightText: 2011-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "./settingsdialog.h"

#include <QDebug>
#include <QDirIterator>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

#include "./settings.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *pParent)
    : QDialog(pParent),
      m_pSettings(Settings::instance()),
      m_sSharePath(Settings::instance()->getSharePath()) {
  m_pUi = new Ui::SettingsDialog();
  m_pUi->setupUi(this);
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  this->setModal(true);
  m_pUi->tabWidget->setCurrentIndex(0);  // Load tab "general" at first start

  // Search Inyoka communities
  QStringList sListCommunities;
  QDir extendedShareDir(m_sSharePath + "/community");
  const QFileInfoList fiListFiles =
      extendedShareDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
  sListCommunities.reserve(fiListFiles.size());
  for (const auto &fi : fiListFiles) {
    sListCommunities << fi.fileName();
  }
  m_pUi->CommunityCombo->addItems(sListCommunities);

  this->readSettings();

  // Enter Qt key-code automatically in text box
  m_pUi->reloadPreviewKeyEdit->installEventFilter(this);

#ifndef _WIN32
  m_pUi->WindowsUpdateCheck->setEnabled(false);
#endif

  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted, this,
          &SettingsDialog::accept);
  connect(m_pUi->buttonBox, &QDialogButtonBox::rejected, this,
          &SettingsDialog::reject);
}

SettingsDialog::~SettingsDialog() {
  if (m_pUi) {
    delete m_pUi;
    m_pUi = nullptr;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SettingsDialog::readSettings() {
  // General settings
  m_pUi->GuiLangCombo->addItems(this->searchTranslations());
  if (-1 != m_pUi->GuiLangCombo->findText(m_pSettings->getGuiLanguage())) {
    m_pUi->GuiLangCombo->setCurrentIndex(
        m_pUi->GuiLangCombo->findText(m_pSettings->getGuiLanguage()));
  } else {
    m_pUi->GuiLangCombo->setCurrentIndex(
        m_pUi->GuiLangCombo->findText(QStringLiteral("auto")));
  }

  m_pUi->codeCompletionCheck->setChecked(m_pSettings->getCodeCompletion());
  m_pUi->syntaxCheck->setChecked(m_pSettings->getSyntaxCheck());
  m_pUi->splitHorizontalRadio->setChecked(m_pSettings->getPreviewHorizontal());
  m_pUi->splitVerticalRadio->setChecked(!m_pSettings->getPreviewHorizontal());
  m_pUi->articleImageDownloadCheck->setChecked(
      m_pSettings->getAutomaticImageDownload());
  m_pUi->linkCheckingCheck->setChecked(m_pSettings->getCheckLinks());
  m_pUi->autosaveEdit->setValue(static_cast<int>(m_pSettings->getAutoSave()));
  m_pUi->timedPreviewsEdit->setValue(
      static_cast<int>(m_pSettings->getTimedPreview()));
  m_pUi->WindowsUpdateCheck->setChecked(m_pSettings->getWindowsCheckUpdate());

  m_pUi->reloadPreviewKeyEdit->setText(
      "0x" + QString::number(m_pSettings->getReloadPreviewKey(), 16));
#ifdef NOPREVIEW
  m_pUi->previewsplitlabel->setVisible(false);
  m_pUi->splitHorizontalRadio->setVisible(false);
  m_pUi->splitVerticalRadio->setVisible(false);
  m_pUi->reloadPreviewKeyLabel->setVisible(false);
  m_pUi->reloadPreviewKeyEdit->setVisible(false);
  m_pUi->scrollbarSyncCheck->setVisible(false);
#endif
  m_pUi->scrollbarSyncCheck->setChecked(m_pSettings->getSyncScrollbars());

  // Font settings
  m_pUi->fontComboBox->setCurrentFont(
      QFont(m_pSettings->getEditorFont().family()));
  m_pUi->fontSizeEdit->setValue(m_pSettings->getEditorFont().pointSizeF());

  // Inyoka community settings
  m_pUi->CommunityCombo->blockSignals(true);  // No change index signal emitted
  if (-1 !=
      m_pUi->CommunityCombo->findText(m_pSettings->getInyokaCommunity())) {
    m_pUi->CommunityCombo->setCurrentIndex(
        m_pUi->CommunityCombo->findText(m_pSettings->getInyokaCommunity()));
  } else {
    m_pUi->CommunityCombo->setCurrentIndex(0);
  }
  m_pSettings->setInyokaCommunity(m_pUi->CommunityCombo->currentText());
  m_pUi->CommunityCombo->blockSignals(false);

  m_pUi->inyokaUserEdit->setText(m_pSettings->getInyokaUser());
  m_pUi->inyokaPasswordEdit->setText(m_pSettings->getInyokaPassword());

  // Proxy
  m_pUi->proxyHostNameEdit->setText(m_pSettings->getProxyHostName());
  m_pUi->proxyPortSpinBox->setValue(m_pSettings->getProxyPort());
  m_pUi->proxyUserNameEdit->setText(m_pSettings->getProxyUserName());
  m_pUi->proxyPasswordEdit->setText(m_pSettings->getProxyPassword());

  // Recent files
  m_pUi->numberRecentFilesEdit->setValue(
      static_cast<quint16>(m_pSettings->getNumOfRecentFiles()));
  m_pUi->numberRecentFilesEdit->setMaximum(
      static_cast<quint16>(Settings::MAX_RECENT_FILES));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SettingsDialog::accept() {
  // General
  QString sOldValue = m_pSettings->getGuiLanguage();
  QString sNewValue = m_pUi->GuiLangCombo->currentText();
  m_pSettings->setGuiLanguage(sNewValue);
  if (sOldValue != sNewValue) {
    m_pUi->retranslateUi(this);
    emit changeGuiLanguage(m_pSettings->getGuiLanguage());
  }

  m_pSettings->setCodeCompletion(m_pUi->codeCompletionCheck->isChecked());
  m_pSettings->setSyntaxCheck(m_pUi->syntaxCheck->isChecked());
  m_pSettings->setPreviewHorizontal(m_pUi->splitHorizontalRadio->isChecked());
  m_pSettings->setAutomaticImageDownload(
      m_pUi->articleImageDownloadCheck->isChecked());
  m_pSettings->setCheckLinks(m_pUi->linkCheckingCheck->isChecked());
  m_pSettings->setAutoSave(static_cast<quint32>(m_pUi->autosaveEdit->value()));
  m_pSettings->setReloadPreviewKey(
      m_pUi->reloadPreviewKeyEdit->text().trimmed());
  m_pSettings->setTimedPreview(
      static_cast<quint32>(m_pUi->timedPreviewsEdit->value()));
  m_pSettings->setSyncScrollbars(m_pUi->scrollbarSyncCheck->isChecked());
  m_pSettings->setWindowsCheckUpdate(m_pUi->WindowsUpdateCheck->isChecked());
  // TODO: Add Pygmentize to dialog
  m_pSettings->setPygmentize(m_pSettings->getPygmentize());

  // Inyoka community
  QString sOldInyokaCommunity(m_pSettings->getInyokaCommunity());
  m_pSettings->setInyokaCommunity(m_pUi->CommunityCombo->currentText());
  m_pSettings->setInyokaUser(m_pUi->inyokaUserEdit->text().trimmed());
  m_pSettings->setInyokaPassword(m_pUi->inyokaPasswordEdit->text().trimmed());

  // Font
  QFont EditorFont(m_pSettings->getEditorFont());
  EditorFont.setFamily(m_pUi->fontComboBox->currentFont().family());
  EditorFont.setPointSizeF(m_pUi->fontSizeEdit->value());
  m_pSettings->setEditorFont(EditorFont);

  // Recent files
  m_pSettings->setNumOfRecentFiles(m_pUi->numberRecentFilesEdit->value());

  // Proxy
  m_pSettings->setProxyHostName(m_pUi->proxyHostNameEdit->text().trimmed());
  m_pSettings->setProxyPort(
      static_cast<quint16>(m_pUi->proxyPortSpinBox->value()));
  m_pSettings->setProxyUserName(m_pUi->proxyUserNameEdit->text().trimmed());
  m_pSettings->setProxyPassword(m_pUi->proxyPasswordEdit->text().trimmed());

  // Plugins
  QStringList oldDisabledPlugins(m_pSettings->getDisabledPlugins());
  QStringList newDisabledPlugins;
  for (int i = 0; i < m_listPlugins.size(); i++) {
    if (m_pUi->pluginsTable->item(i, 0)->checkState() != Qt::Checked) {
      newDisabledPlugins << m_listPlugins[i]->getPluginName();
    }
  }
  oldDisabledPlugins.sort();  // Sort for comparison
  newDisabledPlugins.sort();
  oldDisabledPlugins.removeAll(QLatin1String(""));
  newDisabledPlugins.removeAll(QLatin1String(""));
  m_pSettings->setDisabledPlugins(newDisabledPlugins);

  // If the following settings have been changed, a restart is needed
  if (m_pUi->CommunityCombo->currentText() != sOldInyokaCommunity ||
      oldDisabledPlugins != newDisabledPlugins) {
    QMessageBox::information(this, this->windowTitle(),
                             tr("The editor has to be restarted for "
                                "applying the changes."));
  }

  m_pUi->tabWidget->setCurrentIndex(0);  // Reset first tab after start
  QDialog::accept();
  emit updateEditorSettings();
}

// ----------------------------------------------------------------------------

void SettingsDialog::reject() {
  m_pUi->tabWidget->setCurrentIndex(0);  // Reset first tab after start
  QDialog::reject();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SettingsDialog::eventFilter(QObject *obj, QEvent *event) -> bool {
  // Enter Qt key-code automatically in text box
  if (m_pUi->reloadPreviewKeyEdit == obj) {
    if (QEvent::KeyPress == event->type()) {
      auto *keyEvent = static_cast<QKeyEvent *>(event);
      // Remove key with DEL
      if (Qt::Key_Delete != keyEvent->key()) {
        m_pUi->reloadPreviewKeyEdit->setText(
            "0x" + QString::number(keyEvent->key(), 16));
        return true;
      }
      m_pUi->reloadPreviewKeyEdit->setText(QStringLiteral("0x0"));
    }
  }
  return QObject::eventFilter(obj, event);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void SettingsDialog::changeEvent(QEvent *pEvent) {
  if (nullptr != pEvent) {
    if (QEvent::LanguageChange == pEvent->type()) {
      m_pUi->retranslateUi(this);
    }
  }
  QDialog::changeEvent(pEvent);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SettingsDialog::getAvailablePlugins(
    const QList<IEditorPlugin *> &Plugins,
    const QList<QObject *> &PluginObjList) {
  m_listPlugins = Plugins;
  const quint8 nNUMCOLS = 5;
  const quint8 nWIDTH = 40;

  m_pUi->pluginsTable->setColumnCount(nNUMCOLS);
  m_pUi->pluginsTable->setRowCount(m_listPlugins.size());

  m_pUi->pluginsTable->setColumnWidth(0, nWIDTH);
  m_pUi->pluginsTable->setColumnWidth(1, nWIDTH);
  m_pUi->pluginsTable->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::Stretch);
  m_pUi->pluginsTable->setColumnWidth(3, nWIDTH);
  m_pUi->pluginsTable->setColumnWidth(4, nWIDTH);

  for (int nRow = 0; nRow < m_listPlugins.size(); nRow++) {
    for (int nCol = 0; nCol < nNUMCOLS; nCol++) {
      m_pUi->pluginsTable->setItem(nRow, nCol, new QTableWidgetItem());
    }

    // Checkbox
    m_pUi->pluginsTable->item(nRow, 0)->setFlags(Qt::ItemIsUserCheckable |
                                                 Qt::ItemIsEnabled);
    if (m_pSettings->getDisabledPlugins().contains(
            m_listPlugins.at(nRow)->getPluginName())) {
      m_pUi->pluginsTable->item(nRow, 0)->setCheckState(Qt::Unchecked);
    } else {
      m_pUi->pluginsTable->item(nRow, 0)->setCheckState(Qt::Checked);
    }

    // Icon
    m_pUi->pluginsTable->setIconSize(QSize(22, 22));
    m_pUi->pluginsTable->item(nRow, 1)->setIcon(
        m_listPlugins.at(nRow)->getIcon());
    // Caption
    m_pUi->pluginsTable->item(nRow, 2)->setText(
        m_listPlugins.at(nRow)->getCaption());

    // Settings
    if (m_listPlugins.at(nRow)->hasSettings()) {
      m_listPluginInfoButtons << new QPushButton(
          QIcon::fromTheme(QStringLiteral("configure")), QLatin1String(""));
      connect(m_listPluginInfoButtons.last(), &QPushButton::pressed,
              PluginObjList.at(nRow), [=]() {
                qobject_cast<IEditorPlugin *>(PluginObjList.at(nRow))
                    ->showSettings();
              });

      m_pUi->pluginsTable->setCellWidget(nRow, 3,
                                         m_listPluginInfoButtons.last());

      if (m_pSettings->getDisabledPlugins().contains(
              m_listPlugins[nRow]->getPluginName())) {
        m_listPluginInfoButtons.last()->setEnabled(false);
      }
    }

    // Info
    m_listPluginInfoButtons << new QPushButton(
        QIcon::fromTheme(QStringLiteral("help-about")), QLatin1String(""));
    connect(m_listPluginInfoButtons.last(), &QPushButton::pressed,
            PluginObjList[nRow], [=]() {
              qobject_cast<IEditorPlugin *>(PluginObjList[nRow])->showAbout();
            });
    m_pUi->pluginsTable->setCellWidget(nRow, 4, m_listPluginInfoButtons.last());
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SettingsDialog::searchTranslations() -> QStringList {
  QStringList sList;
  QString sTmp;

  // Translations build in resources
  QDirIterator it(QStringLiteral(":"), QStringList() << QStringLiteral("*.qm"),
                  QDir::NoDotAndDotDot | QDir::Files);
  while (it.hasNext()) {
    it.next();
    sTmp = it.fileName();
    // qDebug() << sTmp;

    if (sTmp.startsWith(qApp->applicationName().toLower() + "_") &&
        sTmp.endsWith(QLatin1String(".qm"))) {
      sList << sTmp.remove(qApp->applicationName().toLower() + "_")
                   .remove(QStringLiteral(".qm"));
    }
  }

  // Check for additional translation files in share folder
  QDirIterator it2(m_sSharePath + "/lang",
                   QStringList() << QStringLiteral("*.qm"),
                   QDir::NoDotAndDotDot | QDir::Files);
  while (it2.hasNext()) {
    it2.next();
    sTmp = it2.fileName();
    // qDebug() << sTmp;

    if (sTmp.startsWith(qApp->applicationName().toLower() + "_")) {
      sTmp = sTmp.remove(qApp->applicationName().toLower() + "_")
                 .remove(QStringLiteral(".qm"));
      if (!sList.contains(sTmp)) {
        sList << sTmp;
      }
    }
  }

  sList.sort();
  sList.push_front(QStringLiteral("auto"));
  return sList;
}
