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
    sSampleBegin_1  = "[[Vorlage(";
    sSampleBegin_11 = "[[Bild(";
    sSampleBegin_12 = "[[Inhaltsverzeichnis(";
    sSampleEnd_1    = ")]]\n";
    sSampleBegin_2  = "{{{#!vorlage ";
    sSampleEnd_2    = "}}}\n";
}

std::string CInsertSyntaxElement::GetElementInyokaCode(const std::string sElement, const std::string sSelectedText){

    // Toolbar buttons
    if (sElement == "boldAct"){
        if (sSelectedText == "")
            return "'''Fett'''";
        else
            return "'''" + sSelectedText + "'''";
    }
    else if (sElement == "italicAct"){
        if (sSelectedText == "")
            return "''Kursiv''";
        else
            return "''" + sSelectedText + "''";
    }
    else if (sElement == "monotypeAct"){
        if (sSelectedText == "")
            return "`Monotype`";
        else
            return "`" + sSelectedText + "`";
    }
    else if (sElement == "wikilinkAct"){
        if (sSelectedText == "")
            return "[:Seitenname:]";
        else
            return "[:" + sSelectedText + ":]";
    }
    else if (sElement == "externalLinkAct"){
        if (sSelectedText == "")
            return "[http://www.example.org/]";
        else
            return "[" + sSelectedText + "]";
    }
    else if (sElement == "codeblockAct"){
        if (sSelectedText == "")
            return "{{{\nCode\n}}}";
        else
            return "{{{\n" + sSelectedText + "\n}}}";
    }

    // Menu / dropdown menu items
    else if (sElement == "insertUnderConstructionAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Baustelle, Datum, \"Bearbeiter\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Baustelle, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertTestedForAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Getestet, " + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Getestet, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertKnowledgeAct"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Wissen\n"
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
                    "[:Rechte: Rechte für Dateien und Ordner ändern]\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Wissen\n, " + sSelectedText + "\n" + sSampleEnd_2;
    }
    else if (sElement == "insertTableOfContentsAct"){
        if (sSelectedText == "")
            return sSampleBegin_12 + "2" + sSampleEnd_1;
        else
            return sSampleBegin_12 + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertAdvancedAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fortgeschritten" + sSampleEnd_1;
        else
            return sSelectedText + sSampleBegin_1 + "Fortgeschritten" + sSampleEnd_1;
    }
    else if (sElement == "insertAwardAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Award, \"Preis\", Link, Preiskategorie, \"Preisträger\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Award, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertBashCommandAct"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Befehl\nShell-Befehl\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Befehl\n" + sSelectedText + "\n" + sSampleEnd_2;
    }
    else if (sElement == "insertNoticeAct"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Hinweis\nHinweistext\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Hinweis\n" + sSelectedText + "\n" + sSampleEnd_2;
    }
    else if (sElement == "insertWarningAct"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Warnung\nWarnungstext\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Warnung\n" + sSelectedText + "\n" + sSampleEnd_2;
    }
    else if (sElement == "insertExpertsAct"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Experten\nHintergrundinformation\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Experten\n" + sSelectedText + "\n" + sSampleEnd_2;
    }
    else if (sElement == "imageAct"){
        if (sSelectedText == "")
            return sSampleBegin_11 + "Bild.png, 200 (optional), left|right (optional)" + sSampleEnd_1;
        else
            return sSampleBegin_11 + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertImageUnderlineAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Bildunterschrift, Bild.png, Bildbreite (optional), \"Beschreibung\", left|right (optional), *-style (optional) " + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Bildunterschrift, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertImageCollectionAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Bildersammlung, Bildhöhe (optional)\n"
                    "Bild1.jpg, \"Beschreibung 1\"\n"
                    "Bild2.png, \"Beschreibung 2\"\n"
                    "Wiki/Icons/Bild3.png, \"Beschreibung 3\"\n" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Bildersammlung, Bildhöhe (optional)\n" + sSelectedText + "\n" + sSampleEnd_1;
    }
    else if (sElement == "insertImageCollectionInTextAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Bildersammlung, left|right, Bildhöhe (optional)\n"
                    "Bild1.jpg, \"Beschreibung 1\"\n"
                    "Bild2.png, \"Beschreibung 2\"\n"
                    "Wiki/Icons/Bild3.png, \"Beschreibung 3\"\n" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Bildersammlung, left|right, Bildhöhe (optional)\n" + sSelectedText + "\n" + sSampleEnd_1;
    }
    else if (sElement == "insertPackageListAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Pakete, paket1, paket2" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Pakete, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertPackageInstallAct"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Paketinstallation\n"
                    "paketname1, Information (optional)\n"
                    "paketname2, Information (optional)\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Paketinstallation\n" + sSelectedText + "\n" + sSampleEnd_2;
    }
    else if (sElement == "insertPPAAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "PPA, PPA-Besitzer, PPA-Name" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "PPA, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertThirdPartyRepoAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fremdquelle, URL zur Fremdquelle, Ubuntuversionen (optional), Komponent/en (optional) " + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fremdquelle, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertThirdPartyRepoAuthAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fremdquelle-auth, key PGP-Schlüsselnummer ODER Link zum PGP-Schlüssel" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fremdquelle-auth, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertThirdPartyPackageAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fremdpaket, (Projekthoster, Projektname) ODER (\"Anbieter\", URL Downloadübersicht), Ubuntuversion/en (optional) " + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fremdpaket, " + sSelectedText + sSampleEnd_1;
    }
    else if (sElement == "insertImprovableAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Ausbaufähig, \"Begründung\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Ausbaufähig, \"" + sSelectedText + "\"" + sSampleEnd_1;
    }
    else if (sElement == "insertFixMeAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fehlerhaft, \"Begründung\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fehlerhaft, \"" + sSelectedText + "\"" + sSampleEnd_1;
    }
    else if (sElement == "insertLeftAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Verlassen, \"Begründung\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Verlassen, \"" + sSelectedText + "\"" + sSampleEnd_1;
    }
    else if(sElement == "insertThirdPartyPackageWarningAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fremd, Paket, \"Kommentar\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fremd, Paket, \"" + sSelectedText + "\"" + sSampleEnd_1;
    }
    else if(sElement == "insertThirdPartyRepoWarningAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fremd, Quelle, \"Kommentar\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fremd, Quelle, \"" + sSelectedText + "\"" + sSampleEnd_1;
    }
    else if(sElement == "insertThirdPartySoftwareWarningAct"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Fremd, Software, \"Kommentar\"" + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Fremd, Software, \"" + sSelectedText + "\"" + sSampleEnd_1;
    }
    else if(sElement == "Keys"){
        if (sSelectedText == "")
            return sSampleBegin_1 + "Tasten, " + sSampleEnd_1;
        else
            return sSampleBegin_1 + "Tasten, " + sSelectedText + sSampleEnd_1;
    }
    else if(sElement == "Table"){
        if (sSelectedText == "")
            return sSampleBegin_2 + "Tabelle\n[:Wiki/Syntax/Tabellen:]\n" + sSampleEnd_2;
        else
            return sSampleBegin_2 + "Tabelle\n" + sSelectedText + "\n" + sSampleEnd_2;
    }
    // Unkonwn element
    else
        return sSelectedText;

}
