/**
 * \file CInsertSyntaxElement.cpp
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
 * Returns HTML code of Inyoka syntax elements.
 */

#include <QDebug>
#include "./CInsertSyntaxElement.h"

CInsertSyntaxElement::CInsertSyntaxElement( const QString &sTransImage )
{
    qDebug() << "Start" << Q_FUNC_INFO;
    m_sSampleBegin_1 = "[[" + sTransImage + "(";
    m_sSampleEnd_1    = ")]]\n" ;
    qDebug() << "End" << Q_FUNC_INFO;
}

QString CInsertSyntaxElement::getElementInyokaCode( const QString &sElement,
                                                    const QString &sSelectedText )
{
    // Toolbar buttons
    if ( sElement == "boldAct" )
    {
        if ( sSelectedText == "" )
        {
            return "'''Fett'''";
        }
        else
        {
            return "'''" + sSelectedText + "'''";
        }
    }
    else if ( sElement == "italicAct" )
    {
        if ( sSelectedText == "" )
        {
            return "''Kursiv''";
        }
        else
        {
            return "''" + sSelectedText + "''";
        }
    }
    else if ( sElement == "monotypeAct" )
    {
        if ( sSelectedText == "" )
        {
            return "`Monotype`";
        }
        else
        {
            return "`" + sSelectedText + "`";
        }
    }
    else if ( sElement == "wikilinkAct" )
    {
        if ( sSelectedText == "" )
        {
            return "[:Seitenname:]";
        }
        else
        {
            return "[:" + sSelectedText + ":]";
        }
    }
    else if ( sElement == "externalLinkAct" )
    {
        if ( sSelectedText == "" )
        {
            return "[http://www.example.org/]";
        }
        else
        {
            return "[" + sSelectedText + "]";
        }
    }
    else if ( sElement == "imageAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Bild.png, 200 (optional), left|right (optional)" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + sSelectedText + m_sSampleEnd_1;
        }
    }

    // Unkonwn element
    else
        return sSelectedText;
}
