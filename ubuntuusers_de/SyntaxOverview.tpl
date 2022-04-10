= Syntax =
{{{#!vorlage Tabelle
<rowclass="kopf">Syntax
Beispiel-Ergebnis
Beschreibung
+++
`'''TEXT'''`
'''fett''' 
Text fett formatieren
+++
`''TEXT''`
''kursiv''
Text kursiv formatieren
+++
`\`TEXT\``
`monotype`
Text mit fester Buchstabenweite formatieren
+++
`__TEXT__`
__unterstrichen__
Text unterstreichen
+++
`--(TEXT)--`
--(durchstreichen)--
Text durchstreichen
+++
`^^(TEXT)^^`
^^(hochgestellt)^^
Text hochgestellt
+++
`,,(TEXT),,`
,,(tiefgestellt),,
Text tiefgestellt
+++
`~+(TEXT)+~`
~+(größer)+~
Schrift größer darstellen
+++
`~-(TEXT)-~`
~-(kleiner)-~
Schrift kleiner darstellen
+++
`[size=X]TEXT[/size]`
[size=14]14A[/size][size=10]10B[/size][size=6]6C[/size]
Text in Schriftgröße X darstellen, X wird in Pixel angegeben
+++
`[color=#ABCDEF]TEXT[/color]`
[color=#ABCDEF]#ABCDEF[/color]
Text farbig darstellen, die Farbe wird als RGB Hex-Wert angegeben.
+++
`[color=red]TEXT[/color]`
[color=red]Farbe[/color]
Text farbig darstellen, die Farbe wird als standardisierter Farbname angegeben
+++
`[mark]TEXT[/mark]`
[mark]Text[/mark]
Textmarker, die Farbkombination ist vorgegeben
+++
`----`

horizontaler Trennstrich
+++
`[[Bild(NAME.png, GRÖSSE, AUSRICHTUNG)]]`
[[Bild(Wiki/Icons/tux.png, x32)]]
Stellt das Bild "NAME.png" dar, "GRÖSSE" und "AUSRICHTUNG" sind optional; "NAME.png" muss sich im Anhang befinden.
+++
`[URL]`
[https://ubuntuusers.de/]
Link auf URL
+++
`[URL TEXT]`
[https://ubuntuusers.de/ Text]
Link auf URL, anstelle der URL wird "TEXT" angezeigt.
+++
`[:WIKISEITE:]`
[:Wiki:]
Link auf "WIKISEITE" im Wiki
+++
`[:WIKISEITE:TEXT]`
[:Wiki:Text]
Link auf "Wikiseite" im Wiki, anstelle des Seitennames wird "Text" angezeigt.
+++
`[[Anker(NAME)]]`
[[Anker(Name)]]
Erzeugt den Anker "NAME" an dieser Stelle, "NAME" ist dabei optional, ohne Namen müsste bei Nutzung auf `#None` statt auf `#NAME` verlinkt werden.
+++
`[#ANKER TEXT]`
[#Syntax Syntax]
Auf einen Anker oder eine Überschrift innerhalb einer Wikiseite verlinken.
+++
`[:WIKISEITE#ANKER:TEXT]`
[:Wiki#Vorwissen:]
Auf einen Anker oder eine Überschrift einer anderen Wikiseite verlinken.
+++
`[INTERWIKI:SUCHBEGRIFF:]`
[wikipedia:Ubuntu:]
Link auf "SUCHBEGRIFF" auf der mit "INTERWIKI" angegebenen Seite.
+++
`[INTERWIKI:SUCHBEGRIFF:ALTERNATIVER TEXT]`
[wikipedia:Ubuntu:Wikipedia Artikel zu Ubuntu]
Link auf "Suchbegriff" auf der mit "Interwiki" angegebenen Seite. Anstelle des Suchbegriffs wird als Linktext aber "alternativer Text" angezeigt.
+++
`[[Anhang(NAME_DES_ANHANGS.txt)]]`
[[Anhang(name_des_anhangs.txt)]]
Auf einen Anhang der Wikiseite verweisen.
+++
`[topic:NAME:TEXT]`
[topic:Name:Text]
Link ins Forum auf den Thread mit dem Namen "NAME", "TEXT" ist optional und wird anstelle des Links angezeigt.
+++
`[post:12345:TEXT]`
[post:12345:Text]
Link ins Forum auf das Posting mit der Nummer "12345", "TEXT" ist optional und wird anstelle des Links angezeigt.
+++
`[forum:NAME:TEXT]`
[forum:Name:Text]
Link ins Forum "NAME", "TEXT" ist optional und wird anstelle des Links angezeigt.
+++
`[paste:NUMMER:TITEL]`
[paste:Nummer:Titel]
Link auf den Eintrag "NUMMER" mit dem Titel "TITEL" im NoPaste Service.
+++
`[ikhaya:NAME_DER_MELDUNG:TEXT]`
[ikhaya:Name_der_Meldung:Text]
Link auf eine Ikhaya-Meldung, "TEXT" wird anstelle des Namens der Meldung angezeigt, ist aber optional.
+++
`[bug:NUMMER:TEXT]`
[bug:nummer:Text]
Link auf den Bug "NUMMER" auf Launchpad, "TEXT" ist optional und wird anstelle des Links angezeigt.
+++
`{{{`[[BR]]`CODE`[[BR]]`}}}`
-
Eine unformatierte Codezeile, Zeilenumbrüche werden übernommen.
+++
`{{{#!code SPRACHE`[[BR]]`QUELLTEXT`[[BR]]`}}}`
-
stellt QUELLTEXT mit Syntaxhervorhebung für "SPRACHE" dar.
+++
`> ZITAT`
-
stellt "ZITAT" als zitierten Text dar.
+++
` * Punkt 1`[[BR]]` * Punkt 2`
-
unnummerierte Liste, die Anzahl der Leerzeichen vor * bestimmen die Ebene.
+++
` 1. Punkt 1`[[BR]]` 1. Punkt 2`
-
nummerierte Liste, die Anzahl der Leerzeichen vor 1. bestimmen die Ebene. Kann auch mit Buchstaben genutzt werden (a.).
+++
`= ÜBERSCHRIFT =`
-
fügt eine Überschrift ein, die Anzahl der Gleichheitszeichen bestimmt die Überschriftenebene (max. 5)
+++
`{{{#!vorlage Tabelle`[[BR]]Zelle 1[[BR]]Zelle 2[[BR]]`+++`[[BR]]Zelle 3[[BR]]Zelle 4[[BR]]`}}}`
-
erzeugt eine Tabelle
+++
` \\ ` und [[Vorlage(Tasten, return)]] oder `[[BR]]`
-
erzwungener Zeilenumbruch
+++
`((FUSSNOTE))`
-
Baut eine Fußnote in einen Artikel oder Thread ein.
+++
`## KOMMENTAR`
-
fügt einen Kommentar ein, der nicht in der Ausgabe erscheint.
+++
`[[Inhaltsverzeichnis(X)]]`
-
fügt ein Inhaltsverzeichnis, generiert aus den Überschriften, ein; "X" gibt die Ebene an, bis zu der die Überschriften berücksichtigt werden.
+++
`#tag: tag1, tag2`
-
ordnet dem Artikel die Tags "tag1" und "tag2" zu.
}}}

= Vorlagen =
{{{#!vorlage Tabelle
<rowclass="kopf">Syntax
Ergebnis
Beschreibung
+++
`[[Vorlage(Tasten, TASTE)]]`
[[Vorlage(Tasten, W+i+k+i)]]
stellt TASTE als Bild dar, kombinieren geht mit +
+++
`[[Vorlage(Getestet, UBUNTUVERSION(EN))]]`

fügt die "Getestet" Infobox für "UBUNTUVERSION(EN)" ein.
+++
`[[Vorlage(Getestet, general)]]`

fügt die "Getestet: general" Infobox ein.
+++
`[[Vorlage(Baustelle, DATUM, "BEARBEITER")]]`

fügt die Infobox für Baustellen (neu erstellte Wiki-Artikel, die noch nicht fertig sind) ein, "DATUM" ist das geplante Fertigstellungsdatum, "BEARBEITER" der aktuelle Autor des Wiki-Artikels.
+++
`[[Vorlage(Fortgeschritten)]]`

fügt die "Artikel-für-Fortgeschrittene"-Infobox hinzu.
+++
`[[Vorlage(Pakete, "foo bar")]]`

fügt das Paketmakro hinzu.
+++
`[[Vorlage(Ausbaufähig, "BEGRÜNDUNG")]]`

markiert einen Wiki-Artikel als ausbaufähig, "BEGRÜNDUNG" ist der entsprechende Informationstext
+++
`[[Vorlage(Fehlerhaft, "BEGRÜNDUNG")]]`

markiert einen Wiki-Artikel als fehlerhaft, "BEGRÜNDUNG" ist der entsprechende Informationstext
+++
`[[Vorlage(Verlassen, "BEGRÜNDUNG")]]`

markiert einen Wiki-Artikel als verlassen, "BEGRÜNDUNG" ist der entsprechende Informationstext
+++
`[[Vorlage(Überarbeitung, SEITE, BEARBEITER)]]`

markiert einen bestehenden Wiki-Artikel als aktuell in Überarbeitung
+++
`[[Vorlage(Archiviert, "TEXT")]]`

markiert einen Wiki-Artikel als archiviert, "TEXT" ist ein zusätzlicher optionaler Informationstext, Verwendung ausschließlich durch Teammitglieder!
+++
`[[Vorlage(Fremd, Paket, "KOMMENTAR")]]`

fügt eine Warnung vor Fremdpaketen ein, "KOMMENTAR" ist optional
+++
`[[Vorlage(Fremd, Quelle, "KOMMENTAR")]]`

fügt eine Warnung vor Fremdquelle ein, "KOMMENTAR" ist optional
+++
`[[Vorlage(Fremd, Software, "KOMMENTAR")]]`

fügt eine Warnung vor Fremdsoftware ein, "KOMMENTAR" ist optional
+++
`[[Vorlage(PPA, LAUNCHPAD-TEAM/PPA-NAME, key PGP-SCHLÜSSELNUMMER, UBUNTUVERSION(EN) )]]`

Block für PPA-Quellen
+++
`[[Vorlage(Fremdquelle, URL, UBUNTIVERSION(EN), KOMPONENTE(N) )]]`

Block für Fremdquellen
+++
`[[Vorlage(Fremdquelle-auth, URL zum PGP-Key)]]`

Block für Fremdquellenauthentifikation
+++
`[[Vorlage(Fremdquelle-auth, key PGP-SCHLÜSSELNUMMER)]]`

Block für Fremdquellenauthentifikation
+++
`[[Vorlage(Fremdpaket, PROJEKTHOSTER, PROJEKTNAME, UBUNTUVERSION(EN))]]`

Block für Fremdpakete von Projekthostern (Launchpad, ...)
+++
`[[Vorlage(Fremdpaket, "ANBIETER", URL zu einer Downloadübersicht, UBUNTIVERSION(EN))]]`

Block für Fremdpakete von Herstellerseiten, etc.
+++
`[[Vorlage(Fremdpaket, "ANBIETER", dl, URL zu einem Download, UBUNBTUVERSION(EN))]]`

Block für Fremdpakete mit Direktlinks auf Dateien
+++
`{{{#!vorlage Wissen`[[BR]]`WISSENSVERMERKE`[[BR]]`\}}}`

fügt den Wissensblock mit "WISSENSVERMERKEN" hinzu
+++
`{{{#!vorlage Hinweis`[[BR]]`HINWEISTEXT`[[BR]]`\}}}`

fügt eine Hinweisbox ein, "HINWEISTEXT" wird darin ausgegeben
+++
`{{{#!vorlage Warnung`[[BR]]`WARNTEXT`[[BR]]`\}}}`

fügt eine Warnbox ein, "WARNTEXT" wird darin ausgegeben
+++
`{{{#!vorlage Befehl`[[BR]]`BEFEHL`[[BR]]`\}}}`

formatiert die Ausgabe von "BEFEHL" ähnlich einer Terminalausgabe
+++
`{{{#!vorlage Paketinstallation`[[BR]]`PAKETE`[[BR]]`\}}}`

formatiert die Ausgabe von zu installierenden Paketen inklusive weiterer Angaben
}}}
