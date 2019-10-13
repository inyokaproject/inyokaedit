## Macro={{{#!vorlage Paketinstallation\n%%paketname1, Information (optional)\npaketname2, Information (optional)%%\n}}}
#############################################################
## Parser für Paketinstallationen
#############################################################
##
## Verwendung:
## {{{#!vorlage Paketinstallation
## paket0
## paket1, main
## paket2, restricted ab Karmic
## paket3, universe, ab Lucid
## paket4, multiverse ab Karmic, universe ab Lucid
## paket5, security, ab hardy - sowas aber auch
## paket6, ppa
## paket7, playdeb
## paket8, getdeb, ab karmic; hier noch weitere Angaben
## paket9, partner ab karmic
## }}}
##
#############################################################
##
## Angezeigt werden eine wikikonform-formatierte Liste der
## Pakete, sowie bei mehr als N Paketen das Paketmakro.
##
#############################################################
##
## Paketliste:
##
<@ for $row in $arguments join_with '
' split_by '
' @>
<@ for $col in $row split_by ',' @>
<@ if $loop.first @>
 * '''<@ $col @>'''
<@ else @>
<@ if $loop.index==2 @> (
<@ endif @>
<@ for $word in $col split_by ' '@>
<@ if ['main','restricted','universe','multiverse','security','ppa','partner'] contains $word @>
<@ if ['main','restricted','universe','multiverse','security','ppa'] contains $word @>''<@ $word @>''
<@ endif @>
<@ if ['partner'] contains $word @>''[:Canonical_Partner:]''
<@ endif @>
<@ else @><@ $word @>
<@ endif @>
<@ if $loop.last or $loop.first @>
<@ else @> 
<@ endif @>
<@ endfor @>
<@ if $loop.last @>)
<@ else @>, 
<@ endif @>
<@ endif @>
<@ endfor @>
<@ endfor @>
##
## Paketmakro bei mehr als N Paketen, N=0:
##
<@ for $row in $arguments split_by '
' @>
<@ if $loop.first @>
<@ if $loop.length > 0 @>
######
## Statt einfach das Makro Pakete hier einzubinden, wird eine Kopie des Makros benutzt,
## damit die Backlinksuche des Makros Pakete, nicht auch noch die Artikel auflistet, die
## das Makro Paketinstallation benutzen.
## Das könnte man später wieder ändern, der Übersicht wegen.
######
Paketliste zum Kopieren:
[[Vorlage(Wiki/Vorlagen/Befehl, 'sudo apt-get install<@ for $row in $arguments split_by '
' @> 
<@ for $col in $row split_by "," @>
<@ if $loop.first @><@ $col @>
<@ endif @>
<@ endfor @>
<@ endfor @>')]]
<@ endif @>
<@ endif @>
<@ endfor @>
##
## Jetzt Installieren - Button
##
Oder mit [:apturl:] die Pakete installieren. Link: [apt://<@ for $row in $arguments split_by '
' @>
<@ for $col in $row split_by "," @>
<@ if $loop.first @><@ $col @>
<@ endif @>
<@ endfor @>
<@ if $loop.revindex > 1 @>,
<@ endif @>
<@ endfor @>]
