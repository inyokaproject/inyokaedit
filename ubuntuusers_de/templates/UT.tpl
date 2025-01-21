## Macro=[[Vorlage(UT, %%Ubuntuversion%%)]]

{{|<title="Dieser Artikel bezieht sich auf Ubuntu Touch, die Variante von Ubuntu für mobile Geräte mit Touchscreen." class="box tested_for">
<@ if ['general', 'xenial', 'focal'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase)@> 
<@ if $arguments as lowercase contain 'general' @>
Dieser Artikel gilt für alle Versionen von Ubuntu Touch.
<@ else @>
Getestet wurde der Artikel für folgende Version(en):
<@ if $arguments as lowercase contain 'xenial' @>
  * [:Xenial_Xerus:Ubuntu Touch], Basis 16.04 Xenial Xerus
<@ endif @><@ if $arguments as lowercase contain 'focal' @>
 * [:Focal_Fossa:Ubuntu Touch], Basis 20.04 Focal Fossa
<@ endif @>
<@ endif @>
<@ else @>
Dieser Artikel ist mit keiner derzeit unterstützten Ubuntu-Touch-Version getestet! Bitte diesen Artikel testen!
<@ endif @>
|}}
<@ if ['general', 'xenial', 'focal'] contains ($arguments.0 as lowercase) or ($arguments.1 as lowercase) or ($arguments.2 as lowercase) or ($arguments.3 as lowercase) @> 
<@ if $arguments as lowercase contain 'general' @>
#getestet: generalut
<@ else @>
<@ if $arguments as lowercase contain 'xenial' @>
#getestet: xenialut
<@ else @>
<@ endif @><@ if $arguments as lowercase contain 'focal' @>
#getestet: focalut
<@ endif @>
<@ endif @>
<@ else @>
#getestet: ungetestetut
<@ endif @>
