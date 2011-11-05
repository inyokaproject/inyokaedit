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
* File Name:  CInsertSyntaxElement.cpp
* Purpose:    Returns HTML code of Inyoka syntax elements
***************************************************************************/

#include "CInsertSyntaxElement.h"

CInsertSyntaxElement::CInsertSyntaxElement()
{
}

std::string CInsertSyntaxElement::GetElementInyokaCode(const std::string sElement, const std::string sSelectedText){

    // Toolbar buttons
    if (sElement == "Fett"){
        if (sSelectedText == "")
            return "'''Fett'''";
        else
            return "'''" + sSelectedText + "'''";
    }
    else if (sElement == "Kursiv"){
        if (sSelectedText == "")
            return "''Kursiv''";
        else
            return "''" + sSelectedText + "''";
    }
    else if (sElement == "Monotype"){
        if (sSelectedText == "")
            return "`Monotype`";
        else
            return "`" + sSelectedText + "`";
    }
    else if (sElement == "Wikilink"){
        if (sSelectedText == "")
            return "[:Seitenname:]";
        else
            return "[:" + sSelectedText + ":]";
    }
    else if (sElement == "ExternerLink"){
        if (sSelectedText == "")
            return "[http://www.example.org/]";
        else
            return "[" + sSelectedText + "]";
    }
    else if (sElement == "Codeblock"){
        if (sSelectedText == "")
            return "{{{\nCode\n}}}";
        else
            return "{{{\n" + sSelectedText + "\n}}}";
    }

    // Menu / dropdown menu items
    else if (sElement == "Baustelle"){
        if (sSelectedText == "")
            return "[[Vorlage(Baustelle, Datum, \"Bearbeiter\")]]\n";
        else
            return "[[Vorlage(Baustelle, " + sSelectedText + ")]]";
    }
    else if (sElement == "Getestet"){
        if (sSelectedText == "")
            return "[[Vorlage(Getestet, )]]\n";
        else
            return "[[Vorlage(Getestet, " + sSelectedText + ")]]";
    }
    else if (sElement == "Wissensblock"){
        if (sSelectedText == "")
            return "{{{#!vorlage Wissen\n"
                    "[:Anwendungen_hinzufügen:Anwendungen hinzufügen]\n"
                    "[:Pakete installieren: Installation von Programmen]\n"
                    "[:Paketquellen freischalten: Bearbeiten von Paketquellen]\n"
                    "[:Paketinstallation_DEB: Ein einzelnes Paket installieren]\n"
                    "[:Programme kompilieren: Pakete aus dem Quellcode erstellen]\n"
                    "[:Terminal: Ein Terminal öffnen]\n"
                    "[:Editor: Einen Editor öffnen]\n"
                    "[:Packprogramme: Archive entpacken]\n"
                    "[:Programme starten: Ein Programm ohne Menüeintrag starten]\n"
                    "[:Autostart: Automatischer Start von Programmen]\n"
                    "[:Menüeditor: Programme zum Menü hinzufügen]\n"
                    "[:sudo: Root-Rechte]\n"
                    "[:Rechte: Rechte für Dateien und Ordner ändern]\n"
                    "}}}\n";
        else
            return "{{{#!vorlage Wissen\n, " + sSelectedText + "\n}}}";
    }
    else if (sElement == "Inhaltsverzeichnis"){
        if (sSelectedText == "")
            return "[[Inhaltsverzeichnis(2)]]\n";
        else
            return "[[Inhaltsverzeichnis(" + sSelectedText + ")]]";
    }
    else if (sElement == "Expertenmarkierung"){
        if (sSelectedText == "")
            return "[[Vorlage(Fortgeschritten)]]\n";
        else
            return sSelectedText + "[[Vorlage(Fortgeschritten)]]";
    }
    else if (sElement == "Preis"){
        if (sSelectedText == "")
            return "[[Vorlage(Award, \"Preis\", Link, Preiskategorie, \"Preisträger\")]]\n";
        else
            return "[[Vorlage(Award, " + sSelectedText + ")]]";
    }
    else if (sElement == "Shellbefehl"){
        if (sSelectedText == "")
            return "{{{#!vorlage Befehl\nShell-Befehl\n}}}\n";
        else
            return "{{{#!vorlage Befehl\n" + sSelectedText + "\n}}}";
    }
    else if (sElement == "Hinweisbox"){
        if (sSelectedText == "")
            return "{{{#!vorlage Hinweis\nHinweistext\n}}}\n";
        else
            return "{{{#!vorlage Hinweis\n" + sSelectedText + "\n}}}";
    }
    else if (sElement == "Warnbox"){
        if (sSelectedText == "")
            return "{{{#!vorlage Warnung\nWarnungstext\n}}}\n";
        else
            return "{{{#!vorlage Warnung\n" + sSelectedText + "\n}}}";
    }
    else if (sElement == "Experteninfo"){
        if (sSelectedText == "")
            return "{{{#!vorlage Experten\nHintergrundinformation\n}}}\n";
        else
            return "{{{#!vorlage Experten\n" + sSelectedText + "\n}}}";
    }
    else if (sElement == "EinfachesBild"){
        if (sSelectedText == "")
            return "[[Bild(Bild.png, 200 (optional), left|right (optional))]]";
        else
            return "[[Bild(" + sSelectedText + ")]]";
    }
    else if (sElement == "Bildunterschrift"){
        if (sSelectedText == "")
            return "[[Vorlage(Bildunterschrift, Bild.png, Bildbreite (optional), \"Beschreibung\", left|right (optional), *-style(optional) )]]\n";
        else
            return "[[Vorlage(Bildunterschrift, " + sSelectedText + ")]]";
    }
    else if (sElement == "BildersammlungUmbruch"){
        if (sSelectedText == "")
            return "[[Vorlage(Bildersammlung, Bildhöhe (optional)\n"
                    "Bild1.jpg, \"Beschreibung 1\"\n"
                    "Bild2.png, \"Beschreibung 2\"\n"
                    "Wiki/Icons/Bild3.png, \"Beschreibung 3\"\n"
                    ")]]\n";
        else
            return "[[Vorlage(Bildersammlung, Bildhöhe (optional)\n" + sSelectedText + "\n)]]";
    }
    else if (sElement == "BildersammlungFliesstext"){
        if (sSelectedText == "")
            return "[[Vorlage(Bildersammlung, left|right, Bildhöhe (optional)\n"
                    "Bild1.jpg, \"Beschreibung 1\"\n"
                    "Bild2.png, \"Beschreibung 2\"\n"
                    "Wiki/Icons/Bild3.png, \"Beschreibung 3\"\n"
                    ")]]\n";
        else
            return "[[Vorlage(Bildersammlung, left|right, Bildhöhe (optional)\n" + sSelectedText + "\n)]]";
    }
    else if (sElement == "Paketemakro"){
        if (sSelectedText == "")
            return "[[Vorlage(Pakete, paket1, paket2)]]\n";
        else
            return "[[Vorlage(Pakete, " + sSelectedText + ")]]\n";
    }
    else if (sElement == "Paketinstallation"){
        if (sSelectedText == "")
            return "{{{#!vorlage Paketinstallation\n"
                    "paketname1, Information (optional)\n"
                    "paketname2, Information (optional)\n"
                    "}}}\n";
        else
            return "{{{#!vorlage Paketinstallation\n" + sSelectedText + "\n}}}\n";
    }
    else if (sElement == "Ppavorlage"){
        if (sSelectedText == "")
            return "[[Vorlage(PPA, PPA-Besitzer, PPA-Name)]]\n";
        else
            return "[[Vorlage(PPA, " + sSelectedText + ")]]\n";
    }
    else if (sElement == "Fremdquellenvorlage"){
        if (sSelectedText == "")
            return "[[Vorlage(Fremdquelle, URL zur Fremdquelle, Ubuntuversionen (optional), Komponent/en (optional) )]]\n";
        else
            return "[[Vorlage(Fremdquelle, " + sSelectedText + ")]]\n";
    }
    else if (sElement == "Fremdquellenauth"){
        if (sSelectedText == "")
            return "[[Vorlage(Fremdquelle-auth, key PGP-Schlüsselnummer ODER Link zum PGP-Schlüssel)]]\n";
        else
            return "[[Vorlage(Fremdquelle-auth, " + sSelectedText + ")]]\n";
    }
    else if (sElement == "Fremdpaket"){
        if (sSelectedText == "")
            return "[[Vorlage(Fremdpaket, (Projekthoster, Projektname) ODER (\"Anbieter\", URL Downloadübersicht), Ubuntuversion/en (optional) )]]\n";
        else
            return "[[Vorlage(Fremdpaket, " + sSelectedText + ")]]\n";
    }
    else if (sElement == "Ausbaufaehig"){
        if (sSelectedText == "")
            return "[[Vorlage(Ausbaufähig, \"Begründung\")]]\n";
        else
            return "[[Vorlage(Ausbaufähig, \"" + sSelectedText + "\")]]\n";
    }
    else if (sElement == "Fehlerhaft"){
        if (sSelectedText == "")
            return "[[Vorlage(Fehlerhaft, \"Begründung\")]]\n";
        else
            return "[[Vorlage(Fehlerhaft, \"" + sSelectedText + "\")]]\n";
    }
    else if (sElement == "Verlassen"){
        if (sSelectedText == "")
            return "[[Vorlage(Verlassen, \"Begründung\")]]\n";
        else
            return "[[Vorlage(Verlassen, \"" + sSelectedText + "\")]]\n";
    }
    else if(sElement == "Fremdpaketewarnung"){
        if (sSelectedText == "")
            return "[[Vorlage(Fremd, Paket, \"Kommentar\")]]";
        else
            return "[[Vorlage(Fremd, Paket, \"" + sSelectedText + "\")]]";
    }
    else if(sElement == "Fremdquellewarnung"){
        if (sSelectedText == "")
            return "[[Vorlage(Fremd, Quelle, \"Kommentar\")]]";
        else
            return "[[Vorlage(Fremd, Quelle, \"" + sSelectedText + "\")]]";
    }
    else if(sElement == "Fremdsoftwarewarnung"){
        if (sSelectedText == "")
            return "[[Vorlage(Fremd, Software, \"Kommentar\")]]";
        else
            return "[[Vorlage(Fremd, Software, \"" + sSelectedText + "\")]]";
    }
    else if(sElement == "Tasten"){
        if (sSelectedText == "")
            return "[[Vorlage(Tasten, )]]";
        else
            return "[[Vorlage(Tasten, " + sSelectedText + ")]]";
    }
    else if(sElement == "Tabelle"){
        if (sSelectedText == "")
            return "{{{#!vorlage Tabelle\n[:Wiki/Syntax/Tabellen:]\n}}}";
        else
            return "{{{#!vorlage Tabelle\n" + sSelectedText + "\n}}}";
    }
    // Unkonwn element
    else
        return sSelectedText;

}
