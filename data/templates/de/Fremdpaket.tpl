## Macro=[[Vorlage(Fremdpaket, %%Projekthoster, Projektname|URL, Ubuntuversion(en)%%)]]
#############################################################
## Eingabemöglichkeiten:
##  mind. 2 Argumente
##  [[Vorlage(Fremdpaket, sourceforge, projektname, trusty)]]
##  [[Vorlage(Fremdpaket, launchpad, projektname, trusty, precise)]]
##  [[Vorlage(Fremdpaket, googlecode, projektname, trusty, precise)]]
##  [[Vorlage(Fremdpaket, "Anbieter", URLzurDL-Übersicht, trusty, precise)]]
##  mind. 3 Argumente
##  [[Vorlage(Fremdpaket, "Anbieter", dl, URLzumDL1, URLzumDL2, URLzumDL3, trusty, saucy, precise)]]
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
<@ if $loop.index < 3 or ['trusty','saucy','raring','quantal','precise','oneiric','natty','lucid','hardy'] contains $arg @>
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
<@ if $arguments contain 'quantal' or $arguments contain 'precise' or $arguments contain 'oneiric' or $arguments contain 'natty' or $arguments contain 'lucid' or $arguments contain 'hardy' @>
Die Pakete können für 
<@ for $arg in $arguments @>
<@ if ['trusty','saucy','raring','quantal','precise','oneiric','natty','lucid','hardy'] contains $arg @>
<@ if $arg == 'trusty' @>[:Trusty_Tahr: Ubuntu 14.04]<@ endif @>
<@ if $arg == 'saucy' @>[:Saucy_Salamander: Ubuntu 13.10]<@ endif @>
<@ if $arg == 'raring' @>[:Raring_Ringtail: Ubuntu 13.04]<@ endif @>
<@ if $arg == 'quantal' @>[:Quantal_Quetzal: Ubuntu 12.10]<@ endif @>
<@ if $arg == 'precise' @>[:Precise_Pangolin: Ubuntu 12.04]<@ endif @>
<@ if $arg == 'oneiric' @>[:Oneiric_Ocelot: Ubuntu 11.10]<@ endif @>
<@ if $arg == 'natty' @>[:Natty_Narwhal: Ubuntu 11.04]<@ endif @>
<@ if $arg == 'lucid' @>[:Lucid Lynx: Ubuntu 10.04]<@ endif @>
<@ if $arg == 'hardy' @>[:Hardy_Heron: Ubuntu 8.04]<@ endif @>
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