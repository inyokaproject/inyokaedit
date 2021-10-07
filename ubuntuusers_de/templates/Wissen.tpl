## Macro={{{#!vorlage Wissen\n%%[:Anwendungen_hinzufügen:Anwendungen hinzufügen]\n[:Pakete installieren:Installation von Programmen]\n[:Paketquellen freischalten:Bearbeiten von Paketquellen]\n[:Paketinstallation_DEB:Ein einzelnes Paket installieren]\n[:Programme kompilieren:Pakete aus dem Quellcode erstellen]\n[:Terminal:Ein Terminal öffnen]\n[:Editor:Einen Editor öffnen]\n[:Packprogramme:Archive entpacken]\n[:Programme starten:Ein Programm ohne Menüeintrag starten]\n[:Autostart:Automatischer Start von Programmen]\n[:Menüeditor:Programme zum Menü hinzufügen]\n[:mit Root-Rechten arbeiten:]\n[:Rechte:Rechte für Dateien und Ordner ändern]%%\n}}}
## Wissensvorlage
##
## Einbinden mit:
## {{{#!vorlage Wissen
## [:Link:]
## [:Link:], optional
## }}}
##
## Bei Änderungen jeweils beide Listen mit alternativen Schreibweisen anpassen bzw. beide Ersetzungen.
##
## Alternative Schreibweisen: "optional", "(optional)", "''(optional)''", "(''optional'')" (entweder durch Leerzeichen oder Komma getrennt)
## Ersetzung: ", ''optional''"
##
{{|<title="Zum Verständnis dieses Artikels sind folgende Seiten hilfreich:" class="box knowledge">
<@ for $arg in $arguments join_with '
' split_by '
'@>
 1. [[Anker(source-<@ $loop.index @>)]]
<@ for $word in $arg split_by ','@>
<@ if ['optional', '(optional)', "''(optional)''", "(''optional'')"] contains (($word as stripped) as lowercase) @>, ''optional''
<@ else @>
<@ for $part in $word split_by ' ' @>
<@ if ['optional', '(optional)', "''(optional)''", "(''optional'')"] contains ($part as lowercase) @>, ''optional''
<@ else @> <@$part@>
<@ endif @>
<@ endfor @>
<@ endif @>
<@ endfor @>
<@ endfor @>
|}}
