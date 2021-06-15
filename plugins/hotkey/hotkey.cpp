/**
 * \file hotkey.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2015-2021 The InyokaEdit developers
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
 * Shows a modal window for hotkey entry selection.
 */

#include "./hotkey.h"

#include <QApplication>
#include <QDebug>
#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QSettings>

#include "../../application/texteditor.h"

#include "ui_hotkey.h"

void Hotkey::initPlugin(QWidget *pParent, TextEditor *pEditor,
                        const QDir &userDataDir,
                        const QString &sSharePath) {
  Q_UNUSED(userDataDir)
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat,
                              QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              QStringLiteral(PLUGIN_NAME));
#else
  m_pSettings = new QSettings(QSettings::IniFormat,
                              QSettings::UserScope,
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
  m_listActions.clear();

  this->loadHotkeyEntries();
  this->buildUi(m_pParent);  // After loading hotkey entries

  connect(m_pUi->buttonBox, &QDialogButtonBox::accepted, this, &Hotkey::accept);
  connect(m_pDialog, &QDialog::rejected, this, &Hotkey::reject);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Hotkey::getPluginName() const -> QString {
  return QStringLiteral(PLUGIN_NAME);
}

auto Hotkey::getPluginVersion() const -> QString {
  return QStringLiteral(PLUGIN_VERSION);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);

  if (!m_translator.load(":/" + QStringLiteral(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QStringLiteral(PLUGIN_NAME).toLower() +
                  "_" + sLang + ".qm";
    if (!m_translator.load(QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" +
                    QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
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

auto Hotkey::getCaption() const -> QString {
  return tr("Hotkey selector");
}
auto Hotkey::getIcon() const -> QIcon {
  if (m_pParent->window()->palette().window().color().lightnessF() < 0.5) {
    return QIcon(QLatin1String(":/hotkey_dark.png"));
  }
  return QIcon(QLatin1String(":/hotkey.png"));
}

auto Hotkey::includeMenu() const -> bool {
  return true;
}
auto Hotkey::includeToolbar() const -> bool {
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
  m_pDialog->setWindowIcon(this->getIcon());

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
    this->createRow(m_listSequenceEdit.at(nRow), m_sListEntries.at(nRow));
  }

  m_pUi->addButton->setIcon(
        QIcon::fromTheme(QStringLiteral("list-add"),
                         QIcon(QLatin1String(":/add.png"))));
  connect(m_pUi->addButton, &QPushButton::pressed, this, &Hotkey::addRow);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::loadHotkeyEntries() {
  // Load entries from default template or config file
  m_listSequenceEdit.clear();
  m_sListEntries.clear();
  uint nNumOfEntries = m_pSettings->value(QStringLiteral("Hotkey/NumOfEntries"),
                                          0).toUInt();
  if (0 == nNumOfEntries) {
    m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL | Qt::Key_B);
    m_sListEntries << QStringLiteral("'''Bold'''");
    m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL | Qt::Key_I);
    m_sListEntries << QStringLiteral("''Italic''");
    m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL | Qt::Key_L);
    m_sListEntries << QStringLiteral("Text %%Selected%%");
    this->writeSettings();
  } else {
    qDebug() << "Reading hotkey entries from config file";
    QString sTmpEntry;
    m_pSettings->beginGroup(QStringLiteral("Hotkey"));
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
  m_sListEntries << tr("'''Bold'''");
  m_listSequenceEdit << new QKeySequenceEdit(Qt::CTRL | Qt::Key_B);
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
                           QIcon::fromTheme(
                             QStringLiteral("list-remove"),
                             QIcon(QLatin1String(":/remove.png"))),
                           QLatin1String(""));
  m_pUi->entriesTable->setCellWidget(nRow, 2, m_listDelRowButtons.last());

  connect(m_listDelRowButtons.last(), &QPushButton::pressed,
          this, &Hotkey::deleteRow);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::deleteRow() {
  QObject* pObj = sender();
  auto *pButton = reinterpret_cast<QPushButton*>(pObj);
  if (pButton != nullptr) {
    int nIndex = m_listDelRowButtons.indexOf(pButton);
    // qDebug() << "DELETE ROW:" << nIndex;
    if (nIndex >= 0 && nIndex < m_sListEntries.size()) {
      m_sListEntries.removeAt(nIndex);
      delete pButton;
      pButton = nullptr;
      m_listDelRowButtons.removeAt(nIndex);
      delete m_listSequenceEdit.at(nIndex);
      m_listSequenceEdit.removeAt(nIndex);
      m_pUi->entriesTable->removeRow(nIndex);
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::registerHotkeys() {
  qDebug() << Q_FUNC_INFO;
  for (const auto act : qAsConst(m_listActionsOld)) {
    m_pParent->removeAction(act);
  }

  m_listActions.clear();
  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_listActions << new QAction(QString::number(i), m_pParent);
    m_listActions.last()->setShortcut(m_listSequenceEdit.at(i)->keySequence());
    connect(m_listActions.last(), &QAction::triggered,
            this, [this, i]() { insertElement(QString::number(i)); });
  }

  m_pParent->addActions(m_listActions);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::insertElement(const QString &sId) {
  QString sText(m_sListEntries[sId.toInt()]);
  sText.replace(QLatin1String("\\n"), QLatin1String("\n"));
  int nPlaceholder1(sText.indexOf(QLatin1String("%%")));
  int nPlaceholder2(sText.lastIndexOf(QLatin1String("%%")));
  int nCurrentPos = m_pEditor->textCursor().position();

  sText.remove(QStringLiteral("%%"));  // Remove placeholder
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
  m_pSettings->remove(QStringLiteral("Hotkey"));
  m_pSettings->beginGroup(QStringLiteral("Hotkey"));
  m_pSettings->setValue(QStringLiteral("NumOfEntries"), m_sListEntries.size());
  for (int i = 0; i < m_sListEntries.size(); i++) {
    m_pSettings->setValue("Entry_" + QString::number(i),
                          m_sListEntries[i]);
    m_pSettings->setValue("Key_" + QString::number(i),
                          m_listSequenceEdit.at(i)->keySequence().toString());
  }
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Hotkey::hasSettings() const -> bool {
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

void Hotkey::setEditorlist(const QList<TextEditor *> &listEditors) {
  Q_UNUSED(listEditors)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Hotkey::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  aboutbox.setIconPixmap(
        QPixmap(
          QStringLiteral(":/preferences-desktop-keyboard-shortcuts.png")));
  aboutbox.setText(QString::fromLatin1("<p><b>%1</b><br />"
                                       "%2</p>"
                                       "<p>%3<br />"
                                       "%4</p>"
                                       "<p><i>%5</i></p>")
                   .arg(this->getCaption(),
                        tr("Version") + ": " + PLUGIN_VERSION,
                        PLUGIN_COPY,
                        tr("Licence") + ": " +
                        "<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a>",
                        tr("Plugin for defining custom hotkeys.")));
  aboutbox.exec();
}
