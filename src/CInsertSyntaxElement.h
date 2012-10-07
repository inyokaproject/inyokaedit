/**
 * \file CInsertSyntaxElement.h
 *
 * \section LICENSE
 *
 * Copyright (C) 2011-2012 The InyokaEdit developers
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

#ifndef INYOKAEDIT_CINSERTSYNTAXELEMENT_H_
#define INYOKAEDIT_CINSERTSYNTAXELEMENT_H_

#include <QString>

/**
 * \class CInsertSyntaxElement
 * \brief Delivering html code of Inyoka elements
 */
class CInsertSyntaxElement
{
public:
    /** \brief Constructor */
    explicit CInsertSyntaxElement( const QString &sTransImage );

    /**
    * \brief Get Inyoka syntax element code
    * \param sElement Element name
    * \param sSelectedText Text which was selected
    * \return Syntax element code
    */
    QString getElementInyokaCode( const QString &sElement,
                                  const QString &sSelectedText );

private:
    QString m_sSampleBegin_1;
    QString m_sSampleEnd_1;
};

#endif // INYOKAEDIT_CINSERTSYNTAXELEMENT_H_
