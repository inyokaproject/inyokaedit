## Macro=[[Vorlage(Fremd, Paket|Quelle|Software, %%"Kommentar"%%)]] 
{{|<title="Hinweis!" class="box warning">
<@ if $arguments contain 'Quelle' @>
Zusätzliche [:Fremdquellen:] können das System gefährden.
<@ endif @>
<@ if $arguments contain 'Paket' @>
[:Fremdquellen: Fremdpakete] können das System gefährden.
<@ endif @>
<@ if $arguments contain 'Software' @>
[:Fremdsoftware:] kann das System gefährden.
<@ endif @>
<@ if $arguments.1 @>
----
'''Anmerkung:''' <@ $arguments.1 @>
<@ endif @>
|}}
