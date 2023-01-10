/**
 * \file uu_knowledgebox.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2013-present The InyokaEdit developers
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
 * Shows a modal window for ubuntuusers.de knowledge box entry selection.
 */

#include "./uu_knowledgebox.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QSettings>

#include "../../application/texteditor.h"
#include "ui_uu_knowledgebox.h"

void Uu_KnowledgeBox::initPlugin(QWidget *pParent, TextEditor *pEditor,
                                 const QDir &userDataDir,
                                 const QString &sSharePath) {
  Q_UNUSED(userDataDir)
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              QStringLiteral(PLUGIN_NAME));
#else
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              QStringLiteral(PLUGIN_NAME));
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  // Since Qt 6 UTF-8 is used by default
  m_pSettings->setIniCodec("UTF-8");
#endif
  m_pParent = pParent;
  m_pEditor = pEditor;
  m_sSharePath = sSharePath;

  this->loadTemplateEntries();
  this->buildUi(m_pParent);  // After loading template entries

  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted, this,
          &Uu_KnowledgeBox::accept);
  connect(m_pUi->buttonBox, &QDialogButtonBox::rejected, m_pDialog,
          &QDialog::reject);
  connect(m_pUi->buttonBox->button(QDialogButtonBox::RestoreDefaults),
          &QPushButton::clicked, this,
          [this]() { this->loadTemplateDefaults(true); });
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Uu_KnowledgeBox::getPluginName() const -> QString {
  return QStringLiteral(PLUGIN_NAME);
}

