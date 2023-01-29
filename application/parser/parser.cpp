/**
 * \file parser.cpp
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-present The InyokaEdit developers
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
 * Parse plain text with inyoka syntax into html code.
 */

#include "./parser.h"

#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocument>

#include "./macros.h"
#ifndef USEQTWEBENGINE
#include "./parseimgmap.h"
#endif
#include "../syntaxcheck.h"
#include "../templates/templates.h"
#include "./parselinks.h"
#include "./parselist.h"
#include "./parsetable.h"
#include "./parsetemplates.h"
#include "./parsetextformats.h"
#include "./parsetxtmap.h"

Parser::Parser(const QString &sSharePath, const QDir &tmpImgDir,
               const QString &sInyokaUrl, const bool bCheckLinks,
               Templates *pTemplates, const QString &sCommunity,
               const QString &sPygmentize, QObject *pParent)
    : m_pRawText(nullptr),
      m_sSharePath(sSharePath),
      m_tmpImgDir(tmpImgDir),
      m_sInyokaUrl(sInyokaUrl),
      m_pTemplates(pTemplates),
      m_sCommunity(sCommunity),
      m_sPygmentize(sPygmentize),
      m_nTimedPreview(0) {
  Q_UNUSED(pParent)
  m_pMacros = new Macros(m_sSharePath, m_tmpImgDir);

  m_pTemplateParser = new ParseTemplates(
      m_pMacros->getTplTranslations(), m_pTemplates->getTemplateMap().keys(),
      m_pTemplates->getFormatStartMap().second, m_sSharePath, m_tmpImgDir,
      m_pTemplates->getTestedWithMap(), m_pTemplates->getTestedWithTouchMap(),
      m_sCommunity);

  m_pLinkParser =
      new ParseLinks(m_sInyokaUrl, m_pTemplates->getIwlMap(), bCheckLinks);
}

