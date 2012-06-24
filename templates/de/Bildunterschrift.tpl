## Macro=[[Vorlage(Bildunterschrift, %%BILDLINK, BILDBREITE, "Beschreibung", left|right%%)]]
## Argument0 = Bildadresse
## Argument1 = Breite, oder Beschreibung
## Argument2 = Beschreibung oder left/right
##
## Zum Verständnis:
## Erstellt eine Tabelle mit zwei Zeilen
## 1. Zeile = Argument0 (Bild-Adresse)
## 2. Zeile = Je nach dem ob "Breite" angegeben, wird Argument1 oder Argument2 in die Zeile eingefügt

||<tablestyle="<@ if $arguments as lowercase contain 'left' @>float: left; clear: right; <@ else @> float: right; clear: left; <@ endif @> margin-top: 0px; border: none;" rowclass="
<@ if $arguments as lowercase contain 'kde-style' @>kde-<@ endif @>
<@ if $arguments as lowercase contain 'xfce-style' @>xfce-<@ endif @>
<@ if $arguments as lowercase contain 'lxde-style' @>lxde-<@ endif @>
<@ if $arguments as lowercase contain 'edu-style' @>edu-<@ endif @>
<@ if $arguments as lowercase contain 'studio-style' @>studio-
<@ endif @>titel"> [[Bild(<@ $arguments.0 @><@ if $arguments.1 matches_regex "\d+" @>, <@ $arguments.1 @>)]]||
||<cellstyle="width:<@ $arguments.1 @>px;"; rowclass="
<@ if $arguments as lowercase contain 'kde-style' @>kde-<@ endif @>
<@ if $arguments as lowercase contain 'xfce-style' @>xfce-<@ endif @>
<@ if $arguments as lowercase contain 'lxde-style' @>lxde-<@ endif @>
<@ if $arguments as lowercase contain 'edu-style' @>edu-<@ endif @>
<@ if $arguments as lowercase contain 'studio-style' @>studio-
<@ endif @>normal" :> <@ $arguments.2 @>||<@ else @>)]]||
||<rowclass="
<@ if $arguments as lowercase contain 'kde-style' @>kde-<@ endif @>
<@ if $arguments as lowercase contain 'xfce-style' @>xfce-<@ endif @>
<@ if $arguments as lowercase contain 'lxde-style' @>lxde-<@ endif @>
<@ if $arguments as lowercase contain 'edu-style' @>edu-<@ endif @>
<@ if $arguments as lowercase contain 'studio-style' @>studio-
<@ endif @>normal" :> <@ $arguments.1 @>||<@ endif @>
