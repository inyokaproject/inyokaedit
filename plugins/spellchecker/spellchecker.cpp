/**
 * \file spellchecker.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2014-2020 The InyokaEdit developers
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
 * Spell checker using Hunspell
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

#include "./spellchecker.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>

SpellChecker::~SpellChecker() = default;

void SpellChecker::initPlugin(QWidget *pParent, TextEditor *pEditor,
                              const QDir &userDataDir,
                              const QString &sSharePath) {
  qDebug() << "initPlugin()" << PLUGIN_NAME << PLUGIN_VERSION;
  Q_UNUSED(pParent)

#if defined __linux__
  m_pSettings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#else
  m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              qApp->applicationName().toLower(),
                              qApp->applicationName().toLower());
#endif

  m_pHunspell = nullptr;
  m_pEditor = pEditor;
  m_UserDataDir = userDataDir;
  m_sSharePath = sSharePath;

  m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_sDictPath = m_pSettings->value("DictionaryPath", "").toString();
  this->setDictPath();
  m_sDictLang = m_pSettings->value("SpellCheckerLanguage",
                                   "de_DE").toString();
  m_sCommunity = m_pSettings->value("InyokaCommunity",
                                    "ubuntuusers_de").toString();
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString SpellChecker::getPluginName() const {
  return PLUGIN_NAME;
}

QString SpellChecker::getPluginVersion() const {
  return PLUGIN_VERSION;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);
  if ("en" == sLang) {
    return;
  }

  if (!m_translator.load(":/" + QString(PLUGIN_NAME).toLower() +
                         "_" + sLang + ".qm")) {
    qWarning() << "Could not load translation" <<
                  ":/" + QString(PLUGIN_NAME).toLower() + "_" + sLang + ".qm";
    if (!m_translator.load(QString(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation" <<
                    m_sSharePath + "/lang/" + QString(PLUGIN_NAME).toLower() +
                    "_" + sLang + ".qm";
      return;
    }
  }

  if (!qApp->installTranslator(&m_translator)) {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString SpellChecker::getCaption() const {
  return tr("Spell checker");
}
QIcon SpellChecker::getIcon() const {
  return QIcon(":/spellchecker.png");
}

bool SpellChecker::includeMenu() const {
  return true;
}
bool SpellChecker::includeToolbar() const {
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::setDictPath() {
  m_sListDicts.clear();

  if (m_sDictPath.isEmpty() || !QDir(m_sDictPath).exists()) {
    // Standard path for Hunspell
    if (QDir("/usr/share/hunspell").exists()) {
      m_sDictPath = "/usr/share/hunspell/";
    } else if (QDir("/usr/local/share/hunspell").exists()) {
      m_sDictPath = "/usr/local/share/hunspell/";
      // Otherwise look for MySpell dictionary
    } else if (QDir("/usr/share/myspell/dicts").exists()) {
      m_sDictPath = "/usr/share/myspell/dicts/";
    } else if (QDir("/usr/local/share/myspell/dicts").exists()) {
      m_sDictPath = "/usr/local/share/myspell/dicts/";
    } else {
      // Fallback and for Windows look in app dir
      m_sDictPath = qApp->applicationDirPath() + "/dicts/";
    }
  }
  if (!m_sDictPath.endsWith('/')) {
    m_sDictPath.append('/');
  }
  // Group already set in costructor
  // m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_pSettings->setValue("DictionaryPath", m_sDictPath);
  // m_pSettings->endGroup();

  QFileInfoList fiListFiles = QDir(m_sDictPath).entryInfoList(
                                QDir::NoDotAndDotDot | QDir::Files);
  foreach (QFileInfo fi, fiListFiles) {
    m_sListDicts << fi.baseName();
  }
  m_sListDicts.removeDuplicates();
  // qDebug() << "Dictionary files:" << m_sListDicts;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SpellChecker::initDictionaries() {
  if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic")
      || !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
    qWarning() << "Spell checker dictionary file does not exist:"
               << m_sDictPath + m_sDictLang << "*.dic *.aff";
    QMessageBox::warning(nullptr, qApp->applicationName(),
                         "Spell checker dictionary file does not exist!\n"
                         "Trying to load fallback dictionary.");

    // Try to load english fallback
    m_sDictLang = "en_GB";
    if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic")
        || !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
      qWarning() << "Spell checker fallback does not exist:"
                 << m_sDictPath + m_sDictLang << "*.dic *.aff";
      m_sDictLang = "en-GB";
      if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic")
          || !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
        qWarning() << "Spell checker fallback does not exist:"
                   << m_sDictPath + m_sDictLang << "*.dic *.aff";
        QMessageBox::warning(nullptr, qApp->applicationName(),
                             "Spell checker fallback "
                             + m_sDictLang + " doesn't exist as well.");
        return false;
      }
    }
  }

  // Init user dictionary
  m_sUserDict = m_UserDataDir.absolutePath() + "/userDict_"
                + m_sDictLang + ".txt";
  if (!QFile::exists(m_sUserDict)) {
    QFile userDictFile(m_sUserDict);
    if (userDictFile.open(QIODevice::WriteOnly)) {
      userDictFile.close();
    } else {
      QMessageBox::warning(nullptr, qApp->applicationName(),
                           "User dictionary file couldn't be opened.");
      qWarning() << "User dictionary file could not be opened/created:"
                 << m_sUserDict;
    }
  }

  QString sDictFile(m_sDictPath + m_sDictLang + ".dic");
  QString sAffixFile(m_sDictPath + m_sDictLang + ".aff");
  QByteArray dictFilePathBA = sDictFile.toLocal8Bit();
  QByteArray affixFilePathBA = sAffixFile.toLocal8Bit();

  // qDebug() << "Using dictionary:" << sDictFile;

  // Detect encoding analyzing the SET option in the affix file
  m_sEncoding = "ISO8859-1";
  QFile _affixFile(sAffixFile);
  if (_affixFile.open(QIODevice::ReadOnly)) {
    QTextStream stream(&_affixFile);
    QRegularExpression enc_detector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*",
                                    QRegularExpression::CaseInsensitiveOption);
    QString sLine("");
    QRegularExpressionMatch match;
    while (!stream.atEnd()) {
      sLine = stream.readLine();
      if (sLine.isEmpty()) { continue; }
      match = enc_detector.match(sLine);
      if (match.hasMatch()) {
        m_sEncoding = match.captured(1);
        // qDebug() << QString("Encoding set to ") + m_sEncoding;
        break;
      }
    }
    _affixFile.close();
  } else {
    QMessageBox::warning(nullptr, qApp->applicationName(),
                         "Dictionary could not be opened.");
    qWarning() << "Dictionary could not be opened:" << sAffixFile;
    return false;
  }
  m_pCodec = QTextCodec::codecForName(
               this->m_sEncoding.toLatin1().constData());

  delete m_pHunspell;
  m_pHunspell = nullptr;
  m_pHunspell = new Hunspell(affixFilePathBA.constData(),
                             dictFilePathBA.constData());

  this->loadAdditionalDict(m_sUserDict);
  this->loadAdditionalDict(m_sSharePath + "/community/" +
                           m_sCommunity + "/ExtendedDict.txt");
  this->loadAdditionalDict(qApp->applicationDirPath() + "/ExtendedDict.txt");
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::loadAdditionalDict(const QString &sFilename) {
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

void SpellChecker::callPlugin() {
  if (!this->initDictionaries()) {
    return;
  }

  m_pCheckDialog = new SpellCheckDialog(this, nullptr);

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

  SpellCheckDialog::SpellCheckAction spellResult = SpellCheckDialog::None;

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

      if (spellResult == SpellCheckDialog::AbortCheck) {
        break;
      }

      switch (spellResult) {
        case SpellCheckDialog::ReplaceOnce:
          cursor.insertText(m_pCheckDialog->replacement());
          break;
        case SpellCheckDialog::ReplaceAll:
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

  if (spellResult != SpellCheckDialog::AbortCheck) {
    QMessageBox::information(nullptr, qApp->applicationName(),
                             tr("Spell check has finished."));
  }
}

void SpellChecker::executePlugin() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool SpellChecker::spell(const QString &sWord) {
#if defined(USE_NEW_HUNSPELL)  // Hunspell >= 1.5
  return m_pHunspell->spell(m_pCodec->fromUnicode(sWord).toStdString());
#else
  return m_pHunspell->spell(m_pCodec->fromUnicode(sWord).constData()) != 0;
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QStringList SpellChecker::suggest(const QString &sWord) {
  int nSuggestions = 0;

#if defined(USE_NEW_HUNSPELL)  // Hunspell >= 1.5
  QStringList sListSuggestions;
  std::vector<std::string> wordlist;
  wordlist = m_pHunspell->suggest(m_pCodec->fromUnicode(sWord).toStdString());

  nSuggestions = static_cast<int>(wordlist.size());
  if (nSuggestions > 0) {
    sListSuggestions.reserve(nSuggestions);
    for (int i = 0; i < nSuggestions; i++) {
      sListSuggestions << m_pCodec->toUnicode(
                            QByteArray::fromStdString(wordlist[i]));
    }
  }
#else
  QStringList sListSuggestions;
  char **suggestWordList;

  // Encode from Unicode to the encoding used by current dictionary
  nSuggestions = m_pHunspell->suggest(&suggestWordList,
                                      m_pCodec->fromUnicode(sWord)
                                      .constData());
  if (nSuggestions > 0) {
    sListSuggestions.reserve(nSuggestions);
    for (int i = 0; i < nSuggestions; ++i) {
      sListSuggestions << m_pCodec->toUnicode(suggestWordList[i]);
      free(suggestWordList[i]);
    }
  }
#endif
  return sListSuggestions;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::replaceAll(const int nPos, const QString &sOld,
                              const QString &sNew) {
  QTextCursor cursor(m_pEditor->document());
  cursor.setPosition(nPos-sOld.length(), QTextCursor::MoveAnchor);

  while(!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString word = cursor.selectedText();

    while(!word.isEmpty()
          && !word.at(0).isLetter()
          && cursor.anchor() < cursor.position()) {
      int cursorPos = cursor.position();
      cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
      cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
      word = cursor.selectedText();
    }

    if(word == sOld) {
      cursor.insertText(sNew);
      QCoreApplication::processEvents();
    }
    cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::ignoreWord(const QString &sWord) {
  this->putWord(sWord);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::putWord(const QString &sWord) {
  m_pHunspell->add(m_pCodec->fromUnicode(sWord).constData());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::addToUserWordlist(const QString &sWord) {
  this->putWord(sWord);
  if (!m_sUserDict.isEmpty()) {
    QFile userDictonaryFile(m_sUserDict);
    if (userDictonaryFile.open(QIODevice::Append)) {
      QTextStream stream(&userDictonaryFile);
      stream << sWord << "\n";
      userDictonaryFile.close();
    } else {
      QMessageBox::warning(nullptr, "Spell checker",
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

bool SpellChecker::hasSettings() const {
  return false;
}

void SpellChecker::showSettings() {
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void SpellChecker::setEditorlist(const QList<TextEditor *> &listEditors) {
  Q_UNUSED(listEditors)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  aboutbox.setIconPixmap(QPixmap(":/spellchecker.png"));
  aboutbox.setText(QString("<p><b>%1</b><br />"
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
                           "<a href=\"http://hunspell.sourceforge.net/\">"
                           "Hunspell</a>.")));
  aboutbox.exec();
}
