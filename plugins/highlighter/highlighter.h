/**
 * \file highlighter.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2021 The InyokaEdit developers
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
 * Class definition for syntax highlighting.
 */
// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_HIGHLIGHTER_HIGHLIGHTER_H_
#define PLUGINS_HIGHLIGHTER_HIGHLIGHTER_H_

#include <QtPlugin>
#include <QTextCharFormat>
#include <QTranslator>
#include <QVector>

#include "./syntaxhighlighter.h"
#include "../../application/ieditorplugin.h"

class QSettings;

class Templates;
class TextEditor;

namespace Ui {
class HighlighterDialog;
}

/**
 * \class Highlighter
 * \brief Syntax highlighting
 */
class Highlighter : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.highlighter")

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
    auto hasSettings() const -> bool override;
    void setCurrentEditor(TextEditor *pEditor) override;
    void setEditorlist(const QList<TextEditor *> &listEditors) override;

 public slots:
    void callPlugin() override;
    void executePlugin() override;
    void showSettings() override;
    void showAbout() override;

 private slots:
    void changedStyle(int nIndex);
    void clickedStyleCell(int nRow, int nCol);
    void accept();

 private:
    void copyDefaultStyles();
    void buildUi(QWidget *pParent);
    void loadHighlighting(const QString &sStyleFile);
    void readValue(const quint16 nRow, const QTextCharFormat &charFormat);
    void saveHighlighting();
    auto createValues(const quint16 nRow) -> QString;
    void saveStyle();
    void readStyle(const QString &sStyle);
    void getTranslations();
    void defineRules();
    void writeFormat(const QString &sKey, const QTextCharFormat &charFormat);
    static auto evalKey(const QString &sKey) -> QTextCharFormat;
    void rehighlightAll();

    Ui::HighlighterDialog *m_pUi;
    QTranslator m_translator;
    QString m_sSharePath;
    QDialog *m_pDialog;
    QSettings *m_pSettings;
    QList<SyntaxHighlighter *> m_ListHighlighters;
    QList<TextEditor *> m_listEditors;
    Templates *m_pTemplates;

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
    QTextCharFormat m_syntaxErrorFormat;
    bool m_bSystemForeground;
    bool m_bSystemBackground;
    QColor m_colorForeground;
    QColor m_colorBackground;
    static const QString sSEPARATOR;
};

#endif  // PLUGINS_HIGHLIGHTER_HIGHLIGHTER_H_
