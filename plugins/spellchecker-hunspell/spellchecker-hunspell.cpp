/**
 * \file spellchecker-hunspell.cpp
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

#include "./spellchecker-hunspell.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

#include "../../application/texteditor.h"
#include "./hunspellcheckdialog.h"

void SpellChecker_Hunspell::initPlugin(QWidget *pParent, TextEditor *pEditor,
                                       const QDir &userDataDir,
                                       const QString &sSharePath,
                                       const bool &bIsDarkTheme) {
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

  m_pHunspell = nullptr;
  m_pEditor = pEditor;
  m_pParent = pParent;
  m_UserDataDir.setPath(userDataDir.absolutePath());
  m_sSharePath = sSharePath;
  m_bIsDarkTheme = bIsDarkTheme;

  m_pSettings->beginGroup("Plugin_" + QStringLiteral(PLUGIN_NAME));
  m_sDictPath =
      m_pSettings->value(QStringLiteral("DictionaryPath"), "").toString();
  this->setDictPath();
  m_sDictLang =
      m_pSettings->value(QStringLiteral("SpellCheckerLanguage"), "de_DE")
          .toString();
  m_sCommunity =
      m_pSettings->value(QStringLiteral("Inyoka/Community"), "ubuntuusers_de")
          .toString();
  m_pSettings->endGroup();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Hunspell::getPluginName() const -> QString {
  return QStringLiteral(PLUGIN_NAME);
}

auto SpellChecker_Hunspell::getPluginVersion() const -> QString {
  return QStringLiteral(PLUGIN_VERSION);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::installTranslator(const QString &sLang) {
  qApp->removeTranslator(&m_translator);

  if (!m_translator.load(":/" + QStringLiteral(PLUGIN_NAME).toLower() + "_" +
                         sLang + ".qm")) {
    qWarning() << "Could not load translation"
               << ":/" + QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang +
                      ".qm";
    if (!m_translator.load(QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang,
                           m_sSharePath + "/lang")) {
      qWarning() << "Could not load translation"
                 << m_sSharePath + "/lang/" +
                        QStringLiteral(PLUGIN_NAME).toLower() + "_" + sLang +
                        ".qm";
      return;
    }
  }

  if (!qApp->installTranslator(&m_translator) && "en" != sLang) {
    qWarning() << "Translator could not be installed!";
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Hunspell::getCaption() const -> QString {
  return tr("Spell checker") + QStringLiteral(" (Hunspell)");
}
auto SpellChecker_Hunspell::getIcon() const -> QIcon {
  if (m_bIsDarkTheme) {
    return QIcon(QLatin1String(":/tools-check-spelling_dark.png"));
  } else {
    return QIcon(QLatin1String(":/tools-check-spelling.png"));
  }
}

auto SpellChecker_Hunspell::includeMenu() const -> bool { return true; }
auto SpellChecker_Hunspell::includeToolbar() const -> bool { return true; }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::setDictPath() {
  m_sListDicts.clear();

  if (m_sDictPath.isEmpty() || !QDir(m_sDictPath).exists()) {
    // Standard path for Hunspell
    if (QDir(QStringLiteral("/usr/share/hunspell")).exists()) {
      m_sDictPath = QStringLiteral("/usr/share/hunspell/");
    } else if (QDir(QStringLiteral("/usr/local/share/hunspell")).exists()) {
      m_sDictPath = QStringLiteral("/usr/local/share/hunspell/");
      // Otherwise look for MySpell dictionary
    } else if (QDir(QStringLiteral("/usr/share/myspell/dicts")).exists()) {
      m_sDictPath = QStringLiteral("/usr/share/myspell/dicts/");
    } else if (QDir(QStringLiteral("/usr/local/share/myspell/dicts"))
                   .exists()) {
      m_sDictPath = QStringLiteral("/usr/local/share/myspell/dicts/");
    } else {
      // Fallback and for Windows look in app dir
      m_sDictPath = qApp->applicationDirPath() + "/dicts/";
    }
  }
  if (!m_sDictPath.endsWith('/')) {
    m_sDictPath.append('/');
  }
  // Group already set in constructor
  // m_pSettings->beginGroup("Plugin_" + QString(PLUGIN_NAME));
  m_pSettings->setValue(QStringLiteral("DictionaryPath"), m_sDictPath);
  // m_pSettings->endGroup();

  const QFileInfoList fiListFiles =
      QDir(m_sDictPath).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
  for (const auto &fi : fiListFiles) {
    m_sListDicts << fi.baseName();
  }
  m_sListDicts.removeDuplicates();
  // qDebug() << "Dictionary files:" << m_sListDicts;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Hunspell::initDictionaries() -> bool {
  if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic") ||
      !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
    qWarning() << "Spell checker dictionary file does not exist:"
               << m_sDictPath + m_sDictLang << "*.dic *.aff";
    QMessageBox::warning(
        nullptr, qApp->applicationName(),
        QString::fromLatin1("Spell checker dictionary file does not exist!\n"
                            "Trying to load fallback dictionary."));

    // Try to load english fallback
    m_sDictLang = QStringLiteral("en_GB");
    if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic") ||
        !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
      qWarning() << "Spell checker fallback does not exist:"
                 << m_sDictPath + m_sDictLang << "*.dic *.aff";
      m_sDictLang = QStringLiteral("en-GB");
      if (!QFile::exists(m_sDictPath + m_sDictLang + ".dic") ||
          !QFile::exists(m_sDictPath + m_sDictLang + ".aff")) {
        qWarning() << "Spell checker fallback does not exist:"
                   << m_sDictPath + m_sDictLang << "*.dic *.aff";
        QMessageBox::warning(nullptr, qApp->applicationName(),
                             "Spell checker fallback " + m_sDictLang +
                                 " doesn't exist as well.");
        return false;
      }
    }
  }

  // Init user dictionary
  m_sUserDict =
      m_UserDataDir.absolutePath() + "/userDict_" + m_sDictLang + ".txt";
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

  QString sDictFile(m_sDictPath + m_sDictLang + ".dic");
  QString sAffixFile(m_sDictPath + m_sDictLang + ".aff");
  QByteArray dictFilePathBA = sDictFile.toLocal8Bit();
  QByteArray affixFilePathBA = sAffixFile.toLocal8Bit();

  // qDebug() << "Using dictionary:" << sDictFile;

  // Detect encoding analyzing the SET option in the affix file
  m_sEncoding = QStringLiteral("ISO8859-1");
  QFile _affixFile(sAffixFile);
  if (_affixFile.open(QIODevice::ReadOnly)) {
    QTextStream stream(&_affixFile);
    static QRegularExpression enc_detector(
        QStringLiteral("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*"),
        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match;
    while (!stream.atEnd()) {
      QString sLine = stream.readLine();
      if (sLine.isEmpty()) {
        continue;
      }
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
                         QStringLiteral("Dictionary could not be opened."));
    qWarning() << "Dictionary could not be opened:" << sAffixFile;
    return false;
  }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  m_pCodec = QTextCodec::codecForName(this->m_sEncoding.toLatin1().constData());
#else
  m_Decoder = QStringDecoder(this->m_sEncoding.toLatin1().constData());
  m_Encoder = QStringEncoder(this->m_sEncoding.toLatin1().constData());
  if (!m_Decoder.isValid() || !m_Encoder.isValid()) {
    QMessageBox::warning(nullptr, qApp->applicationName(),
                         QStringLiteral("No valid string converter found!"));
    qWarning() << "Invalid string converter! Decoder:" << m_Decoder.name()
               << "- Encoder: " << m_Encoder.name();
  }
#endif

  delete m_pHunspell;
  m_pHunspell = nullptr;
  m_pHunspell =
      new Hunspell(affixFilePathBA.constData(), dictFilePathBA.constData());

  this->loadAdditionalDict(m_sUserDict);
  this->loadAdditionalDict(m_sSharePath + "/community/" + m_sCommunity +
                           "/ExtendedDict.txt");
  this->loadAdditionalDict(qApp->applicationDirPath() + "/ExtendedDict.txt");
  return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::loadAdditionalDict(const QString &sFilename) {
  QFile DictonaryFile(sFilename);
  if (DictonaryFile.exists()) {
    if (DictonaryFile.open(QIODevice::ReadOnly)) {
      QTextStream stream(&DictonaryFile);
      for (QString sWord = stream.readLine(); !sWord.isEmpty();
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

void SpellChecker_Hunspell::callPlugin() {
  if (!this->initDictionaries()) {
    return;
  }

  m_pCheckDialog = new HunspellCheckDialog(this, nullptr);

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

  HunspellCheckDialog::SpellCheckAction spellResult = HunspellCheckDialog::None;

  // Don't call cursor.beginEditBlock(), as this prevents the redraw
  // after changes to the content
  // cursor.beginEditBlock();
  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString sWord(cursor.selectedText());

    // Workaround for better recognition of words punctuation etc.
    // does not belong to words
    while (!sWord.isEmpty() && !sWord.at(0).isLetter() &&
           cursor.anchor() < cursor.position()) {
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

      if (spellResult == HunspellCheckDialog::AbortCheck) {
        break;
      }

      switch (spellResult) {
        case HunspellCheckDialog::ReplaceOnce:
          cursor.insertText(m_pCheckDialog->replacement());
          break;
        case HunspellCheckDialog::ReplaceAll:
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

  if (spellResult != HunspellCheckDialog::AbortCheck) {
    QMessageBox::information(nullptr, qApp->applicationName(),
                             tr("Spell check has finished."));
  }
}

void SpellChecker_Hunspell::executePlugin() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Hunspell::spell(const QString &sWord) -> bool {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  return m_pHunspell->spell(m_pCodec->fromUnicode(sWord).toStdString());
#else
  return m_pHunspell->spell(QByteArray(m_Encoder(sWord)).toStdString());
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto SpellChecker_Hunspell::suggest(const QString &sWord) -> QStringList {
  int nSuggestions = 0;
  QStringList sListSuggestions;
  std::vector<std::string> wordlist;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  wordlist = m_pHunspell->suggest(m_pCodec->fromUnicode(sWord).toStdString());
#else
  wordlist = m_pHunspell->suggest(QByteArray(m_Encoder(sWord)).toStdString());
#endif

  nSuggestions = static_cast<int>(wordlist.size());
  if (nSuggestions > 0) {
    sListSuggestions.reserve(nSuggestions);
    for (int i = 0; i < nSuggestions; i++) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      sListSuggestions << m_pCodec->toUnicode(
          QByteArray::fromStdString(wordlist[i]));
#else
      sListSuggestions << m_Decoder(wordlist[i]);
#endif
    }
  }

  return sListSuggestions;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::replaceAll(const int nPos, const QString &sOld,
                                       const QString &sNew) {
  QTextCursor cursor(m_pEditor->document());
  cursor.setPosition(nPos - sOld.length(), QTextCursor::MoveAnchor);

  while (!cursor.atEnd()) {
    QCoreApplication::processEvents();
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
    QString word = cursor.selectedText();

    while (!word.isEmpty() && !word.at(0).isLetter() &&
           cursor.anchor() < cursor.position()) {
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

void SpellChecker_Hunspell::ignoreWord(const QString &sWord) {
  this->putWord(sWord);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::putWord(const QString &sWord) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  m_pHunspell->add(m_pCodec->fromUnicode(sWord).constData());
#else
  m_pHunspell->add(QByteArray(m_Encoder(sWord)).constData());
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::addToUserWordlist(const QString &sWord) {
  this->putWord(sWord);
  if (!m_sUserDict.isEmpty()) {
    QFile userDictonaryFile(m_sUserDict);
    if (userDictonaryFile.open(QIODevice::Append)) {
      QTextStream stream(&userDictonaryFile);
      stream << sWord << "\n";
      userDictonaryFile.close();
    } else {
      QMessageBox::warning(nullptr, QStringLiteral("Spell checker"),
                           "User dictionary " + m_sUserDict +
                               " could not be opened for appending a "
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

auto SpellChecker_Hunspell::hasSettings() const -> bool { return false; }

void SpellChecker_Hunspell::showSettings() {}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::setCurrentEditor(TextEditor *pEditor) {
  m_pEditor = pEditor;
}

void SpellChecker_Hunspell::setEditorlist(
    const QList<TextEditor *> &listEditors) {
  Q_UNUSED(listEditors)
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SpellChecker_Hunspell::showAbout() {
  QMessageBox aboutbox(nullptr);
  aboutbox.setWindowTitle(tr("Info"));
  if (m_bIsDarkTheme) {
    aboutbox.setIconPixmap(
        QPixmap(QStringLiteral(":/tools-check-spelling_dark.png")));
  } else {
    aboutbox.setIconPixmap(
        QPixmap(QStringLiteral(":/tools-check-spelling.png")));
  }
  aboutbox.setText(
      QString::fromLatin1("<p><b>%1</b><br />"
                          "%2</p>"
                          "<p>%3<br />"
                          "%4</p>"
                          "<p><i>%5</i></p>")
          .arg(this->getCaption(), tr("Version") + ": " + PLUGIN_VERSION,
               PLUGIN_COPY,
               tr("Licence") + ": " +
                   "<a href=\"https://www.gnu.org/licenses/gpl-3.0.html\">"
                   "GNU General Public License Version 3</a> and<br />"
                   "incorporates work covered by <a href=\""
                   "https://opensource.org/licenses/BSD-2-Clause\">"
                   "2-Clause BSD License</a>",
               tr("Spell checker based on "
                  "<a href=\"https://hunspell.github.io/\">"
                  "Hunspell</a>.")));
  aboutbox.exec();
}