Parser::~Parser() {
  if (nullptr != m_pLinkParser) {
    delete m_pLinkParser;
    m_pLinkParser = nullptr;
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::updateSettings(const QString &sInyokaUrl, const bool bCheckLinks,
                            const quint32 nTimedPreview) {
  m_sInyokaUrl = sInyokaUrl;
  m_pLinkParser->updateSettings(sInyokaUrl, bCheckLinks);
#ifdef NOPREVIEW
  m_nTimedPreview = nTimedPreview;
#else
  Q_UNUSED(nTimedPreview)
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Parser::genOutput(const QString &sActFile, QTextDocument *pRawDocument,
                       const bool bSyntaxCheck) -> QString {
  qDebug() << "Parsing...";
  // Need a copy otherwise text in editor will be changed
  m_pRawText = pRawDocument->clone();
  m_sCurrentFile = sActFile;
  Parser::removeComments(m_pRawText);

  if (bSyntaxCheck) {
    QPair<int, QString> ret = SyntaxCheck::checkInyokaSyntax(
        m_pRawText, m_pTemplates->getTemplateMap().keys(),
        m_pTemplates->getSmiliesTxtMap().first,
        m_pMacros->getTplTranslations());
    emit this->hightlightSyntaxError(ret);
  }

  m_sListNoTranslate.clear();
  this->filterEscapedChars(m_pRawText);  // Before everything
  this->filterNoTranslate(m_pRawText);   // Before replaceCodeblocks()
  this->replaceCodeblocks(m_pRawText);

  m_pTemplateParser->startParsing(m_pRawText, m_sCurrentFile);

  QStringList sListHeadlines;
  sListHeadlines = Parser::replaceHeadlines(m_pRawText);  // Returns TOC list
  ParseTable::startParsing(m_pRawText);
  m_pMacros->startParsing(m_pRawText, m_sCurrentFile, m_sCommunity,
                          sListHeadlines);
  ParseList::startParsing(m_pRawText);
  m_pLinkParser->startParsing(m_pRawText);

  Parser::replaceHorLines(m_pRawText);  // Before smilies, because of -- smiley

  ParseTextformats::startParsing(m_pRawText, m_pTemplates->getFormatStartMap(),
                                 m_pTemplates->getFormatEndMap());

  // Replace smilies
  ParseTxtMap::startParsing(m_pRawText, m_pTemplates->getSmiliesTxtMap());

  // Replace flags
  // After smilies, because some smilies are using flag format (e.g. {dl})
#ifdef USEQTWEBENGINE
  // Only Qt WebEngine is able to render unicode flags
  this->replaceFlags(m_pRawText);
#else
  ParseImgMap::startParsing(m_pRawText, m_pTemplates->getFlagMap(),
                            m_sSharePath, m_sCommunity);
#endif

  Parser::replaceQuotes(m_pRawText);
  Parser::generateParagraphs(m_pRawText);
  Parser::replaceFootnotes(m_pRawText);

  this->reinstertNoTranslate(m_pRawText);

  // File name
  QString sFilename;
  if (m_sCurrentFile.isEmpty()) {
    sFilename = QStringLiteral("Untitled");
  } else {
    QFileInfo fi(m_sCurrentFile);
    sFilename = fi.baseName();
    sFilename.replace(QLatin1String("_"), QLatin1String(" "));
  }

  // Replace template tags
  // Copy needed, otherwise %tags% will be replaced/removed in template!
  QString sTemplateCopy(m_pTemplates->getPreviewTemplate());
  sTemplateCopy = sTemplateCopy.replace(QLatin1String("%filename%"), sFilename);
  sTemplateCopy = sTemplateCopy.replace(
      QLatin1String("%folder%"),
      m_sSharePath + "/community/" + m_sCommunity + "/web");
  sTemplateCopy = sTemplateCopy.replace(
      QLatin1String("%date%"),
      QDate::currentDate().toString(QStringLiteral("dd.MM.yyyy")));
  sTemplateCopy = sTemplateCopy.replace(
      QLatin1String("%time%"),
      QTime::currentTime().toString(QStringLiteral("hh:mm")));
  sTemplateCopy = sTemplateCopy.replace(QLatin1String("%tags%"),
                                        this->generateTags(m_pRawText));
  sTemplateCopy = sTemplateCopy.replace(QLatin1String("%content%"),
                                        m_pRawText->toPlainText());
  QString sRefresh(QLatin1String(""));
  if (m_nTimedPreview > 0) {
    sRefresh = "<meta http-equiv=\"refresh\" content=\"" +
               QString::number(m_nTimedPreview) + "\">";
  }
  sTemplateCopy = sTemplateCopy.replace(QLatin1String("%refresh%"), sRefresh);
  return sTemplateCopy;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
/*
void Parser::replaceTemplates(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findTemplate;
  QRegularExpressionMatch match;
  QString sMacro;
  QStringList sListArguments;
  int nIndex = 0;

  QStringList sListTplRegExp;
  sListTplRegExp.reserve(m_pMacros->getTplTranslations().size());
  for (const auto &s : m_pMacros->getTplTranslations()) {
    sListTplRegExp << "\\[\\[" + s + "\\(.+\\)\\]\\]";
    // << "\\{\\{\\{#!" + s + " .+\\}\\}\\}";
  }

  findTemplate.setPatternOptions(
        QRegularExpression::InvertedGreedinessOption |  // Only smallest match
        QRegularExpression::DotMatchesEverythingOption |
        QRegularExpression::CaseInsensitiveOption);

  for (const auto &sRegExp : qAsConst(sListTplRegExp)) {
    findTemplate.setPattern(sRegExp);

    while ((match = findTemplate.match(sDoc, nIndex)).hasMatch()) {
      nIndex = match.capturedStart();
      sMacro = match.captured(0);
      // qDebug() << "CAPTURED:" << sMacro;

      for (const auto &s : m_pMacros->getTplTranslations()) {
        for (const auto &sName : m_pTemplates->getTemplateMap().keys()) {
          if (sMacro.startsWith("[[" + s + "(" + sName, Qt::CaseInsensitive)) {
            qDebug() << "Found known macro:" << sName;
            sMacro.remove(0, sMacro.indexOf(QLatin1String("(")) + 1);
            sMacro.remove(QStringLiteral(")]]"));

            // Split by ',' but DON'T split quoted strings containing commas
            QStringList tmpList = sMacro.split(
                  QRegularExpression(QStringLiteral("\"")));  // Split at "
            bool bInside = false;
            sListArguments.clear();
            for (const auto &s : qAsConst(tmpList)) {
              if (bInside) {
                // If 's' is inside quotes, get the whole string
                sListArguments.append(s);
              } else {
                // If 's' is outside quotes, get the split string
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                sListArguments.append(s.split(QRegularExpression(
                                                QStringLiteral(",+")),
                                              QString::SkipEmptyParts));
#else
                sListArguments.append(s.split(QRegularExpression(
                                                QStringLiteral(",+")),
                                              Qt::SkipEmptyParts));
#endif
              }
              bInside = !bInside;
            }

            sListArguments.removeAll(QStringLiteral(" "));
            sListArguments.removeFirst();  // Remove template name

            // Replace arguments
            sMacro = m_pTemplates->getInyokaTplLangMap().value(sName);
            for (int k = 0; k < sListArguments.size(); k++) {
              sMacro.replace("<@ $arguments." + QString::number(k)
                             + " @>", sListArguments[k].trimmed());
            }

            sDoc.replace(nIndex, match.capturedLength(), sMacro);
          }
        }
      }

      // Go on with new start position
      nIndex += sMacro.length();
      // nPos += findTemplate.matchedLength();
    }
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
  sListTplRegExp << QStringLiteral("\\{\\{\\{#!code .+\\}\\}\\}")
                 << QStringLiteral("\\{\\{\\{(?!#!\\S).+\\}\\}\\}");
  QStringList sListLines;

  for (int k = 0; k < sListTplRegExp.size(); k++) {
    QRegularExpression findTemplate(
        sListTplRegExp[k],
        QRegularExpression::InvertedGreedinessOption |  // Only smallest match
            QRegularExpression::DotMatchesEverythingOption |
            QRegularExpression::CaseInsensitiveOption);
    int nIndex = 0;
    QRegularExpressionMatch match;

    while ((match = findTemplate.match(sDoc, nIndex)).hasMatch()) {
      nIndex = match.capturedStart();
      bool bFormated = false;
      QString sMacro = match.captured(0);
      sMacro.remove(QStringLiteral("{{{\n"));
      sMacro.remove(QStringLiteral("{{{"));
      if (sMacro.startsWith(QLatin1String("#!code "), Qt::CaseInsensitive)) {
        bFormated = true;
        sMacro.remove(QStringLiteral("#!code "), Qt::CaseInsensitive);
      }
      sMacro.remove(QStringLiteral("\n}}}"));
      sMacro.remove(QStringLiteral("}}}"));

      sListLines.clear();
      sListLines = sMacro.split(QRegularExpression(QStringLiteral("\\n")));

      // Only plain code
      if (!bFormated) {
        sMacro = QStringLiteral("<pre>");
        for (int i = 0; i < sListLines.size(); i++) {
          // Replace char "<" because it will be interpreted as
          // html tag (see bug #826482)
          sListLines[i].replace('<', QLatin1String("&lt;"));
          sMacro += sListLines[i];
          if (i < sListLines.size() - 1) {
            sMacro += QLatin1String("\n");
          }
        }
        sMacro += QLatin1String("</pre>\n");
      } else {  // Syntax highlighting
        sMacro = QStringLiteral(
            "<div class=\"code\">\n<table "
            "class=\"syntaxtable\"><tbody>\n<tr>\n<td "
            "class=\"linenos\">\n<div class=\"linenodiv\">"
            "<pre>");

        // First column (line numbers)
        for (int i = 1; i < sListLines.size(); i++) {
          sMacro += QString::number(i);
          if (i < sListLines.size() - 1) {
            sMacro += QLatin1String("\n");
          }
        }

        // Second column (code)
        sMacro += QLatin1String(
            "</pre>\n</div>\n</td>\n<td class=\"code\">\n"
            "<div class=\"syntax\">\n<pre>\n");

        QString sCode(QLatin1String(""));

        for (int i = 1; i < sListLines.length(); i++) {
          // Replace char "<" because it will be interpreted as
          // html tag (see bug #826482)
          // sListElements[i].replace('<', "&lt;");

          sCode += sListLines[i];
          if (i < sListLines.size() - 1) {
            sCode += QLatin1String("\n");
          }
        }

        // Syntax highlighting (with pygments if available)
        if (!sListLines.isEmpty()) {
          if (!sListLines[0].trimmed().isEmpty()) {
            sCode = this->highlightCode(sListLines[0], sCode);
          }
        }
        sMacro += sCode +
                  "</pre>\n</div>\n</td>\n</tr>\n</tbody>\n"
                  "</table>\n</div>";
      }

      auto nNoTranslate = static_cast<unsigned int>(m_sListNoTranslate.size());
      m_sListNoTranslate << sMacro;  // Save code block
      sMacro = "%%NO_TRANSLATE_" + QString::number(nNoTranslate) + "%%";

      sDoc.replace(nIndex, match.capturedLength(), sMacro);
      // Go on with new start position
      nIndex += sMacro.length();
    }
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Parser::highlightCode(const QString &sLanguage, const QString &sCode)
    -> QString {
  static bool bChecked(false);
  static bool bPygmentize(false);
  static QFile sPygmentize(m_sPygmentize);
  if (!bChecked) {
    bChecked = true;
    if (sPygmentize.exists()) {
      bPygmentize = true;
      qDebug() << "Pygmentize found:" << sPygmentize.fileName();
    } else {
      qDebug() << "Pygmentize NOT found:" << sPygmentize.fileName();
    }
  }

  if (bPygmentize) {
    QProcess procPygmentize;
    QProcess procEcho;

    // Workaround for passing stdin string with code to pygmentize
    procEcho.setStandardOutputProcess(&procPygmentize);
    procEcho.start(QStringLiteral("echo"), QStringList() << sCode);
    if (!procEcho.waitForStarted()) {
      QMessageBox::critical(nullptr, QStringLiteral("Pygments error"),
                            QStringLiteral("Could not start echo."));
      qCritical() << "Pygments error: Could not start echo.";
      procEcho.kill();
      return sCode;
    }
    if (!procEcho.waitForFinished()) {
      QMessageBox::critical(nullptr, QStringLiteral("Pygments error"),
                            QStringLiteral("Error while using echo."));
      qCritical() << "Pygments error: While using echo.";
      procEcho.kill();
      return sCode;
    }

    procPygmentize.start(
        sPygmentize.fileName(),
        QStringList() << QStringLiteral("-l") << sLanguage
                      << QStringLiteral("-f") << QStringLiteral("html")
                      << QStringLiteral("-O") << QStringLiteral("nowrap")
                      << QStringLiteral("-O") << QStringLiteral("noclasses"));

    if (!procPygmentize.waitForStarted()) {
      QMessageBox::critical(nullptr, QStringLiteral("Pygments error"),
                            QStringLiteral("Could not start pygmentize."));
      qCritical() << "Error while starting pygmentize - waitForStarted";
      procPygmentize.kill();
      return sCode;
    }
    if (!procPygmentize.waitForFinished()) {
      QMessageBox::critical(nullptr, QStringLiteral("Pygments error"),
                            QStringLiteral("Error while using pygmentize."));
      qCritical() << "Error while executing pygmentize - waitForFinished";
      procPygmentize.kill();
      return sCode;
    }

    return QString::fromUtf8(procPygmentize.readAll());
  }

  return sCode;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::filterEscapedChars(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression pattern(QStringLiteral("\\\\."),
                             QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatch match;
  QString sEscChar;
  int nIndex = 0;
  unsigned int nNoTranslate;

  while ((match = pattern.match(sDoc, nIndex)).hasMatch()) {
    nIndex = match.capturedStart();
    sEscChar = match.captured(0);
    if ("\\\\" != sEscChar) {
      sEscChar.remove(0, 1);  // Remove escape char
      nNoTranslate = static_cast<unsigned int>(m_sListNoTranslate.size());
      m_sListNoTranslate << sEscChar;

      sEscChar = "%%NO_TRANSLATE_" + QString::number(nNoTranslate) + "%%";
      sDoc.replace(nIndex, match.capturedLength(), sEscChar);
    }
    // Go on with search
    nIndex += sEscChar.length();
  }
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::filterNoTranslate(QTextDocument *pRawDoc) {
  static QPair<QStringList, QStringList> FormatStartNoTranslateMap(
      m_pTemplates->getFormatStartNoTranslateMap());
  static QPair<QStringList, QStringList> FormatEndNoTranslateMap(
      m_pTemplates->getFormatEndNoTranslateMap());
  QString sDoc;
  QRegularExpression patternFormat;
  unsigned int nNoTranslate;

  ParseTextformats::startParsing(pRawDoc, FormatStartNoTranslateMap,
                                 FormatEndNoTranslateMap);
  sDoc = pRawDoc->toPlainText();  // Init sDoc here; AFTER raw doc is changed

  patternFormat.setPatternOptions(
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
      QRegularExpression::DotMatchesEverythingOption |
      QRegularExpression::CaseInsensitiveOption);

  nNoTranslate = static_cast<unsigned int>(m_sListNoTranslate.size());
  for (int i = 0; i < FormatStartNoTranslateMap.second.size(); i++) {
    patternFormat.setPattern(FormatStartNoTranslateMap.second.at(i) + ".+" +
                             FormatEndNoTranslateMap.second.at(i));
    int nIndex = 0;
    QRegularExpressionMatch match;

    while ((match = patternFormat.match(sDoc, nIndex)).hasMatch()) {
      nIndex = match.capturedStart();
      QString sFormatedText = match.captured();
      m_sListNoTranslate << sFormatedText;
      QString sTmp = "%%NO_TRANSLATE_" + QString::number(nNoTranslate) + "%%";
      sDoc.replace(sFormatedText, sTmp);

      nIndex += sTmp.length();
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
  QString sDoc(QLatin1String(""));

  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if ("----" == block.text()) {
      sDoc += QLatin1String("\n<hr />\n");
    } else {
      sDoc += block.text() + "\n";
    }
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Parser::generateTags(QTextDocument *pRawDoc) -> QString {
  QString sDoc(pRawDoc->toPlainText());
  QString sLine;
  QString sTags(QLatin1String(""));
  QStringList sListTags;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (block.text().trimmed().startsWith(QLatin1String("#tag:")) ||
        block.text().trimmed().startsWith(QLatin1String("# tag:"))) {
      sLine = block.text();
      sTags = block.text().trimmed();
      sTags.remove(QStringLiteral("#tag:"));
      sTags.remove(QStringLiteral("# tag:"));
      sTags = sTags.trimmed();
      QStringList sListElements = sTags.split(QStringLiteral(","));
      sListTags << sListElements;
      sDoc.replace(sLine, QLatin1String(""));
    }
  }

  sTags.clear();
  for (int i = 0; i < sListTags.size(); i++) {
    sListTags[i].remove(QStringLiteral(" "));
    sTags += " <a href=\"" + m_sInyokaUrl + "/Wiki/Tags?tag=" + sListTags[i] +
             "\">" + sListTags[i] + "</a>";
    if (i < sListTags.size() - 1) {
      sTags += QLatin1String(",");
    }
  }

  pRawDoc->setPlainText(sDoc);
  return sTags;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#ifdef USEQTWEBENGINE
void Parser::replaceFlags(QTextDocument *pRawDoc) {
  QRegularExpression findFlag(QStringLiteral("\\{([a-z]{2}|[A-Z]{2})\\}"));
  QString sDoc(pRawDoc->toPlainText());
  QString sCountry;
  QString sHtml(QLatin1String(""));
  int nIndex = 0;
  int nLength(4);
  QRegularExpressionMatch match;

  while ((match = findFlag.match(sDoc, nIndex)).hasMatch()) {
    nIndex = match.capturedStart();
    sHtml.clear();
    sCountry = match.captured(1);
    sCountry = sCountry.toLower();
    if ("en" == sCountry) {
      sCountry = QStringLiteral("gb");
    }
    for (const auto ch : qAsConst(sCountry)) {
      // Unicode char - (Unicode 'a' 97) + (Unicode reg. indicator 'a' 127462)
      // qDebug() << (int)ch.unicode() - 97 + 127462;
      sHtml += "&#" +
               QString::number(static_cast<int>(ch.unicode()) - 97 + 127462) +
               ";";
    }

    sDoc.replace(nIndex, nLength, sHtml);
    nIndex += sHtml.length();
  }

  pRawDoc->setPlainText(sDoc);
}
#endif

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceQuotes(QTextDocument *pRawDoc) {
  QString sDoc(QLatin1String(""));
  QString sLine;
  quint16 nQuotes;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (block.text().startsWith(QLatin1String(">"))) {
      sLine = block.text().trimmed();
      nQuotes = static_cast<quint16>(sLine.count(QStringLiteral(">")));
      sLine.remove(QRegularExpression(QStringLiteral("^>*")));
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
  QString sDoc(QStringLiteral("<p>\n"));

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (block.text().trimmed().isEmpty()) {
      sDoc += QLatin1String("</p>\n<p>\n");
    } else {
      sDoc += block.text() + "\n";
    }
  }
  sDoc += QLatin1String("</p>");

  pRawDoc->setPlainText(sDoc.remove(QStringLiteral("<p>\n</p>\n")));
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::removeComments(QTextDocument *pRawDoc) {
  QString sDoc(QLatin1String(""));

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    if (!block.text().startsWith(QLatin1String("##"))) {
      sDoc += block.text() + "\n";
    }
  }

  pRawDoc->setPlainText(sDoc);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

auto Parser::replaceHeadlines(QTextDocument *pRawDoc) -> QStringList {
  static const quint8 MAXHEAD = 5;
  QString sDoc(QLatin1String(""));
  QString sLine;
  QString sTmp(QLatin1String(""));
  QString sLink(QLatin1String(""));
  quint8 nHeadlineLevel;
  QStringList slistHeadlines;

  // Go through each text block
  for (QTextBlock block = pRawDoc->firstBlock();
       block.isValid() && !(pRawDoc->lastBlock() < block);
       block = block.next()) {
    // Order is important! First level 5, 4, 3, 2, 1
    for (int i = MAXHEAD; i >= 0; i--) {
      sLine = block.text();
      sTmp.fill('=', i);
      if (0 == i) {
        sDoc += sLine + "\n";
        break;
      }
      if (sLine.trimmed().startsWith(sTmp) && sLine.trimmed().endsWith(sTmp) &&
          sLine.trimmed().length() > (i * 2)) {
        nHeadlineLevel = static_cast<quint8>(i);
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
      sLink.replace(QLatin1String(" "), QLatin1String("-"));
      sLink.replace(QStringLiteral("Ä"), QLatin1String("Ae"));
      sLink.replace(QStringLiteral("Ü"), QLatin1String("Ue"));
      sLink.replace(QStringLiteral("Ö"), QLatin1String("Oe"));
      sLink.replace(QStringLiteral("ä"), QLatin1String("ae"));
      sLink.replace(QStringLiteral("ü"), QLatin1String("ue"));
      sLink.replace(QStringLiteral("ö"), QLatin1String("oe"));

      // HeadlineLevel + 1 !
      sLine = "<h" + QString::number(nHeadlineLevel + 1) + " id=\"" + sLink +
              "\">" + sLine + " <a href=\"#" + sLink +
              "\" class=\"headerlink\"> &para;</a></h" +
              QString::number(nHeadlineLevel + 1) + ">\n";
      sDoc += sLine;
      break;
    }
  }
  // qDebug() << "HEADLINES:" << slistHeadlines;

  pRawDoc->setPlainText(sDoc);
  return slistHeadlines;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Parser::replaceFootnotes(QTextDocument *pRawDoc) {
  QString sDoc(pRawDoc->toPlainText());
  QRegularExpression findMacro(
      QStringLiteral("\\(\\(.*\\)\\)"),
      QRegularExpression::InvertedGreedinessOption |  // Only smallest match
          QRegularExpression::DotMatchesEverythingOption |
          QRegularExpression::CaseInsensitiveOption);
  QString sNote;
  int nIndex = 0;
  QString sCount;
  quint16 nCount = 0;
  QString sFootnotes(QLatin1String(""));
  QRegularExpressionMatch match;

  while ((match = findMacro.match(sDoc, nIndex)).hasMatch()) {
    nIndex = match.capturedStart();
    nCount++;

    sNote = match.captured(0);
    sNote.remove(QStringLiteral("(("));
    sNote.remove(QStringLiteral("))"));
    sFootnotes += "<li><a id=\"fn-" + QString::number(nCount) +
                  "\" class="
                  "\"crosslink\" href=\"#bfn-" +
                  QString::number(nCount) + "\">" + QString::number(nCount) +
                  "</a>: " + sNote + "</li>\n";

    sCount = "<a id=\"bfn-" + QString::number(nCount) +
             "\" class=\""
             "footnote\" href=\"#fn-" +
             QString::number(nCount) +
             "\">"
             "&#091;" +
             QString::number(nCount) + "&#093;</a>";

    sDoc.replace(nIndex, match.capturedLength(), sCount);
    // Go on with new start position
    nIndex += sCount.length();
  }

  if (!sFootnotes.isEmpty()) {
    sFootnotes = "<ul class=\"footnotes\">\n" + sFootnotes + "</ul>\n";
  }

  // Replace pRawDoc with adapted document
  pRawDoc->setPlainText(sDoc + sFootnotes);
}