auto Uu_KnowledgeBox::getPluginVersion() const -> QString {
  return QStringLiteral(PLUGIN_VERSION);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);

  if (!m_translator.load(":/" + QStringLiteral(PLUGIN_NAME).toLower() + "_" +
                         sLang + ".qm")) {
    qWarning() << "Could not load translation"
               << ":/" + QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang +
                      ".qm";
    if (!m_translator.load(QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation"
                 << m_sSharePath + "/lang/" +
                        QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang +
                        ".qm";
      return;
    }
  }

  if (qApp->installTranslator(&m_translator) || "en" == sLang) {
    m_pUi->retranslateUi(m_pDialog);
  } else {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Uu_KnowledgeBox::getCaption() const -> QString {
  return tr("Ubuntuusers.de knowledge box");
}
auto Uu_KnowledgeBox::getIcon() const -> QIcon {
  if (m_pParent->window()->palette().window().color().lightnessF() < 0.5) {
    return QIcon(QLatin1String(":/list_alt_dark.png"));
  }
  return QIcon(QLatin1String(":/list_alt.png"));
}

auto Uu_KnowledgeBox::includeMenu() const -> bool { return true; }
auto Uu_KnowledgeBox::includeToolbar() const -> bool { return true; }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::buildUi(QWidget *pParent) {
  m_pDialog = new QDialog(pParent);
  m_pUi = new Ui::Uu_KnowledgeBoxClass();
  m_pUi->setupUi(m_pDialog);
  m_pDialog->setWindowFlags(m_pDialog->windowFlags() &
                            ~Qt::WindowContextHelpButtonHint);
  m_pDialog->setModal(true);
  m_pDialog->setWindowIcon(this->getIcon());

  m_pUi->entriesTable->setColumnCount(3);
  m_pUi->entriesTable->setRowCount(0);
  m_pUi->entriesTable->setColumnWidth(0, 40);
  m_pUi->entriesTable->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::Stretch);
  m_pUi->entriesTable->setColumnWidth(2, 40);

  if (m_sListEntries.size() != m_bListEntryActive.size()) {
    qCritical() << "Error building knowledge box dialog. List sizes:"
                << m_sListEntries.size() << "!=" << m_bListEntryActive.size();
    return;
  }
  for (int nRow = 0; nRow < m_sListEntries.size(); nRow++) {
    this->createRow(m_bListEntryActive[nRow], m_sListEntries[nRow]);
  }

  m_pUi->addButton->setIcon(QIcon::fromTheme(
      QStringLiteral("list-add"), QIcon(QLatin1String(":/add.png"))));
  connect(m_pUi->addButton, &QPushButton::pressed, this,
          &Uu_KnowledgeBox::addRow);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::loadTemplateEntries() {
  // Load entries from default template or config file
  m_bListEntryActive.clear();
  m_sListEntries.clear();
  uint nNumOfEntries =
      m_pSettings->value(QStringLiteral("NumOfEntries"), 0).toUInt();

  if (0 == nNumOfEntries) {
    this->loadTemplateDefaults(false);
  } else {
    qDebug() << "Reading knowledge box entries from config file";
    QString sTmpEntry;
    for (uint i = 0; i < nNumOfEntries; i++) {
      sTmpEntry =
          m_pSettings->value("Entry_" + QString::number(i), "").toString();
      if (!sTmpEntry.isEmpty()) {
        m_sListEntries << sTmpEntry;
        m_bListEntryActive << m_pSettings
                                  ->value("Active_" + QString::number(i), false)
                                  .toBool();
      }
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::loadTemplateDefaults(bool bReset) {
  qDebug() << Q_FUNC_INFO;
  QFile fiDefault(QStringLiteral(":/uu_knowledgebox.default"));
  m_bListEntryActive.clear();
  m_sListEntries.clear();
  for (int i = m_listDelRowButtons.size() - 1; i >= 0; i--) {
    QPushButton *tmpButton;
    tmpButton = m_listDelRowButtons[i];
    delete tmpButton;
    tmpButton = nullptr;
    m_listDelRowButtons.removeAt(i);
    m_pUi->entriesTable->removeRow(i);
  }

  if (!fiDefault.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open uu_knowledgebox.default";
    QMessageBox::warning(
        nullptr, tr("Error"),
        tr("Could not open %1").arg(QStringLiteral("uu_knowledgebox.default")));
  } else {
    QTextStream in(&fiDefault);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Since Qt 6 UTF-8 is used by default
    in.setCodec("UTF-8");
#endif
    QString tmpLine;

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

  if (m_sListEntries.isEmpty()) {
    qWarning() << "Knowledgebox defaults are empty!";
    QMessageBox::warning(nullptr, tr("Error"),
                         tr("ubuntuusers.de knowledgebox defaults are empty!"));
  }

  if (bReset) {
    for (int nRow = 0; nRow < m_sListEntries.size(); nRow++) {
      this->createRow(m_bListEntryActive[nRow], m_sListEntries[nRow]);
    }
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
  for (int i = 0; i < nSize; i++) {
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
    QString sOutput = QStringLiteral("{{{#!vorlage Wissen\n");
    if (m_sListEntries.size() == m_bListEntryActive.size()) {
      for (int i = 0; i < m_sListEntries.size(); i++) {
        if (m_bListEntryActive.at(i)) {
          sOutput += m_sListEntries.at(i) + "\n";
        }
      }
    } else {
      sOutput += QLatin1String("ERROR");
      qCritical() << "Error executing knowledge box template.";
    }
    sOutput += QLatin1String("}}}\n");
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
  m_pUi->entriesTable->editItem(
      m_pUi->entriesTable->item(m_sListEntries.size() - 1, 1));
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
  m_pUi->entriesTable->item(nRow, 0)->setFlags(Qt::ItemIsUserCheckable |
                                               Qt::ItemIsEnabled);
  if (!bActive) {
    m_pUi->entriesTable->item(nRow, 0)->setCheckState(Qt::Unchecked);
  } else {
    m_pUi->entriesTable->item(nRow, 0)->setCheckState(Qt::Checked);
  }

  // Text
  m_pUi->entriesTable->item(nRow, 1)->setText(sText);

  // Delete row button
  m_listDelRowButtons << new QPushButton(
      QIcon::fromTheme(QStringLiteral("list-remove"),
                       QIcon(QLatin1String(":/remove.png"))),
      QLatin1String(""));
  m_pUi->entriesTable->setCellWidget(nRow, 2, m_listDelRowButtons.last());

  connect(m_listDelRowButtons.last(), &QPushButton::pressed, this,
          &Uu_KnowledgeBox::deleteRow);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::deleteRow() {
  QObject *pObj = sender();
  auto *pButton = reinterpret_cast<QPushButton *>(pObj);
  if (pButton != nullptr) {
    int nIndex = m_listDelRowButtons.indexOf(pButton);
    // qDebug() << "DELETE ROW:" << nIndex;
    if (nIndex >= 0 && nIndex < m_sListEntries.size()) {
      m_sListEntries.removeAt(nIndex);
      m_bListEntryActive.removeAt(nIndex);
      delete pButton;
      pButton = nullptr;
      m_listDelRowButtons.removeAt(nIndex);
      m_pUi->entriesTable->removeRow(nIndex);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::writeSettings() {
  m_pSettings->remove(QStringLiteral("General"));
  m_pSettings->setValue(QStringLiteral("NumOfEntries"), m_sListEntries.size());
  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_pSettings->setValue("Entry_" + QString::number(i), m_sListEntries[i]);
    m_pSettings->setValue("Active_" + QString::number(i),
                          m_bListEntryActive[i]);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Uu_KnowledgeBox::hasSettings() const -> bool { return true; }

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
  Q_UNUSED(listEditors)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Uu_KnowledgeBox::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  // aboutbox.setIconPixmap(QPixmap(":/knowledgebox.png"));
  aboutbox.setText(
      QString::fromLatin1("<p><b>%1</b><br />"
                          "%2</p>"
                          "<p>%3<br />"
                          "%4</p>"
                          "<p><i>%5</i></p>")
          .arg(
              this->getCaption(), tr("Version") + ": " + PLUGIN_VERSION,
              PLUGIN_COPY,
              tr("Licence") + ": " +
                  "<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">"
                  "GNU General Public License Version 3</a>",
              tr("Plugin for choosing ubuntuusers.de knowledge box entries.")));
  aboutbox.exec();
}
