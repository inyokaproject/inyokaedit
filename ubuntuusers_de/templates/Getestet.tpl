## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]

{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if ['general', 'hirsute', 'groovy', 'focal', 'bionic', 'xenial'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @>
<@ if $arguments as lowercase contain 'groovy' @>
  * [:Hirsute_Hippo:Ubuntu 21.04] Hirsute Hippo
<@ endif @>
<@ if $arguments as lowercase contain 'groovy' @>
  * [:Groovy_Gorilla:Ubuntu 20.10] Groovy Gorilla
<@ endif @>
<@ if $arguments as lowercase contain 'focal' @>
  * [:Focal_Fossa:Ubuntu 20.04] Focal Fossa
<@ endif @>
<@ if $arguments as lowercase contain 'bionic' @>
  * [:Bionic_Beaver:Ubuntu 18.04] Bionic  Beaver
<@ endif @>
<@ if $arguments as lowercase contain 'xenial' @>
  * [:Xenial_Xerus:Ubuntu 16.04] Xenial Xerus
<@ endif @>
----
~-(Du möchtest den Artikel für eine weitere Ubuntu-Version testen? Mitarbeit im Wiki ist immer willkommen! Dazu sind die Hinweise [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:zum Testen von Artikeln] zu beachten.)-~
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte teste diesen Artikel für eine Ubuntu-Version, welche aktuell unterstützt wird. Dazu sind die Hinweise [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:zum Testen von Artikeln] zu beachten.
#tag: ungetestet
<@ endif @>
|}}
<@ if ['general', 'hirsute', 'groovy', 'focal', 'bionic', 'xenial'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: general
<@ else @>
<@ if $arguments as lowercase contain 'hirsute' @>
#getestet: hirsute
<@ endif @>
<@ if $arguments as lowercase contain 'groovy' @>
#getestet: groovy
<@ endif @>
<@ if $arguments as lowercase contain 'focal' @>
#getestet: focal
<@ endif @>
<@ if $arguments as lowercase contain 'bionic' @>
#getestet: bionic
<@ endif @>
<@ if $arguments as lowercase contain 'xenial' @>
#getestet: xenial
<@ endif @>
<@ endif @>
<@ else @>
#getestet: ungetestet
<@ endif @>
