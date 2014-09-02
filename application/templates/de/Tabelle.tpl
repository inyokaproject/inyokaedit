## Macro={{{#!vorlage Tabelle\n%%<-2 rowclass="titel">Titel\n+++\n<rowclass="kopf">Kopf\nKopf2\n+++\nfoo\nbar\n+++\n<rowclass="highlight">foo2\nbar2\n+++\n<-2 :>Mehr Infos: [:Wiki/Tabellen:]%%\n}}}
## Tabellenvorlage zur vereinfachten und besseren Pflege von Tabellen.
##
## ZeilentrennBedingung um 'as stripped contains' erweitert, da die Bedingung bei Fehlerhafter Eingabe (zB '+++ ') nicht gegriffen hat. Zudem sind so weitere Angaben beim Zeilentrenner möglich (zB. Angabe der Zeilennummer, Kommentare)

||<@ for $line in $arguments split_by '
' @>
<@ if $line as stripped contains '+++' @>
||<@ else @>
<@ $line @> ||
<@ endif @>
<@ endfor @>
