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
#include "CInsertSyntaxElement.h"

CInsertSyntaxElement::CInsertSyntaxElement( const QString &sTransTemplate,
                                            const QString &sTransImage,
                                            const QString &sTransTOC )
{
    qDebug() << "Start" << Q_FUNC_INFO;
    m_sSampleBegin_1  = "[[" + sTransTemplate.toStdString() + "(";
    m_sSampleBegin_11 = "[[" + sTransImage.toStdString() + "(";
    m_sSampleBegin_12 = "[[" + sTransTOC.toStdString() + "(";
    m_sSampleEnd_1    = ")]]\n";
    m_sSampleBegin_2  = "{{{#!" + sTransTemplate.toLower().toStdString() + " ";
    m_sSampleEnd_2    = "}}}\n";
    qDebug() << "End" << Q_FUNC_INFO;
}

std::string CInsertSyntaxElement::getElementInyokaCode( const std::string &sElement,
                                                        const std::string &sSelectedText )
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

    // Menu / dropdown menu items
    else if ( sElement == "insertUnderConstructionAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Baustelle, Datum, \"Bearbeiter\"" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Baustelle, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertTestedForAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Getestet, " + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Getestet, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertKnowledgeAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Wissen\n"
                    "[:Anwendungen_hinzufügen:Anwendungen hinzufügen]\n"
                    "[:Pakete installieren:Installation von Programmen]\n"
                    "[:Paketquellen freischalten:Bearbeiten von Paketquellen]\n"
                    "[:Paketinstallation_DEB:Ein einzelnes Paket installieren]\n"
                    "[:Programme kompilieren:Pakete aus dem Quellcode erstellen]\n"
                    "[:Terminal:Ein Terminal öffnen]\n"
                    "[:Editor:Einen Editor öffnen]\n"
                    "[:Packprogramme:Archive entpacken]\n"
                    "[:Programme starten:Ein Programm ohne Menüeintrag starten]\n"
                    "[:Autostart:Automatischer Start von Programmen]\n"
                    "[:Menüeditor:Programme zum Menü hinzufügen]\n"
                    "[:sudo:Root-Rechte]\n"
                    "[:Rechte:Rechte für Dateien und Ordner ändern]\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Wissen\n, " + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "insertTableOfContentsAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_12 + "2" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_12 + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertAdvancedAct" )
    {
        if ( sSelectedText == "")
        {
            return m_sSampleBegin_1 + "Fortgeschritten" + m_sSampleEnd_1;
        }
        else
        {
            return sSelectedText + m_sSampleBegin_1 + "Fortgeschritten" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertAwardAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Award, \"Preis\", Link, Preiskategorie, \"Preisträger\"" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Award, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertBashCommandAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Befehl\nShell-Befehl\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Befehl\n" + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "insertNoticeAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Hinweis\nHinweistext\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Hinweis\n" + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "insertWarningAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Warnung\nWarnungstext\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Warnung\n" + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "insertExpertsAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Experten\nHintergrundinformation\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Experten\n" + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "imageAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_11 + "Bild.png, 200 (optional), left|right (optional)" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_11 + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertImageUnderlineAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Bildunterschrift, Bild.png, Bildbreite (optional), \"Beschreibung\", left|right (optional), *-style (optional) " + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Bildunterschrift, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertImageCollectionAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Bildersammlung, Bildhöhe (optional)\n"
                    "Bild1.jpg, \"Beschreibung 1\"\n"
                    "Bild2.png, \"Beschreibung 2\"\n"
                    "Wiki/Icons/Bild3.png, \"Beschreibung 3\"\n" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Bildersammlung, Bildhöhe (optional)\n" + sSelectedText + "\n" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertImageCollectionInTextAct" )
    {
        if ( sSelectedText == "")
        {
            return m_sSampleBegin_1 + "Bildersammlung, left|right, Bildhöhe (optional)\n"
                    "Bild1.jpg, \"Beschreibung 1\"\n"
                    "Bild2.png, \"Beschreibung 2\"\n"
                    "Wiki/Icons/Bild3.png, \"Beschreibung 3\"\n" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Bildersammlung, left|right, Bildhöhe (optional)\n" + sSelectedText + "\n" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertPackageListAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Pakete, paket1, paket2" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Pakete, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertPackageInstallAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Paketinstallation\n"
                    "paketname1, Information (optional)\n"
                    "paketname2, Information (optional)\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Paketinstallation\n" + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "insertPPAAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "PPA, PPA-Besitzer, PPA-Name" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "PPA, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertThirdPartyRepoAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Fremdquelle, URL zur Fremdquelle, Ubuntuversionen (optional), Komponent/en (optional) " + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Fremdquelle, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertThirdPartyRepoAuthAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Fremdquelle-auth, key PGP-Schlüsselnummer ODER Link zum PGP-Schlüssel" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Fremdquelle-auth, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertThirdPartyPackageAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Fremdpaket, (Projekthoster, Projektname) ODER (\"Anbieter\", URL Downloadübersicht), Ubuntuversion/en (optional) " + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Fremdpaket, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertImprovableAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Ausbaufähig, \"Begründung\"" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Ausbaufähig, \"" + sSelectedText + "\"" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertFixMeAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Fehlerhaft, \"Begründung\"" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Fehlerhaft, \"" + sSelectedText + "\"" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertLeftAct" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Verlassen, \"Begründung\"" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Verlassen, \"" + sSelectedText + "\"" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "insertThirdPartyWarningAct" )
    {
        if ( sSelectedText == "")
        {
            return m_sSampleBegin_1 + "Fremd, Paket|Quelle|Software, \"Kommentar\"" + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Fremd, Paket|Quelle|Software, \"" + sSelectedText + "\"" + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "Keys" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_1 + "Tasten, " + m_sSampleEnd_1;
        }
        else
        {
            return m_sSampleBegin_1 + "Tasten, " + sSelectedText + m_sSampleEnd_1;
        }
    }
    else if ( sElement == "Table" )
    {
        if ( sSelectedText == "" )
        {
            return m_sSampleBegin_2 + "Tabelle\n<-2 rowclass=\"titel\">Titel\n+++\n" +
                   "<rowclass=\"kopf\">Kopf\nKopf2\n+++\n" +
                   "foo\nbar\n+++\n<rowclass=\"highlight\">foo2\nbar2\n+++\n" +
                    "<-2 :>Mehr Infos: [:Wiki/Tabellen:]\n" + m_sSampleEnd_2;
        }
        else
        {
            return m_sSampleBegin_2 + "Tabelle\n" + sSelectedText + "\n" + m_sSampleEnd_2;
        }
    }
    else if ( sElement == "GameInfoBox" )
    {
        QString sTmp("[[Bild(Wiki/Icons/games.png, 48, right)]]\n= Infobox =\n"
                     "{{{#!vorlage Tabelle\n<-2 tablestyle=\"width: 97%;\" rowclass=\"verlauf\"> Spielname\n+++\n"
                     "Originaltitel:\nOriginaltitel meist Englisch\n+++\n"
                     "Genre:\nGenre z.B. Adventure\n+++\n"
                     "Sprache:\nLänderfähnchen z.B. {de} {en}\n+++\n"
                     "Veröffentlichung:\nJahr der Veröffentlichung\n+++\n"
                     "Publisher:\nName des Herausgebers\n+++\n"
                     "Systemvoraussetzungen:\nOffizielle Systemvoraussetzung\n+++\n"
                     "Medien:\nz.B. Diskette (7) / CD (1) / DVD (1) / Download\n+++\n"
                     "Strichcode / EAN / GTIN:\n[ean:5901234123457:]\n+++\n"
                     "<-2 rowclass=\"kopf\">\n+++\n"
                     "Läuft mit:\nz.B. [:Spiele/DOSBox:DOSBox], [:Spiele/ScummVM: ScummVM], [:Wine:] und/oder nativ\n}}}");
        return sTmp.toStdString();
    }
    // Unkonwn element
    else
        return sSelectedText;
}
