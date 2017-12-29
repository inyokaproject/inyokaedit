/**
 * \file parser.h
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
 * Class definition for parser.
 */

#ifndef INYOKAEDIT_PARSER_H_
#define INYOKAEDIT_PARSER_H_

#include <QWidget>
#include <QtGui>
#include <QDir>
#include <QMessageBox>

#include "./parsetemplates.h"
#include "./parselinks.h"
#include "../templates/templates.h"

// Qt classes
class QString;
class QTextDocument;
class QDir;

/**
 * \class Parser
 * \brief Main parser module.
 */
class Parser : public QObject {
  Q_OBJECT

 public:
  Parser(const QString &sSharePath, const QDir &tmpImgDir,
         const QString &sInyokaUrl, const bool bCheckLinks,
         Templates *pTemplates, const QString &sCommunity);
  ~Parser();

  // PUBLIC FOR DEBUGGING
  void replaceTemplates(QTextDocument *p_rawDoc);

  // Starts generating HTML-code
  QString genOutput(const QString &sActFile, QTextDocument *p_rawDocument,
                    const bool &bSyntaxCheck = false);

 public slots:
  void updateSettings(const QString &sInyokaUrl, const bool bCheckLinks,
                      const QString &sCommunity);

 signals:
  void hightlightSyntaxError(const qint32);

 private:
  // void replaceTemplates(QTextDocument *p_rawDoc);

  void filterEscapedChars(QTextDocument *p_rawDoc);
  void filterNoTranslate(QTextDocument *p_rawDoc);
  void replaceCodeblocks(QTextDocument *p_rawDoc);
  void reinstertNoTranslate(QTextDocument *p_rawDoc);

  void removeComments(QTextDocument *p_rawDoc);
  void generateParagraphs(QTextDocument *p_rawDoc);

  void replaceLists(QTextDocument *p_rawDoc);
  void replaceFlags(QTextDocument *p_rawDoc);
  void replaceImages(QTextDocument *p_rawDoc);
  void replaceQuotes(QTextDocument *p_rawDoc);
  void replaceBreaks(QTextDocument *p_rawDoc);
  void replaceHorLines(QTextDocument *p_rawDoc);
  void replaceHeadlines(QTextDocument *p_rawDoc);
  void replaceTableOfContents(QTextDocument *p_rawDoc);
  void replaceDates(QTextDocument *p_rawDoc);
  void replaceFootnotes(QTextDocument *p_rawDoc);
  QString generateTags(QTextDocument *p_rawDoc);
  QString highlightCode(const QString &sLanguage, const QString &sCode);

  // Text from editor
  QTextDocument *m_pRawText;

  QStringList m_sListNoTranslate;
  QStringList m_sListHeadlines;

  ParseTemplates *m_pTemplateParser;
  ParseLinks *m_pLinkParser;

  const QString m_sSharePath;
  const QDir m_tmpImgDir;
  QString m_sInyokaUrl;
  QString m_sCurrentFile;
  Templates *m_pTemplates;
  QString m_sCommunity;
};

#endif  // INYOKAEDIT_PARSER_H_
