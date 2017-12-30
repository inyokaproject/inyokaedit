/**
 * \file parser.cpp
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
 * Parse plain text with inyoka syntax into html code.
 */

#include "./parser.h"
#include "./parseimgmap.h"
#include "./parselist.h"
#include "./parsetable.h"
#include "./parsetextformats.h"
#include "../syntaxcheck.h"

Parser::Parser(const QString &sSharePath,
               const QDir &tmpImgDir,
               const QString &sInyokaUrl,
               const bool bCheckLinks,
               Templates *pTemplates,
               const QString &sCommunity)
  : m_pRawText(NULL),
    m_sSharePath(sSharePath),
    m_tmpImgDir(tmpImgDir),
    m_sInyokaUrl(sInyokaUrl),
    m_pTemplates(pTemplates),
    m_sCommunity(sCommunity) {
  m_pMacros = new Macros(m_sSharePath, m_tmpImgDir);

  m_pTemplateParser = new ParseTemplates(
                        m_pMacros->getTplTranslations(),
                        m_pTemplates->getListTplNamesINY(),
                        m_pTemplates->getListFormatHtmlStart(),
                        m_sSharePath, m_tmpImgDir,
                        m_pTemplates->getListTestedWith(),
                        m_pTemplates->getListTestedWithStrings(),
                        m_pTemplates->getListTestedWithTouch(),
                        m_pTemplates->getListTestedWithTouchStrings(),
                        m_sCommunity);

  m_pLinkParser = new ParseLinks(m_sInyokaUrl,
                                 m_pTemplates->getListIWLs(),
                                 m_pTemplates->getListIWLUrls(),
                                 bCheckLinks);
}

