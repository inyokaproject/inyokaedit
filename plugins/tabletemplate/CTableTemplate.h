/**
 * \file CTableTemplate.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2014 The InyokaEdit developers
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
 * Class definition table insert dialog
 */

#ifndef INYOKAEDIT_CTABLETEMPLATE_H_
#define INYOKAEDIT_CTABLETEMPLATE_H_

#include <QDialog>
#include <QDir>
#include <QtPlugin>
#include <QSettings>
#include <QString>

#include "../../libs/templates/CTemplates.h"
#include "../../libs/parser/CParser.h"
#include "../../application/CTextEditor.h"
#include "../../application/IEditorPlugin.h"

namespace Ui {
    class CTableTemplateClass;
}

class QDir;
class QTextDocument;

/**
 * \class CTableTemplate
 * \brief Dialog for table insertion
 */
class CTableTemplate : public QObject, IEditorPlugin {
    Q_OBJECT
    Q_INTERFACES(IEditorPlugin)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "InyokaEdit.tabletemplate")
#endif

  public:
    void initPlugin(QWidget *pParent, CTextEditor *pEditor,
                    const QDir userDataDir, bool bDebug);
    QString getPluginName() const;
    QString getPluginVersion() const;
    QTranslator* getPluginTranslator(const QString &sLocale);
    QString getCaption() const;
    QIcon getIcon() const;
    bool hasSettings() const;

  public slots:
    void executePlugin();
    void showSettings();
    void showAbout();

  private slots:
    /** \brief Show preview */
    void preview();

    /** \brief Dialog finished */
    void accept();

  private:
    /**
    * \brief Generate specific table
    * \return String including th generated table code
    */
    QString generateTable();

    Ui::CTableTemplateClass *m_pUi;
    QDialog *m_pDialog;
    QSettings *m_pSettings;
    CTextEditor *m_pEditor;
    CTemplates *m_pTemplates;
    CParser *m_pParser;
    QDir m_dirPreview;
    QTextDocument *m_pTextDocument;

    QStringList m_sListTableStyles;
    QStringList m_sListTableStylesPrefix;
    QString m_sRowClassTitle;
    QString m_sRowClassHead;
    QString m_sRowClassHighlight;
};

#endif  // INYOKAEDIT_CTABLETEMPLATE_H_
