/**
 * \file hotkey.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2015-2018 The InyokaEdit developers
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
 * Shows a modal window for hotkey entry selection.
 */

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>

#include "./hotkey.h"
#include "ui_hotkey.h"

void Hotkey::initPlugin(QWidget *pParent, TextEditor *pEditor,
                        const QDir userDataDir,
                        const QString sSharePath) {
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
  m_pParent = pParent;
  m_pEditor = pEditor;
  m_sSharePath = sSharePath;
  m_listActions.clear();

  this->loadHotkeyEntries();
  this->buildUi(m_pParent);  // After loading hotkey entries

  connect(m_pUi->buttonBox, SIGNAL(accepted()),
          this, SLOT(accept()));
  connect(m_pDialog, SIGNAL(rejected()),
          this, SLOT(reject()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Hotkey::getPluginName() const {
  return PLUGIN_NAME;
}

QString Hotkey::getPluginVersion() const {
  return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::installTranslator(QApplication *pApp, const QString &sLang) {
  pApp->removeTranslator(&m_translator);
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

  if (pApp->installTranslator(&m_translator)) {
    m_pUi->retranslateUi(m_pDialog);
  } else {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Hotkey::getCaption() const {
  return trUtf8("Hotkey selector");
}
QIcon Hotkey::getIcon() const {
  return QIcon(":/preferences-desktop-keyboard-shortcuts.png");
}

bool Hotkey::includeMenu() const {
  return true;
}
bool Hotkey::includeToolbar() const {
  return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::buildUi(QWidget *pParent) {
  m_pDialog = new QDialog(pParent);
  m_pUi = new Ui::HotkeyClass();
  m_pUi->setupUi(m_pDialog);
  m_pDialog->setWindowFlags(m_pDialog->windowFlags()
                            & ~Qt::WindowContextHelpButtonHint);
  m_pDialog->setModal(true);

  m_pSigMapHotkey = new QSignalMapper(this);
  m_pSigMapDeleteRow = new QSignalMapper(this);

  m_pUi->entriesTable->setColumnCount(3);
  m_pUi->entriesTable->setRowCount(0);

  m_pUi->entriesTable->setColumnWidth(0, 100);
  m_pUi->entriesTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
  m_pUi->entriesTable->setColumnWidth(2, 40);
  m_pUi->entriesTable->viewport()->installEventFilter(this);

  if (m_sListEntries.size() != m_listSequenceEdit.size()) {
    qCritical() << "Error building hotkey dialog. List sizes:"
                << m_sListEntries.size() << "!="
                << m_listSequenceEdit.size();
    return;
  }
  for (int nRow = 0; nRow < m_sListEntries.size(); nRow++) {
    this->createRow(m_listSequenceEdit[nRow], m_sListEntries[nRow]);
  }

  connect(m_pSigMapHotkey, SIGNAL(mapped(QString)),
          this, SLOT(insertElement(QString)));
  connect(m_pSigMapDeleteRow, SIGNAL(mapped(QWidget*)),
          this, SLOT(deleteRow(QWidget*)));
  m_pUi->addButton->setIcon(QIcon::fromTheme("list-add",
                                             QIcon(":/list-add.png")));
  connect(m_pUi->addButton, SIGNAL(pressed()),
          this, SLOT(addRow()));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::loadHotkeyEntries() {
  // Load entries from default template or config file
  m_listSequenceEdit.clear();
  m_sListEntries.clear();
  uint nNumOfEntries = m_pSettings->value("Hotkey/NumOfEntries",
                                          0).toUInt();
  if (0 == nNumOfEntries) {
    m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL + Qt::Key_B);
    m_sListEntries << "'''Bold'''";
    m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL + Qt::Key_I);
    m_sListEntries << "''Italic''";
    m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL + Qt::Key_L);
    m_sListEntries << "Text %%Selected%%";
    this->writeSettings();
  } else {
    qDebug() << "Reading hotkey entries from config file";
    QString sTmpEntry("");
    m_pSettings->beginGroup("Hotkey");
    for (uint i = 0; i < nNumOfEntries; i++) {
      sTmpEntry = m_pSettings->value(
                    "Entry_" + QString::number(i), "").toString();
      if (!sTmpEntry.isEmpty()) {
        m_sListEntries << sTmpEntry;
        m_listSequenceEdit << new QKeySequenceEdit(
                                m_pSettings->value("Key_" + QString::number(i),
                                                   "").toString());
      }
    }
    m_pSettings->endGroup();
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::callPlugin() {
  qDebug() << Q_FUNC_INFO;
  m_listActionsOld = m_listActions;
  this->showSettings();
}

void Hotkey::executePlugin() {
  this->registerHotkeys();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::accept() {
  int nSize = m_sListEntries.size();
  m_sListEntries.clear();
  for (int i = 0; i < nSize; i ++) {
    m_sListEntries << m_pUi->entriesTable->item(i, 1)->text();
  }

  this->writeSettings();
  this->registerHotkeys();
  m_pDialog->done(QDialog::Accepted);
}

void Hotkey::reject() {
  this->accept();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::addRow() {
  m_sListEntries << trUtf8("'''Bold'''");
  m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL + Qt::Key_B);
  this->createRow(m_listSequenceEdit.last(), m_sListEntries.last());
  m_pUi->entriesTable->scrollToBottom();
  m_pUi->entriesTable->editItem(m_pUi->entriesTable->item(
                                  m_sListEntries.size() - 1, 1));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::createRow(QKeySequenceEdit *sequenceEdit, const QString &sText) {
  int nRow = m_pUi->entriesTable->rowCount();  // Before setRowCount!
  m_pUi->entriesTable->setRowCount(m_pUi->entriesTable->rowCount() + 1);

  for (int nCol = 0; nCol < m_pUi->entriesTable->columnCount(); nCol++) {
    m_pUi->entriesTable->setItem(nRow, nCol, new QTableWidgetItem());
  }

  // Key sequence
  m_pUi->entriesTable->setCellWidget(nRow, 0, sequenceEdit);

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

void Hotkey::deleteRow(QWidget *widget) {
  QPushButton *button = reinterpret_cast<QPushButton*>(widget);
  if (button != NULL) {
    int nIndex = m_listDelRowButtons.indexOf(button);
    // qDebug() << "DELETE ROW:" << nIndex;
    if (nIndex >= 0 && nIndex < m_sListEntries.size()) {
      m_sListEntries.removeAt(nIndex);
      delete button;
      button = NULL;
      m_listDelRowButtons.removeAt(nIndex);
      delete m_listSequenceEdit[nIndex];
      m_listSequenceEdit.removeAt(nIndex);
      m_pUi->entriesTable->removeRow(nIndex);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::registerHotkeys() {
  qDebug() << Q_FUNC_INFO;
  foreach (QAction* act, m_listActionsOld) {
    m_pParent->removeAction(act);
  }

  m_listActions.clear();
  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_listActions << new QAction(QString::number(i), m_pParent);
    m_listActions.last()->setShortcut(m_listSequenceEdit[i]->keySequence());
    m_pSigMapHotkey->setMapping(m_listActions.last(), QString::number(i));
    connect(m_listActions.last(), SIGNAL(triggered()),
            m_pSigMapHotkey, SLOT(map()));
  }

  m_pParent->addActions(m_listActions);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::insertElement(QString sId) {
  QString sText(m_sListEntries[sId.toInt()]);
  sText.replace("\\n", "\n");
  int nPlaceholder1(sText.indexOf("%%"));
  int nPlaceholder2(sText.lastIndexOf("%%"));
  int nCurrentPos = m_pEditor->textCursor().position();

  sText.remove("%%");  // Remove placeholder
  m_pEditor->insertPlainText(sText);

  // Select placeholder
  if ((nPlaceholder1 != nPlaceholder2)
      && nPlaceholder1 >= 0
      && nPlaceholder2 >= 0) {
    QTextCursor textCursor(m_pEditor->textCursor());
    textCursor.setPosition(nCurrentPos + nPlaceholder1);
    textCursor.setPosition(nCurrentPos + nPlaceholder2 - 2,
                           QTextCursor::KeepAnchor);
    m_pEditor->setTextCursor(textCursor);
  }
  m_pEditor->setFocus();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::writeSettings() {
  m_pSettings->remove("Hotkey");
  m_pSettings->beginGroup("Hotkey");
  m_pSettings->setValue("NumOfEntries", m_sListEntries.size());
  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_pSettings->setValue("Entry_" + QString::number(i),
                          m_sListEntries[i]);
    m_pSettings->setValue("Key_" + QString::number(i),
                          m_listSequenceEdit[i]->keySequence().toString());
  }
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool Hotkey::hasSettings() const {
  return true;
}

void Hotkey::showSettings() {
  m_pDialog->show();
  m_pDialog->exec();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void Hotkey::setEditorlist(QList<TextEditor *> listEditors) {
  Q_UNUSED(listEditors);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::showAbout() {
  QMessageBox aboutbox(NULL);
  aboutbox.setWindowTitle(trUtf8("Info"));
  aboutbox.setIconPixmap(
        QPixmap(":/preferences-desktop-keyboard-shortcuts.png"));
  aboutbox.setText(QString("<p><b>%1</b><br />"
                           "%2</p>"
                           "<p>%3<br />"
                           "%4</p>"
                           "<p><i>%5</i></p>")
                   .arg(this->getCaption())
                   .arg(trUtf8("Version") + ": " + PLUGIN_VERSION)
                   .arg(PLUGIN_COPY)
                   .arg(trUtf8("Licence") + ": " +
                        "<a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a>")
                   .arg(trUtf8("Plugin for defining custom hotkeys.")));
  aboutbox.exec();
}
