/**
 * \file parser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2019 The InyokaEdit developers
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

#ifndef APPLICATION_PARSER_PARSER_H_
#define APPLICATION_PARSER_PARSER_H_

#include <QWidget>
#include <QtGui>
#include <QDir>
#include <QMessageBox>

#include "./macros.h"
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
           Templates *pTemplates, const QString &sCommunity,
           QObject *pParent = nullptr);
    ~Parser();

    // Starts generating HTML-code
    QString genOutput(const QString &sActFile, QTextDocument *pRawDocument,
                      const bool bSyntaxCheck = false);

  public slots:
    void updateSettings(const QString &sInyokaUrl, const bool bCheckLinks);

  signals:
    void hightlightSyntaxError(const qint32);

  private:
    // void replaceTemplates(QTextDocument *pRawDoc);

    void filterEscapedChars(QTextDocument *pRawDoc);
    void filterNoTranslate(QTextDocument *pRawDoc);
    void replaceCodeblocks(QTextDocument *pRawDoc);
    void reinstertNoTranslate(QTextDocument *pRawDoc);

    void removeComments(QTextDocument *pRawDoc);
    void generateParagraphs(QTextDocument *pRawDoc);

    // void replaceFlags(QTextDocument *pRawDoc);
    void replaceQuotes(QTextDocument *pRawDoc);
    void replaceHorLines(QTextDocument *pRawDoc);
    QStringList replaceHeadlines(QTextDocument *pRawDoc);
    void replaceFootnotes(QTextDocument *pRawDoc);
    QString generateTags(QTextDocument *pRawDoc);
    QString highlightCode(const QString &sLanguage, const QString &sCode);

    // Text from editor
    QTextDocument *m_pRawText;

    QStringList m_sListNoTranslate;

    ParseTemplates *m_pTemplateParser;
    ParseLinks *m_pLinkParser;

    const QString m_sSharePath;
    const QDir m_tmpImgDir;
    QString m_sInyokaUrl;
    QString m_sCurrentFile;
    Templates *m_pTemplates;
    Macros *m_pMacros;
    const QString m_sCommunity;
};

#endif  // APPLICATION_PARSER_PARSER_H_
