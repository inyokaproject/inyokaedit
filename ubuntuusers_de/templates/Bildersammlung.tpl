## Macro=[[Vorlage(Bildersammlung, BILDHÖHE\n%%Bild1.jpg, "Beschreibung 1"\nBild2.png, "Beschreibung 2"%%\n)]]
<@ if $arguments.0 as lowercase contains 'left' or  $arguments.0 as lowercase contains 'right' @>

<@ if $arguments.0 as lowercase contains 'left' @>
{{|<style="overflow: auto;">
<@ endif @>

## (left|right, number): schwebende Tabelle mit Höhenangabe:

||<tablestyle="float:<@ $arguments.0 @>; border:none;" rowstyle="background-color: #E2C889;" cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ if $arguments.1 matches_regex "\d+" @>
<@ for $arg in $arguments @>
<@ if $loop.index > 2 @>
<@ if $loop.index is odd @>
<@ if $loop.index > 4 @><cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ endif @>[[Bild(<@ $arg @>, x<@ $arguments.1 @>)]] ||<@ endif @>
<@ endif @>
<@ endfor @>
||<rowstyle="background-color: #F9EAAF;" cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ for $arg in $arguments @>
<@ if $loop.index > 2 @>
<@ if $loop.index is even @>
<@ if $loop.index > 4 @><cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ endif @><@ $arg @> ||<@ endif @>
<@ endif @>
<@ endfor @>

## (left|right): schwebende Tabelle, Bilder nicht skaliert:

<@ else @>
<@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if $loop.index is even @>
<@ if $loop.index > 3 @><cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ endif @>[[Bild(<@ $arg @>)]] ||<@ endif @>
<@ endif @>
<@ endfor @>
||<rowstyle="background-color: #F9EAAF;" cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ for $arg in $arguments @>
<@ if $loop.index > 1 @>
<@ if $loop.index is odd @>
<@ if $loop.index > 3 @><cellstyle="border-width: 0 10px 0 0; border-color: #FFFFFF;" :> <@ endif @><@ $arg @> ||<@ endif @>
<@ endif @>
<@ endfor @>
<@ endif @>

<@ if $arguments.0 as lowercase contains 'left' @>
|}}
<@ endif @>

<@ else @>

## ([number]): umbrechende Tabelle mit Höhenangabe, sonst Höhe = 140px:

{{|<style="overflow: auto;">

<@ for $arg in $arguments @>
<@ if $arguments.0 matches_regex "\d" @>
<@ if $loop.index > 1 @>
<@ if $loop.index is even @>||<tablestyle="float:left; margin:10px 5px; border:none;" cellstyle="background-color: #E2C889; border:none;" :> [[Bild(<@ $arg @>,x<@ $arguments.0 @>)]] ||<@ endif @>
<@ if $loop.index is odd @>
||<cellstyle="background-color: #F9EAAF; border:none;" :> <@ $arg @>||
{{||}}

<@ endif @>
<@ endif @>
<@ else @>
<@ if $loop.index is odd @>||<tablestyle="float:left; margin:10px 5px; border:none;" cellstyle="background-color: #E2C889; border:none;" :> [[Bild(<@ $arg @>,x140)]] ||<@ endif @>
<@ if $loop.index is even @>
||<cellstyle="background-color: #F9EAAF; border:none;" :> <@ $arg @>||
{{||}}

<@ endif @>
<@ endif @>
<@ endfor @>

<@ endif @>
