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
                 const QString &sInyokaUrl,
                 const bool bCheckLinks,
                 CTemplates *pTemplates)
    : m_tmpFileDir(tmpFileOutputDir),
      m_tmpImgDir(tmpImgDir),
      m_sInyokaUrl(sInyokaUrl),
      m_pTemplates(pTemplates) {
    qDebug() << "Calling" << Q_FUNC_INFO;

    m_pTemplateParser = new CParseTemplates(m_pTemplates->getTransTemplate(),
                                            m_pTemplates->getListTplNamesINY(),
                                            m_pTemplates->getListFormatHtmlStart(),
                                            tmpFileOutputDir,
                                            tmpImgDir);

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

void CParser::updateSettings(const QString &sInyokaUrl,
                             const bool bCheckLinks) {
    m_sInyokaUrl = sInyokaUrl;
    m_pLinkParser->updateSettings(sInyokaUrl, bCheckLinks);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CParser::genOutput(const QString &sActFile,
                           QTextDocument *pRawDocument) {
    // Need a copy otherwise text in editor will be changed
    m_pRawText = pRawDocument->clone();
    m_sCurrentFile = sActFile;
    QString sWikitags("");

    // Replace macros with Inyoka markup templates
    // this->replaceTemplates( m_pRawText );

    this->removeComments(m_pRawText);

    m_sListNoTranslate.clear();
    this->replaceCodeblocks(m_pRawText);
    this->filterNoTranslate(m_pRawText);

    m_pTemplateParser->startParsing(m_pRawText, m_sCurrentFile);

    // Replace flags
    m_pMapParser->startParsing(m_pRawText,
                               m_pTemplates->getListFlags(),
                               m_pTemplates->getListFlagsImg());
    // Replace smilies
    m_pMapParser->startParsing(m_pRawText,
                               m_pTemplates->getListSmilies(),
                               m_pTemplates->getListSmiliesImg());

    this->replaceImages(m_pRawText);
    this->replaceHeadlines(m_pRawText);        // And generate list for TOC
    this->replaceTableOfContents(m_pRawText);  // Use before link parser!
    this->replaceLists(m_pRawText);
    this->replaceTables(m_pRawText);

    m_pLinkParser->startParsing(m_pRawText);

    this->replaceQuotes(m_pRawText);
    this->replaceBreaks(m_pRawText);
    this->replaceHorLine(m_pRawText);

    sWikitags = this->generateTags(m_pRawText);

    this->replaceTextformat(m_pRawText,
                            m_pTemplates->getListFormatStart(),
                            m_pTemplates->getListFormatEnd(),
                            m_pTemplates->getListFormatHtmlStart(),
                            m_pTemplates->getListFormatHtmlEnd());

    this->generateParagraphs(m_pRawText);

    this->reinstertNoTranslate(m_pRawText);

    // File name
    QString sFilename;
    if ("" == m_sCurrentFile) {
        sFilename = tr("Untitled", "No file name set");
    } else {
        QFileInfo fi(m_sCurrentFile);
        sFilename = fi.baseName();
    }

    // Replace template tags
    // Copy needed, otherwise %tags% will be replaced/removed in template!
    QString sTemplateCopy(m_pTemplates->getPreviewTemplate());
    sTemplateCopy = sTemplateCopy.replace("%filename%", sFilename);
    QString sRevTextCopy(m_pTemplates->getTransRev());  // Copy needed!
    sRevTextCopy = sRevTextCopy.replace("%date%",
                                        QDate::currentDate().toString("dd.MM.yyyy"))
            .replace("%time%", QTime::currentTime().toString("hh:mm"));
    sTemplateCopy = sTemplateCopy.replace("%revtext%", sRevTextCopy);
    sTemplateCopy = sTemplateCopy.replace("%tagtext%",
                                          m_pTemplates->getTransTag() + " "
                                          + sWikitags);
    sTemplateCopy = sTemplateCopy.replace("%content%",
                                          m_pRawText->toPlainText());

    return sTemplateCopy;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceTemplates(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());
    QString sMacro;
    QStringList sListArguments;
    int nPos = 0;
    QStringList sListTplRegExp;
    sListTplRegExp << "\\[\\[" +  m_pTemplates->getTransTemplate()
                      + "\\(.+\\)\\]\\]";
//                 << "\\{\\{\\{#!" +  m_pTemplates->getTransTemplate()
//                    + " .+\\}\\}\\}";

    QRegExp findTemplate(sListTplRegExp[0], Qt::CaseInsensitive);
    findTemplate.setMinimal(true);

    while ((nPos = findTemplate.indexIn(sDoc, nPos)) != -1) {
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

                sDoc.replace(nPos, findTemplate.matchedLength(), sMacro);
            }
        }

        // Go on with new start position
        nPos += sMacro.length();
        // nPos += findTemplate.matchedLength();
    }

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceCodeblocks(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());
    QStringList sListTplRegExp;
    // Search for {{{#!code ...}}} and {{{ ... [without #!vorlage] ...}}}
    sListTplRegExp << "\\{\\{\\{#!" + m_pTemplates->getTransCodeBlock() + " .+\\}\\}\\}"
                   << "\\{\\{\\{(?!#!" + m_pTemplates->getTransTemplate() + ").+\\}\\}\\}";
    QString sMacro;
    QStringList sListLines;
    int nPos;
    bool bFormated;

    for (int k = 0; k < sListTplRegExp.size(); k++) {
        QRegExp findTemplate(sListTplRegExp[k], Qt::CaseInsensitive);
        findTemplate.setMinimal(true);
        nPos = 0;

        while ((nPos = findTemplate.indexIn(sDoc, nPos)) != -1) {
            bFormated = false;
            sMacro = findTemplate.cap(0);
            sMacro.remove("{{{\n");
            sMacro.remove("{{{");
            if (sMacro.startsWith("#!" + m_pTemplates->getTransCodeBlock() + " ",
                                  Qt::CaseInsensitive)) {
                bFormated = true;
                sMacro.remove("#!" + m_pTemplates->getTransCodeBlock() + " ",
                              Qt::CaseInsensitive);
            }
            sMacro.remove("\n}}}");
            sMacro.remove("}}}");

            sListLines.clear();
            sListLines = sMacro.split(QRegExp("\\n"));

            // Only plain code
            if (!bFormated) {
                sMacro = "<pre>";
                for (int i = 0; i < sListLines.size(); i++) {
                    // Replace char "<" because it will be interpreted as
                    // html tag (see bug #826482)
                    sListLines[i].replace('<', "&lt;");
                    sMacro += sListLines[i];
                    if (i < sListLines.size() - 1) {
                        sMacro += "\n";
                    }
                }
                sMacro += "</pre>\n";
            } else {  // Syntax highlighting
                sMacro = "<div class=\"code\">\n<table class=\"syntaxtable\">"
                        "<tbody>\n<tr>\n<td class=\"linenos\">\n<div "
                        "class=\"linenodiv\"><pre>";

                // First column (line numbers)
                for (int i = 1; i < sListLines.size(); i++) {
                    sMacro += QString::number(i);
                    if (i < sListLines.size() - 1) {
                        sMacro += "\n";
                    }
                }

                // Second column (code)
                sMacro += "</pre>\n</div>\n</td>\n<td class=\"code\">\n"
                        "<div class=\"syntax\">\n<pre>\n";

                QString sCode("");

                for (int i = 1; i < sListLines.length(); i++) {
                    // Replace char "<" because it will be interpreted as
                    // html tag (see bug #826482)
                    // sListElements[i].replace('<', "&lt;");

                    sCode += sListLines[i];
                    if (i < sListLines.size() - 1) {
                        sCode += "\n";
                    }
                }

                // Syntax highlighting (with pygments if available)
                if (sListLines.size() > 0) {
                    if ("" != sListLines[0].trimmed()) {
                        sCode = this->highlightCode(sListLines[0], sCode);
                    }
                }
                sMacro += sCode + "</pre>\n</div>\n</td>\n</tr>\n</tbody>\n"
                        "</table>\n</div>";
            }

            unsigned int nNoTranslate = m_sListNoTranslate.size();
            m_sListNoTranslate << sMacro;  // Save code block
            sMacro = "%%NO_TRANSLATE_" + QString::number(nNoTranslate) + "%%";

            sDoc.replace(nPos, findTemplate.matchedLength(), sMacro);
            // Go on with new start position
            nPos += sMacro.length();
        }
    }

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CParser::highlightCode(const QString &sLanguage, const QString &sCode) {
#ifndef _WIN32
    if (QFile("/usr/bin/pygmentize").exists()) {
        QProcess procPygmentize;
        QProcess procEcho;

        // Workaround for passing stdin string with code to pygmentize
        procEcho.setStandardOutputProcess(&procPygmentize);
        procEcho.start("echo", QStringList() << sCode);
        if (!procEcho.waitForStarted()) {
            QMessageBox::critical(0, "Pygments error",
                                  "Could not start echo.");
            procEcho.kill();
            return sCode;
        }
        if (!procEcho.waitForFinished()) {
            QMessageBox::critical(0, "Pygments error",
                                  "Error while using echo.");
            procEcho.kill();
            return sCode;
        }

        procPygmentize.start("pygmentize", QStringList() << "-l"
                             << sLanguage
                             << "-f" << "html"
                             << "-O" << "nowrap"
                             << "-O" << "noclasses");

        if (!procPygmentize.waitForStarted()) {
            QMessageBox::critical(0, "Pygments error",
                                  "Could not start pygmentize.");
            procPygmentize.kill();
            return sCode;
        }
        if (!procPygmentize.waitForFinished()) {
            QMessageBox::critical(0, "Pygments error",
                                  "Error while using pygmentize.");
            procPygmentize.kill();
            return sCode;
        }

        return procPygmentize.readAll();
    } else {
        return sCode;
    }
#else
    Q_UNUSED(sLanguage);
    return sCode;
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::filterNoTranslate(QTextDocument *p_rawDoc) {
    QStringList sListFormatStart;
    QStringList sListFormatEnd;
    QStringList sListHtmlStart;
    QStringList sListHtmlEnd;
    QString sDoc("");
    QRegExp patternFormat;
    QString sFormatedText;
    int myindex;
    int iLength;
    unsigned int nNoTranslate;

    for (int i = 0; i < m_pTemplates->getListFormatHtmlStart().size(); i++) {
        if (m_pTemplates->getListFormatHtmlStart()[i]
                .contains("class=\"notranslate\"")) {
            sListFormatStart << m_pTemplates->getListFormatStart()[i];
            sListFormatEnd << m_pTemplates->getListFormatEnd()[i];
            sListHtmlStart << m_pTemplates->getListFormatHtmlStart()[i];
            sListHtmlEnd << m_pTemplates->getListFormatHtmlEnd()[i];
        }
    }

    this->replaceTextformat(p_rawDoc, sListFormatStart, sListFormatEnd,
                            sListHtmlStart, sListHtmlEnd);

    patternFormat.setCaseSensitivity(Qt::CaseInsensitive);
    patternFormat.setMinimal(true);  // Search only for smallest match

    sDoc = p_rawDoc->toPlainText();
    // qDebug() << "\n\n" << sDoc << "\n\n";
    nNoTranslate = m_sListNoTranslate.size();
    for (int i = 0; i < sListHtmlStart.size(); i++) {
        patternFormat.setPattern(sListHtmlStart[i] + ".+" + sListHtmlEnd[i]);
        myindex = patternFormat.indexIn(sDoc);

        while (myindex >= 0) {
            sFormatedText = patternFormat.cap();
            iLength = sFormatedText.length();
            m_sListNoTranslate << sFormatedText;
            myindex = patternFormat.indexIn(sDoc, myindex + iLength);
            sDoc.replace(sFormatedText, "%%NO_TRANSLATE_" +
                         QString::number(nNoTranslate) + "%%");
            nNoTranslate++;
        }
    }
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceTextformat(QTextDocument *p_rawDoc,
                                const QStringList &sListFormatStart,
                                const QStringList &sListFormatEnd,
                                const QStringList &sListHtmlStart,
                                const QStringList &sListHtmlEnd) {
    QString sDoc(p_rawDoc->toPlainText());
    QRegExp patternTextformat;
    QString sFormatedText;
    QString sTmpRegExp;
    bool bFoundStart;
    int myindex;
    int iLength;

    patternTextformat.setCaseSensitivity(Qt::CaseInsensitive);
    patternTextformat.setMinimal(true);  // Search only for smallest match

    for (int i = 0; i < sListFormatStart.size(); i++) {
        bFoundStart = true;

        // Start and end is not identical
        if (sListFormatStart[i] != sListFormatEnd[i]) {
            if (!sListFormatStart[i].startsWith("RegExp=")) {
                sDoc.replace(sListFormatStart[i], sListHtmlStart[i]);
            } else {
                sTmpRegExp = sListFormatStart[i];
                sTmpRegExp.remove("RegExp=");
                sTmpRegExp = sTmpRegExp.trimmed();
                patternTextformat.setPattern(sTmpRegExp);

                myindex = patternTextformat.indexIn(sDoc);

                while (myindex >= 0) {
                    QString sCap("");
                    iLength = patternTextformat.matchedLength();
                    sFormatedText = patternTextformat.cap();
                    sCap = patternTextformat.cap(1);

                    if (sCap == "") {
                        sDoc.replace(myindex, iLength, sListHtmlStart[i]);
                    } else {
                        sDoc.replace(myindex, iLength,
                                     sListHtmlStart[i].arg(sCap));
                    }

                    // Go on with RegExp-Search
                    myindex = patternTextformat.indexIn(sDoc, myindex + iLength);
                }
            }
            if (!sListFormatEnd[i].startsWith("RegExp=")) {
                sDoc.replace(sListFormatEnd[i], sListHtmlEnd[i]);
            } else {
                sTmpRegExp = sListFormatEnd[i];
                sTmpRegExp.remove("RegExp=");
                sTmpRegExp = sTmpRegExp.trimmed();
                patternTextformat.setPattern(sTmpRegExp);

                myindex = patternTextformat.indexIn(sDoc);

                while (myindex >= 0) {
                    QString sCap("");
                    iLength = patternTextformat.matchedLength();
                    sFormatedText = patternTextformat.cap();
                    sCap = patternTextformat.cap(1);

                    if (sCap == "") {
                        sDoc.replace(myindex, iLength, sListHtmlEnd[i]);
                    } else {
                        sDoc.replace(myindex, iLength,
                                     sListHtmlEnd[i].arg(sCap));
                    }

                    // Go on with RegExp-Search
                    myindex = patternTextformat.indexIn(sDoc, myindex + iLength);
                }
            }
        } else {  // Start and end is identical
            if (!sListFormatStart[i].startsWith("RegExp=")) {
                while (-1 != sDoc.indexOf(sListFormatStart[i])) {
                    if (bFoundStart) {
                        sDoc.replace(sDoc.indexOf(sListFormatStart[i]),
                                     sListFormatStart[i].length(),
                                     sListHtmlStart[i]);
                    } else {
                        sDoc.replace(sDoc.indexOf(sListFormatStart[i]),
                                     sListFormatStart[i].length(),
                                     sListHtmlEnd[i]);
                    }
                    bFoundStart = !bFoundStart;
                }
            } else {
                sTmpRegExp = sListFormatStart[i];
                sTmpRegExp.remove("RegExp=");
                sTmpRegExp = sTmpRegExp.trimmed();
                patternTextformat.setPattern(sTmpRegExp);

                myindex = patternTextformat.indexIn(sDoc);

                while (myindex >= 0) {
                    QString sCap("");
                    iLength = patternTextformat.matchedLength();
                    sFormatedText = patternTextformat.cap();
                    sCap = patternTextformat.cap(1);

                    if (sCap == "") {
                        if (bFoundStart) {
                            sDoc.replace(myindex, iLength, sListHtmlStart[i]);
                        } else {
                            sDoc.replace(myindex, iLength, sListHtmlEnd[i]);
                        }
                    } else {
                        if (bFoundStart) {
                            sDoc.replace(myindex, iLength,
                                         sListHtmlStart[i].arg(sCap));
                        } else {
                            sDoc.replace(myindex, iLength,
                                         sListHtmlEnd[i].arg(sCap));
                        }
                    }

                    // Go on with RegExp-Search
                    myindex = patternTextformat.indexIn(sDoc, myindex + iLength);
                }
            }
        }
    }

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::reinstertNoTranslate(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());

    // Reinsert filtered monotype codeblock
    for (int i = 0; i < m_sListNoTranslate.size(); i++) {
        sDoc.replace("%%NO_TRANSLATE_" + QString::number(i) + "%%", m_sListNoTranslate[i]);
    }

    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceBreaks(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());

    sDoc.replace("[[BR]]", "<br />");
    sDoc.replace("\\\\", "<br />");

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceHorLine(QTextDocument *p_rawDoc) {
    QString sDoc("");

    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if ("----" == block.text()) {
            sDoc += "\n<hr />\n";
        } else {
            sDoc += block.text() + "\n";
        }
    }

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString CParser::generateTags(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());
    QString sLine("");
    QString sTags("");
    QStringList sListTags;

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if (block.text().trimmed().startsWith("#tag:")
                || block.text().trimmed().startsWith("# tag:")) {
            sLine = block.text();
            sTags = block.text().trimmed();
            sTags.remove("#tag:");
            sTags.remove("# tag:");
            sTags = sTags.trimmed();
            QStringList sListElements = sTags.split(",");
            sListTags << sListElements;
            sDoc.replace(sLine, "");
        }
    }

    sTags.clear();
    for (int i = 0; i < sListTags.size(); i++) {
        sListTags[i].remove(" ");
        sTags += " <a href=\"" + m_sInyokaUrl + "/Wiki/Tags?tag="
                + sListTags[i] + "\">" + sListTags[i] + "</a>";
        if (i < sListTags.size() - 1) {
            sTags += ",";
        }
    }

    p_rawDoc->setPlainText(sDoc);
    return sTags;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceQuotes(QTextDocument *p_rawDoc) {
    QString sDoc("");
    QString sLine("");
    quint16 nQuotes = 0;

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if (block.text().startsWith(">")) {
            sLine = block.text().trimmed();
            nQuotes = sLine.count(">");
            sLine.remove(QRegExp("^>*"));
            for (int n = 0; n < nQuotes; n++) {
                sLine = "<blockquote>" + sLine + "</blockquote>";
            }
            sDoc += sLine + "\n";
        } else {
            sDoc += block.text() + "\n";
        }
    }

    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::generateParagraphs(QTextDocument *p_rawDoc) {
    QString sDoc("<p>\n");

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if (block.text().trimmed().isEmpty()) {
            sDoc += "</p>\n<p>\n";
        } else {
            sDoc += block.text() + "\n";
        }
    }

    p_rawDoc->setPlainText(sDoc + "</p>");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::removeComments(QTextDocument *p_rawDoc) {
    QString sDoc("");

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if (!block.text().startsWith("##")) {
            sDoc += block.text() + "\n";
        }
    }

    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceHeadlines(QTextDocument *p_rawDoc) {
    QString sDoc("");
    QString sLine("");
    QString sTmp("");
    QString sLink("");
    quint16 nHeadlineLevel = 5;
    m_sListHeadline_1.clear();

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        // Order is important! First level 5, 4, 3, 2, 1
        for (int i = 5; i >= 0; i--) {
            sLine = block.text();
            sTmp.fill('=', i);
            nHeadlineLevel = 0;  // Always reset for each line
            if (0 == i) {
                sDoc += sLine + "\n";
                break;
            } else if (sLine.trimmed().startsWith(sTmp)
                       && sLine.trimmed().endsWith(sTmp)
                       && sLine.trimmed().length() > (i*2)) {
                nHeadlineLevel = i;
            } else {
                continue;
            }

            // Remove first and last "="
            sLine.remove(0, sLine.indexOf(sTmp) + sTmp.length());
            sLine.remove(sLine.lastIndexOf(sTmp), sLine.length());
            sLine = sLine.trimmed();

            if (1 == nHeadlineLevel) {
                m_sListHeadline_1 << sLine;
            }

            // Replace characters for valid link
            sLink = sLine;
            sLink.replace(" ", "-");
            sLink.replace(QString::fromUtf8("Ä"), "Ae");
            sLink.replace(QString::fromUtf8("Ü"), "Ue");
            sLink.replace(QString::fromUtf8("Ö"), "Oe");
            sLink.replace(QString::fromUtf8("ä"), "ae");
            sLink.replace(QString::fromUtf8("ü"), "ue");
            sLink.replace(QString::fromUtf8("ö"), "oe");

            // usHeadlineLevel + 1 !!!
            sLine = "<h" + QString::number(nHeadlineLevel+1) + " id=\"" + sLink
                    + "\">" + sLine + " <a href=\"#" + sLink + "\" "
                    "class=\"headerlink\"> &para;</a></h"
                    + QString::number(nHeadlineLevel+1) + ">\n";
            sDoc += sLine;
            break;
        }
    }

    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceTableOfContents(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());
    QString sRegExp("\\[\\[" + m_pTemplates->getTransTOC() + "\\(.*\\)\\]\\]");
    QRegExp findMacro(sRegExp, Qt::CaseInsensitive);
    findMacro.setMinimal(true);
    QString sMacro;
    int nPos = 0;
    // quint16 nTOCLevel;

    // Replace characters for valid links (ä, ü, ö, spaces)
    QStringList sListHeadline_1_Links;
    foreach (QString s, m_sListHeadline_1) {
        sMacro = s;
        sMacro.replace(" ", "-");
        sMacro.replace(QString::fromUtf8("Ä"), "Ae");
        sMacro.replace(QString::fromUtf8("Ü"), "Ue");
        sMacro.replace(QString::fromUtf8("Ö"), "Oe");
        sMacro.replace(QString::fromUtf8("ä"), "ae");
        sMacro.replace(QString::fromUtf8("ü"), "ue");
        sMacro.replace(QString::fromUtf8("ö"), "oe");
        sListHeadline_1_Links << sMacro;
    }

    while ((nPos = findMacro.indexIn(sDoc, nPos)) != -1) {
        sMacro = findMacro.cap(0);
        sMacro.remove("[[" + m_pTemplates->getTransTOC() + "(");
        sMacro.remove(")]]");

        /*
        nTOCLevel = sMacro.trimmed().toUShort();
        qDebug() << "TOC level:" << nTOCLevel;
        if (1 != nTOCLevel) {
            QMessageBox::information(0, "Information",
                                     "The preview of table of contents does "
                                     "not supports sub headlines currently.");
        }
        */

        sMacro = "<div class=\"toc\">\n<div class=\"head\">"
                + m_pTemplates->getTransTOC() + "</div>\n";
        for (int i = 0; i < m_sListHeadline_1.size(); i++) {
            sMacro += " 1. [#" + sListHeadline_1_Links[i] + " "
                    + m_sListHeadline_1[i] + "]\n";
        }
        sMacro += "\n</div>\n";

        sDoc.replace(nPos, findMacro.matchedLength(), sMacro);
        // Go on with new start position
        nPos += sMacro.length();
    }

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceImages(QTextDocument *p_rawDoc) {
    QString sDoc(p_rawDoc->toPlainText());
    QRegExp findImages("\\[\\[" + m_pTemplates->getTransImage()
                       + "\\(.+\\)\\]\\]");
    int iLength;
    QString sTmpImage;
    QStringList sListTmpImageInfo;

    QString sImageUrl("");
    QString sImageAlign("");
    // QString sImageAlt("");

    QString sImagePath("");
    if ("" != m_sCurrentFile) {
        QFileInfo fiArticleFile(m_sCurrentFile);
        sImagePath = fiArticleFile.absolutePath();
    }

    double iImgHeight, iImgWidth;
    double tmpH, tmpW;

    findImages.setMinimal(true);
    int myindex = findImages.indexIn(sDoc);
    while (myindex >= 0) {
        iLength = findImages.matchedLength();
        sTmpImage = findImages.cap();

        sTmpImage.remove("[[" + m_pTemplates->getTransImage() + "(");
        sTmpImage.remove(")]]");

        sImageAlign = "";
        iImgHeight = 0;
        iImgWidth = 0;
        tmpH = 0;
        tmpW = 0;

        sListTmpImageInfo.clear();
        sListTmpImageInfo << sTmpImage.split(",");

        sImageUrl = sListTmpImageInfo[0].trimmed();
        if (sImageUrl.startsWith("Wiki/")) {
            sImageUrl = m_tmpFileDir.absolutePath() + "/" + sImageUrl;
        } else if ("" != sImagePath &&
                   QFile(sImagePath + "/" + sImageUrl).exists()) {
            sImageUrl = sImagePath + "/" + sImageUrl;
        } else {
           sImageUrl = m_tmpImgDir.absolutePath() + "/" + sImageUrl;
        }

        for (int i = 1; i < sListTmpImageInfo.length(); i++) {
            // Found integer (width)
            if (sListTmpImageInfo[i].trimmed().toUInt() != 0) {
                tmpW = sListTmpImageInfo[i].trimmed().toUInt();
            } else if (sListTmpImageInfo[i].trimmed().startsWith("x")) {
                // Found x+integer (height)
                tmpH = sListTmpImageInfo[i].remove("x").trimmed().toUInt();
            } else if (sListTmpImageInfo[i].contains("x")) {
                // Found int x int (width x height)
                QString sTmp = sListTmpImageInfo[i];  // Copy needed!
                tmpW = sListTmpImageInfo[i].remove(
                            sListTmpImageInfo[i].indexOf("x"),
                            sListTmpImageInfo[i].length()).trimmed().toUInt();
                tmpH = sTmp.remove(0, sTmp.indexOf("x")+1).trimmed().toUInt();
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
        sDoc.replace(myindex, iLength, sTmpImage);

        // Go on with RegExp-Search
        myindex = findImages.indexIn(sDoc, myindex + iLength);
    }

    // Replace p_rawDoc with adapted document
    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceLists(QTextDocument *p_rawDoc) {
    QString sDoc("");
    QString sLine("");
    int nPreviousIndex = -1;
    int nCurrentIndex = -1;
    QList<bool> bArrayListType;  // Unsorted = false, sorted = true

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if (block.text().trimmed().startsWith("*")
                || block.text().trimmed().startsWith("1.")) {
            sLine = block.text();

            if (sLine.indexOf(" * ") >= 0) {  // Unsorted list
                nPreviousIndex = nCurrentIndex;
                nCurrentIndex = sLine.indexOf(" * ");
                sLine.remove(0, sLine.indexOf(" * ") + 3);
                // qDebug() << "LIST:" << sLine << nCurrentIndex << false;

                if (nCurrentIndex != nPreviousIndex) {
                    if (nCurrentIndex > nPreviousIndex) {  // New tag
                        sDoc += "<ul>\n";
                        bArrayListType << false;
                    } else {  // Close previous tag and maybe create new
                        if (!bArrayListType.isEmpty()) {
                            if (false == bArrayListType.last()) {
                                sDoc += "</ul>\n";
                            } else {
                                sDoc += "</ol>\n";
                            }
                            bArrayListType.removeLast();
                        }

                        if (!bArrayListType.isEmpty()) {
                            if (true == bArrayListType.last()) {
                                sDoc += "</ol>\n<ul>\n";
                                bArrayListType.removeLast();
                                bArrayListType << false;
                            }
                        }
                    }
                }
                sDoc += "<li>" + sLine + "</li>\n";

            } else if (sLine.indexOf(" 1. ") >= 0) {  // Sorted list
                nPreviousIndex = nCurrentIndex;
                nCurrentIndex = sLine.indexOf(" 1. ");
                sLine.remove(0, sLine.indexOf(" 1. ") + 4);
                // qDebug() << "LIST:" << sLine << nCurrentIndex << true;

                if (nCurrentIndex != nPreviousIndex) {
                    if (nCurrentIndex > nPreviousIndex) {  // New tag
                        sDoc += "<ol class=\"arabic\">\n";
                        bArrayListType << true;
                    } else {  // Close previous tag and maybe create new
                        if (!bArrayListType.isEmpty()) {
                            if (false == bArrayListType.last()) {
                                sDoc += "</ul>\n";
                            } else {
                                sDoc += "</ol>\n";
                            }
                            bArrayListType.removeLast();
                        }

                        if (!bArrayListType.isEmpty()) {
                            if (false == bArrayListType.last()) {
                                sDoc += "</ul>\n<ol class=\"arabic\">\n";
                                bArrayListType.removeLast();
                                bArrayListType << true;
                            }
                        }
                    }
                }
                sDoc += "<li>" + sLine + "</li>\n";

            } else {  // Not a list element
                // Close all open tags
                while (!bArrayListType.isEmpty()) {
                    if (false == bArrayListType.last()) {
                        sDoc += "</ul>\n";
                    } else {
                        sDoc += "</ol>\n";
                    }
                    bArrayListType.removeLast();
                }
                nPreviousIndex = -1;
                nCurrentIndex = -1;
                sDoc += block.text() + "\n";
                // qDebug() << "LIST END";
            }

        } else {  // Everything else
            // Close all open tags
            while (!bArrayListType.isEmpty()) {
                if (false == bArrayListType.last()) {
                    sDoc += "</ul>\n";
                } else {
                    sDoc += "</ol>\n";
                }
                bArrayListType.removeLast();
            }
            nPreviousIndex = -1;
            nCurrentIndex = -1;
            sDoc += block.text() + "\n";
            // qDebug() << "LIST END";
        }
    }

    p_rawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CParser::replaceTables(QTextDocument *p_rawDoc) {
    Q_UNUSED(p_rawDoc);
/*
    QString sDoc("");
    QString sLine("");
    bool bLineStart = false;
    bool bLineEnd = false;

    // Go through each text block
    for (QTextBlock block = p_rawDoc->firstBlock();
         block.isValid() && !(p_rawDoc->lastBlock() < block);
         block = block.next()) {
        if (block.text().trimmed().startsWith("||")
                && block.text().trimmed().endsWith("||")
                && bLineStart == false) {


        } else if (block.text().trimmed().startsWith("||")) {
            bLineStart = true;
            sLine = block.text().trimmed()
        } else if(bLineStart) {

        }
    }
*/
}
