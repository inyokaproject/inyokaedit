/**
 * \file CParser.cpp
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
 * Parse plain text with inyoka syntax into html code.
 */

#include "./CParser.h"

// Constructor
CParser::CParser(const QDir &tmpFileOutputDir,
                 const QDir &tmpImgDir,
                 const QString sInyokaUrl,
                 const bool bCheckLinks,
                 CTemplates *pTemplates)
    : m_tmpFileDir(tmpFileOutputDir),
      m_tmpImgDir(tmpImgDir),
      m_sInyokaUrl(sInyokaUrl),
      m_pTemplates(pTemplates),
      m_sSEPARATOR("$UGLY_SEPARATOR$") {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pLinkParser = new CParseLinks(m_sInyokaUrl,
                                    m_pTemplates->getIWLs()->getElementTypes(),
                                    m_pTemplates->getIWLs()->getElementUrls(),
                                    bCheckLinks,
                                    m_pTemplates->getTransAnchor());

    m_pMapParser = new CParseImgMap();
}

// Destructor
CParser::~CParser() {
    if (NULL != m_pLinkParser) {
        delete m_pLinkParser;
        m_pLinkParser = NULL;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::updateSettings(const QString sInyokaUrl,
                             const bool bCheckLinks) {
    m_sInyokaUrl = sInyokaUrl;
    m_pLinkParser->updateSettings(sInyokaUrl, bCheckLinks);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CParser::genOutput(const QString &sActFile,
                           QTextDocument *pRawDocument) {
    QString sHtmlBody("");
    QString sWikitags("");
    QString sSample("");
    QTextBlock it;

    // Need a copy otherwise text in editor will be changed
    m_pRawText = pRawDocument->clone();

    // Replace macros with Inyoka markup templates
    // this->replaceTemplates( m_pRawText );

    // Replace text format
    this->replaceTextformat(m_pRawText);

    // Replace all links
    m_pLinkParser->startParsing(m_pRawText);

    // Replace flags
    m_pMapParser->startParsing(m_pRawText,
                               m_pTemplates->getListFlags(),
                               m_pTemplates->getListFlagsImg());
    // Replace smilies
    m_pMapParser->startParsing(m_pRawText,
                               m_pTemplates->getListSmilies(),
                               m_pTemplates->getListSmiliesImg());

    // Replace keys
    this->replaceKeys(m_pRawText);

    // Replace images
    this->replaceImages(m_pRawText);

    // Replace breaks (\\ or [[BR]])
    this->replaceBreaks(m_pRawText);

    // Replace horizontal line (----)
    this->replaceHorLine(m_pRawText);


    // Go through each text block and call depending parse function
    for (it = m_pRawText->firstBlock();
         it.isValid() && !(m_pRawText->lastBlock() < it);
         it = it.next()) {
        // Macro samples
        if (it.text().trimmed().startsWith(
                    "[[" + m_pTemplates->getTransTemplate() + "(",
                    Qt::CaseSensitive)
                && !(it.text().trimmed().startsWith(
                         "[[" + m_pTemplates->getTransTemplate() + "("
                         + trUtf8("Tasten"), Qt::CaseSensitive))
                && !(it.text().trimmed().startsWith(
                         "[[" + m_pTemplates->getTransTemplate() + "("
                         + trUtf8("Bildersammlung"), Qt::CaseSensitive))) {
            sHtmlBody += parseMacro(it);
        } else if (it.text().trimmed().startsWith(
                       "[[" + m_pTemplates->getTransTOC(),
                       Qt::CaseInsensitive)) {
            // Table of contents
            sHtmlBody += parseTableOfContents(it);
        } else if (it.text().trimmed().startsWith(
                       "#tag:") || it.text().trimmed().startsWith("# tag:")) {
            // Article tags
            sWikitags = generateTags(it);
        } else if (it.text().trimmed().startsWith("=")) {
            // Headline
            sHtmlBody += parseHeadline(it);
        } else if (it.text().trimmed().startsWith(
                       "{{{#!" + m_pTemplates->getTransTemplate().toLower(),
                       Qt::CaseSensitive)
                   || it.text().trimmed().startsWith(
                       "{{{#!" + m_pTemplates->getTransTemplate(),
                       Qt::CaseSensitive)) {
            // Text sample
            sSample = it.text();
            it = it.next();
            for (; it.isValid()
                 && !(m_pRawText->lastBlock() < it)
                 && it.text().trimmed() != "}}}"; it = it.next()) {
                sSample += m_sSEPARATOR + it.text();
            }
            sHtmlBody += parseTextSample(sSample);
            // it = it.next();
        } else if (it.text().trimmed().startsWith(
                       "{{{", Qt::CaseSensitive)
                   || it.text().trimmed().startsWith(
                       "{{{#!" + m_pTemplates->getTransCodeBlock().toLower(),
                       Qt::CaseSensitive)
                   || it.text().trimmed().startsWith(
                       "{{{#!" + m_pTemplates->getTransCodeBlock(),
                       Qt::CaseSensitive)) {
            // Codeblock
            sSample = it.text();

            // Only one line
            if (sSample.trimmed().endsWith("}}}")) {
                sHtmlBody += parseCodeBlock(sSample);
            } else {
                it = it.next();
                for (; it.isValid()
                     && !(m_pRawText->lastBlock() < it)
                     && it.text().trimmed() != "}}}"; it = it.next()) {
                    sSample += m_sSEPARATOR + it.text();
                    if (it.text().endsWith("}}}")) {
                        break;
                    }
                }
                sHtmlBody += parseCodeBlock(sSample);
            }
        } else if (it.text().trimmed().startsWith(
                       "[["+ m_pTemplates->getTransTemplate() + "("
                       + trUtf8("Bildersammlung"), Qt::CaseSensitive)) {
            // Image collection
            sSample = it.text();
            it = it.next();
            for (; it.isValid()
                 && !(m_pRawText->lastBlock() < it)
                 && it.text().trimmed() != ")]]"; it = it.next()) {
                sSample += m_sSEPARATOR + it.text();
            }
            sHtmlBody += parseImageCollection(sSample);
        } else if (it.text().trimmed().startsWith("* ")
                   || it.text().trimmed().startsWith("1. ")) {
            // List
            sSample = it.text();
            it = it.next();
            QTextBlock tmpBlock = it;  // Next to last block
            for (; tmpBlock.isValid()
                 && !(m_pRawText->lastBlock() < tmpBlock);
                 tmpBlock = tmpBlock.next()) {
                if (tmpBlock.text().trimmed().startsWith("* ")
                        || tmpBlock.text().trimmed().startsWith("1. ")) {
                    sSample += m_sSEPARATOR + tmpBlock.text();
                } else {
                    it = tmpBlock.previous();
                    break;
                }

                if (it == m_pRawText->lastBlock()) {
                    break;
                }
            }
            sHtmlBody += parseList(sSample);
        } else if (it.text().trimmed().startsWith(">")) {
            // Quotes
            sSample = it.text();
            int nQuotes = sSample.count(">");
            sSample.remove(QRegExp("^>*"));
            for (int n = 0; n < nQuotes; n++) {
                sSample = "<blockquote>" + sSample + "</blockquote>";
            }
            sHtmlBody += sSample;
        } else {
            // Everything else
            // Filter comments (##)
            if (!(it.text().trimmed().startsWith("##"))
                    && it.text() != ""
                    && it.text() != "\n") {
                sHtmlBody += "<p>" + it.text() + "</p>\n";
            }
        }
    }

    sHtmlBody = this->reinstertNoTranslate(sHtmlBody);

    // File name
    QFileInfo fi(sActFile);
    QString sFilename;
    if ("" == sActFile) {
        sFilename = tr("Untitled", "No file name set");
    } else {
        sFilename = fi.fileName();
    }

    // Replace template tags
    // Copy needed, otherwise %tags% will be replaced/removed in template!
    QString sTemplateCopy(m_pTemplates->getPreviewTemplate());
    sTemplateCopy = sTemplateCopy.replace("%filename%", sFilename);
    QString sRevTextCopy(m_pTemplates->getTransRev());  // Copy needed!
    sRevTextCopy= sRevTextCopy.replace(
                "%date%",
                QDate::currentDate().toString("dd.MM.yyyy"))
            .replace("%time%", QTime::currentTime().toString("hh:mm"));
    sTemplateCopy = sTemplateCopy.replace("%revtext%", sRevTextCopy);
    sTemplateCopy = sTemplateCopy.replace("%tagtext%",
                                          m_pTemplates->getTransTag() + " "
                                          + sWikitags);
    sTemplateCopy = sTemplateCopy.replace("%content%", sHtmlBody);

    return sTemplateCopy;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceTemplates(QTextDocument *p_rawDoc) {
    qDebug() << Q_FUNC_INFO;

    QStringList sListTplRegExp;
    sListTplRegExp << "\\[\\[" +  m_pTemplates->getTransTemplate()
                      + "\\(.+\\)\\]\\]";
//                 << "\\{\\{\\{#!" +  m_pTemplates->getTransTemplate()
//                    + " .+\\}\\}\\}";
    QString sMyDoc = p_rawDoc->toPlainText();
    QString sMacro;
    QStringList sListArguments;
    int nPos = 0;

    QRegExp findTemplate(sListTplRegExp[0], Qt::CaseInsensitive);
    findTemplate.setMinimal(true);

    while ((nPos = findTemplate.indexIn(sMyDoc, nPos)) != -1) {
        sMacro =  findTemplate.cap(0);
        // qDebug() << "CAPTURED:" << sMacro;

        for (int i = 0; i < m_pTemplates->getListTplNamesINY().size(); i++) {
            if (sMacro.startsWith("[[" +  m_pTemplates->getTransTemplate()
                                  + "(" + m_pTemplates->getListTplNamesINY()[i],
                                  Qt::CaseInsensitive)) {
                qDebug() << "Found known macro:"
                         << m_pTemplates->getListTplNamesINY()[i];
                sMacro.remove("[[" +  m_pTemplates->getTransTemplate() + "(");
                sMacro.remove(")]]");

                // Split by ',' but DON'T split quoted strings containing commas
                QStringList tmpList = sMacro.split(QRegExp("\""));  // Split "
                bool bInside = false;
                sListArguments.clear();
                foreach (QString s, tmpList) {
                    if (bInside) {
                        // If 's' is inside quotes, get the whole string
                        sListArguments.append(s);
                    } else {
                        // If 's' is outside quotes, get the splitted string
                        sListArguments.append(s.split(QRegExp(",+"),
                                                      QString::SkipEmptyParts));
                    }
                    bInside = !bInside;
                }

                sListArguments.removeAll(" ");
                sListArguments.removeFirst();  // Remove template name

                // Replace arguments
                sMacro = m_pTemplates->getListTemplatesINY()[i];
                for (int k = 0; k < sListArguments.size(); k++) {
                    sMacro.replace("<@ $arguments." + QString::number(k)
                                   + " @>", sListArguments[k].trimmed());
                }

                sMyDoc.replace(nPos, findTemplate.matchedLength(), sMacro);
            }
        }

        // Go on with new start position
        nPos += sMacro.length();
        // nPos += findTemplate.matchedLength();
    }

    // Replace p_rawDoc with document with formated text
    p_rawDoc->setPlainText(sMyDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// REPLACE TEXT FORMATS
void CParser::replaceTextformat(QTextDocument *p_rawDoc) {
    QString sMyDoc = p_rawDoc->toPlainText();
    bool bFoundStart;

    QRegExp patternTextformat;
    int myindex;
    int iLength;
    QString sFormatedText, sTmpRegExp;
    unsigned int nNoTranslate = 0;
    m_sListNoTranslate.clear();

    patternTextformat.setCaseSensitivity(Qt::CaseInsensitive);
    // Find "smallest" match.
    // See: http://doc.qt.nokia.com/qq/qq01-seriously-weird-qregexp.html
    patternTextformat.setMinimal(true);

    for (int i = 0; i < m_pTemplates->getListFormatStart().size(); i++) {
        bFoundStart = true;

        // Start and end is not identical
        if (m_pTemplates->getListFormatStart()[i] != m_pTemplates->getListFormatEnd()[i]) {
            if (!m_pTemplates->getListFormatStart()[i].startsWith("RegExp=")) {
                sMyDoc.replace(m_pTemplates->getListFormatStart()[i],
                               m_pTemplates->getListFormatHtmlStart()[i]);
            } else {
                sTmpRegExp = m_pTemplates->getListFormatStart()[i];
                sTmpRegExp.remove("RegExp=");
                sTmpRegExp = sTmpRegExp.trimmed();
                patternTextformat.setPattern(sTmpRegExp);

                myindex = patternTextformat.indexIn(sMyDoc);

                while (myindex >= 0) {
                    QString sCap("");
                    iLength = patternTextformat.matchedLength();
                    sFormatedText = patternTextformat.cap();
                    sCap = patternTextformat.cap(1);

                    if (sCap == "") {
                        sMyDoc.replace(myindex, iLength,
                                       m_pTemplates->getListFormatHtmlStart()[i]);
                    } else {
                        sMyDoc.replace(myindex, iLength,
                                       m_pTemplates->getListFormatHtmlStart()[i].arg(sCap));
                    }

                    // Go on with RegExp-Search
                    myindex = patternTextformat.indexIn(sMyDoc, myindex + iLength);
                }
            }
            if (!m_pTemplates->getListFormatEnd()[i].startsWith("RegExp=")) {
                sMyDoc.replace(m_pTemplates->getListFormatEnd()[i],
                               m_pTemplates->getListFormatHtmlEnd()[i]);
            } else {
                sTmpRegExp = m_pTemplates->getListFormatEnd()[i];
                sTmpRegExp.remove("RegExp=");
                sTmpRegExp = sTmpRegExp.trimmed();
                patternTextformat.setPattern(sTmpRegExp);

                myindex = patternTextformat.indexIn(sMyDoc);

                while (myindex >= 0) {
                    QString sCap("");
                    iLength = patternTextformat.matchedLength();
                    sFormatedText = patternTextformat.cap();
                    sCap = patternTextformat.cap(1);

                    if (sCap == "") {
                        sMyDoc.replace(myindex, iLength,
                                       m_pTemplates->getListFormatHtmlEnd()[i]);
                    } else {
                        sMyDoc.replace(myindex, iLength,
                                       m_pTemplates->getListFormatHtmlEnd()[i].arg(sCap));
                    }

                    // Go on with RegExp-Search
                    myindex = patternTextformat.indexIn(sMyDoc, myindex + iLength);
                }
            }
        } else {  // Start and end is identical
            if (!m_pTemplates->getListFormatStart()[i].startsWith("RegExp=")) {
                while (-1 != sMyDoc.indexOf(m_pTemplates->getListFormatStart()[i])) {
                    if (bFoundStart) {
                        sMyDoc.replace(sMyDoc.indexOf(m_pTemplates->getListFormatStart()[i]),
                                       m_pTemplates->getListFormatStart()[i].length(),
                                       m_pTemplates->getListFormatHtmlStart()[i]);
                    } else {
                        sMyDoc.replace(sMyDoc.indexOf(m_pTemplates->getListFormatStart()[i]),
                                       m_pTemplates->getListFormatStart()[i].length(),
                                       m_pTemplates->getListFormatHtmlEnd()[i]);
                    }
                    bFoundStart = !bFoundStart;
                }
            } else {
                sTmpRegExp = m_pTemplates->getListFormatStart()[i];
                sTmpRegExp.remove("RegExp=");
                sTmpRegExp = sTmpRegExp.trimmed();
                patternTextformat.setPattern(sTmpRegExp);

                myindex = patternTextformat.indexIn(sMyDoc);

                while (myindex >= 0) {
                    QString sCap("");
                    iLength = patternTextformat.matchedLength();
                    sFormatedText = patternTextformat.cap();
                    sCap = patternTextformat.cap(1);

                    if (sCap == "") {
                        if (bFoundStart) {
                            sMyDoc.replace(myindex, iLength,
                                           m_pTemplates->getListFormatHtmlStart()[i]);
                        } else {
                            sMyDoc.replace(myindex, iLength,
                                           m_pTemplates->getListFormatHtmlEnd()[i]);
                        }
                    } else {
                        if (bFoundStart) {
                            sMyDoc.replace(myindex, iLength,
                                           m_pTemplates->getListFormatHtmlStart()[i].arg(sCap));
                        } else {
                            sMyDoc.replace(myindex, iLength,
                                           m_pTemplates->getListFormatHtmlEnd()[i].arg(sCap));
                        }
                    }

                    // Go on with RegExp-Search
                    myindex = patternTextformat.indexIn(sMyDoc, myindex + iLength);
                }
            }
        }

        // Filter monotype code block which should not be formated
        patternTextformat.setPattern("<code class=\"notranslate\">.+</code>");
        myindex = patternTextformat.indexIn(sMyDoc);

        while (myindex >= 0) {
            iLength = patternTextformat.matchedLength();
            sFormatedText = patternTextformat.cap();
            m_sListNoTranslate << sFormatedText;
            sMyDoc.replace(sFormatedText, "%%" +
                           QString::number(nNoTranslate) + "%%");
            nNoTranslate++;
            myindex = patternTextformat.indexIn(sMyDoc, myindex + iLength);
        }
    }

    // Replace p_rawDoc with document with formated text
    p_rawDoc->setPlainText(sMyDoc);
}

QString CParser::reinstertNoTranslate(const QString &sRawDoc) {
    QString sMyDoc = sRawDoc;

    // Reinsert filtered monotype codeblock
    for (int i = 0; i < m_sListNoTranslate.size(); i++) {
        sMyDoc.replace("%%" + QString::number(i) + "%%", m_sListNoTranslate[i]);
    }

    return sMyDoc;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Replace KEYS
void CParser::replaceKeys(QTextDocument *p_rawDoc) {
    QRegExp findKeys("\\[\\[" + m_pTemplates->getTransTemplate()
                     + "\\(Tasten,[\\w\\s\\?\\-\\=\\'\\,\\.\\`\\\"\\^\\<"
                     "\\[\\]\\#\\+]+\\)\\]\\]");
    QString sMyDoc = p_rawDoc->toPlainText();
    int iLength;
    QString sTmpKey;
    QString sTmpKeyCode("");
    bool bIsMouseButton;
    QStringList sListTmpKeys;

    int myindex = findKeys.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findKeys.matchedLength();
        sTmpKey = findKeys.cap();
        sTmpKey.remove("[[" + m_pTemplates->getTransTemplate()
                       + trUtf8("(Tasten,"));
        sTmpKey.remove(")]]");

        sListTmpKeys.clear();
        sListTmpKeys = sTmpKey.split("+");
        sTmpKey.clear();

        for (int i = 0; i < sListTmpKeys.size(); i++) {
            bIsMouseButton = false;
            // Remove possible spaces before and after string
            sListTmpKeys[i] = sListTmpKeys[i].trimmed();
            sListTmpKeys[i][0] = sListTmpKeys[i][0].toLower();

            if (sListTmpKeys[i] == "backspace"
                    || sListTmpKeys[i] == trUtf8("löschen")
                    || sListTmpKeys[i] == trUtf8("rückschritt")) {
                sTmpKeyCode = "&#9003;";
            } else if (sListTmpKeys[i] == "ctrl") {
                sTmpKeyCode = "Strg";
            } else if (sListTmpKeys[i] == "del"
                     || sListTmpKeys[i] == "delete"
                     || sListTmpKeys[i] == "entfernen") {
                sTmpKeyCode = "Entf";
            } else if (sListTmpKeys[i] == "return"
                     || sListTmpKeys[i] == "enter"
                     || sListTmpKeys[i] == "eingabe") {
                sTmpKeyCode = "&#9166;";
            } else if (sListTmpKeys[i] == "escape") {
                sTmpKeyCode = "Esc";
            } else if (sListTmpKeys[i] == "eckig_auf") {
                sTmpKeyCode = "[";
            } else if (sListTmpKeys[i] == "eckig_zu") {
                sTmpKeyCode = "]";
            } else if (sListTmpKeys[i] == "bild auf"
                       || sListTmpKeys[i] == "bild-auf"
                       || sListTmpKeys[i] == "bild-rauf") {
                sTmpKeyCode = "Bild &uarr;";
            } else if (sListTmpKeys[i] == "bild ab"
                       || sListTmpKeys[i] == "bild-ab"
                       || sListTmpKeys[i] == "bild-runter") {
                sTmpKeyCode = "Bild &darr;";
            } else if (sListTmpKeys[i] == "print"
                       || sListTmpKeys[i] == "prtsc"
                       || sListTmpKeys[i] == "sysrq") {
                sTmpKeyCode = "Druck";
            } else if (sListTmpKeys[i] == "mac"
                       || sListTmpKeys[i] == "appel"
                       || sListTmpKeys[i] == "apfel"
                       || sListTmpKeys[i] == "cmd") {
                sTmpKeyCode = "&#8984;";
            } else if (sListTmpKeys[i] == "caps"
                       || sListTmpKeys[i] == "feststell"
                       || sListTmpKeys[i] == "feststelltaste"
                       || sListTmpKeys[i] == trUtf8("groß")) {
                sTmpKeyCode = "&dArr;";
            } else if (sListTmpKeys[i] == "shift"
                       || sListTmpKeys[i] == "umschalt"
                       || sListTmpKeys[i] == "umsch") {
                sTmpKeyCode = "&uArr;";
            } else if (sListTmpKeys[i] == "at") {
                sTmpKeyCode = "&#64;";
            } else if (sListTmpKeys[i] == "cherry"
                     || sListTmpKeys[i] == "keyman") {
                sTmpKeyCode = "Keym&#64;n";
            } else if (sListTmpKeys[i] == "space"
                     || sListTmpKeys[i] == "leer"
                     || sListTmpKeys[i] == "leertaste"
                     || sListTmpKeys[i] == "leerzeichen"
                     || sListTmpKeys[i] == "leerschritt") {
                sTmpKeyCode = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                       "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
            } else if (sListTmpKeys[i] == "tab"
                       || sListTmpKeys[i] == "tabulator") {
                sTmpKeyCode = "Tab &#8644;";
            } else if (sListTmpKeys[i] == "win") {
                sTmpKeyCode = "Windows";
            } else if (sListTmpKeys[i] == "hoch"
                       || sListTmpKeys[i] == "auf"
                       || sListTmpKeys[i] == "up"
                       || sListTmpKeys[i] == "rauf"
                       || sListTmpKeys[i] == "pfeil-hoch") {
                sTmpKeyCode = "&uarr;";
            } else if (sListTmpKeys[i] == "runter"
                     || sListTmpKeys[i] == "ab"
                     || sListTmpKeys[i] == "down"
                     || sListTmpKeys[i] == "pfeil-runter"
                     || sListTmpKeys[i] == "pfeil-ab") {
                sTmpKeyCode = "&darr;";
            } else if (sListTmpKeys[i] == "links"
                       || sListTmpKeys[i] == "left"
                       || sListTmpKeys[i] == "pfeil-links") {
                sTmpKeyCode = "&larr;";
            } else if (sListTmpKeys[i] == "rechts"
                     || sListTmpKeys[i] == "right"
                     || sListTmpKeys[i] == "pfeil-rechts") {
                sTmpKeyCode = "&rarr;";
            } else if (sListTmpKeys[i] == "\",\""
                     || sListTmpKeys[i] == "\',\'") {
                sTmpKeyCode = ",";
            } else if (sListTmpKeys[i] == "minus") {
                sTmpKeyCode = "-";
            } else if (sListTmpKeys[i] == "plus") {
                sTmpKeyCode = "+";
            } else if (sListTmpKeys[i] == "\"`\"") {
                sTmpKeyCode = "`";
            } else if (sListTmpKeys[i] == "sz") {
                sTmpKeyCode = "&szlig;";
            } else if (sListTmpKeys[i] == "gleich") {
                sTmpKeyCode = "=";
            } else if (sListTmpKeys[i] == "num"
                       || sListTmpKeys[i] == "num-taste"
                       || sListTmpKeys[i] == "num-Taste"
                       || sListTmpKeys[i] == "num-lock-taste"
                       || sListTmpKeys[i] == "num-Lock-Taste") {
                sTmpKeyCode = "num &dArr;";
            } else if (sListTmpKeys[i] == "fragezeichen") {
                sTmpKeyCode = "?";
            } else if (sListTmpKeys[i] == "break") {
                sTmpKeyCode = "Pause";
            } else if (sListTmpKeys[i] == "rollen"
                       || sListTmpKeys[i] == "bildlauf") {
                sTmpKeyCode = "&dArr; Rollen";
            } else if (sListTmpKeys[i] == "slash") {
                sTmpKeyCode = "/";
            } else if (sListTmpKeys[i] == "any") {
                sTmpKeyCode = "ANY KEY";
            } else if (sListTmpKeys[i] == "panic") {
                sTmpKeyCode = "PANIC";
            } else if (sListTmpKeys[i] == "koelsch") {
                sTmpKeyCode = "K&ouml;lsch";
            } else {
                if (sListTmpKeys[i] == "lmt"
                        || sListTmpKeys[i] == "lmb") {
                    bIsMouseButton = true;
                    sTmpKey += "<img src=\"img/wiki/mouse_left.png\" alt="
                            "\"linke Maustaste\" class=\"image-default\" />";
                } else if (sListTmpKeys[i] == "rmt"
                           || sListTmpKeys[i] == "rmb") {
                    bIsMouseButton = true;
                    sTmpKey += "<img src=\"img/wiki/mouse_right.png\" alt="
                            "\"rechte Maustaste\" class=\"image-default\" />";
                } else if (sListTmpKeys[i] == "mmt"
                           || sListTmpKeys[i] == "mmb") {
                    bIsMouseButton = true;
                    sTmpKey += "<img src=\"img/wiki/mouse_midd.png\" alt="
                            "\"mittlere Maustaste\" class=\"image-default\" />";
                } else {
                    // Everything else: First character to Upper
                    // first characters had been changed to lower at beginning
                    sListTmpKeys[i][0] = sListTmpKeys[i][0].toUpper();
                    sTmpKeyCode = sListTmpKeys[i];
                }
            }

            if (!bIsMouseButton) {
                sTmpKey += "<span class=\"key\">" + sTmpKeyCode + "</span>";
            }

            // "+" between keys
            if (i != sListTmpKeys.size()-1) {
                sTmpKey += " + ";
            }
        }
        // Replace text with Html code
        sMyDoc.replace(myindex, iLength, sTmpKey);

        // Go on with RegExp-Search
        myindex = findKeys.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace p_rawDoc with document with HTML links
    p_rawDoc->setPlainText(sMyDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// MACROS [[Vorlage(...)]]
QString CParser::parseMacro(QTextBlock actParagraph) {
    QString sParagraph = actParagraph.text();
    // QString sParagraph = actParagraph;

    // Remove brackets
    sParagraph.remove("[[" +  m_pTemplates->getTransTemplate() + "(",
                      Qt::CaseSensitive);
    sParagraph.remove(")]]");

    // Separate elementes from macro (between ,)
    QStringList sListElements = sParagraph.split(",");

    QString sOutput("<strong>ERROR: Found unknown item [["
                    +  m_pTemplates->getTransTemplate() +
                    "(" + sListElements[0] + "</strong>\n");

    // Remove every quote sign and spaces before / behind strings (trimmed)
    for (int i = 0; i < sListElements.size(); i++) {
        sListElements[i].remove("\"");
        sListElements[i] = sListElements[i].trimmed();
    }

    // Under construction (Baustelle)
    if (sListElements[0].toLower() == trUtf8("Baustelle").toLower()
            || sListElements[0].toLower() == trUtf8("InArbeit").toLower()) {
        // Get and check date
        QString sDate;
        if (sListElements.size() >= 2) {
            // Extract date
            QStringList sListDate = sListElements[1].split(".");
            // Wrong date format
            if (3 != sListDate.size()) {
                sDate = "";
            } else {  // Correct number of date elements
                // Wrong date
                if (sListDate[0].toInt() <= 0
                        || sListDate[0].toInt() > 31
                        || sListDate[1].toInt() <= 0
                        || sListDate[1].toInt() > 12
                        || sListDate[2].toInt() <= 0) {
                    sDate = "";
                } else {  // Correct date
                    // Add 0 to date if < 10
                    for (int i = 0; i < sListDate.size(); i++) {
                        if (sListDate[i].toInt() < 10
                                && !sListDate[i].startsWith("0")) {
                            sListDate[i] = "0" + sListDate[i];
                        }
                    }
                    sDate = sListDate[0] +"."+ sListDate[1] +"."+ sListDate[2];
                }
            }

            QString sTmpUrl = m_sInyokaUrl;
            sTmpUrl.remove("wiki.");
            QString sLinkUser("");
            int iCntUser;
            if (sDate == "") {
                // Entry with index 1 = first user (no correct date given)
                iCntUser = 1;
            } else {
                iCntUser = 2;  // Entry after date = first user
            }

            // Generate user list
            for (; iCntUser < sListElements.size(); iCntUser++)  {
                // Replace possible spaces
                sListElements[iCntUser].replace(" ", "_");
                sLinkUser += "<a href=\"" + sTmpUrl + "/user/"
                        + sListElements[iCntUser]
                        + "/ \" class=\"crosslink user\">"
                        + sListElements[iCntUser] + "</a>";
                // Comma or "and" between users
                if (iCntUser == sListElements.size() - 2) {
                    sLinkUser += " " + trUtf8("und") + " ";
                } else {
                    sLinkUser += ", ";
                }
            }
            sOutput = trUtf8("Dieser Artikel wird momentan von %1 erstellt.")
                    .arg(sLinkUser);

            if (sDate != "") {
                sOutput += " "
                        + trUtf8("Als Fertigstellungsdatum wurde der %1 angegeben.")
                        .arg(sDate);
            } else {
                sOutput += " "
                        + trUtf8("Solltest du dir nicht sicher sein, "
                                 "ob an dieser Anleitung noch gearbeitet wird, "
                                 "kontrolliere das Datum der letzten Änderung "
                                 "und entscheide, wie du weiter vorgehst.");
            }
        } else {  // No parameter given
            sOutput = trUtf8("Dieser Artikel wird momentan erstellt. "
                             "Solltest du dir nicht sicher sein, ob an dieser "
                             "Anleitung noch gearbeitet wird, kontrolliere das "
                             "Datum der letzten Änderung und entscheide, wie "
                             "du weiter vorgehst.");
        }

        sOutput = insertBox("box workinprogress",
                            trUtf8("Artikel in Arbeit"),
                            sOutput,
                            trUtf8("Insbesondere heißt das, dass dieser "
                                   "Artikel noch nicht fertig ist und dass "
                                   "wichtige Teile fehlen oder sogar falsch "
                                   "sein können. Bitte diesen Artikel nicht "
                                   "als Anleitung für Problemlösungen "
                                   "benutzen."));
    }
    // ------------------------------------------------------------------------

    // TESTED (Getestet)
    else if (sListElements[0].toLower() == trUtf8("Getestet").toLower()) {
        if (sListElements.size() >= 2) {
            // Article untested
            if (sListElements[1] == "" || sListElements[1] == " ") {
                sOutput = trUtf8("Dieser Artikel ist mit keiner aktuell "
                                 "unterstützten Ubuntu-Version getestet! Bitte "
                                 "diesen Artikel testen und das getestet-Tag "
                                 "entsprechend anpassen.");
            } else if (sListElements[1].toLower() == trUtf8("general")
                       .toLower()) {  // Tested "general"
                sOutput = trUtf8("Dieser Artikel ist größtenteils für alle "
                                 "Ubuntu-Versionen gültig.");
            } else {  // Article tested with ubuntu versions
                sOutput = "<ul>\n";

                for (int i = 1; i < sListElements.size(); i++) {
                    sOutput += "<li>\n"
                               "<p>\n";
                    if (sListElements[i].toLower() == "raring") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Raring_Ringtail\" class=\"internal\">"
                                "Ubuntu 13.04</a> \"Raring Ringtail\"\n";
                    } else if (sListElements[i].toLower() == "quantal") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Quantal_Quetzal\" class=\"internal\">"
                                "Ubuntu 12.10</a> \"Quantal Quetzal\"\n";
                    } else if (sListElements[i].toLower() == "precise") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Precise_Pangolin\" class=\"internal\">"
                                "Ubuntu 12.04</a> \"Precise Pangolin\"\n";
                    } else if (sListElements[i].toLower() == "oneiric") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Oneiric_Ocelot\" class=\"internal\">"
                                "Ubuntu 11.10</a> \"Oneiric Ocelot\"\n";
                    } else if (sListElements[i].toLower() == "natty") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Natty_Narwhal\" class=\"internal\">"
                                "Ubuntu 11.04</a> \"Natty Narwhal\"\n";
                    } else if (sListElements[i].toLower() == "maverick") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Maverick_Meerkat\" class=\"internal\">"
                                "Ubuntu 10.10</a> \"Maverick Meerkat\"\n";
                    } else if (sListElements[i].toLower() == "lucid") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Lucid_Lynx\" class=\"internal\">"
                                "Ubuntu 10.04</a> \"Lucid Lynx\"\n";
                    } else if (sListElements[i].toLower() == "hardy") {
                        sOutput += "<a href=\"" + m_sInyokaUrl
                                + "/Hardy_Heron\" class=\"internal\">"
                                "Ubuntu 8.04</a> \"Hardy Heron\"\n";
                    } else {
                        sOutput +=  "Unknown Version\n";
                    }

                    sOutput += "</p>\n"
                               "</li>\n";
                }
                sOutput += "</ul>\n";
            }
        } else {
            sOutput = trUtf8("Dieser Artikel ist mit keiner aktuell "
                             "unterstützten Ubuntu-Version getestet! Bitte "
                             "diesen Artikel testen und das getestet-Tag "
                             "entsprechend anpassen.");
        }

        sOutput = insertBox("box tested_for",
                            trUtf8("Dieser Artikel wurde für die folgenden "
                                   "Ubuntu-Versionen getestet:"),
                            sOutput);
    }
    // ------------------------------------------------------------------------

    // ADVANCED (Fortgeschritten)
    else if (sListElements[0].toLower() == trUtf8("Fortgeschritten").toLower()) {
        sOutput = insertBox("box advanced",
                            trUtf8("Artikel für fortgeschrittene Anwender"),
                            trUtf8("Dieser Artikel erfordert mehr Erfahrung im "
                                   "Umgang mit Linux und ist daher nur für "
                                   "fortgeschrittene Benutzer gedacht."));
    }
    // ------------------------------------------------------------------------

    // PACKAGE-LIST (Pakete-Makro)  -- OBSOLETE --
    else if (sListElements[0].toLower() == trUtf8("Pakete").toLower()) {
        // Generate output
        sOutput = "<div class=\"package-list\">\n"
                  "<div class=\"contents\">\n"
                  "<p>";
        sOutput += trUtf8("Paketliste zum Kopieren:");
        sOutput += " <strong>apt-get</strong></p>\n"
                   "<div class=\"bash\">"
                   "<div class=\"contents\">\n"
                   "<pre class=\"notranslate\"> sudo apt-get install";
        for (int i = 1; i < sListElements.size(); i++) {
            sOutput += " " + sListElements[i].trimmed();
        }
        sOutput += "</pre>\n</div>\n</div>\n</div>\n</div>\n";
    }
    // ------------------------------------------------------------------------

    // IMPROVABLE (Ausbaufähig)
    else if (sListElements[0].toLower() == trUtf8("Ausbaufähig").toLower()) {
        sOutput = "";
        // Remark available?
        if (sListElements.size() >= 2) {
            sOutput = sListElements[1];
        }

        sOutput = insertBox("box improvable",
                            trUtf8("Ausbaufähige Anleitung"),
                            trUtf8("Dieser Anleitung fehlen noch einige "
                                   "Informationen. Wenn Du etwas verbessern "
                                   "kannst, dann editiere den Beitrag, um die "
                                   "Qualität des Wikis noch weiter zu "
                                   "verbessern."),
                            sOutput);
    }
    // ------------------------------------------------------------------------

    // FIXME (Fehlerhaft)
    else if (sListElements[0].toLower() == trUtf8("Fehlerhaft").toLower()) {
        sOutput = "";
        // Remark available?
        if (sListElements.size() >= 2) {
            sOutput = sListElements[1];
        }

        sOutput = insertBox("box fixme",
                            trUtf8("Fehlerhafte Anleitung"),
                            trUtf8("Diese Anleitung ist fehlerhaft. Wenn du "
                                   "weißt, wie du sie ausbessern kannst, nimm "
                                   "dir bitte die Zeit und bessere sie aus."),
                            sOutput);
    }
    // ------------------------------------------------------------------------

    // LEFT (Verlassen)
    else if (sListElements[0].toLower() == trUtf8("Verlassen").toLower()) {
        sOutput = "";
        // Remark available?
        if (sListElements.size() >= 2) {
            sOutput = sListElements[1];
        }

        sOutput = insertBox("box left",
                            trUtf8("Verlassene Anleitung"),
                            trUtf8("Dieser Artikel wurde von seinem Ersteller "
                                   "verlassen und wird nicht mehr weiter von "
                                   "ihm gepflegt. Wenn Du den Artikel "
                                   "fertigstellen oder erweitern kannst, dann "
                                   "bessere ihn bitte aus."),
                            sOutput);
    }
    // ------------------------------------------------------------------------

    // ARCHIVED (Archiviert)
    else if (sListElements[0].toLower() == trUtf8("Archiviert").toLower()) {
        sOutput = "";
        // Remark available?
        if (sListElements.size() >= 2) {
            sOutput = sListElements[1];
        }

        sOutput = insertBox("box improvable",
                            trUtf8("Archivierte Anleitung"),
                            trUtf8("Dieser Artikel wurde archiviert, da er - "
                                   "oder Teile daraus - nur noch unter einer "
                                   "älteren Ubuntu-Version nutzbar ist. Diese "
                                   "Anleitung wird vom Wiki-Team weder auf "
                                   "Richtigkeit überprüft noch anderweitig "
                                   "gepflegt. Zusätzlich wurde der Artikel für "
                                   "weitere Änderungen gesperrt."),
                            sOutput);
    }
    // ------------------------------------------------------------------------

    // COPY (Kopie)
    else if (sListElements[0].toLower() == trUtf8("Kopie").toLower()) {
        sOutput = "";
        // Generate temp. link
        if (sListElements.size() >= 2) {
            // Replace possible spaces
            sListElements[1].replace(" ", "_");
            sOutput = "<a href=\"" + m_sInyokaUrl
                    + "/" + trUtf8("Baustelle") + "/" + sListElements[1]
                    + "\" class=\"internal missing\">" + trUtf8("Baustelle")
                    + "/" + sListElements[1] + "</a>";
        }

        sOutput = insertBox("box warning",
                            trUtf8("Achtung!"),
                            trUtf8("Diese Seite wird aktuell überarbeitet. "
                                   "Bitte hier keine Änderungen mehr vornehmen"
                                   ", sondern in %1!").arg(sOutput));
    }
    // ------------------------------------------------------------------------

    // WORK IN PROGRESS (Überarbeitung)
    else if (sListElements[0].toLower() == trUtf8("Überarbeitung").toLower()) {
        sOutput = trUtf8("Dieser Artikel wird momentan überarbeitet.");

        // Correct number of elements?
        if (sListElements.size() >= 4) {
            // Replace possible spaces
            for (int i = 1; i < sListElements.size(); i++) {
                sListElements[i].replace(" ", "_");
            }

            QString sTmpUrl = m_sInyokaUrl;
            sTmpUrl.remove("wiki.");
            // Generate output
            sOutput += "<ul>";
            sOutput += "<li><p>" + trUtf8("Geplante Fertigstellung:") + " "
                    + sListElements[1] + "</p></li>\n";
            QString sTmpLink = "<a href=\"" + m_sInyokaUrl + "/"
                    + sListElements[2] + "\" class=\"internal missing\"> "
                    + sListElements[2] +" </a>";
            sOutput += "<li><p>" + trUtf8("Derzeit gültiger Artikel:") + " "
                    + sTmpLink + "</p></li>\n";

            // Generate user list
            QString sTmpLink2("");
            for (int i = 3; i < sListElements.size(); i++) {
                sTmpLink2 += "<a href=\"" + sTmpUrl + "/user/"
                        + sListElements[i] + "/ \" class=\"crosslink user\">"
                        + sListElements[i] + "</a>";
                // Comma or "and" between users
                if (i == sListElements.size() - 2) {
                    sTmpLink2 += " " + trUtf8("und") + " ";
                } else {
                    sTmpLink2 += ", ";
                }
            }
            sOutput += "<li><p>" + trUtf8("Bearbeiter:")
                    + sTmpLink2 + "</p></li>\n";
            sOutput += "</ul>\n";
            QString sTmpLink3 = "<a href=\"" + m_sInyokaUrl
                    + "/" + trUtf8("Baustelle") + "/" + sListElements[2]
                    + "?action=log\" class=\"crosslink\">"
                    + trUtf8("letzten Änderung") + "</a>";
            sOutput += "<p>" + trUtf8("Solltest du dir nicht sicher sein, ob "
                                      "an dieser Anleitung noch gearbeitet "
                                      "wird, kontrolliere das Datum der %1 und "
                                      "entscheide, wie du weiter vorgehst.")
                    .arg(sTmpLink3) + "</p>\n";
        }

        sOutput = insertBox("box workinprogress",
                            trUtf8("Artikel wird überarbeitet"),
                            sOutput,
                            trUtf8("Insbesondere heißt das, dass dieser "
                                   "Artikel noch nicht fertig ist und dass "
                                   "wichtige Teile fehlen oder sogar falsch "
                                   "sein können. Bitte diesen Artikel nicht "
                                   "als Anleitung für Problemlösungen "
                                   "benutzen!"));
    }
    // ------------------------------------------------------------------------

    // THIRD-PARTY SOURCE / PACKAGE / SOFTWARE WARNING (Fremdquellen /
    // -pakete / -software Warnung)
    else if (sListElements[0].toLower() == trUtf8("Fremd").toLower()) {
        QString sRemark("");
        sOutput = "";

        if (sListElements.size() >= 2) {
            // Package
            if (sListElements[1].toLower() == trUtf8("Paket").toLower()) {
                sOutput = "<p><a href=\"" + m_sInyokaUrl
                        + "/" + trUtf8("Fremdquellen")
                        + "\" class=\"internal\">" + trUtf8("Fremdpakete")
                        + "</a> " + trUtf8("können das System gefährden.")
                        + "</p>\n";
            } else if (sListElements[1].toLower() == trUtf8("Quelle").toLower()) {
                // Source
                sOutput = "<p>" + trUtf8("Zusätzliche") + " <a href=\""
                        + m_sInyokaUrl + "/"
                        + trUtf8("Fremdquellen") + "\" class=\"internal\">"
                        + trUtf8("Fremdquellen") + "</a> "
                        + trUtf8("können das System gefährden.") + "</p>\n";
            } else if (sListElements[1].toLower() == trUtf8("Software").toLower()) {
                // Software
                sOutput = "<p><a href=\"" + m_sInyokaUrl + "/"
                        + trUtf8("Fremdsoftware") + "\" class=\"internal\">"
                        + trUtf8("Fremdsoftware") + "</a> "
                        + trUtf8("kann das System gefährden.") + "</p>\n";
            }
            // Remark available
            if (sListElements.size() >= 3) {
                sRemark = sListElements[2];
            }
        }

        sOutput = insertBox("box warning",
                            trUtf8("Hinweis!"),
                            sOutput, sRemark);
    }
    // ------------------------------------------------------------------------

    // PPA
    else if (sListElements[0].toLower() == trUtf8("PPA").toLower()) {
        QString sOutsideBox("");
        QString sRemark("");
        QString sUser("");
        QString sPPA("");
        sOutput = "";
        if (sListElements.size() >= 2) {
            // Replace possible spaces
            sListElements[1].replace(" ", "_");
            if (sListElements[1].contains('/')) {
                QStringList sList;
                sList = sListElements[1].split("/");
                sUser = sList[0];
                sPPA = sList[1];
            } else if (sListElements.size() >= 3) {
                sListElements[2].replace(" ", "_");
                sUser = sListElements[1];
                sPPA = sListElements[2];
            } else {
                sUser = sListElements[1];
                sPPA = sListElements[1];
            }

            // Generate output
            sOutsideBox = "<p>" + trUtf8("Adresszeile zum")
                    + " <a href=\"" + m_sInyokaUrl + "/"
                    + trUtf8("Paketquellen_freischalten/PPA#PPA-hinzufuegen")
                    + "\" class=\"internal\">" + trUtf8("Hinzufügen") + "</a> "
                    + trUtf8("des PPAs:") + "</p>";
            sOutsideBox += "<ul>\n<li>\n";
            sOutsideBox += "<p><strong>ppa:" + sUser + "/" + sPPA + "</strong></p>\n";
            sOutsideBox += "</li>\n</ul>";

            sOutput = trUtf8("Zusätzliche") + " <a href=\""
                    + m_sInyokaUrl + "/" + trUtf8("Fremdquellen")
                    + "\" class=\"internal\">" + trUtf8("Fremdquellen")
                    + "</a> " + trUtf8("können das System gefährden.");
            QString sTmpLink = "<img src=\"img/interwiki/ppa.png\" "
                    "class=\"image-default\" alt=\"PPA\" /> "
                    "<a href=\"https://launchpad.net/~" + sUser
                     + "/+archive/" + sPPA + "\" rel=\"nofollow\" "
                    "class=\"external\">" + trUtf8("PPA Beschreibung") + "</a>";
            QString sTmpLink2 = "<a href=\"https://launchpad.net/~"
                    + sUser + "\" class=\"interwiki "
                    "interwiki-lpuser\">" + sUser + "</a>";
            sRemark = trUtf8("Ein PPA unterstützt nicht zwangsläufig alle "
                      "Ubuntu-Versionen. Weitere Informationen bietet "
                      "die %1 vom Benutzer/Team %2.")
                    .arg(sTmpLink).arg(sTmpLink2);
        }

        sOutput = sOutsideBox + insertBox("box warning",
                                          trUtf8("Hinweis!"),
                                          sOutput, sRemark )
                + "<p>" + trUtf8("Damit Pakete aus dem PPA genutzt werden "
                                 "können, müssen die Paketquellen neu")
                + " <a href=\"" + m_sInyokaUrl + "/"
                + trUtf8("apt/apt-get#apt-get-update") + "\" "
                "class=\"internal\">" + trUtf8("eingelesen") + "</a> "
                + trUtf8("werden.") + "</p>";
    }
    // ------------------------------------------------------------------------

    // AUTHENTICATE THIRD-PARTY REPO (Fremdquelle authentifizieren)
    else if (sListElements[0].toLower() == trUtf8("Fremdquelle-auth").toLower()) {
        if (sListElements.size() == 2) {
            // Key
            if (sListElements[1].startsWith("key")) {
                sListElements[1].remove("key");
                sListElements[1].remove(" ");

                QString sTmpLink = "<a href=\"" + m_sInyokaUrl
                        + "/" + trUtf8("Fremdquellen") + "\" "
                        "class=\"internal\">" + trUtf8("Fremdquelle") + "</a>";
                sOutput = "<p>" + trUtf8("Um die %1 zu authentifizieren, kann "
                                         "man den Signierungsschlüssel mit "
                                         "folgendem Befehl importieren:")
                        .arg(sTmpLink);
                sOutput += "</p>\n"
                           "<div class=\"bash\">\n"
                           "<div class=\"contents\">\n"
                           "<pre> sudo apt-key adv --recv-keys --keyserver "
                           "keyserver.ubuntu.com " + sListElements[1]
                           + " </pre>\n"
                           "</div>\n"
                           "</div>\n";
            } else {  // Url
                QString sTmpLink = "<a href=\"" + m_sInyokaUrl
                        + "/" + trUtf8("Fremdquellen") + "\" class="
                        "\"internal\">" + trUtf8("Fremdquelle") + "</a>";
                QString sTmpLink2 = "<a href=\"" + sListElements[1] + "\" "
                        "rel=\"nofollow\" class=\"external\">"
                        + trUtf8("Signierungsschlüssel herunterladen") + "</a>";
                QString sTmpLink3 = "<a href=\"" + m_sInyokaUrl
                        + "/" + trUtf8("Paketquellen_freischalten") + "\" "
                        "class=\"internal\">" + trUtf8("Paketverwaltung "
                                                       "hinzufügen") + "</a>";
                sOutput = "<p>" + trUtf8("Um die %1 zu authentifizieren, kann "
                                         "man entweder den %2 und in der %3 "
                                         "oder folgenden Befehl ausführen:")
                        .arg(sTmpLink).arg(sTmpLink2).arg(sTmpLink3) + "</p>\n";
                sOutput += "<div class=\"bash\">\n"
                           "<div class=\"contents\">\n"
                           "<pre>wget -q " + sListElements[1] + " -O- | sudo "
                           "apt-key add - </pre>\n"
                           "</div>\n"
                           "</div>\n";
            }
        }
    }

    // ------------------------------------------------------------------------

    // THIRD-PARTY REPO (Fremdquelle)
    else if (sListElements[0].toLower() == trUtf8("Fremdquelle").toLower()) {
        if (sListElements.size() >= 3) {
            // Generate output
            QString sTmpLink = "<a href=\"" + m_sInyokaUrl
                    + "/" + trUtf8("Fremdquellen") + "\" class=\"internal\">"
                    + trUtf8("Fremdquelle") + "</a>";
            QString sTmpLink2 = "<a href=\"" + m_sInyokaUrl + "/"
                    + trUtf8("Paketquellen_freischalten") + "\" "
                    "class=\"internal\">" + trUtf8("Paketquelle freischalten")
                    + "</a>";
            sOutput = "<p>" + trUtf8("Um aus der %1 zu installieren, muss man "
                                     "unabhängig von der Ubuntu-Version die "
                                     "folgende %2:")
                    .arg(sTmpLink).arg(sTmpLink2) + "</p>\n";

            QString sTmpLink3 = "<a href=\"" + m_sInyokaUrl + "/"
                    + trUtf8("Fremdquellen") + "\" class=\"internal\">"
                    + trUtf8("Fremdquelle") + "</a>";
            sOutput += insertBox("box warning",
                                 trUtf8("Hinweis!"),
                                 trUtf8("Zusätzliche %1 können das System "
                                        "gefährden.").arg(sTmpLink3));

            sOutput += "<div class=\"thirpartyrepo-outer\">\n"
                       "<div class=\"contents\">\n"
                       "<div class=\"selector\">\n"
                       "<strong>" + trUtf8("Version:") + " </strong>"
                       + sListElements[2] + "\n"
                       "</div>\n";

            if (sListElements.size() == 3) {
                sOutput += "<pre> deb " + sListElements[1] + " "
                        + sListElements[2] + " </pre>";
            } else {
                if (sListElements[3].startsWith(" ")) {
                    sListElements[3].remove(0, 1);
                }
                sOutput += "<pre> deb " + sListElements[1] + " "
                        + sListElements[2] + " " + sListElements[3] + "</pre>";
            }

            sOutput += "</div>\n"
                       "</div>\n";
        }
    }

    // ------------------------------------------------------------------------

    // THIRD-PARTY PACKAGE (Fremdpaket)
    else if (sListElements[0].toLower() == trUtf8("Fremdpaket").toLower()) {
        if (sListElements.size() >= 3) {
            // Case 1: [[Vorlage(Fremdpaket, Projekthoster,
            // Projektname, Ubuntuversion(en))]]
            if (!(sListElements[2].startsWith("http"))
                    && sListElements[2] != "dl") {
                // LAUNCHPAD
                if (sListElements[1].toLower() == "launchpad") {
                    sOutput = "<p>" + trUtf8("Beim <a href=\"%1/Launchpad\" class=\"internal\">Launchpad</a>-Projekt "
                                     "<a href=\"https://launchpad.net/%2\" class=\"interwiki interwiki-launchpad\">%3</a> "
                                     "werden <a href=\"https://launchpad.net/%4/+download\" class=\"interwiki interwiki-launchpad\">DEB-Pakete</a> "
                                     "angeboten. ")
                            .arg(m_sInyokaUrl)
                            .arg(sListElements[2])
                            .arg(sListElements[2])
                            .arg(sListElements[2]);
                } else if (sListElements[1].toLower() == "sourceforge") {
                    // SOURCEFORGE
                    sOutput = "<p>" + trUtf8("Beim <a href=\"http://de.wikipedia.org/wiki/SourceForge\" class=\"interwiki interwiki-wikipedia\">SourceForge</a>-Projekt "
                                     "<a href=\"http://sourceforge.net/projets/%1\" class=\"interwiki interwiki-sourceforge\">%2</a> "
                                     "werden <a href=\"http://sourceforge.net/projects/%3/files\" class=\"interwiki interwiki-sourceforge\">DEB-Pakete</a> "
                                     "angeboten. ")
                            .arg(sListElements[2])
                            .arg(sListElements[2])
                            .arg(sListElements[2]);
                } else if (sListElements[1].toLower() == "googlecode") {
                    // GOOGLE CODE
                    sOutput = "<p>" + trUtf8("Beim <a href=\"http://code.google.com/intl/de\" rel =\"nofollow\" class=\"external\">Google Code</a> "
                                     "<img src=\"img/flags/de.png\" alt=\"(de)\" /> -Projekt "
                                     "<a href=\"http://code.google.com/p/%1\" class=\"interwiki interwiki-googlecode\">%2</a> "
                                     "werden <a href=\"http://code.google.com/p/%3/downloads\" class=\"interwiki interwiki-googlecode\">DEB-Pakete</a> "
                                     "angeboten. ")
                            .arg(sListElements[2])
                            .arg(sListElements[2])
                            .arg(sListElements[2]);
                }
            }

            // Case 2: [[Vorlage(Fremdpaket, "Anbieter", URL
            // Downloadübersicht, Ubuntuversion(en))]]
            else if (sListElements[2].startsWith("http")) {
                sOutput = "<p>" + trUtf8("Von %1 werden <a href=\"%2\" class=\"external\">DEB-Pakete</a> <img src=\"img/flags/dl.png\" alt=\"(dl)\" /> angeboten. ")
                          .arg(sListElements[1]).arg(sListElements[2]);  // [http: ] WAS REPLACED AUTOMATICALY BEVORE!!!
            }

            // Case 3: [[Vorlage(Fremdpaket, "Anbieter", dl, URL zu einem Download, Ubuntuversion(en))]]
            else if (sListElements[2].startsWith("dl")) {
                sOutput = "<p>" + trUtf8("Von %1 werden folgende DEB-Pakete angeboten:").arg(sListElements[1]) + "</p>"
                          "<ul>\n<li><p><a href=\"" + sListElements[3] + "\" rel=\"nofollow\" class=\"external\">" + sListElements[3] + "</a> "
                          "<img src=\"img/flags/dl.png\" alt=\"(dl)\" /></p></li>\n</ul>\n";
            }
        }

        // No ubuntu version
        if ((sListElements.size() == 3
             && !sListElements[2].startsWith("dl"))
                || (sListElements.size() == 4
                    && sListElements[2].startsWith("dl"))) {
            sOutput += trUtf8("Die unterstützten Ubuntuversionen und "
                              "Architekturen werden aufgelistet.") + " ";
        } else {
            if (sListElements.size() >= 3) {
                int i = 3;
                if (sListElements[2].startsWith("dl")) {
                    i = 4;
                }

                QString sUbuntuVersions("");
                for (int j = i; j < sListElements.size(); j++) {
                    sUbuntuVersions += sListElements[j] + " ";
                }
                sOutput += trUtf8("Die Pakete können für %1 "
                                  "heruntergeladen werden. ").arg(sUbuntuVersions);
            }
        }
        QString sTmpLink = "<a href=\"" + m_sInyokaUrl
                + "/" + trUtf8("Paketinstallation_DEB") + "\" class="
                "\"internal\">" + trUtf8("DEB-Pakete noch installiert werden")
                + "</a>";
        sOutput += trUtf8("Nachdem man sie für die korrekte Ubuntuversion "
                          "geladen hat, müssen die %1.")
                .arg(sTmpLink) + "</p>\n";

        // Warning box
        sOutput += insertBox("box warning",
                             trUtf8("Hinweis!"),
                             "<a href=\"" + m_sInyokaUrl + "/"
                             + trUtf8("Fremdquellen") + "\" class=\"internal\">"
                             + trUtf8("Fremdpakete") + "</a> "
                             + trUtf8("können das System gefährden."));
    }
    // ------------------------------------------------------------------------

    // IMAGE WITH SUBSCRIPTION
    else if (sListElements[0].toLower() == trUtf8("Bildunterschrift").toLower()) {
        QString sImageLink("");
        QString sImageWidth("");
        QString sImageDescription("");
        QString sImageAlign("left");
        QString sImageStyle("");
        double iImgHeight, iImgWidth;

        sImageLink = sListElements[1].trimmed();
        if (sImageLink.startsWith("Wiki/")) {
            sImageLink = m_tmpFileDir.absolutePath() + "/" + sImageLink;
        } else if (QFile(m_tmpImgDir.absolutePath() + "/" + sImageLink).exists()) {
            sImageLink = m_tmpImgDir.absolutePath() + "/" + sImageLink;
        }

        for (int i = 2; i < sListElements.length(); i++) {
            // Found integer -> width
            if (sListElements[i].trimmed().toUInt() != 0) {
                sImageWidth = sListElements[i].trimmed();
            } else if (sListElements[i].trimmed() == "left"
                       || sListElements[i].trimmed() == "right") {
                // Alignment
                sImageAlign = sListElements[i].trimmed();
            } else if (sListElements[i].trimmed() == "xfce-style"
                       || sListElements[i].trimmed() == "kde-style"
                       || sListElements[i].trimmed() == "edu-style"
                       || sListElements[i].trimmed() == "lxde-style"
                       || sListElements[i].trimmed() == "studio-style") {
                // Style
                sImageStyle = sListElements[i].trimmed().remove("-style");
            } else {
                // Everything else is description...
                sImageDescription += sListElements[i];
            }
        }

        iImgWidth = QImage(sImageLink).width();
        if (sImageWidth != "") {
            iImgHeight = static_cast<double>(QImage(sImageLink).height()) / (iImgWidth / sImageWidth.toDouble());
        } else {
            // Default
            sImageWidth = "140";
            iImgHeight = static_cast<double>(QImage(sImageLink).height()) / (iImgWidth / 140);
        }

        sOutput = "<table style=\"float: " + sImageAlign
                + "; clear: both; border: none\">\n<tbody>\n";

        // No style info -> default
        if (sImageStyle == "") {
            sOutput += "<tr class=\"titel\">\n";
        } else {
            sOutput += "<tr class=\"" + sImageStyle + "-titel\">\n";
        }

        sOutput += "<td>\n<a href=\"" + sImageLink + "\" "
                "class=\"crosslink\">\n" + "<img src=\"" + sImageLink + "\" "
                "alt=\"" + sImageLink + "\" class=\"image-default\" "
                + "height=\"" + QString::number(static_cast<int>(iImgHeight))
                + "\" width=\"" + sImageWidth + "\"/>\n</a>\n"
                + "</td>\n</tr>\n";

        // No style info -> default
        if (sImageStyle == "") {
            sOutput += "<tr class=\"normal\">\n";
        } else {
            sOutput += "<tr class=\"" + sImageStyle + "-kopf\">\n";
        }

        sOutput += "<td style=\"text-align: center\">" + sImageDescription
                + "</td>\n</tr>" + "</tbody>\n</table>\n";
    }

    // ------------------------------------------------------------------------

    return (sOutput);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Replace BREAKS
void CParser::replaceBreaks(QTextDocument *p_rawDoc) {
    QString sMyDoc = p_rawDoc->toPlainText();

    sMyDoc.replace("[[BR]]", "<br />");
    sMyDoc.replace("\\\\", "<br />");

    // Replace p_rawDoc with document with HTML links
    p_rawDoc->setPlainText(sMyDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Replace HORIZONTAL LINE
void CParser::replaceHorLine(QTextDocument *p_rawDoc) {
    QString sMyDoc = p_rawDoc->toPlainText();

    sMyDoc.replace("----", "\n<hr />\n");

    // Replace p_rawDoc with document with HTML links
    p_rawDoc->setPlainText(sMyDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Create table of contents
QString CParser::parseTableOfContents(QTextBlock tabofcontents) {
    QString sLine = tabofcontents.text().trimmed();
    QString sOutput("TABLE OF CONTENT");
    quint16 nTOCLevel;

    if (!sLine.endsWith("]]")) {
        return sLine;
    }

    // Remove brackets
    sLine.remove("[[" + m_pTemplates->getTransTOC(), Qt::CaseInsensitive);
    sLine.remove("(");
    sLine.remove(")");
    sLine.remove("]]");

    nTOCLevel = sLine.trimmed().toUShort();
    qDebug() << "TOC level:" << QString::number(nTOCLevel);

    /*
    if ( nTOCLevel != 1 )
    {
        QMessageBox::information( 0, "Information",
                                  tr("The preview of table of contents does "
                                     "not supports sub headlines currently."));
    }
    */

    QTextBlock curBlock = m_pRawText->firstBlock();
    QStringList sListHeadlineLevel1, sListHeadlineLevel1_Links;
    QString sTmpString;

    for (; curBlock.isValid()
         && !(m_pRawText->lastBlock() < curBlock); curBlock = curBlock.next()) {
        if ((curBlock.text().startsWith("=")
             || curBlock.text().startsWith(" ="))
                && (!curBlock.text().startsWith("==")
                    && !curBlock.text().startsWith(" =="))) {
            sTmpString = curBlock.text();

            sTmpString.remove("=");
            sTmpString = sTmpString.trimmed();
            sListHeadlineLevel1 << sTmpString;

            // Replace characters for valid links (ä, ü, ö, spaces)
            sTmpString.replace(" ", "-");
            sTmpString.replace(QString::fromUtf8("Ä"), "Ae");
            sTmpString.replace(QString::fromUtf8("Ü"), "Ue");
            sTmpString.replace(QString::fromUtf8("Ö"), "Oe");
            sTmpString.replace(QString::fromUtf8("ä"), "ae");
            sTmpString.replace(QString::fromUtf8("ü"), "ue");
            sTmpString.replace(QString::fromUtf8("ö"), "oe");
            sListHeadlineLevel1_Links << sTmpString;
        }
    }

    sOutput = "<div class=\"toc\">\n<div class=\"head\">"
            + m_pTemplates->getTransTOC() + "</div>\n<ol class=\"arabic\">\n";

    for (int i = 0; i < sListHeadlineLevel1.size(); i++) {
        sOutput += "<li>\n<a href=\"#" + sListHeadlineLevel1_Links[i]
                + "\" class=\"crosslink\">" + sListHeadlineLevel1[i]
                + "</a>\n</li>\n";
    }

    sOutput += "</ol>\n</div>\n";

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// TAGS (end of page)
QString CParser::generateTags(QTextBlock actParagraph) {
    QString sParagraph = actParagraph.text();
    QString sOutput("");

    // Remove tag sample
    sParagraph.remove("#tag:");
    sParagraph.remove("# tag:");

    // Separate elements
    QStringList sListElements = sParagraph.split(",");
    if  (sListElements.size() == 0) {
        sOutput = " ";
    } else {
        // Remove spaces and generate output
        for (int i = 0; i < sListElements.size(); i++) {
            sListElements[i].remove(" ");
            sOutput += " <a href=\"" + m_sInyokaUrl
                    + "/Wiki/Tags?tag=" + sListElements[i] + "\">"
                    + sListElements[i] + "</a>";
            if (i < sListElements.size() - 1) {
                sOutput += ",";
            }
        }
    }

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Headline
QString CParser::parseHeadline(QTextBlock actParagraph) {
    QString sParagraph = actParagraph.text();
    QString sOutput("<strong>FOUND WRONG FORMATED HEADLINE</strong>\n");
    QString sLink("");
    quint16 nHeadlineLevel = 5;

    // Remove spaces at beginning and end
    sParagraph = sParagraph.trimmed();

    // Order is important! First level 5, 4, 3, 2, 1
    if (sParagraph.startsWith("=====")) {
        nHeadlineLevel = 5;
    } else if (sParagraph.startsWith("====")) {
        nHeadlineLevel = 4;
    } else if (sParagraph.startsWith("===")) {
        nHeadlineLevel = 3;
    } else if (sParagraph.startsWith("==")) {
        nHeadlineLevel = 2;
    } else if (sParagraph.startsWith("=")) {
        nHeadlineLevel = 1;
    }

    // Remove = and spaces at beginning and end
    sParagraph.remove("=");
    sParagraph = sParagraph.trimmed();

    // Replace characters for valid link
    sLink = sParagraph;
    sLink.replace(" ", "-");
    sLink.replace(QString::fromUtf8("Ä"), "Ae");
    sLink.replace(QString::fromUtf8("Ü"), "Ue");
    sLink.replace(QString::fromUtf8("Ö"), "Oe");
    sLink.replace(QString::fromUtf8("ä"), "ae");
    sLink.replace(QString::fromUtf8("ü"), "ue");
    sLink.replace(QString::fromUtf8("ö"), "oe");

    // usHeadlineLevel + 1 !!!
    sOutput = "<h" + QString::number(nHeadlineLevel+1) + " id=\"" + sLink
            + "\">" + sParagraph + " <a href=\"#" + sLink + "\" "
            "class=\"headerlink\"> &para;</a></h"
            + QString::number(nHeadlineLevel+1) + ">\n";

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Text samples {{{!#vorlage
QString CParser::parseTextSample(const QString &s_ActParagraph) {
    QString sParagraph = s_ActParagraph;

    sParagraph.remove("{{{#!"
                      + m_pTemplates->getTransTemplate().toLower() + " ");
    sParagraph.remove("{{{#!" + m_pTemplates->getTransTemplate() + " ");

    // Separate elementes from macro (between separator)
    QStringList sListElements = sParagraph.split(m_sSEPARATOR);
    for (int i = 0; i < sListElements.length(); i++) {
        sListElements[i] = sListElements[i].trimmed();
    }

    QString sOutput("<strong>ERROR: Found unknown item: {{{#!"
                    + m_pTemplates->getTransTemplate().toLower() + " "
                    + sListElements[0] + "</strong>\n");

    // KNOWLEGE BOX (Wissensblock)
    if (sListElements[0].toLower() == trUtf8("Wissen").toLower()) {
        sOutput = "<ol class=\"arabic\">\n";
        for (int i = 1; i < sListElements.length(); i++) {
            sOutput += "<li>\n<p><a id=\"source-" + QString::number(i)
                    + "\" href=\"#source-" + QString::number(i) + "\" "
                    "class=\"crosslink anchor\"> </a>\n" + sListElements[i]
                    + "</p>\n</li>\n";
        }
        sOutput += "</ol>\n";

        sOutput = insertBox("box knowledge",
                            trUtf8("Zum Verständnis dieses Artikels sind "
                                   "folgende Seiten hilfreich:"),
                            sOutput);
    }

    // WARNING (Warnung)
    else if (sListElements[0].toLower() == trUtf8("Warnung").toLower()) {
        sOutput = "";
        for (int i = 1; i < sListElements.length(); i++) {
            sOutput += sListElements[i] + " ";
        }

        sOutput = insertBox("box warning", trUtf8("Achtung!"), sOutput);
    }

    // NOTICE (Hinweis)
    else if (sListElements[0].toLower() == trUtf8("Hinweis").toLower()) {
        sOutput = "";
        for (int i = 1; i < sListElements.length(); i++) {
            sOutput += sListElements[i] + " ";
        }

        sOutput = insertBox("box notice", trUtf8("Hinweis:"), sOutput);
    }

    // EXPERT-INFO (Experteninformationen)
    else if (sListElements[0].toLower() == trUtf8("Experten").toLower()) {
        sOutput = "";
        for (int i = 1; i < sListElements.length(); i++) {
            sOutput += sListElements[i] + " ";
        }

        sOutput = insertBox("box experts", trUtf8("Experten-Info:"), sOutput);
    }

    // BASH (Befehl)
    else if (sListElements[0].toLower() == trUtf8("Befehl").toLower()) {
        sOutput = "<div class=\"bash\">\n"
                  "<div class=\"contents\">\n"
                  "<pre>";
        for (int i = 1; i < sListElements.length(); i++) {
            sListElements[i].replace("<", "&lt;");
            sOutput += sListElements[i] + "\n";
        }
        sOutput += "</pre>\n"
                   "</div>\n"
                   "</div>\n";
    }

    // PACKAGE INSTALLATION (Paketinstallation)
    else if (sListElements[0].toLower() == trUtf8("Paketinstallation").toLower()) {
        QStringList sListPackages, sListPackagesTMP;

        sOutput = "<ul>\n";

        for (int i = 1; i < sListElements.length(); i++) {
            sListPackagesTMP.clear();
            sListPackagesTMP = sListElements[i].split(",");

            // Package with source
            if (sListPackagesTMP.length() >= 2) {
                sOutput += "<li><p><strong>" + sListPackagesTMP[0].trimmed()
                        + "</strong> (<em>" + sListPackagesTMP[1].trimmed()
                        + "</em>)</p></li>";
            } else {  // Only package
                sOutput += "<li><p><strong>" + sListPackagesTMP[0].trimmed()
                        + "</strong></p></li>";
            }

            sListPackages << sListPackagesTMP[0].trimmed();
        }
        sOutput += "</ul>\n"
                   "<p>\n"
                   "<a href=\"apt://";

        // Apt packages install list / button
        for (int i = 0; i < sListPackages.length(); i++) {
            sOutput += sListPackages[i].trimmed();
            if (i < sListPackages.size() - 1) {
                sOutput += ",";
            }
        }

        sOutput += "\" rel=\"nofollow\" class=\"external\">"
                   "<img src=\"img/wiki/button.png\" "
                   "alt=\"Wiki-Installbutton\" class=\"image-default\" /></a> "
                   "mit <a href=\"" + m_sInyokaUrl + "/apturl\" "
                   "class=\"internal\">apturl</a></p>";

        // Copy console output
        sOutput += "<div class=\"package-list\">\n"
                   "<div class=\"contents\">\n"
                   "<p>";
        sOutput += trUtf8("Paketliste zum Kopieren:");
        sOutput += " <strong>apt-get</strong></p>\n"
                   "<div class=\"bash\">"
                   "<div class=\"contents\">\n"
                   "<pre class=\"notranslate\"> sudo apt-get install";
        for (int i = 0; i < sListPackages.size(); i++) {
            sOutput += " " + sListPackages[i].trimmed();
        }
        sOutput += "</pre>\n</div>\n</div>\n</div>\n</div>\n";
    }

    // BUILD DEPENDENCY INSTALLATION (Builddeps)
    else if (sListElements[0].toLower() == trUtf8("Builddeps").toLower()) {
        sOutput = "<div class=\"package-list\">\n"
                  "<div class=\"contents\">\n"
                  "<p>";
        sOutput += trUtf8("Befehl zum Installieren der Build-Abhängigkeiten:");
        sOutput += " <strong>apt-get</strong></p>\n"
                   "<div class=\"bash\">"
                   "<div class=\"contents\">\n"
                   "<pre class=\"notranslate\"> sudo apt-get build-dep";
        for (int i = 1; i < sListElements.size(); i++) {
            sOutput += " " + sListElements[i].trimmed();
        }
        sOutput += "</pre>\n</div>\n</div>\n</div>\n</div>\n";
    }

    // TABLE (Tabelle)
    else if (sListElements[0].toLower() == trUtf8("Tabelle").toLower()) {
        QRegExp tablePattern("\\<{1,1}[\\w\\s=.-\":;^|]+\\>{1,1}");
        QRegExp connectCells("-\\d{1,2}");
        QRegExp connectRows("\\|\\d{1,2}");
        QRegExp rowclassPattern("rowclass=\\\"[\\w.%-]+\\\"");
        QRegExp cellclassPattern("cellclass=\\\"[\\w.%-]+\\\"");

        bool bCellStyleWasSet;
        QRegExp cellStylePattern("cellstyle=\\\"[\\w\\s:;%#-]+\\\"");
        QRegExp rowStylePattern("rowstyle=\\\"[\\w\\s:;%#-]+\\\"");
        QRegExp tableStylePattern("tablestyle=\\\"[\\w\\s:;%#-]+\\\"");

        int iLength;
        QString sTmpCellStyle, sStyleInfo, sTmpTD;

        if (sListElements.length() >= 2) {
            if (tableStylePattern.indexIn(sListElements[1]) >= 0) {
                sTmpCellStyle = tableStylePattern.cap();
                sOutput = "<table style=" + sTmpCellStyle.remove("tablestyle=")
                        + ">\n<tbody>\n";
            } else {
                sOutput = "<table>\n<tbody>\n";
            }
        } else {
            sOutput = "<table>\n<tbody>\n";
        }

        for (int i = 1; i < sListElements.length(); i++) {
            bCellStyleWasSet = false;
            sTmpTD.clear();

            if (sListElements[i] == "+++") {  // New line
                sOutput += "</tr>\n";
            } else {  // New cell
                int myindex = tablePattern.indexIn(sListElements[i]);

                // Check if found style info is in reality a html text format
                bool bTextformat = false;
                foreach (const QString &sTmp,
                         m_pTemplates->getListFormatHtmlStart()) {
                    if (sListElements[i].trimmed().startsWith(sTmp)) {
                        bTextformat = true;
                    }
                }

                // Found style info && pattern which was found is not
                // a <span class=...> element or html text format
                if (myindex >= 0
                        && !sListElements[i].trimmed().startsWith("<span")
                        && !bTextformat) {
                    iLength = tablePattern.matchedLength();
                    sStyleInfo = tablePattern.cap();

                    // Start tr
                    if (i == 1 || sListElements[i-1] == "+++"
                            || rowclassPattern.indexIn(sStyleInfo) >= 0
                            || rowStylePattern.indexIn(sStyleInfo) >= 0) {
                        sOutput += "<tr";
                    }

                    // Found row class info --> in tr
                    if (rowclassPattern.indexIn(sStyleInfo) >= 0) {
                        sTmpCellStyle = rowclassPattern.cap();
                        sOutput += " class="
                                + sTmpCellStyle.remove("rowclass=");
                    }
                    // Found row sytle info --> in tr
                    if (rowStylePattern.indexIn(sStyleInfo) >= 0) {
                        sTmpCellStyle = rowStylePattern.cap();
                        sOutput += " style=\""
                                + sTmpCellStyle.remove("rowstyle=")
                                .remove("\"") + "\"";
                    }

                    // Close tr
                    if (i == 1 || sListElements[i-1] == "+++"
                        || rowclassPattern.indexIn(sStyleInfo) >= 0
                        || rowStylePattern.indexIn(sStyleInfo) >= 0) {
                        sOutput += ">\n";
                    }

                    // Start td
                    sOutput += "<td";

                    // Found cellclass info
                    if (cellclassPattern.indexIn(sStyleInfo) >= 0) {
                        sTmpCellStyle = cellclassPattern.cap();
                        sTmpTD += " class="
                                + sTmpCellStyle.remove("cellclass=");
                    }

                    // Connect cells info (-integer, e.g. -3)
                    if (connectCells.indexIn(sStyleInfo) >= 0) {
                        sTmpTD += " colspan=\""
                                + connectCells.cap().remove("-") + "\"";
                    }

                    // Connect ROWS info (|integer, e.g. |2)
                    if (connectRows.indexIn(sStyleInfo) >= 0) {
                        sTmpTD += " rowspan=\""
                                + connectRows.cap().remove("|") + "\"";
                    }

                    // Cell style attributs
                    if (cellStylePattern.indexIn(sStyleInfo) >= 0) {
                        sTmpTD += " style=\""
                                + cellStylePattern.cap().remove("cellstyle=")
                                .remove("\"");
                        bCellStyleWasSet = true;
                    }

                    // Text align center
                    if (sStyleInfo.contains("<:")
                            || sStyleInfo.contains(" : ")
                            || sStyleInfo.contains(":>")) {
                        if (bCellStyleWasSet) {
                            sTmpTD += " text-align: center;";
                        } else {
                            sTmpTD += " style=\"text-align: center;";
                        }
                        bCellStyleWasSet = true;
                    }
                    // Text align left
                    if (sStyleInfo.contains("<(")
                            || sStyleInfo.contains("(")
                            || sStyleInfo.contains("(>")) {
                        if (bCellStyleWasSet) {
                            sTmpTD += " text-align: left;";
                        } else {
                            sTmpTD += " style=\"text-align: left;";
                        }
                        bCellStyleWasSet = true;
                    }
                    // Text align center
                    if (sStyleInfo.contains("<)")
                            || sStyleInfo.contains(" ) ")
                            || sStyleInfo.contains(")>")) {
                        if (bCellStyleWasSet) {
                            sTmpTD += " text-align: right;";
                        } else {
                            sTmpTD += " style=\"text-align: right;";
                        }
                        bCellStyleWasSet = true;
                    }
                    // Text vertical align top
                    if (sStyleInfo.contains("<^")
                            || sStyleInfo.contains(" ^ ")
                            || sStyleInfo.contains("^>")) {
                        if (bCellStyleWasSet) {
                            sTmpTD += " text-align: top;";
                        } else {
                            sTmpTD += " style=\"vertical-align: top;";
                        }
                        bCellStyleWasSet = true;
                    }
                    // Text vertical align bottom
                    if (sStyleInfo.contains("<v")
                            || sStyleInfo.contains(" v ")
                            || sStyleInfo.contains("v>")) {
                        if (bCellStyleWasSet) {
                            sTmpTD += " text-align: bottom;";
                        } else {
                            sTmpTD += " style=\"vertical-align: bottom;";
                        }
                        bCellStyleWasSet = true;
                    }

                    // Closing style section
                    if (bCellStyleWasSet) {
                        sTmpTD += "\"";
                    }

                    // Remove style info (remove only, if line starts with
                    // "<" otherwise element)
                    if (sListElements[i].startsWith("<")) {
                        sListElements[i].replace(myindex, iLength, "");
                    }

                    sOutput += sTmpTD + ">" + sListElements[i] + "</td>\n";
                } else {  // Normal cell without style info
                    if (i == 1) {
                        sOutput += "<tr>\n";
                    }
                    if (sListElements[i-1] == "+++") {
                        sOutput += "<tr>\n";
                    }
                    sOutput += "<td>" + sListElements[i] + "</td>\n";
                }
            }
        }

        sOutput += "</tr></tbody>\n</table>";
    }
    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Code blocks {{{ and {{{!#code
QString CParser::parseCodeBlock(const QString &s_ActParagraph) {
    QString sParagraph = s_ActParagraph;
    QString sOutput("<strong>FOUND WRONG FORMATED CODE BLOCK</strong>");

    sParagraph.remove("{{{");
    sParagraph.remove("}}}");

    // Separate elementes from macro (between separator)
    QStringList sListElements = sParagraph.split(m_sSEPARATOR);
    for (int i = 0; i < sListElements.length(); i++) {
        sListElements[i] = sListElements[i].trimmed();
    }

    // Only plain code
    if (!sListElements[0].startsWith(
                "#!" +  m_pTemplates->getTransCodeBlock().toLower())
            && !sListElements[0].startsWith(
                "#!" +  m_pTemplates->getTransCodeBlock())) {
        sOutput = "<pre>";

        int i = 0;
        if (sListElements[0] == "") {
            i = 1;
        }
        for (; i < sListElements.length(); i++) {
            // Replace char "<" because it will be interpreted as
            // html tag (see bug #826482)
            sListElements[i].replace('<', "&lt;");

            sOutput += sListElements[i];
            if (i < sListElements.size() - 1) {
                sOutput += "\n";
            }
        }

        return sOutput + "</pre>\n";
    } else {  // Syntax highlighting
        // int iNumberOfLines = sListElements.size()-1;
        sOutput = "<div class=\"code\">\n<table class=\"syntaxtable\">"
                "<tbody>\n<tr>\n<td class=\"linenos\">\n<div "
                "class=\"linenodiv\"><pre>";

        // First column (line numbers)
        for (int i = 1; i < sListElements.size(); i++) {
            sOutput += QString::number(i);
            if (i < sListElements.size() - 1) {
                sOutput += "\n";
            }
        }

        // Second column (code)
        sOutput += "</pre>\n</div>\n</td>\n<td class=\"code\">\n"
                "<div class=\"syntax\">\n<pre>\n";

        QString sCode("");

        for (int i = 1; i < sListElements.length(); i++) {
            // Replace char "<" because it will be interpreted as
            // html tag (see bug #826482)
            // sListElements[i].replace('<', "&lt;");

            sCode += sListElements[i];
            if (i < sListElements.size() - 1) {
                sCode += "\n";
            }
        }

        // Syntax highlighting with Pygments (only on Unix)
#if not defined _WIN32
        if (QFile("/usr/bin/pygmentize").exists()) {
            QString sHighlightLang = sListElements[0].remove(
                        "#!" +  m_pTemplates->getTransCodeBlock(),
                        Qt::CaseInsensitive);
            sHighlightLang = sHighlightLang.trimmed();

            if ("" != sHighlightLang) {
                // Start pygmentize
                QProcess procPygmentize;
                QProcess procEcho;

                // Workaround for passing stdin string with code to pygmentize
                procEcho.setStandardOutputProcess(&procPygmentize);
                procEcho.start("echo", QStringList() << sCode);
                if (!procEcho.waitForStarted()) {
                    QMessageBox::critical(0, "Pygments error",
                                          "Could not start echo.");
                    procEcho.kill();
                    return sOutput + sCode + "</pre>\n</div>\n</td>\n</tr>\n"
                            "</tbody>\n</table>\n</div>";
                }
                if (!procEcho.waitForFinished()) {
                    QMessageBox::critical(0, "Pygments error",
                                          "Error while using echo.");
                    procEcho.kill();
                    return sOutput + sCode + "</pre>\n</div>\n</td>\n</tr>\n"
                            "</tbody>\n</table>\n</div>";
                }

                procPygmentize.start("pygmentize", QStringList() << "-l"
                                     << sHighlightLang
                                     << "-f" << "html"
                                     << "-O" << "nowrap"
                                     << "-O" << "noclasses");

                if (!procPygmentize.waitForStarted()) {
                    QMessageBox::critical(0, "Pygments error",
                                          "Could not start pygmentize.");
                    procPygmentize.kill();
                    return sOutput + sCode + "</pre>\n</div>\n</td>\n</tr>\n"
                            "</tbody>\n</table>\n</div>";
                }
                if (!procPygmentize.waitForFinished()) {
                    QMessageBox::critical(0, "Pygments error",
                                          "Error while using pygmentize.");
                    procPygmentize.kill();
                    return sOutput + sCode + "</pre>\n</div>\n</td>\n</tr>\n"
                            "</tbody>\n</table>\n</div>";
                }

                QString sCode2 = procPygmentize.readAll();
                if ("" != sCode2) {
                    sCode = sCode2;
                }
            }
        }
#endif
        return sOutput + sCode + "</pre>\n</div>\n</td>\n</tr>\n</tbody>\n"
                "</table>\n</div>";
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Image collection [[Vorlage(Bildersammlung, ... )]]
QString CParser::parseImageCollection(const QString &s_ActParagraph) {
    QString sParagraph = s_ActParagraph;
    QString sOutput("");

    sParagraph.remove("[[" + m_pTemplates->getTransTemplate()
                      + "(Bildersammlung");
    sParagraph.remove(")]]");

    QString sImageUrl("");
    QString sImageCollHeight("140");
    QString sImageCollAlign("");
    QStringList sListImages;
    double iImgHeight, iImgWidth;

    // Separate elementes from macro (between separator)
    QStringList sListElements = sParagraph.split(m_sSEPARATOR);
    if (sListElements.length() == 0) {
        return "<strong>ERROR: Image collection</strong>\n";
    }

    for (int i = 0; i < sListElements.length(); i++) {
        sListElements[i] = sListElements[i].trimmed();
        sListElements[i].remove("\"");

        // First entry (height, align)
        if (i == 0) {
            sListImages << sListElements[0].split(",");
            for (int j = 0; j < sListImages.length(); j++) {
                if (sListImages[j].trimmed().toUInt() != 0) {
                    sImageCollHeight = sListImages[j].trimmed();
                } else if (sListImages[j].trimmed() == "left"
                           || sListImages[j].trimmed() == "right") {
                    sImageCollAlign = sListImages[j].trimmed();
                }
            }
        } else {
            sListImages.clear();
            sListImages << sListElements[i].split(",");
            if (sListImages.length() <= 1) {
                return "<strong>ERROR: Image collection</strong>\n";
            }
        }

        // With word wrap
        if (sImageCollAlign == "" && i > 0) {
            if (i == 1) {
                sOutput += "<div style=\"clear: both\">\n<div "
                        "class=\"contents\"> </div>\n</div>";
            }

            sImageUrl = sListImages[0].trimmed();
            if (sImageUrl.startsWith("Wiki/")) {
                sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;
            } else if (QFile(m_tmpImgDir.absolutePath() + "/"
                             + sImageUrl).exists()) {
                sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
            }

            iImgHeight = QImage(sImageUrl).height();
            iImgWidth = static_cast<double>(QImage(sImageUrl).width()) / (iImgHeight / sImageCollHeight.toDouble());

            sOutput += "<table style=\"float: left; margin: 10px 5px; border: none\">\n<tbody>\n<tr>\n";
            sOutput += "<td style=\"text-align: center; background-color: #E2C889; border: none\">";
            sOutput += "<a href=\"" + sImageUrl + "\" class=\"crosslink\">";
            sOutput += "<img src=\"" + sImageUrl + "\" alt=\"" + sImageUrl + "\" class=\"image-default\" ";
            sOutput += "width=\"" + QString::number(static_cast<int>(iImgWidth)) + "\" height=\"" + sImageCollHeight + "\"/></a></td>\n</tr>\n";
            sOutput += "<tr>\n<td style=\"text-align: center; background-color: #F9EAAF; border: none\">" + sListImages[1] + "</td>\n</tr>\n";
            sOutput += "</tbody>\n</table>\n";
        }
    }

    if (sImageCollAlign == "") {
        sOutput += "<div style=\"clear: both\">\n<div class=\"contents\"> "
                "</div>\n</div>";
    }

    // In continuous text
    if (sImageCollAlign != "") {
        sOutput = "<table style=\"float: " + sImageCollAlign
                + "; clear: both; border: none\">\n<tbody>\n<tr "
                "style=\"background-color: #E2C889\">\n";
        for (int i = 1; i < sListElements.length(); i++) {
            sListImages.clear();
            sListImages << sListElements[i].split(",");
            if (sListImages.length() <= 1) {
                return "<strong>ERROR: Image collection</strong>\n";
            }

            sImageUrl = sListImages[0].trimmed();
            if (sImageUrl.startsWith("Wiki/")) {
                sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;
            } else if (QFile(m_tmpImgDir.absolutePath() + "/"
                           + sImageUrl).exists()) {
                sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
            }

            iImgHeight = QImage(sImageUrl).height();
            iImgWidth = static_cast<double>(QImage(sImageUrl).width()) / (iImgHeight / sImageCollHeight.toDouble());

            sOutput += "<td style=\"text-align: center; border-width: 0 10px 0 0; border-color: #FFFFFF \">\n";
            sOutput += "<img src=\"" + sImageUrl + "\" alt=\"" + sImageUrl + "\" class=\"image-default\" ";
            sOutput += "width=\"" + QString::number(static_cast<int>(iImgWidth)) + "\" height=\"" + sImageCollHeight + "\"/></td>\n";
        }
        sOutput += "</tr>\n<tr style=\"background-color: #F9EAAF\">";

        for (int i = 1; i < sListElements.length(); i++) {
            sListImages.clear();
            sListImages << sListElements[i].split(",");

            sOutput += "<td style=\"text-align: center; border-width: "
                    "0 10px 0 0; border-color: #FFFFFF\">\n"
                    + sListImages[1].trimmed() + "</td>\n";
        }
        sOutput += "</tr></tbody>\n</table>\n";
    }

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// REPLACE IMAGES

void CParser::replaceImages(QTextDocument *p_rawDoc) {
    QRegExp findImages("\\[\\[" + m_pTemplates->getTransImage()
                       + "\\([\\w\\s\\-,./=\"]+\\)\\]\\]");
    QString sMyDoc = p_rawDoc->toPlainText();
    int iLength;
    QString sTmpImage;
    QStringList sListTmpImageInfo;

    QString sImageUrl("");
    QString sImageWidth("");
    QString sImageHeight("");
    QString sImageAlign("left");
    // QString sImageAlt("");

    double iImgHeight, iImgWidth;
    double tmpH, tmpW;

    int myindex = findImages.indexIn(sMyDoc);
    while (myindex >= 0) {
        iLength = findImages.matchedLength();
        sTmpImage = findImages.cap();

        sTmpImage.remove("[[" + m_pTemplates->getTransImage() + "(");
        sTmpImage.remove(")]]");

        sImageWidth = "";
        sImageHeight = "";
        sImageAlign = "left";
        iImgHeight = 0;
        iImgWidth = 0;
        tmpH = 0;
        tmpW = 0;

        sListTmpImageInfo.clear();
        sListTmpImageInfo << sTmpImage.split(",");

        sImageUrl = sListTmpImageInfo[0].trimmed();
        if (sImageUrl.startsWith("Wiki/")) {
            sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;
        } else if (QFile(m_tmpImgDir.absolutePath() + "/"
                         + sImageUrl).exists()) {
            sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
        }

        // sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;

        for (int i = 1; i < sListTmpImageInfo.length(); i++) {
            // Found integer (width)
            if (sListTmpImageInfo[i].trimmed().toUInt() != 0) {
                sImageWidth = sListTmpImageInfo[i].trimmed();
                tmpW = sListTmpImageInfo[i].trimmed().toUInt();
            } else if (sListTmpImageInfo[i].startsWith("x\\d")) {
                // Found x+integer (height)
                sImageHeight = sListTmpImageInfo[i].remove("x").trimmed();
                tmpH = sImageHeight.toUInt();
            } else if (sListTmpImageInfo[i].contains("\\dx\\d")) {
                // Found int x int (width x height)
                QString sTmp;
                sTmp = sListTmpImageInfo[i];
                sImageWidth = sListTmpImageInfo[i].remove(
                            sListTmpImageInfo[i].indexOf("x"),
                            sListTmpImageInfo[i].length() ).trimmed();
                tmpW = sImageWidth.toUInt();
                sImageHeight = sTmp.remove(0, sTmp.indexOf("x")).trimmed();
                tmpH = sImageHeight.toUInt();
            } else if (sListTmpImageInfo[i].trimmed() == "left"
                     || sListTmpImageInfo[i].trimmed() == "align=left") {
                // Found alignment
                sImageAlign = "left";
            } else if (sListTmpImageInfo[i].trimmed() == "right"
                       ||  sListTmpImageInfo[i].trimmed() == "align=right") {
                sImageAlign = "right";
            }
        }

        // No size given
        if (tmpH == 0 && tmpW == 0) {
            iImgHeight = QImage(sImageUrl).height();
            tmpH = iImgHeight;
            iImgWidth = QImage(sImageUrl).width();
            tmpW = iImgWidth;
        }

        if (tmpH > tmpW) {
            iImgHeight = QImage(sImageUrl).height();
            tmpW = static_cast<double>(QImage(sImageUrl).width()) / (iImgHeight / static_cast<double>(tmpH));
        } else if (tmpW > tmpH) {
            iImgWidth = QImage(sImageUrl).width();
            tmpH = static_cast<double>(QImage(sImageUrl).height()) / (iImgWidth / static_cast<double>(tmpW));
        }

        // HTML code
        sTmpImage = "<a href=\"" + sImageUrl + "\" class=\"crosslink\">";
        sTmpImage += "<img src=\"" + sImageUrl + "\" alt=\"" + sImageUrl
                + "\" height=\"" + QString::number(tmpH) + "\" width=\""
                + QString::number(tmpW) + "\" ";
        sTmpImage += "class=\"image-" + sImageAlign + "\" /></a>";

        // Replace
        sMyDoc.replace(myindex, iLength, sTmpImage);

        // Go on with RegExp-Search
        myindex = findImages.indexIn(sMyDoc, myindex + iLength);
    }

    // Replace p_rawDoc with document with HTML links
    p_rawDoc->setPlainText(sMyDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// List
QString CParser::parseList(const QString &s_ActParagraph) {
    QString sParagraph = s_ActParagraph;
    QString sOutput("<strong>ERROR: List</strong>\n");

    // Separate elementes from macro (between separator)
    QStringList sListElements = sParagraph.split(m_sSEPARATOR);
    int iArrayLevel[sListElements.length()];
    bool iArrayArabic[sListElements.length()];

    for (int i = 0; i < sListElements.length(); i++) {
        if (sListElements[i].trimmed().startsWith("*")) {
            if (i == 0) {
                sOutput = "<ul>\n";
            }

            iArrayLevel[i] = sListElements[i].indexOf("*");
            iArrayArabic[i] = false;
            sListElements[i] = sListElements[i].remove(
                        0, iArrayLevel[i]+1).trimmed();
        } else {
            if (i == 0) {
                sOutput = "<ol class=\"arabic\">\n";
            }

            iArrayLevel[i] = sListElements[i].indexOf("1.");
            iArrayArabic[i] = true;
            sListElements[i] = sListElements[i].remove(
                        0, iArrayLevel[i]+2).trimmed();
        }

        if (i > 0 && (iArrayLevel[i-1] < iArrayLevel[i])) {
            for (int j = iArrayLevel[i] - iArrayLevel[i-1]; j > 0; j--) {
                if (iArrayArabic[i] == false) {
                    sOutput += "<ul>\n";
                } else {
                    sOutput += "<ol class=\"arabic\">\n";
                }
            }
        }
        if (i > 0 && (iArrayLevel[i-1] > iArrayLevel[i])) {
            for (int j = iArrayLevel[i-1] - iArrayLevel[i]; j > 0; j--) {
                if (iArrayArabic[i] == false) {
                    sOutput += "</ul>\n";
                } else {
                    sOutput += "</ol>\n";
                }
            }
        }

        sOutput += "<li><p>" + sListElements[i] + "</p></li>\n";

        if (i == sListElements.length() - 1) {
            for (int k = iArrayLevel[i]; k > 0; k--) {
                if (iArrayArabic[i] == false) {
                    sOutput += "</ul>\n";
                } else {
                    sOutput += "</ol>\n";
                }
            }
        }
    }

    sOutput += "</ul>";

    return sOutput;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// Insert box
QString CParser::insertBox(const QString &sClass, const QString &sHeadline,
                           const QString &sContents, const QString &sRemark) {
    QString sReturn("");

    // Generate output
    sReturn = "<div class=\"" + sClass + "\">\n";
    sReturn += "<h3 class=\"" + sClass + "\">" + sHeadline +  "</h3>\n";
    sReturn += "<div class=\"contents\">\n";
    sReturn += "<p>" + sContents + "</p>\n";
    // Remark available
    if (sRemark != "" && sRemark != " ") {
        sReturn += "<hr />\n<p><strong>" + trUtf8("Anmerkung:") + "</strong> "
                + sRemark + "</p>\n";
    }
    sReturn += "</div>\n"
               "</div>\n";

    return sReturn;
}
