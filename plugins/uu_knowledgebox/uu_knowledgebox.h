// SPDX-FileCopyrightText: 2013-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_UU_KNOWLEDGEBOX_UU_KNOWLEDGEBOX_H_
#define PLUGINS_UU_KNOWLEDGEBOX_UU_KNOWLEDGEBOX_H_

#include <QString>
#include <QTranslator>
#include <QtPlugin>

#include "../../application/ieditorplugin.h"

class QDialog;
class QPushButton;
class QSettings;

class TextEditor;

namespace Ui {
class Uu_KnowledgeBoxClass;
}

class Uu_KnowledgeBox : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.uuknowledgebox")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir &userDataDir, const QString &sSharePath,
                  const bool &bIsDarkTheme) override;
  auto getPluginName() const -> QString override;
  auto getPluginVersion() const -> QString override;
  auto getTranslator(const QString &sLang) -> QTranslator * override;
  auto getCaption() const -> QString override;
  auto getIcon() const -> QIcon override;
  auto includeMenu() const -> bool override;
  auto includeToolbar() const -> bool override;
  bool hasSettings() const override;
  void setCurrentEditor(TextEditor *pEditor) override;
  void setEditorlist(const QList<TextEditor *> &listEditors) override;

 public slots:
  void callPlugin() override;
  void executePlugin() override;
  void changeLanguage() override;
  void showSettings() override;
  void showAbout() override;

 private slots:
  void accept();
  void loadTemplateDefaults(bool bReset);
  void addRow();
  void deleteRow();

 private:
  void loadTemplateEntries();
  void buildUi(QWidget *pParent);
  void writeSettings();
  void createRow(const bool bActive, const QString &sText);

  QWidget *m_pParent;
  Ui::Uu_KnowledgeBoxClass *m_pUi;
  QTranslator m_translator;
  QString m_sSharePath;
  bool m_bIsDarkTheme;
  QDialog *m_pDialog;
  QSettings *m_pSettings;
  TextEditor *m_pEditor;
  QList<bool> m_bListEntryActive;
  QStringList m_sListEntries;
  QList<QPushButton *> m_listDelRowButtons;
  bool m_bCalledSettings;
};

#endif  // PLUGINS_UU_KNOWLEDGEBOX_UU_KNOWLEDGEBOX_H_
