## Macro={{{#!vorlage Infobox %%"Name des Programms", "[http://example.org/ Projektseite] {en}", "Bildschirmfoto.png", "Installationsmethode(n)"\n+++\nWert 1\nSchlüssel 1%%\n}}}
<@ for $arg in $arguments split_by '+++
' @>
<@ if $loop.first @>
||<-2 tablestyle="float:right; clear:right; margin-top: 1em; margin-left: 1.5em;" rowclass="verlauf"><@ $arguments.0 @>||
||<-2 cellstyle="text-align: center; line-height: 2.2em;"> <@ $arguments.1 @>
<@ if $arguments.2 ends_with png or $arguments.2 ends_with jpg @>

\\
[[Bild(<@ $arguments.2 @>, 260, )]]
<@ endif @>||
<@ else @>
<@ for $attr in $arg split_by '
' @>
<@ if $loop.first @>
||<@ $attr @>:||
<@ elseif $loop.index == 2 @><@ $attr @>||
<@ endif @>
<@ endfor @>
<@ endif @>
<@ endfor @>
||<-3 rowclass="highlight">||
||Installation:||<@ $arguments.3 @>||
