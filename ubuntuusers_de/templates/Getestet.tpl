## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]

{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if ['general', 'lunar', 'jammy', 'mantic', 'focal'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @>
<@ if $arguments as lowercase contain 'mantic' @>
  * [:Mantic_Minotaur:Ubuntu 23.10] Mantic Minotaur
<@ endif @>
<@ if $arguments as lowercase contain 'lunar' @>
  * [:Lunar_Lobster:Ubuntu 23.04] Lunar Lobster
<@ endif @>
<@ if $arguments as lowercase contain 'jammy' @>
  * [:Jammy_Jellyfish:Ubuntu 22.04] Jammy Jellyfish
<@ endif @>
<@ if $arguments as lowercase contain 'focal' @>
  * [:Focal_Fossa:Ubuntu 20.04] Focal Fossa
<@ endif @>
----
~-(Du möchtest den Artikel für eine weitere Ubuntu-Version testen? Mitarbeit im Wiki ist immer willkommen! Dazu sind die Hinweise [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:zum Testen von Artikeln] zu beachten.)-~
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte teste diesen Artikel für eine Ubuntu-Version, welche aktuell unterstützt wird. Dazu sind die Hinweise [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:zum Testen von Artikeln] zu beachten.
#tag: ungetestet
<@ endif @>
|}}
<@ if ['general', 'lunar', 'jammy', 'mantic', 'focal'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: general
<@ else @>
<@ if $arguments as lowercase contain 'lunar' @>
#getestet: lunar
<@ endif @>
<@ if $arguments as lowercase contain 'jammy' @>
#getestet: jammy
<@ endif @>
<@ if $arguments as lowercase contain 'mantic' @>
#getestet: mantic
<@ endif @>
<@ if $arguments as lowercase contain 'focal' @>
#getestet: focal
<@ endif @>
<@ endif @>
<@ else @>
#getestet: ungetestet
<@ endif @>
