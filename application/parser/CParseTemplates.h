/**
 * \file CParseTemplates.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2016 The InyokaEdit developers
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

#ifndef INYOKAEDIT_CPARSETEMPLATES_H_
#define INYOKAEDIT_CPARSETEMPLATES_H_

#include <QString>
#include <QStringList>
#include <QTextDocument>

#include "./CProvisionalTplParser.h"

class CParseTemplates {
  public:
    CParseTemplates(QString sTransTpl,
                    QStringList sListTplNames,
                    const QStringList &sListHtmlStart,
                    const QDir &tmpFileOutputDir,
                    const QDir &tmpImgDir);
    void startParsing(QTextDocument *pRawDoc,
                      const QString &sCurrentFile);

  private:
    CProvisionalTplParser *m_pProvTplTarser;
    QString m_sTransTpl;
    QStringList m_sListTplNames;
    QString m_sCurrentFile;
};

#endif  // INYOKAEDIT_CPARSETEMPLATES_H_
