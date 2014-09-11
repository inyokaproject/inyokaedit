/**
 * \file CKnowledgeBox.cpp
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
 * Shows a modal window for knowledge box entry selection.
 */

#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QMessageBox>

#include "./CKnowledgeBox.h"
#include "ui_CKnowledgeBox.h"


void CKnowledgeBox::initPlugin(QWidget *pParent, CTextEditor *pEditor,
                               const QDir userDataDir,
                               const QString sSharePath) {
    Q_UNUSED(userDataDir);
    qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined _WIN32
    m_pSettings = new QSettings(QSettings::IniFormat,
                                QSettings::UserScope,
                                qApp->applicationName().toLower(),
                                PLUGIN_NAME);

    m_pSettingsApp = new QSettings(QSettings::IniFormat,
                                   QSettings::UserScope,
                                   qApp->applicationName().toLower(),
                                   qApp->applicationName().toLower());
#else
    m_pSettings = new QSettings(QSettings::NativeFormat,
                                QSettings::UserScope,
                                qApp->applicationName().toLower(),
                                PLUGIN_NAME);

    m_pSettingsApp = new QSettings(QSettings::NativeFormat,
                                   QSettings::UserScope,
                                   qApp->applicationName().toLower(),
                                   qApp->applicationName().toLower());
#endif
    m_pSettings->setIniCodec("UTF-8");
    m_sTplLang = m_pSettingsApp->value("TemplateLanguage", "de").toString();
    m_pEditor = pEditor;
    m_sSharePath = sSharePath;
    m_pTemplates = new CTemplates(m_sTplLang, m_sSharePath);

    this->loadTemplateEntries();
    this->buildUi(pParent);  // After loading template entries

    connect(m_pUi->buttonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CKnowledgeBox::getPluginName() const {
    return PLUGIN_NAME;
}

QString CKnowledgeBox::getPluginVersion() const {
    return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QTranslator* CKnowledgeBox::getPluginTranslator(const QString &sLocale) {
    QTranslator* pPluginTranslator = new QTranslator(this);
    QString sLocaleFile = QString(PLUGIN_NAME) + "_" + sLocale;
    if (!pPluginTranslator->load(sLocaleFile, m_sSharePath + "/lang")) {
        qWarning() << "Could not load plugin translation:" << sLocaleFile;
        return NULL;
    }
    return pPluginTranslator;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CKnowledgeBox::getCaption() const {
    return trUtf8("Knowledge box selector");
}

QIcon CKnowledgeBox::getIcon() const {
    return QIcon();
    // return QIcon(":/knowledgebox.png");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::buildUi(QWidget *pParent) {
    m_pDialog = new QDialog(pParent);
    m_pUi = new Ui::CKnowledgeBoxClass();
    m_pUi->setupUi(m_pDialog);
    m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                              & ~Qt::WindowContextHelpButtonHint);
    m_pDialog->setModal(true);

    m_pSigMapDeleteRow = new QSignalMapper(this);

    m_pUi->entriesTable->setColumnCount(3);
    m_pUi->entriesTable->setRowCount(0);

    m_pUi->entriesTable->setColumnWidth(0, 40);
#if QT_VERSION >= 0x050000
    m_pUi->entriesTable->horizontalHeader()->setSectionResizeMode(
                1, QHeaderView::Stretch);
#else
    m_pUi->entriesTable->horizontalHeader()->setResizeMode(
                1, QHeaderView::Stretch);
#endif
    m_pUi->entriesTable->setColumnWidth(2, 40);

    if (m_sListEntries.size() != m_bListEntryActive.size()) {
        qCritical() << "Error building knowledge box dialog. List sizes:"
                    << m_sListEntries.size() << "!="
                    << m_bListEntryActive.size();
        return;
    }
    for (int nRow = 0; nRow < m_sListEntries.size(); nRow++) {
        this->createRow(m_bListEntryActive[nRow], m_sListEntries[nRow]);
    }

    connect(m_pSigMapDeleteRow, SIGNAL(mapped(QWidget*)),
            this, SLOT(deleteRow(QWidget*)));
    m_pUi->addButton->setIcon(QIcon::fromTheme("list-add",
                                               QIcon(":/list-add.png")));
    connect(m_pUi->addButton, SIGNAL(pressed()),
            this, SLOT(addRow()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::loadTemplateEntries() {
    // Load entries from default template or config file
    m_bListEntryActive.clear();
    m_sListEntries.clear();
    uint nNumOfEntries = m_pSettings->value(m_sTplLang + "/NumOfEntries",
                                            0).toUInt();
    if (0 == nNumOfEntries) {
        this->loadTemplateDefaults();
    } else {
        qDebug() << "Reading knowledge box entries from config file";
        QString sTmpEntry("");
        m_pSettings->beginGroup(m_sTplLang);
        for (uint i = 0; i < nNumOfEntries; i++) {
            sTmpEntry = m_pSettings->value(
                        "Entry_" + QString::number(i), "").toString();
            if (!sTmpEntry.isEmpty()) {
                m_sListEntries << sTmpEntry;
                m_bListEntryActive << m_pSettings->value(
                                          "Active_" + QString::number(i),
                                          false).toBool();
            }
        }
        m_pSettings->endGroup();
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::loadTemplateDefaults() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    QStringList sListTemplates;
    QStringList sListTplMacros;
    m_bListEntryActive.clear();
    m_sListEntries.clear();

    sListTemplates = m_pTemplates->getListTplNamesINY();
    sListTplMacros = m_pTemplates->getListTplMacrosINY();

    int nIndex = sListTemplates.indexOf(m_pTemplates->getTransKnowledge());
    if (nIndex >= 0  &&
            sListTplMacros.size() >= 2 &&
            sListTplMacros.size() >= nIndex) {
        m_sListEntries = sListTplMacros[nIndex].split("\\n");
        m_sListEntries.removeFirst();
        m_sListEntries.removeLast();
        if (m_sListEntries.size() >= 1) {
            m_sListEntries[0].remove("%%");
            m_sListEntries[m_sListEntries.size() - 1].remove("%%");
        }
    } else {
        qCritical() << "Error while loading knowledge box template.";
    }

    for (int i = 0; i < m_sListEntries.size(); i++) {
        m_bListEntryActive << true;
    }

    this->writeSettings();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::executePlugin() {
    qDebug() << "Calling" << Q_FUNC_INFO;
    m_bCalledSettings = false;
    m_pDialog->show();
    m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::accept() {
    int nSize = m_sListEntries.size();
    m_sListEntries.clear();
    m_bListEntryActive.clear();
    for (int i = 0; i < nSize; i ++) {
        if (!m_pUi->entriesTable->item(i, 1)->text().isEmpty()) {
            m_sListEntries << m_pUi->entriesTable->item(i, 1)->text();
            if (m_pUi->entriesTable->item(i, 0)->checkState() != Qt::Checked) {
                m_bListEntryActive << false;
            } else {
                m_bListEntryActive << true;
            }
        }
    }

    if (!m_bCalledSettings) {
        QString sOutput = "{{{#!" + m_pTemplates->getTransTemplate().toLower() +
                  " " + m_pTemplates->getTransKnowledge() + "\n";
        if (m_sListEntries.size() == m_bListEntryActive.size()) {
            for (int i = 0; i < m_sListEntries.size(); i++) {
                if (m_bListEntryActive[i]) {
                    sOutput += m_sListEntries[i] + "\n";
                }
            }
        } else {
            sOutput += "ERROR";
            qCritical() << "Error executing knowledge box template.";
        }
        sOutput += "}}}\n";
        m_pEditor->insertPlainText(sOutput);
    }

    this->writeSettings();
    m_pDialog->done(QDialog::Accepted);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::addRow() {
    m_sListEntries << trUtf8("[:Article:New entry]");
    m_bListEntryActive << true;
    this->createRow(m_bListEntryActive.last(), m_sListEntries.last());
    m_pUi->entriesTable->scrollToBottom();
    m_pUi->entriesTable->editItem(m_pUi->entriesTable->item(
                                      m_sListEntries.size() - 1, 1));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::createRow(const bool &bActive, const QString &sText) {
    int nRow = m_pUi->entriesTable->rowCount();  // Before setRowCount!
    m_pUi->entriesTable->setRowCount(m_pUi->entriesTable->rowCount() + 1);

    for (int nCol = 0; nCol < m_pUi->entriesTable->columnCount(); nCol++) {
        m_pUi->entriesTable->setItem(nRow, nCol, new QTableWidgetItem());
    }

    // Checkbox
    m_pUi->entriesTable->item(nRow, 0)->setFlags(
                Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (false == bActive) {
        m_pUi->entriesTable->item(nRow, 0)->setCheckState(Qt::Unchecked);
    } else {
        m_pUi->entriesTable->item(nRow, 0)->setCheckState(Qt::Checked);
    }

    // Text
    m_pUi->entriesTable->item(nRow, 1)->setText(sText);

    // Delete row button
    m_listDelRowButtons << new QPushButton(
                               QIcon::fromTheme("list-remove",
                                                QIcon(":/list-remove.png")), "");
    m_pUi->entriesTable->setCellWidget(nRow, 2, m_listDelRowButtons.last());

    m_pSigMapDeleteRow->setMapping(m_listDelRowButtons.last(),
                                   m_listDelRowButtons.last());
    connect(m_listDelRowButtons.last(), SIGNAL(pressed()),
            m_pSigMapDeleteRow, SLOT(map()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::deleteRow(QWidget *widget) {
    QPushButton *button = reinterpret_cast<QPushButton*>(widget);
    if (button != NULL) {
        int nIndex = m_listDelRowButtons.indexOf(button);
        // qDebug() << "DELETE ROW:" << nIndex;
        if (nIndex >= 0 && nIndex < m_sListEntries.size()) {
            m_sListEntries.removeAt(nIndex);
            m_bListEntryActive.removeAt(nIndex);
            delete button;
            button = NULL;
            m_listDelRowButtons.removeAt(nIndex);
            m_pUi->entriesTable->removeRow(nIndex);
        }
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::writeSettings() {
    m_pSettings->remove(m_sTplLang);
    m_pSettings->beginGroup(m_sTplLang);
    m_pSettings->setValue("NumOfEntries", m_sListEntries.size());
    for (int i = 0; i < m_sListEntries.size(); i++) {
        m_pSettings->setValue("Entry_" + QString::number(i),
                              m_sListEntries[i]);
        m_pSettings->setValue("Active_" + QString::number(i),
                              m_bListEntryActive[i]);
    }
    m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CKnowledgeBox::hasSettings() const {
    return true;
}

void CKnowledgeBox::showSettings() {
    m_bCalledSettings = true;
    m_pDialog->show();
    m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CKnowledgeBox::showAbout() {
    QDate nDate = QDate::currentDate();
    QMessageBox aboutbox(NULL);

    aboutbox.setWindowTitle(trUtf8("Info"));
    // aboutbox.setIconPixmap(QPixmap(":/knowledgebox.png"));
    aboutbox.setText("<p><b>" + this->getCaption() + "</b><br />"
                     + trUtf8("Version") + ": " + PLUGIN_VERSION +"</p>"
                     + "<p>&copy; 2013-" + QString::number(nDate.year())
                     + " &ndash; " + QString::fromUtf8("Thorsten Roth")
                     + "<br />" + trUtf8("Licence") + ": "
                     + "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
                       "GNU General Public License Version 3</a></p>"
                     + "<p><i>"
                     + trUtf8("Plugin for choosing knowledge box entries.")
                     + "</i></p>");
    aboutbox.exec();
}

// ----------------------------------------------------------------------------

#if QT_VERSION < 0x050000
    Q_EXPORT_PLUGIN2(knowledgebox, CKnowledgeBox)
#endif
