/**
 * \file parsetxtmap.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2021 The InyokaEdit developers
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
 * Class definition for text map parser.
 */

#ifndef APPLICATION_PARSER_PARSETXTMAP_H_
#define APPLICATION_PARSER_PARSETXTMAP_H_

#include <QPair>
#include <QStringList>

class QTextDocument;

class ParseTxtMap {
 public:
    ParseTxtMap();
    static void startParsing(QTextDocument *pRawDoc,
                             const QPair<QStringList, QStringList> &map);
};

#endif  // APPLICATION_PARSER_PARSETXTMAP_H_
