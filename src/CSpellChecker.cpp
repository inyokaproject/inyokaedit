/**
 * \file CSpellChecker.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Spell checker using Hunspell
 *
 * \section SOURCE
 * Original code form: http://developer.qt.nokia.com/wiki/Spell_Checking_with_Hunspell
 */

#include "./CSpellChecker.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>

CSpellChecker::CSpellChecker(const QString &sDictionaryPath,
                             const QString &sUserDictionary,
                             const QString &sAppPath,
                             QWidget *pParent)
    : m_sUserDictionary(sUserDictionary),
      m_sAppPath(sAppPath){
    qDebug() << "Calling" << Q_FUNC_INFO;

    QString sDictFile(sDictionaryPath + ".dic");
    QString sAffixFile(sDictionaryPath + ".aff");
    QByteArray dictFilePathBA = sDictFile.toLocal8Bit();
    QByteArray affixFilePathBA = sAffixFile.toLocal8Bit();
    m_pHunspell = new Hunspell(affixFilePathBA.constData(),
                               dictFilePathBA.constData());

    // Detect encoding analyzing the SET option in the affix file
    m_sEncoding = "ISO8859-1";
    QFile _affixFile(sAffixFile);
    if (_affixFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&_affixFile);
        QRegExp enc_detector("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*",
                             Qt::CaseInsensitive);
        for (QString sLine = stream.readLine();
             !sLine.isEmpty();
             sLine = stream.readLine()) {
            if (enc_detector.indexIn(sLine) > -1) {
                m_sEncoding = enc_detector.cap(1);
                // qDebug() << QString("Encoding set to ") + _encoding;
                break;
            }
        }
        _affixFile.close();
    }
    m_pCodec = QTextCodec::codecForName(this->m_sEncoding.toLatin1().constData());

    this->loadAdditionalDict(m_sUserDictionary);
    this->loadAdditionalDict(m_sAppPath + "/ExtendedDict.txt");

    m_pCheckDialog = new CSpellCheckDialog(this, pParent);
}

CSpellChecker::~CSpellChecker() {
    delete m_pHunspell;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSpellChecker::loadAdditionalDict(const QString &sFilename) {
    if (!sFilename.isEmpty()) {
        QFile DictonaryFile(sFilename);
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

void CSpellChecker::start(CTextEditor *pEditor) {
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(QColor("#ff6060")));
    highlightFormat.setForeground(QBrush(QColor("#000000")));
    // Alternative format
    // highlightFormat.setUnderlineColor(QColor("red"));
    // highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    // Save the position of the current cursor
    QTextCursor oldCursor(pEditor->textCursor());

    // Create a new cursor to walk through the text
    QTextCursor cursor(pEditor->document());

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
            pEditor->setTextCursor(tmpCursor);
            pEditor->ensureCursorVisible();

            // Highlight the unknown word
            QTextEdit::ExtraSelection es;
            es.cursor = cursor;
            es.format = highlightFormat;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            pEditor->setExtraSelections(esList);
            QCoreApplication::processEvents();

            // Ask user what to do
            CSpellCheckDialog::SpellCheckAction spellResult =
                    m_pCheckDialog->checkWord(sWord);

            // Reset the word highlight
            esList.clear();
            pEditor->setExtraSelections(esList);
            QCoreApplication::processEvents();

            if (spellResult == CSpellCheckDialog::AbortCheck) {
                break;
            }

            switch (spellResult) {
                case CSpellCheckDialog::ReplaceOnce:
                    cursor.insertText(m_pCheckDialog->replacement());
                    break;

                default:
                    break;
            }
            QCoreApplication::processEvents();
        }
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }

    if (m_pCheckDialog != NULL) {
        delete m_pCheckDialog;
    }
    m_pCheckDialog = NULL;

    // cursor.endEditBlock();
    pEditor->setTextCursor(oldCursor);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool CSpellChecker::spell(const QString &sWord) {
    // Encode from Unicode to the encoding used by current dictionary
    return m_pHunspell->spell(m_pCodec->fromUnicode(sWord).constData()) != 0;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QStringList CSpellChecker::suggest(const QString &sWord) {
    char **suggestWordList;

    // Encode from Unicode to the encoding used by current dictionary
    int nSuggestions = m_pHunspell->suggest(&suggestWordList,
                                            m_pCodec->fromUnicode(sWord)
                                            .constData());
    QStringList sListSuggestions;
    for (int i = 0; i < nSuggestions; ++i) {
        sListSuggestions << m_pCodec->toUnicode(suggestWordList[i]);
        free(suggestWordList[i]);
    }
    return sListSuggestions;
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSpellChecker::ignoreWord(const QString &sWord) {
    this->putWord(sWord);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSpellChecker::putWord(const QString &sWord) {
    m_pHunspell->add(m_pCodec->fromUnicode(sWord).constData());
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CSpellChecker::addToUserWordlist(const QString &sWord) {
    this->putWord(sWord);
    if (!m_sUserDictionary.isEmpty()) {
        QFile userDictonaryFile(m_sUserDictionary);
        if (userDictonaryFile.open(QIODevice::Append)) {
            QTextStream stream(&userDictonaryFile);
            stream << sWord << "\n";
            userDictonaryFile.close();
        } else {
            QMessageBox::warning(0, "Spell checker",
                                 "User dictionary in " + m_sUserDictionary
                                 + " could not be opened for appending a "
                                 "new word.");
            qWarning() << "User dictionary in" << m_sUserDictionary
                       << "could not be opened for appending a new word.";
        }
    } else {
        qDebug() << "User dictionary not set.";
    }
}
