## Macro={{{#!vorlage Builddeps\n%%paketname%%\n}}}
<@ for $row in $arguments split_by '
' @><@ if $loop.first @>
{{|<class="package-list">
Befehl zum Installieren der Build-Abhängigkeiten:
[[Vorlage(Wiki/Vorlagen/Befehl, 'sudo apt-get build-dep<@ for $row in $arguments split_by '
' @> <@ for $col in $row split_by "," @><@ if $loop.first @><@ $col @><@ endif @><@ endfor @><@ endfor @>')]]
[[Vorlage(Wiki/Vorlagen/Befehl, 'sudo aptitude build-depends<@ for $row in $arguments split_by '
' @> <@ for $col in $row split_by "," @><@ if $loop.first @><@ $col @><@ endif @><@ endfor @><@ endfor @>')]]
|}}
<@ endif @><@ endfor @> 
