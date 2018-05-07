/**
 * \file provisionaltplparser.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2018 The InyokaEdit developers
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
    QString parseAdvanced();
    QString parseArchived(const QStringList &sListArgs);
    QString parseBash(const QStringList &sListArgs);
    QString parseBuilddeps(const QStringList &sListArgs);
    QString parseCode(const QStringList &sListArgs);
    QString parseCopy(const QStringList &sListArgs);
    QString parseExperts(const QStringList &sListArgs);
    QString parseFixme(const QStringList &sListArgs);
    QString parseForeignAuth(const QStringList &sListArgs);
    QString parseForeignPackage(const QStringList &sListArg);
    QString parseForeignSource(const QStringList &sListArgs);
    QString parseForeignWarning(const QStringList &sListArg);
    QString parseIconOverview(const QStringList &sListArgs);
    QString parseIkhayaAuthor(const QStringList &sListArgs);
    QString parseIkhayaAward(const QStringList &sListArgs);
    QString parseIkhayaImage(const QStringList &sListArgs);
    QString parseIkhayaProjectPresentation();
    QString parseImageCollect(const QStringList &sListArgs);
    QString parseImageSub(const QStringList &sListArgs);
    QString parseImprovable(const QStringList &sListArgs);
    QString parseInfobox(const QStringList &sListArgs);
    QString parseKeys(const QStringList &sListArgs);
    QString parseKnowledge(const QStringList &sListArgs);
    QString parseLeft(const QStringList &sListArgs);
    QString parseNotice(const QStringList &sListArgs);
    QString parseOBS(const QStringList &sListArgs);
    QString parseOverview(const QStringList &sListArgs);
    QString parseOverview2(const QStringList &sListArgs);
    QString parsePackage(const QStringList &sListArgs);
    QString parsePkgInstall(const QStringList &sListArgs);
    QString parsePkgInstallBut(const QStringList &sListArgs);
    QString parsePPA(const QStringList &sListArgs);
    QString parseProjects(const QStringList &sListArgs);
    QString parseSidebar(const QStringList &sListArgs);
    QString parseStatusIcon(const QStringList &sListArgs);
    QString parseTable(const QStringList &sListArgs);
    QString parseTested(const QStringList &sListArgs);
    QString parseTestedUT(const QStringList &sListArgs);
    QString parseUnderConst(const QStringList &sListArgs);
    QString parseWarning(const QStringList &sListArgs);
    QString parseWorkInProgr(const QStringList &sListArgs);

    QString insertBox(const QString &sClass, const QString &sHeadline,
                      const QString &sContents, const QString &sRemark = "");

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
