/**
 * \file uu_knowledgebox.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2013-2019 The InyokaEdit developers
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
 * Shows a modal window for ubuntuusers.de knowledge box entry selection.
 */

#include "./uu_knowledgebox.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>

#include "ui_uu_knowledgebox.h"

void Uu_KnowledgeBox::initPlugin(QWidget *pParent, TextEditor *pEditor,
                                 const QDir &userDataDir,
                                 const QString &sSharePath) {
  Q_UNUSED(userDataDir);
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined _WIN32
  m_pSettings = new QSettings(QSettings::IniFormat,
                              QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              PLUGIN_NAME);
#else
  m_pSettings = new QSettings(QSettings::NativeFormat,
                              QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              PLUGIN_NAME);
#endif
  m_pSettings->setIniCodec("UTF-8");
  m_pEditor = pEditor;
  m_sSharePath = sSharePath;

  this->loadTemplateEntries();
  this->buildUi(pParent);  // After loading template entries

  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted,
          this, &Uu_KnowledgeBox::accept);
  connect(m_pUi->buttonBox, &QDialogButtonBox::rejected,
          m_pDialog, &QDialog::reject);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Uu_KnowledgeBox::getPluginName() const {
  return PLUGIN_NAME;
}

QString Uu_KnowledgeBox::getPluginVersion() const {
  return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);
  if ("en" == sLang) {
    return;
  }

  if (!m_translator.load(":/" + QString(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QString(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
    if (!m_translator.load(QString(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" + QString(PLUGIN_NAME).toLower() +
                    "_" + sLang + ".qm";
      return;
    }
  }

  if (qApp->installTranslator(&m_translator)) {
    m_pUi->retranslateUi(m_pDialog);
  } else {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Uu_KnowledgeBox::getCaption() const {
  return tr("Ubuntuusers.de knowledge box");
}
QIcon Uu_KnowledgeBox::getIcon() const {
  return QIcon();
  // return QIcon(":/knowledgebox.png");
}

bool Uu_KnowledgeBox::includeMenu() const {
  return true;
}
bool Uu_KnowledgeBox::includeToolbar() const {
  return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::buildUi(QWidget *pParent) {
  m_pDialog = new QDialog(pParent);
  m_pUi = new Ui::Uu_KnowledgeBoxClass();
  m_pUi->setupUi(m_pDialog);
  m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                            & ~Qt::WindowContextHelpButtonHint);
  m_pDialog->setModal(true);

  m_pSigMapDeleteRow = new QSignalMapper(this);

  m_pUi->entriesTable->setColumnCount(3);
  m_pUi->entriesTable->setRowCount(0);
  m_pUi->entriesTable->setColumnWidth(0, 40);
  m_pUi->entriesTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
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

  connect(m_pSigMapDeleteRow,
          static_cast<void(QSignalMapper::*)(QWidget *)>(&QSignalMapper::mapped),
          this, &Uu_KnowledgeBox::deleteRow);
  m_pUi->addButton->setIcon(QIcon::fromTheme("list-add",
                                             QIcon(":/list-add.png")));
  connect(m_pUi->addButton, &QPushButton::pressed,
          this, &Uu_KnowledgeBox::addRow);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::loadTemplateEntries() {
  // Load entries from default template or config file
  m_bListEntryActive.clear();
  m_sListEntries.clear();
  uint nNumOfEntries = m_pSettings->value("NumOfEntries", 0).toUInt();

  if (0 == nNumOfEntries) {
    this->loadTemplateDefaults();
  } else {
    qDebug() << "Reading knowledge box entries from config file";
    QString sTmpEntry("");
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
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::loadTemplateDefaults() {
  qDebug() << Q_FUNC_INFO;
  QFile fiDefault(":/uu_knowledgebox.default");
  m_bListEntryActive.clear();
  m_sListEntries.clear();

  if (!fiDefault.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open uu_knowledgebox.default";
    QMessageBox::warning(NULL, tr("Error"), tr("Could not open %1")
                         .arg("uu_knowledgebox.default"));
  } else {
    QTextStream in(&fiDefault);
    in.setCodec("UTF-8");
    QString tmpLine("");

    while (!in.atEnd()) {
      tmpLine = in.readLine().trimmed();
      if (!tmpLine.trimmed().isEmpty()) {
        m_sListEntries << tmpLine.trimmed();
      }
    }
    fiDefault.close();
  }

  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_bListEntryActive << true;
  }

  if (0 == m_sListEntries.size()) {
    qWarning() << "Knowledgebox defaults are empty!";
    QMessageBox::warning(
          NULL, tr("Error"),
          tr("ubuntuusers.de knowledgebox defaults are empty!"));
  }

  this->writeSettings();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::callPlugin() {
  qDebug() << Q_FUNC_INFO;
  m_bCalledSettings = false;
  m_pDialog->show();
  m_pDialog->exec();
}

void Uu_KnowledgeBox::executePlugin() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::accept() {
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
    QString sOutput = "{{{#!vorlage Wissen\n";
    if (m_sListEntries.size() == m_bListEntryActive.size()) {
      for (int i = 0; i < m_sListEntries.size(); i++) {
        if (m_bListEntryActive.at(i)) {
          sOutput += m_sListEntries.at(i) + "\n";
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

void Uu_KnowledgeBox::addRow() {
  m_sListEntries << tr("[:Article:New entry]");
  m_bListEntryActive << true;
  this->createRow(m_bListEntryActive.last(), m_sListEntries.last());
  m_pUi->entriesTable->scrollToBottom();
  m_pUi->entriesTable->editItem(m_pUi->entriesTable->item(
                                  m_sListEntries.size() - 1, 1));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::createRow(const bool bActive, const QString &sText) {
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
  connect(m_listDelRowButtons.last(), &QPushButton::pressed,
          m_pSigMapDeleteRow,
          static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::deleteRow(QWidget *widget) {
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

void Uu_KnowledgeBox::writeSettings() {
  m_pSettings->remove("General");
  m_pSettings->setValue("NumOfEntries", m_sListEntries.size());
  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_pSettings->setValue("Entry_" + QString::number(i),
                          m_sListEntries[i]);
    m_pSettings->setValue("Active_" + QString::number(i),
                          m_bListEntryActive[i]);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool Uu_KnowledgeBox::hasSettings() const {
  return true;
}

void Uu_KnowledgeBox::showSettings() {
  m_bCalledSettings = true;
  m_pDialog->show();
  m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void Uu_KnowledgeBox::setEditorlist(const QList<TextEditor *> &listEditors) {
  Q_UNUSED(listEditors);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::showAbout() {
  QMessageBox aboutbox(NULL);
  aboutbox.setWindowTitle(tr("Info"));
  // aboutbox.setIconPixmap(QPixmap(":/knowledgebox.png"));
  aboutbox.setText(
        QString("<p><b>%1</b><br />"
                "%2</p>"
                "<p>%3<br />"
                "%4</p>"
                "<p><i>%5</i></p>")
        .arg(this->getCaption(),
             tr("Version") + ": " + PLUGIN_VERSION,
             PLUGIN_COPY,
             tr("Licence") + ": " +
             "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
             "GNU General Public License Version 3</a>",
             tr("Plugin for choosing ubuntuusers.de knowledge box entries.")));
  aboutbox.exec();
}
