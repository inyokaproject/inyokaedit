/**
 * \file parsetable.h
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
 * Class definition for table parser.
 */

#ifndef APPLICATION_PARSER_PARSETABLE_H_
#define APPLICATION_PARSER_PARSETABLE_H_

#include <QString>

class QTextDocument;

class ParseTable {
 public:
    ParseTable();
    static void startParsing(QTextDocument *pRawDoc);

 private:
    static auto createTable(const QStringList &sListLines) -> QString;
};

#endif  // APPLICATION_PARSER_PARSETABLE_H_
