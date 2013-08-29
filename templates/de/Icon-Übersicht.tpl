## Macro={{{#!vorlage Icon-Übersicht\n%%Überschrift/Stil\nbild1.png\nbild2.png%%\n}}}
## Vorlage für Icon-Übersichts-Tabellen unterhalb [:Wiki/Icons:]
##
##-> Anwendungsbeispiel
##
##    Erste Zeile ("Ubuntu Icons") ist der Titel,
##    darauf folgt optional die CSS-Klasse getrennt durch einen Schrägstrich;
##    der Rest sind Einträge (Dateiname.Endung)
##
##    {{{#!vorlage Icon-Übersicht
##    Ubuntu Icons/kde
##    ubuntu.png
##    kubuntu.png
##    xubuntu.png
##    }}}
##
##-> Quelltext
<@ for $arg in $arguments split_by '
' @>
<@ if $loop.first @>
<@ for $att in $arg split_by "/"@>
<@ if $loop.first @>
||<-4 tablestyle="width: 95%;" rowclass="
<@ endif @>
<@ if $loop.last @>
<@ if $att == $arg @><@ else @><@ $att @>-<@ endif @>titel">
<@ endif @>
<@ endfor @>
<@ for $att in $arg split_by "/"@>
<@ if $loop.first @><@ $att @> ||

<@ endif @>
<@ if $loop.last @>||<rowclass="
<@ if $att == $arg @><@ else @><@ $att @>-
<@ endif @>kopf" cellstyle="width: 24%"> Icon ||<cellstyle="width: 25"%> Dateiname ||<cellstyle="width: 24"%> Icon ||<cellstyle="width: 25"%> Dateiname ||
<@ endif @>
<@ endfor @>
<@ else @>
<@ if $loop.index is even @>
|| [[Bild(Wiki/Icons/<@ $arg @>, x32, center)]] || '''<@ $arg @>''' ||
<@ if $loop.last @><-2>||<@ endif @>
<@ endif @>
<@ if $loop.index is odd @> [[Bild(Wiki/Icons/<@ $arg @>, x32, center)]] || '''<@ $arg @>''' ||
<@ endif @>
<@ endif @>
<@ endfor @>
