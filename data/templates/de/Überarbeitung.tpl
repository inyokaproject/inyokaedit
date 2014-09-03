## Macro=[[Vorlage(Überarbeitung, %%Datum, Artikel, Bearbeiter%%)]]
{{|<class="box workinprogress" title="Artikel wird überarbeitet">Dieser Artikel wird momentan überarbeitet.
<@ for $arg in $arguments @>
<@ if $loop.index == 1 @>
 * Geplante Fertigstellung: <@ $arg @><@ endif @>
<@ if $loop.index == 2 @>
 * Derzeitig gültiger Artikel: [:<@ $arg @>:]
 * Bearbeiter: <@ endif @>
<@ if $loop.index >= 3 @><@ if ['Projektleitung', 'Supporter', 'Moderatoren', 'Ikhayateam', 'Wikiteam', 'Serverteam'] contains $arg @>[:ubuntuusers/<@ $arg @>:<@ $arg @>]<@ else @>[user:<@ $arg @>:]<@ endif @>
<@ if $loop.revindex > 2 @>, <@ endif @>
<@ if $loop.revindex == 2 @> und <@ endif @><@ endif @>
<@ endfor @>
Solltest du dir nicht sicher sein, ob an dieser Anleitung noch gearbeitet wird, kontrolliere das Datum der
<@ if $arguments.1 == "" @>letzten Änderung
<@ else @>[http://wiki.ubuntuusers.de/Baustelle/<@ $arguments.1 @>?action=log letzten Änderung]
<@ endif @> und entscheide, wie du weiter vorgehst.
----
'''Achtung''': Insbesondere heißt das, dass dieser Artikel noch nicht fertig ist und dass wichtige Teile fehlen oder sogar falsch sein können. Bitte diesen Artikel nicht als Anleitung für Problemlösungen benutzen!|}}
