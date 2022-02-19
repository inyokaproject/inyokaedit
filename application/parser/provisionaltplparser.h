/**
 * \file provisionaltplparser.h
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
 * Provisional template parser
 */

#ifndef APPLICATION_PARSER_PROVISIONALTPLPARSER_H_
#define APPLICATION_PARSER_PROVISIONALTPLPARSER_H_

#include <QDir>
#include <QHash>
#include <QString>
#include <QStringList>

class ProvisionalTplParser {
 public:
    ProvisionalTplParser(const QStringList &sListHtmlStart,
                         const QString &sSharePath,
                         const QDir &tmpImgDir,
                         const QHash<QString, QString> &TestedWithMap,
                         const QHash<QString, QString> &TestedWithTouchMap,
                         const QString &sCommunity);

    auto parseTpl(const QStringList &sListArgs,
                  const QString &sCurrentFile) -> QString;

 private:
    static auto parseAdvanced() -> QString;
    static auto parseArchived(const QStringList &sListArgs) -> QString;
    static auto parseBash(const QStringList &sListArgs) -> QString;
    static auto parseBuilddeps(const QStringList &sListArgs) -> QString;
    static auto parseCode(const QStringList &sListArgs) -> QString;
    static auto parseCopy(const QStringList &sListArgs) -> QString;
    static auto parseExperts(const QStringList &sListArgs) -> QString;
    static auto parseFixme(const QStringList &sListArgs) -> QString;
    static auto parseForeignAuth(const QStringList &sListArgs) -> QString;
    static auto parseForeignPackage(const QStringList &sListArg) -> QString;
    static auto parseForeignSource(const QStringList &sListArgs) -> QString;
    static auto parseForeignWarning(const QStringList &sListArg) -> QString;
    static auto parseIconOverview(const QStringList &sListArgs) -> QString;
    static auto parseIkhayaAuthor(const QStringList &sListArgs) -> QString;
    static auto parseIkhayaAward(const QStringList &sListArgs) -> QString;
    static auto parseIkhayaImage(const QStringList &sListArgs) -> QString;
    static auto parseIkhayaProjectPresentation() -> QString;
    auto parseImageCollect(const QStringList &sListArgs) -> QString;
    auto parseImageSub(const QStringList &sListArgs) -> QString;
    static auto parseImprovable(const QStringList &sListArgs) -> QString;
    static auto parseInfobox(const QStringList &sListArgs) -> QString;
    static auto parseKeys(const QStringList &sListArgs) -> QString;
    static auto parseKnowledge(const QStringList &sListArgs) -> QString;
    static auto parseLeft(const QStringList &sListArgs) -> QString;
    static auto parseNotice(const QStringList &sListArgs) -> QString;
    static auto parseOBS(const QStringList &sListArgs) -> QString;
    static auto parseOverview(const QStringList &sListArgs) -> QString;
    static auto parseOverview2(const QStringList &sListArgs) -> QString;
    static auto parsePackage(const QStringList &sListArgs) -> QString;
    auto parsePipInstall(const QStringList &sListArgs) -> QString;
    static auto parsePkgInstall(const QStringList &sListArgs) -> QString;
    auto parsePkgInstallBut(const QStringList &sListArgs) -> QString;
    static auto parsePPA(const QStringList &sListArgs) -> QString;
    static auto parseProjects(const QStringList &sListArgs) -> QString;
    static auto parseSidebar(const QStringList &sListArgs) -> QString;
    static auto parseStatusIcon(const QStringList &sListArgs) -> QString;
    auto parseTable(const QStringList &sListArgs) -> QString;
    auto parseTested(const QStringList &sListArgs) -> QString;
    auto parseTestedUT(const QStringList &sListArgs) -> QString;
    static auto parseUnderConst(const QStringList &sListArgs) -> QString;
    static auto parseWarning(const QStringList &sListArgs) -> QString;
    static auto parseWorkInProgr(const QStringList &sListArgs) -> QString;

    static auto insertBox(
        const QString &sClass,
        const QString &sHeadline,
        const QString &sContents,
        const QString &sRemark = QLatin1String("")) -> QString;

    QStringList m_sListHtmlStart;
    QString m_sCurrentFile;
    const QString m_sSharePath;
    QDir m_tmpImgDir;
    QHash<QString, QString> m_TestedWithMap;
    QHash<QString, QString> m_TestedWithTouchMap;
    const QString m_sCommunity;
};

#endif  // APPLICATION_PARSER_PROVISIONALTPLPARSER_H_
