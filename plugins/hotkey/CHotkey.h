/**
 * \file CHotkey.h
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
 * Class definition hotkey dialog
 */

#ifndef INYOKAEDIT_CHOTKEY_H_
#define INYOKAEDIT_CHOTKEY_H_

#include <QDialog>
#include <QKeySequenceEdit>
#include <QtPlugin>
#include <QPushButton>
#include <QShortcut>
#include <QSettings>
#include <QSignalMapper>
#include <QString>

#include "../../application/CTextEditor.h"
#include "../../application/IEditorPlugin.h"

namespace Ui {
  class CHotkeyClass;
}
class QDir;
class QTextDocument;

/**
 * \class CHotkey
 * \brief Dialog for hotkey definition
 */
class CHotkey : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.hotkey")

 public:
  void initPlugin(QWidget *pParent, CTextEditor *pEditor,
                  const QDir userDataDir, const QString sSharePath);
  QString getPluginName() const;
  QString getPluginVersion() const;
  QTranslator* getPluginTranslator(const QString &sSharePath,
                                   const QString &sLocale);
  QString getCaption() const;
  QIcon getIcon() const;
  bool includeMenu() const;
  bool includeToolbar() const;
  bool hasSettings() const;
  void setCurrentEditor(CTextEditor *pEditor);
  void setEditorlist(QList<CTextEditor *> listEditors);

 public slots:
  void callPlugin();
  void executePlugin();
  void showSettings();
  void showAbout();

 private slots:
  void accept();
  void reject();
  void addRow();
  void deleteRow(QWidget *widget);
  void insertElement(QString sId);

 private:
  void loadHotkeyEntries();
  void buildUi(QWidget *pParent);
  void registerHotkeys();
  void writeSettings();
  void createRow(QKeySequenceEdit *sequenceEdit, const QString &sText);

  QWidget *m_pParent;
  Ui::CHotkeyClass *m_pUi;
  QDialog *m_pDialog;
  QSettings *m_pSettings;
  QSettings *m_pSettingsApp;
  CTextEditor *m_pEditor;
  QString m_sSharePath;
  QList<QKeySequenceEdit *> m_listSequenceEdit;
  QStringList m_sListEntries;
  QList<QAction *> m_listActions;
  QList<QAction *> m_listActionsOld;
  QSignalMapper *m_pSigMapHotkey;
  QSignalMapper *m_pSigMapDeleteRow;
  QList<QPushButton *> m_listDelRowButtons;
};

#endif  // INYOKAEDIT_CHOTKEY_H_
