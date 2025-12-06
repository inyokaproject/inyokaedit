// SPDX-FileCopyrightText: 2012-2025 The InyokaEdit developers
// SPDX-License-Identifier: GPL-3.0-or-later

// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_
#define PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_

#include <QDialog>
#include <QDir>
#include <QString>
#include <QTranslator>
#include <QtPlugin>

#ifdef USEQTWEBENGINE
#include <QMenu>  // Needed to compile with Qt 6.2 ?!
#include <QWebEngineView>
#endif

#include "../../application/ieditorplugin.h"

class QSettings;
class QTextDocument;

class Parser;
class Templates;
class TextEditor;

namespace Ui {
class Uu_TableTemplateClass;
}

class Uu_TableTemplate : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.uutabletemplate")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir &userDataDir, const QString &sSharePath,
                  const bool &bIsDarkTheme) override;
  auto getPluginName() const -> QString override;
  auto getPluginVersion() const -> QString override;
  void installTranslator(const QString &sLang) override;
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
  void showSettings() override;
  void showAbout() override;

 private slots:
  void convertToBaseTemplate();
  void convertToNewTemplate();
  void accept();
#ifdef USEQTWEBENGINE
  void preview();
#endif

 private:
  auto generateTable() -> QString;

  QWidget *m_pParent;
  Ui::Uu_TableTemplateClass *m_pUi;
  QDialog *m_pDialog;
  bool m_bIsDarkTheme;
  QSettings *m_pSettings;
  TextEditor *m_pEditor;
  Templates *m_pTemplates;
  Parser *m_pParser;
  QDir m_dirPreview;
  QTextDocument *m_pTextDocument;
#ifdef USEQTWEBENGINE
  QWebEngineView *m_pPreviewWebview;
#endif

  QTranslator m_translator;
  QString m_sSharePath;
  QStringList m_sListTableClasses;
  QStringList m_sListTableStyles;
  QStringList m_sListTableStylesPrefix;
  QString m_sRowClassTitle;
  QString m_sRowClassHead;
  QString m_sRowClassHighlight;
  bool m_bBaseToNew;
};

#endif  // PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_
