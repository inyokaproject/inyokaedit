## Macro=[[Vorlage(Howto, %%Verfasser%%)]]
{{|<title="Hinweis:" class="box notice">
Dieses Howto wurde von
<@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if $loop.last @>
und
<@ else @>
,
<@ endif @>
<@ endif @>
[user:<@ $arg @>:]
<@ endfor @>
erstellt. Bei Problemen mit der Anleitung melde dies bitte in der dazugehörigen Diskussion und wende dich gegebenenfalls zusätzlich an den/die Verfasser des Howtos.
|}}
