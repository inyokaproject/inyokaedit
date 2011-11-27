/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CInsertSyntaxElement.h
* Purpose:    Class definition
***************************************************************************/

#ifndef CINSERTSYNTAXELEMENT_H
#define CINSERTSYNTAXELEMENT_H

#include <string>

class CInsertSyntaxElement
{
public:
    CInsertSyntaxElement();

    std::string GetElementInyokaCode(const std::string sElement, const std::string sSelectedText);

private:
    std::string sSampleBegin_1;
    std::string sSampleBegin_11;
    std::string sSampleBegin_12;
    std::string sSampleEnd_1;
    std::string sSampleBegin_2;
    std::string sSampleEnd_2;
};

#endif // CINSERTSYNTAXELEMENT_H
