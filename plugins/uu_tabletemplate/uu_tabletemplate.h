/**
 * \file uu_tabletemplate.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2012-2018 The InyokaEdit developers
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
 * Class definition ubuntuusers.de table template dialog
 */

#ifndef PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_
#define PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_

#include <QDialog>
#include <QDir>
#include <QtPlugin>
#include <QSettings>
#include <QString>

#ifdef USEQTWEBKIT
#include <QtWebKitWidgets/QWebView>
#else
#include <QWebEngineView>
#endif

#include "../../application/parser/parser.h"
#include "../../application/templates/templates.h"
#include "../../application/texteditor.h"
#include "../../application/ieditorplugin.h"

namespace Ui {
  class Uu_TableTemplateClass;
}
class QDir;
class QTextDocument;

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
                    const QDir userDataDir, const QString sSharePath);
    QString getPluginName() const;
    QString getPluginVersion() const;
    void installTranslator(QApplication *pApp, const QString &sLang);
    QString getCaption() const;
    QIcon getIcon() const;
    bool includeMenu() const;
    bool includeToolbar() const;
    bool hasSettings() const;
    void setCurrentEditor(TextEditor *pEditor);
    void setEditorlist(QList<TextEditor *> listEditors);

  public slots:
    void callPlugin();
    void executePlugin();
    void showSettings();
    void showAbout();

  private slots:
    /** \brief Show preview */
    void preview();

    /** \brief Convert base template to new table template */
    void convertToBaseTemplate();

    /** \brief Convert new table template to base template */
    void convertToNewTemplate();

    /** \brief Dialog finished */
    void accept();

  private:
    /**
    * \brief Generate specific table
    * \return String including the generated table code
    */
    QString generateTable();

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
#else
    QWebEngineView *m_pPreviewWebview;
#endif

    QTranslator m_translator;
    QString m_sSharePath;
    QStringList m_sListTableStyles;
    QStringList m_sListTableStylesPrefix;
    QString m_sRowClassTitle;
    QString m_sRowClassHead;
    QString m_sRowClassHighlight;
    bool m_bBaseToNew;
};

#endif  // PLUGINS_UU_TABLETEMPLATE_UU_TABLETEMPLATE_H_
