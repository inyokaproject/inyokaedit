/**
 * \file provisionaltplparser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2020 The InyokaEdit developers
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
 * Provisional template parser
 */

#ifndef APPLICATION_PARSER_PROVISIONALTPLPARSER_H_
#define APPLICATION_PARSER_PROVISIONALTPLPARSER_H_

#include <QDir>
#include <QString>
#include <QStringList>

class ProvisionalTplParser {
  public:
    ProvisionalTplParser(const QStringList &sListHtmlStart,
                         const QString &sSharePath,
                         const QDir &tmpImgDir,
                         const QStringList &sListTestedWith,
                         const QStringList &sListTestedWithStrings,
                         const QStringList &sListTestedWithTouch,
                         const QStringList &sListTestedWithTouchStrings,
                         const QString &sCommunity);

    QString parseTpl(const QStringList &sListArgs, const QString &sCurrentFile);

  private:
    static QString parseAdvanced();
    static QString parseArchived(const QStringList &sListArgs);
    static QString parseBash(const QStringList &sListArgs);
    static QString parseBuilddeps(const QStringList &sListArgs);
    static QString parseCode(const QStringList &sListArgs);
    static QString parseCopy(const QStringList &sListArgs);
    static QString parseExperts(const QStringList &sListArgs);
    static QString parseFixme(const QStringList &sListArgs);
    static QString parseForeignAuth(const QStringList &sListArgs);
    static QString parseForeignPackage(const QStringList &sListArg);
    static QString parseForeignSource(const QStringList &sListArgs);
    static QString parseForeignWarning(const QStringList &sListArg);
    static QString parseIconOverview(const QStringList &sListArgs);
    static QString parseIkhayaAuthor(const QStringList &sListArgs);
    static QString parseIkhayaAward(const QStringList &sListArgs);
    static QString parseIkhayaImage(const QStringList &sListArgs);
    static QString parseIkhayaProjectPresentation();
    QString parseImageCollect(const QStringList &sListArgs);
    QString parseImageSub(const QStringList &sListArgs);
    static QString parseImprovable(const QStringList &sListArgs);
    static QString parseInfobox(const QStringList &sListArgs);
    static QString parseKeys(const QStringList &sListArgs);
    static QString parseKnowledge(const QStringList &sListArgs);
    static QString parseLeft(const QStringList &sListArgs);
    static QString parseNotice(const QStringList &sListArgs);
    static QString parseOBS(const QStringList &sListArgs);
    static QString parseOverview(const QStringList &sListArgs);
    static QString parseOverview2(const QStringList &sListArgs);
    static QString parsePackage(const QStringList &sListArgs);
    QString parsePipInstall(const QStringList &sListArgs);
    static QString parsePkgInstall(const QStringList &sListArgs);
    QString parsePkgInstallBut(const QStringList &sListArgs);
    static QString parsePPA(const QStringList &sListArgs);
    static QString parseProjects(const QStringList &sListArgs);
    static QString parseSidebar(const QStringList &sListArgs);
    static QString parseStatusIcon(const QStringList &sListArgs);
    QString parseTable(const QStringList &sListArgs);
    QString parseTested(const QStringList &sListArgs);
    QString parseTestedUT(const QStringList &sListArgs);
    static QString parseUnderConst(const QStringList &sListArgs);
    static QString parseWarning(const QStringList &sListArgs);
    static QString parseWorkInProgr(const QStringList &sListArgs);

    static QString insertBox(const QString &sClass,
                             const QString &sHeadline,
                             const QString &sContents,
                             const QString &sRemark = "");

    QStringList m_sListHtmlStart;
    QString m_sCurrentFile;
    const QString m_sSharePath;
    QDir m_tmpImgDir;
    QStringList m_sListTestedWith;
    QStringList m_sListTestedWithStrings;
    QStringList m_sListTestedWithTouch;
    QStringList m_sListTestedWithTouchStrings;
    const QString m_sCommunity;
};

#endif  // APPLICATION_PARSER_PROVISIONALTPLPARSER_H_
