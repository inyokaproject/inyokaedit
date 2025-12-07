// SPDX-FileCopyrightText: 2014-2025 The InyokaEdit developers
// SPDX-FileCopyrightText: 2011 Volker Götz
// SPDX-License-Identifier: GPL-3.0-or-later AND BSD-2-Clause

// Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell

// clazy:excludeall=ctor-missing-parent-argument

#ifndef PLUGINS_SPELLCHECKER_HUNSPELL_SPELLCHECKER_HUNSPELL_H_
#define PLUGINS_SPELLCHECKER_HUNSPELL_SPELLCHECKER_HUNSPELL_H_

#include <QAction>  // Cannot use forward declaration (since Qt 6)
#include <QDir>
#include <QObject>
#include <QString>
#include <QStringDecoder>
#include <QStringEncoder>
#include <QTextCursor>
#include <QTranslator>
#include <QtPlugin>
#include <hunspell/hunspell.hxx>

#include "../../application/ieditorplugin.h"

class QSettings;

class TextEditor;
class HunspellCheckDialog;

class SpellChecker_Hunspell : public QObject, IEditorPlugin {
  Q_OBJECT
  Q_INTERFACES(IEditorPlugin)
  Q_PLUGIN_METADATA(IID "InyokaEdit.spellchecker.hunspell")

 public:
  void initPlugin(QWidget *pParent, TextEditor *pEditor,
                  const QDir &userDataDir, const QString &sSharePath,
                  const bool &bIsDarkTheme) override;
  auto getPluginName() const -> QString override;
  auto getPluginVersion() const -> QString override;
  auto getTranslator(const QString &sLang) -> QTranslator * override;
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
  void changeLanguage() override;
  void showSettings() override;
  void showAbout() override;

 private:
  friend class HunspellCheckDialog;

  void setDictPath();
  auto initDictionaries() -> bool;
  void loadAdditionalDict(const QString &sFilename);

  auto spell(const QString &sWord) -> bool;
  auto suggest(const QString &sWord) -> QStringList;
  void ignoreWord(const QString &sWord);
  void addToUserWordlist(const QString &sWord);
  void putWord(const QString &sWord);
  void replaceAll(const int nPos, const QString &sOld, const QString &sNew);

  Hunspell *m_pHunspell;
  TextEditor *m_pEditor;
  QAction *m_pExecuteAct;
  HunspellCheckDialog *m_pCheckDialog;
  QSettings *m_pSettings;
  QWidget *m_pParent;
  QTextCursor m_oldCursor;
  QString m_sDictPath;
  QStringList m_sListDicts;
  QString m_sDictLang;
  QString m_sUserDict;
  QDir m_UserDataDir;
  bool m_bIsDarkTheme;
  QTranslator m_translator;
  QString m_sSharePath;
  QString m_sCommunity;
  QString m_sEncoding;
  QStringDecoder m_Decoder;
  QStringEncoder m_Encoder;
};

#endif  // PLUGINS_SPELLCHECKER_HUNSPELL_SPELLCHECKER_HUNSPELL_H_
