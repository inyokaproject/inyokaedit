## Macro=[[Vorlage(PPA, %%PPA-Besitzer, PPA-Name%%)]]
Adresszeile zum [:Paketquellen_freischalten/PPA#PPA-hinzufuegen:Hinzufügen] des PPAs:
## Um zur alten Version der Vorlage kompatibel zu bleiben
## zunächst die Variante
## [[Vorlage(PPA, shkn/xnoise)]]
<@ if $arguments.0 contain '/'@>
## $arguments.0 ist beispielsweise shkn/xnoise
## teilt man den String bei /, erhält man zwei Argumente.
## das erste ist der Benutzer shkn:
##
## <@ for $arg in $arguments.0 split_by "/"@>
## <@ if $loop.first @><@ $arg @>
## <@ endif @>
## <@ endfor @>
##
## das letzte der PPA-Name xnoise:
## <@ for $arg in $arguments.0 split_by "/"@>
## <@ if $loop.last @>
## <@ $arg @>
## <@ endif @><@ endfor @>
##
## Ziel ist folgender Code:
## * '''ppa:shkn/xnoise''' 
## {{|<title="Hinweis!" class="box warning">
## Zusätzliche [:Fremdquellen:] können das System gefährden. 
## ----
## Weitere Informationen sind der [[Bild(Wiki/Vorlagen/PPA/ppa.png)]] [https://launchpad.net/~shkn/+archive/xnoise PPA Beschreibung] des Eigentümers/Teams [lpuser:shkn:] zu entnehmen.
## |}}
##
## Darin ersetzt man nun alle Vorkommen von shkn und xnoise durch die oben aufgeführten Codeschnipsel.
##
 * '''ppa:<@ for $arg in $arguments.0 split_by "/"@>
<@ if $loop.first @><@ $arg @>
<@ endif @>
<@ endfor @>/<@ for $arg in $arguments.0 split_by "/"@>
<@ if $loop.last @>
<@ $arg @>
<@ endif @><@ endfor @>''' 
{{|<title="Hinweis!" class="box warning">
Zusätzliche [:Fremdquellen:] können das System gefährden. 
----
Ein PPA unterstützt nicht zwangsläufig alle Ubuntu-Versionen. Weitere Informationen sind der [[Bild(Wiki/Vorlagen/PPA/ppa.png)]] [https://launchpad.net/~<@ for $arg in $arguments.0 split_by "/"@>
<@ if $loop.first @><@ $arg @>
<@ endif @>
<@ endfor @>/+archive/<@ for $arg in $arguments.0 split_by "/"@>
<@ if $loop.last @>
<@ $arg @>
<@ endif @><@ endfor @> PPA-Beschreibung] des Eigentümers/Teams [lpuser:<@ for $arg in $arguments.0 split_by "/"@>
<@ if $loop.first @><@ $arg @>
<@ endif @>
<@ endfor @>:] zu entnehmen.
|}}
<@ else @>
## nun zur wesentlich eleganteren Variante
## [Vorlage(PPA, shkn, xnoise)]
## hier muss im "Zielcode" nur shkn mit <@ $arguments.0 @>
## und xnoise mit <@ $arguments.1 @> ersetzt werden,
## nur zur Kompatibilität noch ein workaround für "ppa-PPAs"
 * '''ppa:<@ $arguments.0 @>/<@ for $arg in $arguments @>
<@ if $loop.last @>
<@ if $loop.length > 2 @>ppa 
<@ else @><@ $arguments.1 @>
<@ endif @>
<@ if $loop.length == 1 @>ppa
<@ endif @>
<@ endif @><@ endfor @>''' 
{{|<title="Hinweis!" class="box warning">
Zusätzliche [:Fremdquellen:] können das System gefährden. 
----
Ein PPA unterstützt nicht zwangsläufig alle Ubuntu-Versionen. Weitere Informationen sind der [[Bild(Wiki/Vorlagen/PPA/ppa.png)]] [https://launchpad.net/~<@ $arguments.0 @>/+archive/
<@ for $arg in $arguments @>
<@ if $loop.last @>
<@ if $loop.length > 2 @>ppa 
<@ else @><@ $arguments.1 @>
<@ endif @>
<@ if $loop.length == 1 @>ppa
<@ endif @>
<@ endif @><@ endfor @> PPA-Beschreibung] des Eigentümers/Teams [lpuser:<@ $arguments.0 @>:] zu entnehmen.
|}}
<@ endif @>
