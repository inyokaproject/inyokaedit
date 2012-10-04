## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]
{{|<title="Dieser Artikel wurde für die folgenden Ubuntu-Versionen getestet:" class="box tested_for">
<@ if $arguments.0 @>
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @><@ if $arguments as lowercase contain 'quantal' @>
  * [:Quantal_Quetzal:Ubuntu 12.10] „Quantal Quetzal“
<@ endif @><@ if $arguments as lowercase contain 'precise' @>
  * [:Precise_Pangolin:Ubuntu 12.04] „Precise Pangolin“
<@ endif @><@ if $arguments as lowercase contain 'oneiric' @>
  * [:Oneiric_Ocelot:Ubuntu 11.10] „Oneiric Ocelot“
<@ endif @><@ if $arguments as lowercase contain 'natty' @>
  * [:Natty_Narwhal:Ubuntu 11.04] „Natty Narwhal“
<@ endif @><@ if $arguments as lowercase contain 'maverick' @>
  * [:Maverick_Meerkat:Ubuntu 10.10] „Maverick Meerkat“
<@ endif @><@ if $arguments as lowercase contain 'lucid' @>
  * [:Lucid Lynx:Ubuntu 10.04] „Lucid Lynx“
<@ endif @><@ if $arguments as lowercase contain 'karmic' @>
  * [:Karmic_Koala:Ubuntu 9.10] „Karmic Koala“
<@ endif @><@ if $arguments as lowercase contain 'jaunty' @>
  * [:Jaunty_Jackalope:Ubuntu 9.04] „Jaunty Jackalope“
<@ endif @><@ if $arguments as lowercase contain 'hardy' @>
  * [:Hardy_Heron:Ubuntu 8.04] „Hardy Heron“
<@ endif @><@ if $arguments as lowercase contain 'dapper' @>
  * [:Dapper_Drake:Ubuntu 6.06] „Dapper Drake“
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte diesen Artikel testen und das getestet-Tag entsprechend anpassen.
#tag: ungetestet
<@ endif @>
|}}
