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

## Setze Metadatum, aber nur außerhalb bestimmter Namensräume um Übersichten frei von unsinnigen Einträgen zu halten:
## Wiki/: Beschreibungen dieser Vorlage und dies Vorlage selbst sind nicht fehlerhaft.
## Baustelle/: Unfertiges ist noch nicht fehlerhaft.
## Archiv/: Archiviertes mag fehlerhaft sein, soll aber nicht ausgebessert werden.
## Trash/: Gelöschtes mag fehlerhaft sein, ist aber nicht mehr relevant und muss daher nicht ausgebessert werden.

## <@ if '[[PageName]]' contains '/Vorlagen/' @><@ else @>#tag: fehlerhaft<@ endif @>
<@     if '[[PageName]]' starts_with 'Wiki/' @>
<@ elseif '[[PageName]]' starts_with 'Baustelle/' @>
<@ elseif '[[PageName]]' starts_with 'Archiv/' @>
<@ elseif '[[PageName]]' starts_with 'Trash/' @>
<@ else @>#tag: fehlerhaft
<@ endif @>
