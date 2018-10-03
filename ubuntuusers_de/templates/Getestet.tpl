## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]
{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if ['general', 'cosmic', 'bionic', 'xenial', 'trusty' ] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @>
<@ if $arguments as lowercase contain 'cosmic' @>
  * [:Cosmic_Cuttlefish:Ubuntu 18.10] Cosmic Cuttlefish
<@ endif @><@ if $arguments as lowercase contain 'bionic' @>
  * [:Bionic_Beaver:Ubuntu 18.04] Bionic  Beaver
<@ endif @><@ if $arguments as lowercase contain 'xenial' @>
  * [:Xenial_Xerus:Ubuntu 16.04] Xenial Xerus
<@ endif @><@ if $arguments as lowercase contain 'trusty' @>
  * [:Trusty_Tahr:Ubuntu 14.04] Trusty Tahr
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte diesen Artikel testen und das Getestet-Tag entsprechend anpassen.
#tag: ungetestet
<@ endif @>
|}}
<@ if ['general', 'cosmic', 'bionic', 'xenial', 'trusty'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: general
<@ else @>
<@ if $arguments as lowercase contain 'cosmic' @>
#getestet: cosmic
<@ endif @>
<@ if $arguments as lowercase contain 'bionic' @>
#getestet: bionic
<@ endif @>
<@ if $arguments as lowercase contain 'xenial' @>
#getestet: xenial
<@ endif @><@ if $arguments as lowercase contain 'trusty' @>
#getestet: trusty
<@ endif @>
<@ endif @>
<@ else @>
#getestet: ungetestet
<@ endif @>