Parser::~Parser() {
  if (NULL != m_pLinkParser) {
    delete m_pLinkParser;
    m_pLinkParser = NULL;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::updateSettings(const QString &sInyokaUrl,
                            const bool bCheckLinks) {
  m_sInyokaUrl = sInyokaUrl;
  m_pLinkParser->updateSettings(sInyokaUrl, bCheckLinks);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Parser::genOutput(const QString &sActFile,
                          QTextDocument *pRawDocument,
                          const bool &bSyntaxCheck) {
  qDebug() << "Parsing...";
  // Need a copy otherwise text in editor will be changed
  m_pRawText = pRawDocument->clone();
  m_sCurrentFile = sActFile;
  QStringList sListHeadlines;
  this->removeComments(m_pRawText);

  m_sListNoTranslate.clear();
  this->filterEscapedChars(m_pRawText);  // Before everything
  this->filterNoTranslate(m_pRawText);   // Before replaceCodeblocks()
  if (bSyntaxCheck) {
    qint32 nRet = SyntaxCheck::checkInyokaSyntax(m_pRawText,
                                                 m_pTemplates->getListTplNamesINY(),
                                                 m_pTemplates->getListSmilies(),
                                                 m_pMacros->getTplTranslations());
    emit this->hightlightSyntaxError(nRet);
  }
  this->replaceCodeblocks(m_pRawText);

  m_pTemplateParser->startParsing(m_pRawText, m_sCurrentFile);

  this->replaceHeadlines(m_pRawText, sListHeadlines);  //Generates list for TOC
  ParseTable::startParsing(m_pRawText);
  m_pMacros->startParsing(m_pRawText, m_sCurrentFile,
                          m_sCommunity, sListHeadlines);
  ParseList::startParsing(m_pRawText);
  m_pLinkParser->startParsing(m_pRawText);

  // Replace flags
  ParseImgMap::startParsing(m_pRawText,
                            m_pTemplates->getListFlags(),
                            m_pTemplates->getListFlagsImg(),
                            m_sSharePath,
                            m_sCommunity);
  // Replace smilies
  ParseImgMap::startParsing(m_pRawText,
                            m_pTemplates->getListSmilies(),
                            m_pTemplates->getListSmiliesImg(),
                            m_sSharePath,
                            m_sCommunity);

  ParseTextformats::startParsing(m_pRawText,
                                 m_pTemplates->getListFormatStart(),
                                 m_pTemplates->getListFormatEnd(),
                                 m_pTemplates->getListFormatHtmlStart(),
                                 m_pTemplates->getListFormatHtmlEnd());

  this->replaceQuotes(m_pRawText);
  this->replaceHorLines(m_pRawText);
  this->generateParagraphs(m_pRawText);
  this->replaceFootnotes(m_pRawText);

  this->reinstertNoTranslate(m_pRawText);

  // File name
  QString sFilename;
  if (m_sCurrentFile.isEmpty()) {
    sFilename = "Untitled";
  } else {
    QFileInfo fi(m_sCurrentFile);
    sFilename = fi.baseName();
    sFilename.replace("_", " ");
  }

  // Replace template tags
  // Copy needed, otherwise %tags% will be replaced/removed in template!
  QString sTemplateCopy(m_pTemplates->getPreviewTemplate());
  sTemplateCopy = sTemplateCopy.replace("%filename%", sFilename);
  sTemplateCopy = sTemplateCopy.replace("%folder%",
                                        m_sSharePath + "/community/" +
                                        m_sCommunity + "/web");
  sTemplateCopy = sTemplateCopy.replace("%date%",
                                        QDate::currentDate().toString("dd.MM.yyyy"));
  sTemplateCopy = sTemplateCopy.replace("%time%",
                                        QTime::currentTime().toString("hh:mm"));
  sTemplateCopy = sTemplateCopy.replace("%tags%",
                                        this->generateTags(m_pRawText));
  sTemplateCopy = sTemplateCopy.replace("%content%",
                                        m_pRawText->toPlainText());
  return sTemplateCopy;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
/*
void Parser::replaceTemplates(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QString sMacro;
  QStringList sListArguments;
  int nPos = 0;
  QStringList sListTplRegExp;
  sListTplRegExp << "\\[\\[" + m_pTemplates->getTransTemplate()
                    + "\\(.+\\)\\]\\]";
  //                 << "\\{\\{\\{#!" + m_pTemplates->getTransTemplate()
  //                    + " .+\\}\\}\\}";

  QRegExp findTemplate(sListTplRegExp[0], Qt::CaseInsensitive);
  findTemplate.setMinimal(true);

  while ((nPos = findTemplate.indexIn(sDoc, nPos)) != -1) {
    sMacro = findTemplate.cap(0);
    // qDebug() << "CAPTURED:" << sMacro;

    for (int i = 0; i < m_pTemplates->getListTplNamesINY().size(); i++) {
      if (sMacro.startsWith("[[" + m_pTemplates->getTransTemplate()
                            + "(" + m_pTemplates->getListTplNamesINY()[i],
                            Qt::CaseInsensitive)) {
        qDebug() << "Found known macro:"
                 << m_pTemplates->getListTplNamesINY()[i];
        sMacro.remove("[[" + m_pTemplates->getTransTemplate() + "(");
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

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}
*/
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceCodeblocks(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QStringList sListTplRegExp;
  // Search for {{{#!code ...}}} and {{{ ... without #!X ...}}}
  sListTplRegExp << "\\{\\{\\{#!code .+\\}\\}\\}"
                 << "\\{\\{\\{(?!#!\\S).+\\}\\}\\}";
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
      if (sMacro.startsWith("#!code ", Qt::CaseInsensitive)) {
        bFormated = true;
        sMacro.remove("#!code ", Qt::CaseInsensitive);
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
          if (!sListLines[0].trimmed().isEmpty()) {
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

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Parser::highlightCode(const QString &sLanguage, const QString &sCode) {
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
      qCritical() << "Pygments error: Could not start echo.";
      procEcho.kill();
      return sCode;
    }
    if (!procEcho.waitForFinished()) {
      QMessageBox::critical(0, "Pygments error",
                            "Error while using echo.");
      qCritical() << "Pygments error: While using echo.";
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
      qCritical() << "Error while starting pygmentize - waitForStarted";
      procPygmentize.kill();
      return sCode;
    }
    if (!procPygmentize.waitForFinished()) {
      QMessageBox::critical(0, "Pygments error",
                            "Error while using pygmentize.");
      qCritical() << "Error while executing pygmentize - waitForFinished";
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

void Parser::filterEscapedChars(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QRegExp pattern("\\\\.", Qt::CaseInsensitive);
  QString sEscChar("");
  int nPos(0);
  unsigned int nNoTranslate(0);

  while ((nPos = pattern.indexIn(sDoc, nPos)) != -1) {
    sEscChar = pattern.cap(0);
    if ("\\\\" != sEscChar) {
      sEscChar.remove(0, 1);  // Remove escape char
      nNoTranslate = m_sListNoTranslate.size();
      m_sListNoTranslate << sEscChar;

      sDoc.replace(nPos, pattern.matchedLength(), "%%NO_TRANSLATE_" +
                   QString::number(nNoTranslate) + "%%");
    }
    // Go on with search
    nPos += sEscChar.length();
  }
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::filterNoTranslate(QTextDocument *pRawDoc) {
  QStringList sListFormatStart;
  QStringList sListFormatEnd;
  QStringList sListHtmlStart;
  QStringList sListHtmlEnd;
  QString sDoc("");
  QRegExp patternFormat;
  QString sFormatedText;
  int nIndex;
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

  ParseTextformats::startParsing(pRawDoc, sListFormatStart, sListFormatEnd,
                                 sListHtmlStart, sListHtmlEnd);

  patternFormat.setCaseSensitivity(Qt::CaseInsensitive);
  patternFormat.setMinimal(true);  // Search only for smallest match

  sDoc = pRawDoc->toPlainText();  // Init sDoc here; AFTER raw doc is changed
  // qDebug() << "\n\n" << sDoc << "\n\n";
  nNoTranslate = m_sListNoTranslate.size();
  for (int i = 0; i < sListHtmlStart.size(); i++) {
    patternFormat.setPattern(sListHtmlStart[i] + ".+" + sListHtmlEnd[i]);
    nIndex = patternFormat.indexIn(sDoc);

    while (nIndex >= 0) {
      sFormatedText = patternFormat.cap();
      m_sListNoTranslate << sFormatedText;
      nIndex = patternFormat.indexIn(sDoc,
                                     nIndex + sFormatedText.length());
      sDoc.replace(sFormatedText, "%%NO_TRANSLATE_" +
                   QString::number(nNoTranslate) + "%%");
      nNoTranslate++;
    }
  }
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::reinstertNoTranslate(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());

  // Reinsert filtered monotype codeblock
  // Has to be decremental, because of possible nested blocks
  for (int i = m_sListNoTranslate.size() - 1; i >= 0; i--) {
    sDoc.replace("%%NO_TRANSLATE_" + QString::number(i) + "%%",
                 m_sListNoTranslate[i]);
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceHorLines(QTextDocument *pRawDoc) {
  QString sDoc("");

  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if ("----" == block.text()) {
      sDoc += "\n<hr />\n";
    } else {
      sDoc += block.text() + "\n";
    }
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

QString Parser::generateTags(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QString sLine("");
  QString sTags("");
  QStringList sListTags;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
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

  pRawDoc->setPlainText(sDoc);
  return sTags;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceQuotes(QTextDocument *pRawDoc) {
  QString sDoc("");
  QString sLine("");
  quint16 nQuotes = 0;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
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

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::generateParagraphs(QTextDocument *pRawDoc) {
  QString sDoc("<p>\n");

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (block.text().trimmed().isEmpty()) {
      sDoc += "</p>\n<p>\n";
    } else {
      sDoc += block.text() + "\n";
    }
  }

  pRawDoc->setPlainText(sDoc + "</p>");
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::removeComments(QTextDocument *pRawDoc) {
  QString sDoc("");

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (!block.text().startsWith("##")) {
      sDoc += block.text() + "\n";
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceHeadlines(QTextDocument *pRawDoc,
                              QStringList &slistHeadlines) {
  QString sDoc("");
  QString sLine("");
  QString sTmp("");
  QString sLink("");
  quint16 nHeadlineLevel = 5;
  slistHeadlines.clear();

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
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

      switch (nHeadlineLevel) {
        case 1:
          sTmp = "##1##" + sLine;
          break;
        case 2:
          sTmp = "##2##" + sLine;
          break;
        case 3:
          sTmp = "##3##" + sLine;
          break;
        case 4:
          sTmp = "##4##" + sLine;
          break;
        case 5:
          sTmp = "##5##" + sLine;
          break;
        default:
          qWarning() << "Found strange formated headline:" << sLine;
      }
      slistHeadlines << sTmp;  // Used for table of contents

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
  // qDebug() << "HEADLINES:" << slistHeadlines;

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceFootnotes(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QString sRegExp("\\(\\(.*\\)\\)");
  QRegExp findMacro(sRegExp, Qt::CaseInsensitive);
  findMacro.setMinimal(true);
  QString sNote("");
  QString sIndex("");
  int nPos = 0;
  quint16 nIndex = 0;
  QString sFootnotes("");

  while ((nPos = findMacro.indexIn(sDoc, nPos)) != -1) {
    nIndex++;

    sNote = findMacro.cap(0);
    sNote.remove("((");
    sNote.remove("))");
    sFootnotes += "<li><a id=\"fn-" + QString::number(nIndex) +
                  "\" class="
                  "\"crosslink\" href=\"#bfn-" + QString::number(nIndex) +
                  "\">" + QString::number(nIndex) + "</a>: " + sNote +
                  "</li>\n";

    sIndex = "<a id=\"bfn-" + QString::number(nIndex) +
             "\" class=\""
             "footnote\" href=\"#fn-" + QString::number(nIndex) +
             "\">"
             "&#091;" + QString::number(nIndex) + "&#093;</a>";

    sDoc.replace(nPos, findMacro.matchedLength(), sIndex);
    // Go on with new start position
    nPos += sIndex.length();
  }

  if (!sFootnotes.isEmpty()) {
    sFootnotes = "<ul class=\"footnotes\">\n" + sFootnotes + "</ul>\n";
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc + sFootnotes);
}
