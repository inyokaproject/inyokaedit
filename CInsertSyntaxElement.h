/**
 * \file CInsertSyntaxElement.h
 * \author See AUTHORS
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 by the respective authors (see AUTHORS)
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
 * Class definition for inserting Inyoka elements.
 */

#ifndef CINSERTSYNTAXELEMENT_H
#define CINSERTSYNTAXELEMENT_H

#include <string>

/**
 * \class CInsertSyntaxElement
 * \brief Delivering html code of Inyoka elements
 */
class CInsertSyntaxElement
{
public:
    /** \brief Constructor */
    CInsertSyntaxElement();

    /**
    * \brief Get Inyoka syntax element code
    * \param sElement Element name
    * \param sSelectedText Text which was selected
    * \return Syntax element code
    */
    std::string getElementInyokaCode( const std::string sElement, const std::string sSelectedText );

private:
    std::string m_sSampleBegin_1;
    std::string m_sSampleBegin_11;
    std::string m_sSampleBegin_12;
    std::string m_sSampleEnd_1;
    std::string m_sSampleBegin_2;
    std::string m_sSampleEnd_2;
};

#endif // CINSERTSYNTAXELEMENT_H
