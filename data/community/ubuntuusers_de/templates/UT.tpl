## Macro=[[Vorlage(UT, %%Ubuntuversion%%)]]
{{|<title="Dieser Artikel bezieht sich auf Ubuntu Touch, der Variante von Ubuntu für mobile Geräte mit Touchscreen." class="box ut_box">
<@ if ['general', 'vivid', 'utopic'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel gilt für alle Ubuntu Touch Versionen.
<@ else @>
Getestet wurde der Artikel für folgende Version(en):
<@ if $arguments as lowercase contain 'vivid' @>
  * [:Vivid_Vervet:Ubuntu Touch 15.04] Vivid Vervet
<@ endif @><@ if $arguments as lowercase contain 'utopic' @>
  * [:Utopic_Unicorn:Ubuntu Touch 14.10] Utopic Unicorn
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner aktuell unterstützten Ubuntu Touch Version getestet! Bitte diesen Artikel testen!
<@ endif @>
|}}

