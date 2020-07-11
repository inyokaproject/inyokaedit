/**
 * \file uu_knowledgebox.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2013-2020 The InyokaEdit developers
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
 * Class definition ubuntuusers.de knowledge box dialog
 */
// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_UU_KNOWLEDGEBOX_UU_KNOWLEDGEBOX_H_
#define PLUGINS_UU_KNOWLEDGEBOX_UU_KNOWLEDGEBOX_H_

#include <QtPlugin>
#include <QString>
#include <QTranslator>

#include "../../application/ieditorplugin.h"

class QDialog;
class QPushButton;
class QSettings;

class TextEditor;

namespace Ui {
class Uu_KnowledgeBoxClass;
}

/**
 * \class Uu_KnowledgeBox
 * \brief Dialog for table insertion
 */
class Uu_KnowledgeBox : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.uuknowledgebox")

 public:
    void initPlugin(QWidget *pParent, TextEditor *pEditor,
                    const QDir &userDataDir,
                    const QString &sSharePath) override;
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
    void accept();
    void addRow();
    void deleteRow();

 private:
    void loadTemplateDefaults();
    void loadTemplateEntries();
    void buildUi(QWidget *pParent);
    void writeSettings();
    void createRow(const bool bActive, const QString &sText);

    QWidget *m_pParent;
    Ui::Uu_KnowledgeBoxClass *m_pUi;
    QTranslator m_translator;
    QString m_sSharePath;
    QDialog *m_pDialog;
    QSettings *m_pSettings;
    QString m_sExt;
    TextEditor *m_pEditor;
    QList<bool> m_bListEntryActive;
    QStringList m_sListEntries;
    QList<QPushButton *> m_listDelRowButtons;
    bool m_bCalledSettings;
};

#endif  // PLUGINS_UU_KNOWLEDGEBOX_UU_KNOWLEDGEBOX_H_
