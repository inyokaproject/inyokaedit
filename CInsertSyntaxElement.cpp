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

//--------------------------------------------------------------------------------------------------------

std::string CInsertSyntaxElement::GetInyokaHeading(const int index, const std::string sSelectedText){

    if (index != 0 && index != 1) {
        // -1 because of separator (considered as "item")
        switch (index-1) {
        default:
        case 1:
            // No text selected
            if (sSelectedText == "")
                return "= Überschrift =";
            // Enclose selected text with =
            else
                return "= " + sSelectedText + " =";
            break;
        case 2:
            if (sSelectedText == "")
                return "== Überschrift ==";
            else
                return "== " + sSelectedText + " ==";
            break;
        case 3:
            if (sSelectedText == "")
                return "=== Überschrift ===";
            else
                return "=== " + sSelectedText + " ===";
            break;
        case 4:
            if (sSelectedText == "")
                return "==== Überschrift ====";
            else
                return "==== " + sSelectedText + " ====";
            break;
        case 5:
            if (sSelectedText == "")
                return "===== Überschrift =====";
            else
                return "===== " + sSelectedText + " =====";
            break;
        }
    }

    // Return selcted text if something went wrong
    return sSelectedText;

}

//--------------------------------------------------------------------------------------------------------

std::string CInsertSyntaxElement::GetInterwikiLink(const std::string sElement, const std::string sSelectedText){

    if (sElement == "iWikiForum"){
        if (sSelectedText == "")
            return "[forum:Name:Text]";
        else
            return "[forum:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiTopic"){
        if (sSelectedText == "")
            return "[topic:Name:Text]";
        else
            return "[topic:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiPost"){
        if (sSelectedText == "")
            return "[post:Nummer:Text]";
        else
            return "[post:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiUser"){
        if (sSelectedText == "")
            return "[user:Name:]";
        else
            return "[user:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiIkhaya"){
        if (sSelectedText == "")
            return "[ikhaya:Name_der_Meldung:Text]";
        else
            return "[ikhaya:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiCalendar"){
        if (sSelectedText == "")
            return "[calendar:Jahr/Monat/Jahr/Tag/Name:Text]";
        else
            return "[calendar:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiTicket"){
        if (sSelectedText == "")
            return "[ticket:Nummer:Text]";
        else
            return "[ticket:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiBehind"){
        if (sSelectedText == "")
            return "[behind:Jahr/Monat/Tag/Name:Text]";
        else
            return "[behind:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiPaste"){
        if (sSelectedText == "")
            return "[paste:Nummer:Text]";
        else
            return "[paste:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiPlanet"){
        if (sSelectedText == "")
            return "[planet:Nummer:Text]";
        else
            return "[planet:" + sSelectedText + ":]";
    }

    // Canonical & co
    else if (sElement == "iWikiCanonical"){
        if (sSelectedText == "")
            return "[canonical:Seitenname:Text]";
        else
            return "[canonical:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiCanonicalblog"){
        if (sSelectedText == "")
            return "[canonicalblog:Seitenname:Text]";
        else
            return "[canonicalblog:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiPackages"){
        if (sSelectedText == "")
            return "[packages:Paketname:Text]";
        else
            return "[packages:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiApt"){
        if (sSelectedText == "")
            return "[apt:Paket:]";
        else
            return "[apt:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiUbuntu"){
        if (sSelectedText == "")
            return "[ubuntu:Wikiseite:Text]";
        else
            return "[ubuntu:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiUbuntu_doc"){
        if (sSelectedText == "")
            return "[ubuntu_doc:Seitenname:Text]";
        else
            return "[ubuntu_doc:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiAskubuntu"){
        if (sSelectedText == "")
            return "[askubuntu:Seitenname:Text]";
        else
            return "[askubuntu:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiUbuntu_fr"){
        if (sSelectedText == "")
            return "[ubuntu_fr:Wikiseite:Text]";
        else
            return "[ubuntu_fr:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiUbuntuone"){
        if (sSelectedText == "")
            return "[ubuntuone:Name:Text]";
        else
            return "[ubuntuone:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiKubuntu"){
        if (sSelectedText == "")
            return "[kubuntu:Seite:Text]";
        else
            return "[kubuntu:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiKubuntuDoc"){
        if (sSelectedText == "")
            return "[kubuntu_doc:Wikiseite:Text]";
        else
            return "[kubuntu_doc:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiKubuntuDe"){
        if (sSelectedText == "")
            return "[kubuntu-de:Wikiseite:Text]";
        else
            return "[kubuntu-de:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiLubuntu"){
        if (sSelectedText == "")
            return "[lubuntu:Seite:Text]";
        else
            return "[lubuntu:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiXubuntu"){
        if (sSelectedText == "")
            return "[xubuntu:Seite:Text]";
        else
            return "[xubuntu:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiUbuntustudio"){
        if (sSelectedText == "")
            return "[ubuntustudio:Seite:Text]";
        else
            return "[ubuntustudio:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiEdubuntu"){
        if (sSelectedText == "")
            return "[edubuntu:Seite:Text]";
        else
            return "[edubuntu:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiDebian"){
        if (sSelectedText == "")
            return "[debian:Wikiseite:Text]";
        else
            return "[debian:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiDebian_de"){
        if (sSelectedText == "")
            return "[debian_de:Wikiseite:Text]";
        else
            return "[debian_de:" + sSelectedText + ":]";
    }

    // Wikimedia
    else if (sElement == "iWikiWikipedia"){
        if (sSelectedText == "")
            return "[wikipedia:Wikiseite:Text]";
        else
            return "[wikipedia:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiWikipedia_en"){
        if (sSelectedText == "")
            return "[wikipedia_en:Wikiseite:Text]";
        else
            return "[wikipedia_en:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiWikibooks"){
        if (sSelectedText == "")
            return "[wikibooks:Wikibooksseite:Text]";
        else
            return "[wikibooks:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiWikimedia"){
        if (sSelectedText == "")
            return "[wikimedia:Seite:Text]";
        else
            return "[wikimedia:" + sSelectedText + ":]";
    }

    // Source & project hoster, bugtracker
    else if (sElement == "iWikiFreshmeat"){
        if (sSelectedText == "")
            return "[freshmeat:Projektname:Text]";
        else
            return "[freshmeat:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiGetdeb"){
        if (sSelectedText == "")
            return "[getdeb:Programmname:Text]";
        else
            return "[getdeb:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiGooglecode"){
        if (sSelectedText == "")
            return "[googlecode:Projektname:Text]";
        else
            return "[googlecode:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiLaunchpad"){
        if (sSelectedText == "")
            return "[launchpad:Seite:Text]";
        else
            return "[launchpad:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiBug"){
        if (sSelectedText == "")
            return "[bug:Nummer:Text]";
        else
            return "[bug:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiLpuser"){
        if (sSelectedText == "")
            return "[lpuser:Name:Text]";
        else
            return "[lpuser:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiPlaydeb"){
        if (sSelectedText == "")
            return "[playdeb:Spielname:Text]";
        else
            return "[playdeb:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiSourceforge"){
        if (sSelectedText == "")
            return "[sourceforge:Projektname:Text]";
        else
            return "[sourceforge:" + sSelectedText + ":]";
    }

    // Misc
    else if (sElement == "iWikiDropbox"){
        if (sSelectedText == "")
            return "[dropbox:Pfad:]";
        else
            return "[dropbox:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiEan"){
        if (sSelectedText == "")
            return "[ean:Nummer:]";
        else
            return "[ean:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiFb"){
        if (sSelectedText == "")
            return "[fb:Seite:Text]";
        else
            return "[fb:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiGoogle"){
        if (sSelectedText == "")
            return "[google:Suchbegriff:Text]";
        else
            return "[google:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiGplus"){
        if (sSelectedText == "")
            return "[gplus:Seite:Text]";
        else
            return "[gplus:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiHolarse"){
        if (sSelectedText == "")
            return "[holarse:Seite:Text]";
        else
            return "[holarse:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiIdentica"){
        if (sSelectedText == "")
            return "[identica:Seite:Text]";
        else
            return "[identica:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiInternetWayback"){
        if (sSelectedText == "")
            return "[iawm:Website:Text]";
        else
            return "[iawm:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiIsbn"){
        if (sSelectedText == "")
            return "[isbn:Nummer:Text]";
        else
            return "[isbn:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiLastfm"){
        if (sSelectedText == "")
            return "[lastfm:Gruppe:Text]";
        else
            return "[lastfm:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiLiflg"){
        if (sSelectedText == "")
            return "[liflg:Seite:Text]";
        else
            return "[liflg:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiLinuxgaming"){
        if (sSelectedText == "")
            return "[linuxgaming:Seite:Text]";
        else
            return "[linuxgaming:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiOsm"){
        if (sSelectedText == "")
            return "[osm:Wikiseite:Text]";
        else
            return "[osm:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiTwitter"){
        if (sSelectedText == "")
            return "[twitter:Seite:Text]";
        else
            return "[twitter:" + sSelectedText + ":]";
    }
    else if (sElement == "iWikiYoutube"){
        if (sSelectedText == "")
            return "[youtube:Videonummer:Text]";
        else
            return "[youtube:" + sSelectedText + ":]";
    }

    else{
        return sSelectedText;
    }
}
