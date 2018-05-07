/**
 * \file spellchecker.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2018 The InyokaEdit developers
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
 * Class definition for spell checker using hunspell.
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#ifndef PLUGINS_SPELLCHECKER_SPELLCHECKER_H_
#define PLUGINS_SPELLCHECKER_SPELLCHECKER_H_

#include <QObject>
#include <QTranslator>
#include <QtPlugin>
#include <QSettings>
#include <QString>
#include <QTextCursor>

#include "../../application/texteditor.h"
#include "../../application/ieditorplugin.h"
#include "./spellcheckdialog.h"

#if defined _WIN32
#include "../windows_files/hunspell-mingw/include/hunspell.hxx"
#else
#include <hunspell/hunspell.hxx>
#endif

/**
 * \class SpellChecker
 * \brief Spell checker using hunspell.
 */
class SpellChecker : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.spellchecker")

  public:
    ~SpellChecker();

    void initPlugin(QWidget *pParent, TextEditor *pEditor,
                    const QDir &userDataDir, const QString &sSharePath);
    QString getPluginName() const;
    QString getPluginVersion() const;
    void installTranslator(const QString &sLang);
    QString getCaption() const;
    QIcon getIcon() const;
    bool includeMenu() const;
    bool includeToolbar() const;
    bool hasSettings() const;
    void setCurrentEditor(TextEditor *pEditor);
    void setEditorlist(const QList<TextEditor *> &listEditors);

  public slots:
    void callPlugin();
    void executePlugin();
    void showSettings();
    void showAbout();

  private:
    friend class SpellCheckDialog;

    void setDictPath();
    bool initDictionaries();
    void loadAdditionalDict(const QString &sFilename);

    bool spell(const QString &sWord);
    QStringList suggest(const QString &sWord);
    void ignoreWord(const QString &sWord);
    void addToUserWordlist(const QString &sWord);
    void putWord(const QString &sWord);

    Hunspell *m_pHunspell;
    TextEditor *m_pEditor;
    QAction *m_pExecuteAct;
    SpellCheckDialog *m_pCheckDialog;
    QSettings *m_pSettings;
    QTextCursor m_oldCursor;
    QString m_sDictPath;
    QStringList m_sListDicts;
    QString m_sDictLang;
    QString m_sUserDict;
    QDir m_UserDataDir;
    QTranslator m_translator;
    QString m_sSharePath;
    QString m_sCommunity;
    QString m_sEncoding;
    QTextCodec *m_pCodec;
};

#endif  // PLUGINS_SPELLCHECKER_SPELLCHECKER_H_
