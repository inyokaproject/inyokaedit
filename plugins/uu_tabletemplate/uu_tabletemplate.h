/**
 * \file uu_tabletemplate.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2012-present The InyokaEdit developers
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
 * Class definition ubuntuusers.de table template dialog
 */
// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_
#define PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_

#include <QDialog>
#include <QDir>
#include <QString>
#include <QTranslator>
#include <QtPlugin>

#ifdef USEQTWEBKIT
#include <QtWebKitWidgets/QWebView>
#endif
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

/**
 * \class Uu_TableTemplate
 * \brief Dialog for table insertion
 */
class Uu_TableTemplate : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.uutabletemplate")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir &userDataDir, const QString &sSharePath) override;
  auto getPluginName() const -> QString override;
  auto getPluginVersion() const -> QString override;
  void installTranslator(const QString &sLang) override;
  auto getCaption() const -> QString override;
  auto getIcons() const -> QPair<QIcon, QIcon> override;
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
#ifndef NOPREVIEW
  void preview();
#endif

 private:
  auto generateTable() -> QString;

  QWidget *m_pParent;
  Ui::Uu_TableTemplateClass *m_pUi;
  QDialog *m_pDialog;
  QSettings *m_pSettings;
  TextEditor *m_pEditor;
  Templates *m_pTemplates;
  Parser *m_pParser;
  QDir m_dirPreview;
  QTextDocument *m_pTextDocument;
#ifdef USEQTWEBKIT
  QWebView *m_pPreviewWebview;
#endif
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
