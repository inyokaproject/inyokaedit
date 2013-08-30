## Macro=
## Vorlage Projekte
## 
## {{{#!vorlage Projekte, "Überschrift "
## +++
## Wiki/Icons/icon.png
## Projekt-Teil
## Beschreibung
## [:Link:], [http://... Link]
## +++
## }}}
##
##

<@ for $arg in $arguments split_by '+++
' @>
<@ if $loop.first @>
||<tablestyle="width: 96%; margin-left: 1em;" -2 rowclass="verlauf"><@ $arguments.0 @>||
<@ else @>
<@ for $attr in $arg split_by '
' @>
<@ if $loop.index == 1 @>
||<rowstyle="background-color: #f2f2f2;" cellstyle="border-right-color: #f2f2f2;font-weight:bold;" :>[[Bild(<@ $attr @>, 32)]][[BR]]
<@ elseif $loop.index == 2 @><@ $attr @>
<@ elseif $loop.index == 3 @>||<cellstyle="padding-top:0.8em;border-left-color: #f2f2f2;"> <@ $attr @>
<@ elseif $loop.index == 4 @>{{|<style="color:#000000;margin-left:1em;">
<@ for $atom in $attr split_by ', ' @><@ $atom @>
<@ if $loop.revindex > 1 @> | 
<@ endif @>
<@ endfor @> |}} ||
<@ elseif $loop.last @>
<@ endif @>
<@ endfor @>
<@ endif @>
<@ endfor @>
