/**
 * \file CHighlighter.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2015 The InyokaEdit developers
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
 * Class definition for syntax highlighting.
 */

#ifndef INYOKAEDIT_CHIGHLIGHTER_H_
#define INYOKAEDIT_CHIGHLIGHTER_H_

#include <QHash>
#include <QtPlugin>
#include <QSettings>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include "CSyntaxHighlighter.h"
#include "../../application/templates/CTemplates.h"
#include "../../application/CTextEditor.h"
#include "../../application/IEditorPlugin.h"

namespace Ui {
    class CHighlighterDialog;
}

class QSettings;

/**
 * \class CHighlighter
 * \brief Syntax highlighting
 */
class CHighlighter : public QObject, IEditorPlugin {
    Q_OBJECT
    Q_INTERFACES(IEditorPlugin)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "InyokaEdit.highlighter")
#endif

  public:
    void initPlugin(QWidget *pParent, CTextEditor *pEditor,
                    const QDir userDataDir,
                    const QString sSharePath);
    QString getPluginName() const;
    QString getPluginVersion() const;
    QTranslator* getPluginTranslator(const QString &sSharePath,
                                     const QString &sLocale);
    QString getCaption() const;
    QIcon getIcon() const;
    bool includeMenu() const;
    bool includeToolbar() const;
    bool hasSettings() const;

  public slots:
    void callPlugin();
    void executePlugin();
    void showSettings();
    void showAbout();

  private slots:
    void changedStyle(int nIndex);
    void clickedStyleCell(int nRow, int nCol);
    void accept();

  private:
    void copyDefaultStyles();
    void buildUi(QWidget *pParent);
    void loadHighlighting(const QString &sStyleFile);
    void readValue(const quint16 nRow,
                   const QTextCharFormat &charFormat);
    void saveHighlighting();
    QString createValues(const quint16 nRow);
    void saveStyle();
    void readStyle(const QString &sStyle);
    void getTranslations();
    void defineRules();
    void writeFormat(const QString &sKey, const QTextCharFormat &charFormat);
    void evalKey(const QString &sKey, QTextCharFormat &charFormat);

    Ui::CHighlighterDialog *m_pUi;
    QDialog *m_pDialog;
    QSettings *m_pSettings;
    CSyntaxHighlighter *m_pHighlighter;
    CTextEditor *m_pEditor;
    CTemplates *m_pTemplates;

    QString m_sTplLang;
    QString m_sSharePath;
    QString m_sStyleFile;
    QString m_sExt;

    QSettings *m_pStyleSet;
    QStringList m_sListMacroKeywords;
    QStringList m_sListParserKeywords;

    QVector<HighlightingRule> m_highlightingRules;
    QTextCharFormat m_headingsFormat;
    QTextCharFormat m_interwikiLinksFormat;
    QTextCharFormat m_linksFormat;
    QTextCharFormat m_tablecellsFormat;
    QTextCharFormat m_newTableLineFormat;
    QTextCharFormat m_macrosFormat;
    QTextCharFormat m_parserFormat;
    QTextCharFormat m_textformatFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_imgMapFormat;
    QTextCharFormat m_listFormat;
    QTextCharFormat m_miscFormat;
    bool m_bSystemForeground;
    bool m_bSystemBackground;
    QColor m_colorForeground;
    QColor m_colorBackground;
};

#endif  // INYOKAEDIT_CHIGHLIGHTER_H_
