/**
 * \file CSettingsDialog.cpp
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
 * Settings gui.
 */

#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QKeyEvent>
#include <QPushButton>

#include "./CSettingsDialog.h"
#include "ui_CSettingsDialog.h"

CSettingsDialog::CSettingsDialog(CSettings *pSettings,
                                 CHighlighter *pHighlighter,
                                 const QString &sSharePath, QWidget *pParent)
    : QDialog(pParent),
      m_pSettings(pSettings),
      m_pHighlighter(pHighlighter),
      m_sSharePath(sSharePath) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pUi = new Ui::CSettingsDialog();
    m_pUi->setupUi(this);
    this->setWindowFlags(this->windowFlags()
                         & ~Qt::WindowContextHelpButtonHint);
    this->setModal(true);
    m_pUi->tabWidget->setCurrentIndex(0);  // Load tab "general" at first start

#if QT_VERSION >= 0x050000
    m_pUi->styleTable->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch);
#else
    m_pUi->styleTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

#if defined _WIN32
    m_sExt = ".ini";
#else
    m_sExt = ".conf";
    m_pUi->WindowsUpdateCheck->setEnabled(false);
#endif

    ///////////////////
    // Load Settings //
    ///////////////////

    // General
    m_pUi->codeCompletionCheck->setChecked(m_pSettings->m_bCodeCompletion);
    m_pUi->previewInEditorCheck->setChecked(m_pSettings->m_bTmpPreviewInEditor);
    m_pUi->previewAlongsideCheck->setChecked(m_pSettings->m_bTmpPreviewAlongside);
    m_pUi->inyokaUrlEdit->setText(m_pSettings->getInyokaUrl());
    m_pUi->articleImageDownloadCheck->setChecked(m_pSettings->m_bAutomaticImageDownload);
    m_pUi->linkCheckingCheck->setChecked(m_pSettings->m_bCheckLinks);
    m_pUi->autosaveEdit->setValue(m_pSettings->m_nAutosave);
    m_pUi->reloadPreviewKeyEdit->setText(
                "0x" + QString::number(m_pSettings->getReloadPreviewKey(), 16));
    m_pUi->timedPreviewsEdit->setValue(m_pSettings->m_nTimedPreview);
    m_pUi->scrollbarSyncCheck->setChecked(m_pSettings->m_bSyncScrollbars);
    m_pUi->WindowsUpdateCheck->setChecked(m_pSettings->m_bWinCheckUpdate);

    QStringList sListGuiLanguages;
    sListGuiLanguages << "auto" << "en";
    QDir appDir(m_sSharePath + "/lang");
    QFileInfoList fiListFiles = appDir.entryInfoList(
                QDir::NoDotAndDotDot | QDir::Files);
    foreach (QFileInfo fi, fiListFiles) {
        if ("qm" == fi.suffix() && fi.baseName().startsWith(qAppName() + "_")) {
            sListGuiLanguages << fi.baseName().remove(qAppName() + "_");
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

    m_bTmpPreviewInEditor = m_pSettings->m_bPreviewInEditor;
    m_bTmpPreviewAlongside = m_pSettings->m_bPreviewAlongside;

    // Font
    m_pUi->fontComboBox->setCurrentFont(QFont(m_pSettings->m_sFontFamily));
    m_pUi->fontSizeEdit->setValue(m_pSettings->m_nFontsize);

    // Style
    QFileInfo fiStylePath(m_pSettings->m_sStyleFile);
    fiListFiles = fiStylePath.absoluteDir().entryInfoList(
                QDir::NoDotAndDotDot | QDir::Files);
    foreach (QFileInfo fi, fiListFiles) {
        if (fi.fileName().endsWith("-style" + m_sExt)) {
            m_sListStyleFiles << fi.fileName().remove(m_sExt);
        }
    }
    m_sListStyleFiles.push_front(trUtf8("Create new style..."));
    m_pUi->styleFilesBox->addItems(m_sListStyleFiles);
    m_pUi->styleFilesBox->insertSeparator(1);
    QFileInfo fiStyle(m_pSettings->m_sStyleFile);
    m_pUi->styleFilesBox->setCurrentIndex(
                m_pUi->styleFilesBox->findText(fiStyle.baseName()));
    this->loadHighlighting(fiStyle.baseName());

    // Recent files
    m_pUi->numberRecentFilesEdit->setValue((quint16)m_pSettings->m_nMaxLastOpenedFiles);
    m_pUi->numberRecentFilesEdit->setMaximum((quint16)m_pSettings->m_cMAXFILES);

    // Proxy
    m_pUi->proxyHostNameEdit->setText(m_pSettings->m_sProxyHostName);
    m_pUi->proxyPortSpinBox->setValue(m_pSettings->m_nProxyPort);
    m_pUi->proxyUserNameEdit->setText(m_pSettings->m_sProxyUserName);
    m_pUi->proxyPasswordEdit->setText(m_pSettings->m_sProxyPassword);

    m_sProxyHostName = m_pSettings->m_sProxyHostName;
    m_nProxyPort = m_pSettings->m_nProxyPort;
    m_sProxyUserName = m_pSettings->m_sProxyUserName;
    m_sProxyPassword = m_pSettings->m_sProxyPassword;

    connect(m_pUi->previewAlongsideCheck, SIGNAL(clicked(bool)),
            this, SLOT(changedPreviewAlongside(bool)));
    connect(m_pUi->previewInEditorCheck, SIGNAL(clicked(bool)),
            this, SLOT(changedPreviewInEditor(bool)));

    connect(m_pUi->styleFilesBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changedStyle(int)));

    QStringList sListHeader;
    sListHeader << trUtf8("Color") << trUtf8("Bold")
                << trUtf8("Italic") << trUtf8("Background");
    m_pUi->styleTable->setHorizontalHeaderLabels(sListHeader);
    sListHeader.clear();
    sListHeader << trUtf8("Background") << trUtf8("Text color")
                << trUtf8("Text formating") << trUtf8("Heading")
                << trUtf8("Hyperlink") << trUtf8("InterWiki")
                << trUtf8("Macro") << trUtf8("Parser") << trUtf8("List")
                << trUtf8("Table line") << trUtf8("Table cell format")
                << trUtf8("ImgMap") << trUtf8("Misc") << trUtf8("Comment");
    m_pUi->styleTable->setVerticalHeaderLabels(sListHeader);

    connect(m_pUi->styleTable, SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(clickedStyleCell(int, int)));
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
    m_pSettings->m_bTmpPreviewInEditor = m_pUi->previewInEditorCheck->isChecked();
    m_pSettings->m_bTmpPreviewAlongside = m_pUi->previewAlongsideCheck->isChecked();
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

    // Style
    this->saveHighlighting();
    m_pHighlighter->rehighlight();

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
    if (m_pUi->previewAlongsideCheck->isChecked() != m_bTmpPreviewAlongside
            || m_pUi->previewInEditorCheck->isChecked() != m_bTmpPreviewInEditor
            || m_pUi->proxyHostNameEdit->text() != m_sProxyHostName
            || m_pUi->proxyPortSpinBox->value() != m_nProxyPort
            || m_pUi->proxyUserNameEdit->text() != m_sProxyUserName
            || m_pUi->proxyPasswordEdit->text() != m_sProxyPassword
            || m_pUi->GuiLangCombo->currentText() != m_sGuiLang
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

void CSettingsDialog::changedPreviewAlongside(bool bState) {
    if (bState) {
        m_pUi->previewInEditorCheck->setChecked(true);
        m_pUi->scrollbarSyncCheck->setEnabled(true);
    } else {
        m_pUi->scrollbarSyncCheck->setEnabled(false);
    }
}

void CSettingsDialog::changedPreviewInEditor(bool bState) {
    if (!bState) {
        m_pUi->previewAlongsideCheck->setChecked(false);
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettingsDialog::loadHighlighting(const QString &sStyleFile) {
    m_pHighlighter->readStyle(sStyleFile);

    // Background
    if (m_pHighlighter->m_bSystemBackground) {
        m_pUi->styleTable->item(0, 0)->setText("System");
    } else {
        m_pUi->styleTable->item(0, 0)->setText(
                    m_pHighlighter->m_colorBackground.name());
    }
    // Foreground
    if (m_pHighlighter->m_bSystemForeground) {
        m_pUi->styleTable->item(1, 0)->setText("System");
    } else {
        m_pUi->styleTable->item(1, 0)->setText(
                    m_pHighlighter->m_colorForeground.name());
    }

    readValue(2, m_pHighlighter->m_textformatFormat);      // Text format
    readValue(3, m_pHighlighter->m_headingsFormat);        // Heading
    readValue(4, m_pHighlighter->m_linksFormat);           // Hyperlink
    readValue(5, m_pHighlighter->m_interwikiLinksFormat);  // InterWiki
    readValue(6, m_pHighlighter->m_macrosFormat);          // Macro
    readValue(7, m_pHighlighter->m_parserFormat);          // Parser
    readValue(8, m_pHighlighter->m_listFormat);            // List
    readValue(9, m_pHighlighter->m_newTableLineFormat);    // Table line
    readValue(10, m_pHighlighter->m_tablecellsFormat);     // Table cell
    readValue(11, m_pHighlighter->m_imgMapFormat);         // Image map
    readValue(12, m_pHighlighter->m_miscFormat);           // Misc
    readValue(13, m_pHighlighter->m_commentFormat);        // Comment
}

// ----------------------------------------------------------------------------

void CSettingsDialog::readValue(const quint16 nRow,
                                const QTextCharFormat &charFormat) {
    // Foreground
    m_pUi->styleTable->item(nRow, 0)->setText(
                charFormat.foreground().color().name());
    // Bold
    if (charFormat.font().bold()) {
        m_pUi->styleTable->item(nRow, 1)->setCheckState(Qt::Checked);
    } else {
        m_pUi->styleTable->item(nRow, 1)->setCheckState(Qt::Unchecked);
    }
    // Italic
    if (charFormat.font().italic()) {
        m_pUi->styleTable->item(nRow, 2)->setCheckState(Qt::Checked);
    } else {
        m_pUi->styleTable->item(nRow, 2)->setCheckState(Qt::Unchecked);
    }
    // Background
    if (charFormat.background().color() != Qt::transparent) {
        m_pUi->styleTable->item(nRow, 3)->setText(
                    charFormat.background().color().name());
    } else {
        m_pUi->styleTable->item(nRow, 3)->setText("");
    }
}

// ----------------------------------------------------------------------------

void CSettingsDialog::clickedStyleCell(int nRow, int nCol) {
    if (0 == nCol || 3 == nCol) {
        QColorDialog colorDialog;
        QColor initialColor(m_pUi->styleTable->item(nRow, nCol)->text());
        QColor newColor = colorDialog.getColor(initialColor);
        if (newColor.isValid()) {
            m_pUi->styleTable->item(nRow, nCol)->setText(newColor.name());
        } else if (newColor.name().isEmpty()) {
            m_pUi->styleTable->item(nRow, nCol)->setText("");
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettingsDialog::saveHighlighting() {
    m_pSettings->m_sStyleFile = m_pUi->styleFilesBox->currentText();
    m_pHighlighter->readStyle(m_pUi->styleFilesBox->currentText());

    // Background
    if ("system" == m_pUi->styleTable->item(0, 0)->text().toLower()) {
        m_pHighlighter->m_bSystemBackground = true;
    } else {
        m_pHighlighter->m_bSystemBackground = false;
        m_pHighlighter->m_colorBackground.setNamedColor(
                    m_pUi->styleTable->item(0, 0)->text());
    }
    // Foreground
    if ("system" == m_pUi->styleTable->item(1, 0)->text().toLower()) {
        m_pHighlighter->m_bSystemForeground = true;
    } else {
        m_pHighlighter->m_bSystemForeground = false;
        m_pHighlighter->m_colorForeground.setNamedColor(
                    m_pUi->styleTable->item(1, 0)->text());
    }

    m_pHighlighter->evalKey(this->createValues(2),
                            m_pHighlighter->m_textformatFormat);
    m_pHighlighter->evalKey(this->createValues(3),
                            m_pHighlighter->m_headingsFormat);
    m_pHighlighter->evalKey(this->createValues(4),
                            m_pHighlighter->m_linksFormat);
    m_pHighlighter->evalKey(this->createValues(5),
                            m_pHighlighter->m_interwikiLinksFormat);
    m_pHighlighter->evalKey(this->createValues(6),
                            m_pHighlighter->m_macrosFormat);
    m_pHighlighter->evalKey(this->createValues(7),
                            m_pHighlighter->m_parserFormat);
    m_pHighlighter->evalKey(this->createValues(8),
                            m_pHighlighter->m_listFormat);
    m_pHighlighter->evalKey(this->createValues(9),
                            m_pHighlighter->m_newTableLineFormat);
    m_pHighlighter->evalKey(this->createValues(10),
                            m_pHighlighter->m_tablecellsFormat);
    m_pHighlighter->evalKey(this->createValues(11),
                            m_pHighlighter->m_imgMapFormat);
    m_pHighlighter->evalKey(this->createValues(12),
                            m_pHighlighter->m_miscFormat);
    m_pHighlighter->evalKey(this->createValues(13),
                            m_pHighlighter->m_commentFormat);

    m_pHighlighter->saveStyle();
    m_pHighlighter->readStyle(m_pUi->styleFilesBox->currentText());
}

// ----------------------------------------------------------------------------

QString CSettingsDialog::createValues(const quint16 nRow) {
    QString sReturn("");
    QString sTmp("");
    sTmp = m_pUi->styleTable->item(nRow, 0)->text();
    sTmp.remove(0, 1).push_front("0x");
    sTmp.append("|");
    sReturn += sTmp;
    if (m_pUi->styleTable->item(nRow, 1)->checkState() == Qt::Checked) {
        sReturn += "true|";
    } else { sReturn += "false|"; }
    if (m_pUi->styleTable->item(nRow, 2)->checkState() == Qt::Checked) {
        sReturn += "true|";
    } else { sReturn += "false|"; }
    sTmp = m_pUi->styleTable->item(nRow, 3)->text();
    sTmp.remove(0, 1).push_front("0x");
    sReturn += sTmp;
    return sReturn;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSettingsDialog::changedStyle(int nIndex) {
    QString sFileName("");

    if (0 == nIndex) {  // Create new style
        bool bOk;
        QFileInfo fiStyle(m_pSettings->getStyleFile());

        sFileName = QInputDialog::getText(0, trUtf8("New style"),
                                              trUtf8("Please insert name of "
                                                     "new style file:"),
                                              QLineEdit::Normal,
                                              "",
                                              &bOk);
        // Click on "cancel" or string is empty
        if (true != bOk || sFileName.isEmpty()) {
            // Reset selection
            m_pUi->styleFilesBox->setCurrentIndex(
                        m_pUi->styleFilesBox->findText(fiStyle.baseName()));
            return;
        } else {
            QFile fileStyle;
            sFileName = sFileName + "-style";
            bOk = fileStyle.copy(fiStyle.absoluteFilePath(),
                                 fiStyle.absolutePath() + "/"
                                 + sFileName + m_sExt);
            if (true != bOk) {
                QMessageBox::warning(0, "Error", "Could not create new style.");
                qWarning() << "Could not create new style file:"
                           << fiStyle.absolutePath() + "/" + sFileName + m_sExt;
                return;
            }
            m_pUi->styleFilesBox->addItem(sFileName);
            m_pUi->styleFilesBox->setCurrentIndex(
                        m_pUi->styleFilesBox->findText(sFileName));
        }
    } else {  // Load existing style file
        sFileName = m_pUi->styleFilesBox->currentText();
    }

    loadHighlighting(sFileName);
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
