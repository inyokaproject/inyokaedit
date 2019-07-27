## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]
{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if ['general', 'disco', 'bionic', 'xenial'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @>
<@ if $arguments as lowercase contain 'disco' @>
  * [:Disco_Dingo:Ubuntu 19.04] Disco Dingo
<@ endif @><@ if $arguments as lowercase contain 'bionic' @>
  * [:Bionic_Beaver:Ubuntu 18.04] Bionic  Beaver
<@ endif @><@ if $arguments as lowercase contain 'xenial' @>
  * [:Xenial_Xerus:Ubuntu 16.04] Xenial Xerus
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte hilf mit, indem du diesen Artikel testest. [:Wiki/FAQ_-_häufig_gestellte_Fragen/#Wikiartikel-testen:].
#tag: ungetestet
<@ endif @>
|}}
<@ if ['general', 'disco', 'bionic', 'xenial'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) or ($arguments.4 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: general
<@ else @>
<@ if $arguments as lowercase contain 'disco' @>
#getestet: disco
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
