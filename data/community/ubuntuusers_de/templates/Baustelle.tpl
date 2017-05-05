## Macro=[[Vorlage(Baustelle, %%Datum, Bearbeiter%%)]]
{{|<class="box workinprogress" title="Artikel in Arbeit">Dieser Artikel wird momentan von
<@ for $arg in $arguments @>
<@ if $arguments.0 matches_regex "(\d{1,2})\.(\d{1,2})\.(\d{2}|\d{4})" @>
<@ if $loop.index >= 2 @> [user:<@ $arg @>:]
<@ if $loop.revindex > 2 @>, <@ endif @>
<@ if $loop.revindex == 2 @> und <@ endif @><@ endif @>
<@ else @> [user:<@ $arg @>:]
<@ if $loop.revindex > 2 @>, <@ endif @>
<@ if $loop.revindex == 2 @> und <@ endif @>
<@ endif @>
<@ endfor @> erstellt.
<@ if $arguments.0 matches_regex "(\d{1,2})\.(\d{1,2})\.(\d{2}|\d{4})" @>
Als Fertigstellungsdatum wurde der <@ $arguments.0 @> angegeben.
<@ else @>
Solltest du dir nicht sicher sein, ob an dieser Anleitung noch gearbeitet wird, kontrolliere das Datum der letzten Änderung und entscheide, wie du weiter vorgehst.
<@ endif @>
----
'''Achtung''': Insbesondere heißt das, dass dieser Artikel noch nicht fertig ist und dass wichtige Teile fehlen oder sogar falsch sein können. Bitte diesen Artikel nicht als Anleitung für Problemlösungen benutzen!|}}
