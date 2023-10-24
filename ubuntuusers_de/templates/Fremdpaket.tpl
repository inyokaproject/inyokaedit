## Macro=[[Vorlage(Fremdpaket, %%Projekthoster, Projektname|URL, Ubuntuversion(en)%%)]]

#############################################################
## Eingabemöglichkeiten:
##  mind. 2 Argumente
##  [[Vorlage(Fremdpaket, sourceforge, projektname, focal)]]
##  [[Vorlage(Fremdpaket, launchpad, projektname, focal, bionic)]]
##  [[Vorlage(Fremdpaket, "Anbieter", URLzurDL-Übersicht, focal, bionic)]]
##  mind. 3 Argumente
##  [[Vorlage(Fremdpaket, "Anbieter", dl, URLzumDL1, URLzumDL2, URLzumDL3, focal, bionic)]]
#############################################################
## Der folgende Block unterscheidet obige Eingabemöglichkeiten.
#############################################################
<@ if ['sourceforge','launchpad'] contains $arguments.0 @>
<@ if $arguments.0 == 'sourceforge' @>
Beim [wikipedia:SourceForge:]-Projekt [sourceforge:<@ $arguments.1 @>:] werden [sourceforge:<@ $arguments.1 @>/files/: DEB-Pakete] angeboten. 
<@ endif @>
<@ if $arguments.0 == 'launchpad' @>
Beim [:Launchpad:]-Projekt [launchpad:<@ $arguments.1 @>:] werden [launchpad:<@ $arguments.1 @>/+download: DEB-Pakete] angeboten. 
<@ endif @>
<@ else @>
Von <@ $arguments.0 @> werden 
<@ if $arguments.1 == 'dl' @>
folgende DEB-Pakete angeboten:
<@ for $arg in $arguments @>
<@ if $loop.index < 3 or ['jammy', 'focal', 'lunar', 'mantic'] contains $arg @>
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
<@ if $arguments contain 'lunar' or $arguments contain 'jammy' or $arguments contain 'mantic' or $arguments contain 'focal' @>
Die Pakete können für 
<@ for $arg in $arguments @>
<@ if ['lunar', 'jammy', 'mantic', 'focal'] contains $arg @>
<@ if $arg == 'lunar' @>[:Lunar_Lobster: Ubuntu 23.04]<@ endif @>
<@ if $arg == 'jammy' @>[:Jammy_Jellyfish: Ubuntu 22.04]<@ endif @>
<@ if $arg == 'mantic' @>[:Mantic_Minotaur: Ubuntu 23.10]<@ endif @>
<@ if $arg == 'focal' @>[:Focal_Fossa: Ubuntu 20.04]<@ endif @>
<@ if $loop.revindex > 2 @>, <@ endif @>
<@ if $loop.revindex == 2 @> und <@ endif @>
<@ endif @>
<@ endfor @>
 heruntergeladen werden.
<@ else @>
Die unterstützten Ubuntu-Versionen und Architekturen werden aufgelistet.
<@ endif @>
#############################################################
## Der Rest ist für alle Varianten gleich.
#############################################################
Nachdem man sie für die korrekte Ubuntu-Version und Architektur geladen hat, müssen die [:Paketinstallation_DEB: DEB-Pakete noch installiert werden].
[[Vorlage(Fremd, Paket)]]
