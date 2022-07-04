/**
 * \file spellchecker-nuspell.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2022 The InyokaEdit developers
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
 * Spell checker using Nuspell
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
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Original code form: https://wiki.qt.io/Spell-Checking-with-Hunspell
 */

#include "./spellchecker-nuspell.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>
#include <QStringList>

#include <nuspell/finder.hxx>

#include "./nuspellcheckdialog.h"
#include "../../application/texteditor.h"

void SpellChecker_Nuspell::initPlugin(QWidget *pParent, TextEditor *pEditor,
                                      const QDir &userDataDir,
                                      const QString &sSharePath) {
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;

#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  m_pEditor = pEditor;
  m_pParent = pParent;
  m_UserDataDir = userDataDir;
  m_sSharePath = sSharePath;

  m_pSettings->beginGroup("Plugin_" + QStringLiteral(PLUGIN_NAME));
  m_sDictLang = m_pSettings->value(QStringLiteral("SpellCheckerLanguage"),
                                   "de_DE").toString();
  m_sCommunity = m_pSettings->value(QStringLiteral("Inyoka/Community"),
                                    "ubuntuusers_de").toString();
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Nuspell::getPluginName() const -> QString {
  return QStringLiteral(PLUGIN_NAME);
}

auto SpellChecker_Nuspell::getPluginVersion() const -> QString {
  return QStringLiteral(PLUGIN_VERSION);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);

  if (!m_translator.load(":/" + QStringLiteral(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QStringLiteral(PLUGIN_NAME).toLower() +
                  "_" + sLang + ".qm";
    if (!m_translator.load(QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" +
                    QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
      return;
    }
  }

  if (!qApp->installTranslator(&m_translator) && "en" != sLang) {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Nuspell::getCaption() const -> QString {
  return tr("Spell checker") + QStringLiteral(" (Nuspell)");
}
auto SpellChecker_Nuspell::getIcon() const -> QIcon {
  if (m_pParent->window()->palette().window().color().lightnessF() < 0.5) {
    return QIcon(QLatin1String(":/spellchecker_dark.png"));
  }
  return QIcon(QLatin1String(":/spellchecker.png"));
}

auto SpellChecker_Nuspell::includeMenu() const -> bool{
  return true;
}
auto SpellChecker_Nuspell::includeToolbar() const -> bool {
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Nuspell::initDictionaries() -> bool {
  m_sListDicts.clear();
  auto searchdirs = std::vector<std::filesystem::path>();
  nuspell::append_default_dir_paths(searchdirs);
  auto dict_list = nuspell::search_default_dirs_for_dicts();

  // Add application folder to search folder for dictionaries
  std::filesystem::path app_path(
        QString(qApp->applicationDirPath() + "/dicts/").toStdU16String());
  auto appdirs = std::vector<std::filesystem::path>();
  appdirs.push_back(app_path);
  nuspell::search_dirs_for_dicts(appdirs, dict_list);
  searchdirs.push_back(app_path);

  QString sTmp;
  for (auto s : dict_list) {
    sTmp = QString::fromUtf16(s.u16string().c_str());
    // qDebug() << sTmp;
    sTmp.remove(QStringLiteral(".aff"));
    if (sTmp.contains('\\')) {
      sTmp = sTmp.right(sTmp.length() - sTmp.lastIndexOf('\\') - 1);
    } else {
      sTmp = sTmp.right(sTmp.length() - sTmp.lastIndexOf('/') -1);
    }
    m_sListDicts << sTmp;
  }

  auto dict_path = nuspell::search_dirs_for_one_dict(searchdirs,
                                                     m_sDictLang.toStdString());
  if (std::empty(dict_path)) {
    qWarning() << "Can not find the requested dictionary:" << m_sDictLang;
    qDebug() << "Found dictionaries:";
    for (auto s : dict_list) {
      qDebug() << QString::fromUtf16(s.u16string().c_str());
    }
    QMessageBox::warning(nullptr, qApp->applicationName(),
                         tr("Can not find the requested dictionary!"));
    return false;
  }

  try {
    m_Dict.load_aff_dic(dict_path);
  }
  catch (const nuspell::Dictionary_Loading_Error& e) {
    qWarning() << e.what() << '\n';
    QMessageBox::warning(nullptr, qApp->applicationName(),
                         QString::fromLatin1(e.what()));
    return false;
  }

  // Init user dictionary
  m_sUserDict = m_UserDataDir.absolutePath() + "/userDict_"
                + m_sDictLang + ".txt";
  if (!QFile::exists(m_sUserDict)) {
    QFile userDictFile(m_sUserDict);
    if (userDictFile.open(QIODevice::WriteOnly)) {
      userDictFile.close();
    } else {
      QMessageBox::warning(
            nullptr, qApp->applicationName(),
            QStringLiteral("User dictionary file couldn't be opened."));
      qWarning() << "User dictionary file could not be opened/created:"
                 << m_sUserDict;
    }
  }

  this->loadAdditionalDict(m_sUserDict);
  this->loadAdditionalDict(m_sSharePath + "/community/" +
                           m_sCommunity + "/ExtendedDict.txt");
  this->loadAdditionalDict(qApp->applicationDirPath() + "/ExtendedDict.txt");
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::loadAdditionalDict(const QString &sFilename) {
  QFile DictonaryFile(sFilename);
  if (DictonaryFile.exists()) {
    if (DictonaryFile.open(QIODevice::ReadOnly)) {
      QTextStream stream(&DictonaryFile);
      for (QString sWord = stream.readLine();
           !sWord.isEmpty();
           sWord = stream.readLine()) {
        putWord(sWord);
      }
      DictonaryFile.close();
    } else {
      qWarning() << "Dictionary" << sFilename << "could not be opened.";
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::callPlugin() {
  if (!this->initDictionaries()) {
    return;
  }

  m_pCheckDialog = new NuspellCheckDialog(this, nullptr);
  m_pCheckDialog->setWindowIcon(this->getIcon());

  QTextCharFormat highlightFormat;
  highlightFormat.setBackground(QBrush(QColor(255, 96, 96)));
  highlightFormat.setForeground(QBrush(QColor(0, 0, 0)));
  // Alternative format
  // highlightFormat.setUnderlineColor(QColor("red"));
  // highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

  // Save the position of the current cursor
  m_oldCursor = m_pEditor->textCursor();

  // Create a new cursor to walk through the text
  QTextCursor cursor(m_pEditor->document());
  QList<QTextEdit::ExtraSelection> esList;

  NuspellCheckDialog::SpellCheckAction spellResult = NuspellCheckDialog::None;

  // Don't call cursor.beginEditBlock(), as this prevents the redraw
  // after changes to the content
  // cursor.beginEditBlock();
  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString sWord(cursor.selectedText());

    // Workaround for better recognition of words punctuation etc.
    // does not belong to words
    while (!sWord.isEmpty()
           && !sWord.at(0).isLetter()
           && cursor.anchor() < cursor.position()) {
      int cursorPos = cursor.position();
      cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
      cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
      sWord = cursor.selectedText();
    }

    if (!sWord.isEmpty() && !this->spell(sWord)) {
      QTextCursor tmpCursor(cursor);
      tmpCursor.setPosition(cursor.anchor());
      m_pEditor->setTextCursor(tmpCursor);
      m_pEditor->ensureCursorVisible();
      esList.clear();

      // Highlight the unknown word
      QTextEdit::ExtraSelection es;
      es.cursor = cursor;
      es.format = highlightFormat;

      esList << es;
      m_pEditor->setExtraSelections(esList);
      QCoreApplication::processEvents();

      // Ask user what to do
      spellResult = m_pCheckDialog->checkWord(sWord);

      // Reset the word highlight
      esList.clear();
      m_pEditor->setExtraSelections(esList);
      QCoreApplication::processEvents();

      if (spellResult == NuspellCheckDialog::AbortCheck) {
        break;
      }

      switch (spellResult) {
        case NuspellCheckDialog::ReplaceOnce:
          cursor.insertText(m_pCheckDialog->replacement());
          break;
        case NuspellCheckDialog::ReplaceAll:
          this->replaceAll(cursor.position(), sWord,
                           m_pCheckDialog->replacement());
          break;
        default:
          break;
      }
      QCoreApplication::processEvents();
    }
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
  }

  delete m_pCheckDialog;
  m_pCheckDialog = nullptr;

  // cursor.endEditBlock();
  m_pEditor->setTextCursor(m_oldCursor);

  if (spellResult != NuspellCheckDialog::AbortCheck) {
    QMessageBox::information(nullptr, qApp->applicationName(),
                             tr("Spell check has finished."));
  }
}

void SpellChecker_Nuspell::executePlugin() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Nuspell::spell(const QString &sWord) -> bool {
  if (!m_UserWordsList.contains(sWord)) {
    return m_Dict.spell(sWord.toStdString());
  }
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Nuspell::suggest(const QString &sWord) -> QStringList {
  auto sugs = std::vector<std::string>();
  m_Dict.suggest(sWord.toStdString(), sugs);

  QStringList suggestions;
  std::transform(
        sugs.begin(), sugs.end(), std::back_inserter(suggestions),
        [](const std::string &v){
    return QString::fromStdString(v);
  });

  return suggestions;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::replaceAll(const int nPos, const QString &sOld,
                                      const QString &sNew) {
  QTextCursor cursor(m_pEditor->document());
  cursor.setPosition(nPos-sOld.length(), QTextCursor::MoveAnchor);

  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString word = cursor.selectedText();

    while (!word.isEmpty()
          && !word.at(0).isLetter()
          && cursor.anchor() < cursor.position()) {
      int cursorPos = cursor.position();
      cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
      cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
      word = cursor.selectedText();
    }

    if (word == sOld) {
      cursor.insertText(sNew);
      QCoreApplication::processEvents();
    }
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::ignoreWord(const QString &sWord) {
  this->putWord(sWord);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::putWord(const QString &sWord) {
  m_UserWordsList << sWord;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::addToUserWordlist(const QString &sWord) {
  this->putWord(sWord);
  if (!m_sUserDict.isEmpty()) {
    QFile userDictonaryFile(m_sUserDict);
    if (userDictonaryFile.open(QIODevice::Append)) {
      QTextStream stream(&userDictonaryFile);
      stream << sWord << "\n";
      userDictonaryFile.close();
    } else {
      QMessageBox::warning(nullptr, QStringLiteral("Spell checker"),
                           "User dictionary " + m_sUserDict
                           + " could not be opened for appending a "
                             "new word.");
      qWarning() << "User dictionary" << m_sUserDict
                 << "could not be opened for appending a new word.";
    }
  } else {
    qDebug() << "User dictionary not set.";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Nuspell::hasSettings() const -> bool {
  return false;
}

void SpellChecker_Nuspell::showSettings() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void SpellChecker_Nuspell::setEditorlist(
    const QList<TextEditor *> &listEditors) {
  Q_UNUSED(listEditors)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Nuspell::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  aboutbox.setIconPixmap(QPixmap(QStringLiteral(":/spellchecker.png")));
  aboutbox.setText(QString::fromLatin1("<p><b>%1</b><br />"
                                       "%2</p>"
                                       "<p>%3<br />"
                                       "%4</p>"
                                       "<p><i>%5</i></p>")
                   .arg(this->getCaption(),
                        tr("Version") + ": " + PLUGIN_VERSION,
                        PLUGIN_COPY,
                        tr("Licence") + ": " +
                        "<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">"
                        "GNU General Public License Version 3</a> and<br />"
                        "incorporates work covered by <a href=\""
                        "https://opensource.org/licenses/BSD-2-Clause\">"
                        "2-Clause BSD License</a>",
                        tr("Spell checker based on "
                           "<a href=\"https://nuspell.github.io/\">"
                           "Nuspell</a>.")));
  aboutbox.exec();
}
