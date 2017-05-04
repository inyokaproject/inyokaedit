/**
 * \file CSpellChecker.h
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
 * Class definition for spell checker using hunspell.
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#ifndef INYOKAEDIT_CSPELLCHECKER_H_
#define INYOKAEDIT_CSPELLCHECKER_H_

#include <QObject>
#include <QTranslator>
#include <QtPlugin>
#include <QSettings>
#include <QString>
#include <QTextCursor>

#include "../../application/CTextEditor.h"
#include "../../application/IEditorPlugin.h"
#include "./CSpellCheckDialog.h"

#if defined _WIN32
#include "../windows_files/hunspell-mingw/include/hunspell.hxx"
#else
#include <hunspell/hunspell.hxx>
#endif

/**
 * \class CSpellChecker
 * \brief Spell checker using hunspell.
 */
class CSpellChecker : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.spellchecker")

 public:
  ~CSpellChecker();

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

 private:
  friend class CSpellCheckDialog;

  void setDictPath();
  bool initDictionaries();
  void loadAdditionalDict(const QString &sFilename);

  bool spell(const QString &sWord);
  QStringList suggest(const QString &sWord);
  void ignoreWord(const QString &sWord);
  void addToUserWordlist(const QString &sWord);
  void putWord(const QString &sWord);

  Hunspell *m_pHunspell;
  CTextEditor *m_pEditor;
  QAction *m_pExecuteAct;
  CSpellCheckDialog *m_pCheckDialog;
  QSettings *m_pSettings;
  QTextCursor m_oldCursor;
  QString m_sDictPath;
  QStringList m_sListDicts;
  QString m_sDictLang;
  QString m_sUserDict;
  QDir m_UserDataDir;
  QString m_sSharePath;
  QString m_sEncoding;
  QTextCodec *m_pCodec;
};

#endif  // INYOKAEDIT_CSPELLCHECKER_H_
