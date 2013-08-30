## Macro=
## Diese Vorlage soll nur noch unter dem Namen "Baustelle" verwendet werden - bitte hier keine Änderungen vornehmen sondern nur in [:Wiki/Vorlagen/Baustelle:]
{{|<class="box workinprogress" title="Artikel in Arbeit">Dieser Artikel wird momentan <@if $arguments.1 @>von [user:<@ $arguments.1 @>:]<@if $arguments.2 @> und [user:<@ $arguments.2 @>:]<@ endif @><@ endif @> erstellt.
<@ if $arguments.0 matches_regex "(\d{1,2})\.(\d{1,2})\.(\d{2}|\d{4})" @>
Als Fertigstellungsdatum wurde der <@ $arguments.0 @> angegeben.
<@ else @>
Solltest du dir nicht sicher sein, ob an dieser Anleitung noch gearbeitet wird, kontrolliere das Datum der letzten Änderung und entscheide, wie du weiter vorgehst.
<@ endif @>
----
'''Achtung''': Insbesondere heißt das, dass dieser Artikel noch nicht fertig ist und dass wichtige Teile fehlen oder sogar falsch sein können. Bitte diesen Artikel nicht als Anleitung für Problemlösungen benutzen!|}}
