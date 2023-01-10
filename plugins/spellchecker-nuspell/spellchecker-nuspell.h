/**
 * \file spellchecker-nuspell.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-present The InyokaEdit developers
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
 * Class definition for spell checker using nuspell.
 *
 * \section SOURCE
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * Copyright (c) 2011, Volker Götz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell
 */
// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_SPELLCHECKER_NUSPELL_SPELLCHECKER_NUSPELL_H_
#define PLUGINS_SPELLCHECKER_NUSPELL_SPELLCHECKER_NUSPELL_H_

#include <QAction>  // Cannot use forward declaration (since Qt 6)
#include <QDir>
#include <QObject>
#include <QString>
#include <QTextCursor>
#include <QTranslator>
#include <QtPlugin>
#include <nuspell/dictionary.hxx>

#include "../../application/ieditorplugin.h"

class QSettings;

class TextEditor;
class NuspellCheckDialog;

/**
 * \class SpellChecker_Nuspell
 * \brief Spell checker using nuspell.
 */
class SpellChecker_Nuspell : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.spellchecker.nuspell")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir &userDataDir, const QString &sSharePath) override;
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

 private:
  friend class NuspellCheckDialog;

  auto initDictionaries() -> bool;
  void loadAdditionalDict(const QString &sFilename);

  auto spell(const QString &sWord) -> bool;
  auto suggest(const QString &sWord) -> QStringList;
  void ignoreWord(const QString &sWord);
  void addToUserWordlist(const QString &sWord);
  void putWord(const QString &sWord);
  void replaceAll(const int nPos, const QString &sOld, const QString &sNew);

  TextEditor *m_pEditor;
  QAction *m_pExecuteAct;
  NuspellCheckDialog *m_pCheckDialog;
  nuspell::Dictionary m_Dict;
  QSettings *m_pSettings;
  QWidget *m_pParent;
  QTextCursor m_oldCursor;
  QStringList m_sListDicts;
  QString m_sDictLang;
  QString m_sUserDict;
  QDir m_UserDataDir;
  QStringList m_UserWordsList;
  QTranslator m_translator;
  QString m_sSharePath;
  QString m_sCommunity;
};

#endif  // PLUGINS_SPELLCHECKER_NUSPELL_SPELLCHECKER_NUSPELL_H_
