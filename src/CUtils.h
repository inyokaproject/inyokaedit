/**
 * \file CUtils.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2013 The InyokaEdit developers
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
 * Class definition for utile functions.
 */

#ifndef INYOKAEDIT_CUTILS_H_
#define INYOKAEDIT_CUTILS_H_

class CUtils {
  public:
    CUtils();

    static bool getOnlineState();
    static void setProxy(const QString &sHostName, const quint16 nPort,
                         const QString &sUser, const QString &sPassword);
};

#endif  // INYOKAEDIT_CUTILS_H_
