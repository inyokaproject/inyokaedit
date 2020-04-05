## Macro=[[Vorlage(Fremdpaket, %%Projekthoster, Projektname|URL, Ubuntuversion(en)%%)]]
#############################################################
## Eingabemöglichkeiten:
##  mind. 2 Argumente
##  [[Vorlage(Fremdpaket, sourceforge, projektname, xenial)]]
##  [[Vorlage(Fremdpaket, launchpad, projektname, xenial, bionic)]]
##  [[Vorlage(Fremdpaket, googlecode, projektname, xenial, bionic)]]
##  [[Vorlage(Fremdpaket, "Anbieter", URLzurDL-Übersicht, xenial, bionic)]]
##  mind. 3 Argumente
##  [[Vorlage(Fremdpaket, "Anbieter", dl, URLzumDL1, URLzumDL2, URLzumDL3, xenial, bionic)]]
#############################################################
## Der folgende Block unterscheidet obige Eingabemöglichkeiten.
#############################################################
<@ if ['sourceforge','launchpad','googlecode'] contains $arguments.0 @>
<@ if $arguments.0 == 'sourceforge' @>
Beim [wikipedia:SourceForge:]-Projekt [sourceforge:<@ $arguments.1 @>:] werden [sourceforge:<@ $arguments.1 @>/files/: DEB-Pakete] angeboten. 
<@ endif @>
<@ if $arguments.0 == 'launchpad' @>
Beim [:Launchpad:]-Projekt [launchpad:<@ $arguments.1 @>:] werden [launchpad:<@ $arguments.1 @>/+download: DEB-Pakete] angeboten. 
<@ endif @>
<@ if $arguments.0 == 'googlecode' @>
Beim [http://code.google.com/intl/de/ Google Code] {de} -Projekt [googlecode:<@ $arguments.1 @>:] werden [googlecode:<@ $arguments.1 @>/downloads/: DEB-Pakete] angeboten. 
<@ endif @>
<@ else @>
Von <@ $arguments.0 @> werden 
<@ if $arguments.1 == 'dl' @>
folgende DEB-Pakete angeboten:
<@ for $arg in $arguments @>
<@ if $loop.index < 3 or ['focal', 'eoan', 'bionic', 'xenial'] contains $arg @>
##nichts tun
<@ else @>
 * [<@ $arg @> <@ for $arg2 in $arg split_by "/"@><@ if $loop.last @><@ $arg2 @><@ endif @><@ endfor @>] {dl}
<@ endif @>
<@ endfor @>
<@ else @>
[<@ $arguments.1 @> DEB-Pakete] {dl} angeboten. 
<@ endif @>
<@ endif @>
#############################################################
## Der folgende Block dient zum Erzeugen eines Satzes, der die unterstützten Version auflistet und mit "," bzw. "und" verknüpft.
#############################################################
<@ if $arguments contain 'focal' or $arguments contain 'eoan' or $arguments contain 'bionic' or $arguments contain 'xenial' @>
Die Pakete können für 
<@ for $arg in $arguments @>
<@ if ['focal', 'eoan', 'bionic', 'xenial'] contains $arg @>
<@ if $arg == 'focal' @>[:Focal_Fossa: Ubuntu 20.04]<@ endif @>
<@ if $arg == 'eoan' @>[:Eoan_Ermine: Ubuntu 19.10]<@ endif @>
<@ if $arg == 'bionic' @>[:Bionic_Beaver: Ubuntu 18.04]<@ endif @>
<@ if $arg == 'xenial' @>[:Xenial_Xerus: Ubuntu 16.04]<@ endif @>
<@ if $loop.revindex > 2 @>, <@ endif @>
<@ if $loop.revindex == 2 @> und <@ endif @>
<@ endif @>
<@ endfor @>
 heruntergeladen werden.
<@ else @>
Die unterstützten Ubuntuversionen und Architekturen werden aufgelistet.
<@ endif @>
#############################################################
## Der Rest ist für alle Varianten gleich.
#############################################################
Nachdem man sie für die korrekte Ubuntuversion und Architektur geladen hat, müssen [:Paketinstallation_DEB: DEB-Pakete noch installiert werden].
[[Vorlage(Fremd, Paket)]]
