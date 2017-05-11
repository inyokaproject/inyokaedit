## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]
{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if ['general', 'zesty', 'yakkety', 'xenial', 'trusty' ] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @><@ if $arguments as lowercase contain 'zesty' @>
  * [:Zesty Zapus:Ubuntu 17.04] Zesty Zapus
<@ endif@><@ if $arguments as lowercase contain 'yakkety' @>
  * [:Yakkety_Yak:Ubuntu 16.10] Yakkety Yak
<@ endif @><@ if $arguments as lowercase contain 'xenial' @>
  * [:Xenial_Xerus:Ubuntu 16.04] Xenial Xerus
<@ endif @><@ if $arguments as lowercase contain 'trusty' @>
  * [:Trusty_Tahr:Ubuntu 14.04] Trusty Tahr
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte diesen Artikel testen und das getestet-Tag entsprechend anpassen.
#tag: ungetestet
<@ endif @>
|}}
<@ if ['general', 'zesty', 'yakkety', 'xenial', 'trusty', 'precise'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: general
<@ else @>
<@ if $arguments as lowercase contain 'zesty' @>
#getestet: zesty
<@ endif @><@ if $arguments as lowercase contain 'yakkety' @>
#getestet: yakkety
<@ endif @><@ if $arguments as lowercase contain 'xenial' @>
#getestet: xenial
<@ endif @><@ if $arguments as lowercase contain 'trusty' @>
#getestet: trusty
<@ endif @><@ if $arguments as lowercase contain 'precise' @>
#getestet: precise
<@ endif @>
<@ endif @>
<@ else @>
#getestet: ungetestet
<@ endif @>
