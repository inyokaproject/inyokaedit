/**
 * \file parser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2022 The InyokaEdit developers
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
 * Class definition for parser.
 */

#ifndef APPLICATION_PARSER_PARSER_H_
#define APPLICATION_PARSER_PARSER_H_

#include <QDir>
#include <QString>
#include <QStringList>

class QTextDocument;

class Macros;
class ParseLinks;
class ParseTemplates;
class Templates;

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
           const QString &sPygmentize, QObject *pParent = nullptr);
    ~Parser();

    // Starts generating HTML-code
    QString genOutput(const QString &sActFile, QTextDocument *pRawDocument,
                      const bool bSyntaxCheck = false);

 public slots:
    void updateSettings(const QString &sInyokaUrl, const bool bCheckLinks,
                        const quint32 nTimedPreview);

 signals:
    void hightlightSyntaxError(const QPair<int, QString>);

 private:
    // void replaceTemplates(QTextDocument *pRawDoc);

    void filterEscapedChars(QTextDocument *pRawDoc);
    void filterNoTranslate(QTextDocument *pRawDoc);
    void replaceCodeblocks(QTextDocument *pRawDoc);
    void reinstertNoTranslate(QTextDocument *pRawDoc);

    static void removeComments(QTextDocument *pRawDoc);
    static void generateParagraphs(QTextDocument *pRawDoc);

#ifdef USEQTWEBENGINE
    void replaceFlags(QTextDocument *pRawDoc);
#endif
    static void replaceQuotes(QTextDocument *pRawDoc);
    static void replaceHorLines(QTextDocument *pRawDoc);
    static auto replaceHeadlines(QTextDocument *pRawDoc) -> QStringList;
    static void replaceFootnotes(QTextDocument *pRawDoc);
    auto generateTags(QTextDocument *pRawDoc) -> QString;
    auto highlightCode(const QString &sLanguage,
                       const QString &sCode) -> QString;

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
    const QString m_sPygmentize;
    quint32 m_nTimedPreview;
};

#endif  // APPLICATION_PARSER_PARSER_H_
