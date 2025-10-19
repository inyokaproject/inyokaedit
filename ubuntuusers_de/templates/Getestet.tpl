## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]

{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if ['general', 'plucky', 'questing', 'noble', 'jammy'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @>
<@ if $arguments as lowercase contain 'plucky' @>
  * [:Plucky Puffin:Ubuntu 25.04] Plucky Puffin
<@ endif @>
<@ if $arguments as lowercase contain 'questing' @>
  * [:Questing Quokka:Ubuntu 25.10] Questing Quokka
<@ endif @>
<@ if $arguments as lowercase contain 'noble' @>
  * [:Noble Numbat:Ubuntu 24.04] Noble Numbat
<@ endif @>
<@ if $arguments as lowercase contain 'jammy' @>
  * [:Jammy_Jellyfish:Ubuntu 22.04] Jammy Jellyfish
<@ endif @>
----
~-(Du möchtest den Artikel für eine weitere Ubuntu-Version testen? Mitarbeit im Wiki ist immer willkommen! Dazu sind die Hinweise [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:zum Testen von Artikeln] zu beachten.)-~
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte teste diesen Artikel für eine Ubuntu-Version, welche aktuell unterstützt wird. Dazu sind die Hinweise [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:zum Testen von Artikeln] zu beachten.
#tag: ungetestet
<@ endif @>
|}}
<@ if ['general', 'plucky', 'questing', 'noble', 'jammy'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: general
<@ else @>
<@ if $arguments as lowercase contain 'plucky' @>
#getestet: plucky
<@ endif @>
<@ if $arguments as lowercase contain 'questing' @>
#getestet: questing
<@ endif @>
<@ if $arguments as lowercase contain 'noble' @>
#getestet: noble
<@ endif @>
<@ if $arguments as lowercase contain 'jammy' @>
#getestet: jammy
<@ endif @>
<@ endif @>
<@ else @>
#getestet: ungetestet
<@ endif @>
