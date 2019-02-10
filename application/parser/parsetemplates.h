/**
 * \file parsetemplates.h
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
 * Class definition for parsing templates.
 */

#ifndef APPLICATION_PARSER_PARSETEMPLATES_H_
#define APPLICATION_PARSER_PARSETEMPLATES_H_

#include <QString>
#include <QStringList>
#include <QTextDocument>

#include "./provisionaltplparser.h"

class ParseTemplates {
  public:
    ParseTemplates(const QStringList &sListTransTpl,
                   const QStringList &sListTplNames,
                   const QStringList &sListHtmlStart,
                   const QString &sSharePath,
                   const QDir &tmpImgDir,
                   const QStringList &sListTestedWith,
                   const QStringList &sListTestedWithStrings,
                   const QStringList &sListTestedWithTouch,
                   const QStringList &sListTestedWithTouchStrings,
                   const QString &sCommunity);

    void startParsing(QTextDocument *pRawDoc, const QString &sCurrentFile);

  private:
    ProvisionalTplParser *m_pProvTplTarser;
    QStringList m_sListTransTpl;
    QStringList m_sListTplNames;
    QString m_sCurrentFile;
};

#endif  // APPLICATION_PARSER_PARSETEMPLATES_H_
