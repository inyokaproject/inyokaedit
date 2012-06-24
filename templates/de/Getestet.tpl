## Macro=[[Vorlage(Getestet, %%Ubuntuversion%%)]]
{{|<title="Dieser Artikel wurde für die folgenden
Ubuntu-Versionen getestet:" class="box tested_for">
<@ if $arguments.0 @>
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel ist größtenteils für alle Ubuntu-Versionen gültig.
<@ else @><@ if $arguments as lowercase contain 'precise' @>
  * [:Precise_Pangolin:Ubuntu Precise Pangolin 12.04]
<@ endif @><@ if $arguments as lowercase contain 'oneiric' @>
  * [:Oneiric_Ocelot:Ubuntu Oneiric Ocelot 11.10]
<@ endif @><@ if $arguments as lowercase contain 'natty' @>
  * [:Natty_Narwhal:Ubuntu Natty Narwhal 11.04]
<@ endif @><@ if $arguments as lowercase contain 'maverick' @>
  * [:Maverick_Meerkat:Ubuntu Maverick Meerkat 10.10]
<@ endif @><@ if $arguments as lowercase contain 'lucid' @>
  * [:Lucid Lynx:Ubuntu Lucid Lynx 10.04]
<@ endif @><@ if $arguments as lowercase contain 'karmic' @>
  * [:Karmic_Koala:Ubuntu Karmic Koala 9.10]
<@ endif @><@ if $arguments as lowercase contain 'jaunty' @>
  * [:Jaunty_Jackalope:Ubuntu Jaunty Jackalope 9.04]
<@ endif @><@ if $arguments as lowercase contain 'intrepid' @>
  * [:Intrepid_Ibex:Ubuntu Intrepid Ibex 8.10]
<@ endif @><@ if $arguments as lowercase contain 'hardy' @>
  * [:Hardy_Heron:Ubuntu Hardy Heron 8.04]
<@ endif @><@ if $arguments as lowercase contain 'gutsy' @>
  * [:Gutsy_Gibbon:Ubuntu Gutsy Gibbon 7.10]
<@ endif @><@ if $arguments as lowercase contain 'feisty' @>
  * [:Feisty_Fawn:Ubuntu Feisty Fawn 7.04]
<@ endif @><@ if $arguments as lowercase contain 'edgy' @>
  * [:Edgy_Eft:Ubuntu Edgy Eft 6.10]
<@ endif @><@ if $arguments as lowercase contain 'dapper' @>
  * [:Dapper_Drake:Ubuntu Dapper Drake 6.06]
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu-Version getestet! Bitte diesen Artikel testen und das getestet-Tag entsprechend anpassen.
#tag: ungetestet
<@ endif @>
|}}
