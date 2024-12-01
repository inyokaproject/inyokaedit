## Macro=[[Vorlage(Fehlerhaft, %%"Begründung"%%)]]
{{|<title="Fehlerhafte Anleitung" class="box fixme">
Diese Anleitung ist fehlerhaft. Wenn du weißt, wie du sie ausbessern kannst, nimm dir bitte die Zeit und bessere sie aus.

<@ for $X in $arguments @>
<@ if $loop.first @>
----
'''Anmerkung: '''
<@ endif @>

<@ $X @>

<@ endfor @>
|}}

<@ if '[[PageName]]' contains '/Vorlagen/' @><@ else @>#tag: fehlerhaft<@ endif @>
