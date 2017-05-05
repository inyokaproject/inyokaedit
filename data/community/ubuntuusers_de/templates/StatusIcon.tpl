## Macro=[[Vorlage(StatusIcon, ja | nein | bedingt | alpha | buggy | manuell | plugin | unbekannt)]]

<@ for $arg in $arguments @>
<@ if $loop.last @>
<@ if $arg contains 'ja' @>[[Bild(Wiki/Vorlagen/StatusIcon/ja.png, alt="Voll unterstützt", title="Voll unterstützt",)]]
<@ elseif $arg contains 'nein' @>[[Bild(Wiki/Vorlagen/StatusIcon/nein.png, alt="Nicht unterstützt", title="Nicht unterstützt",)]]
<@ elseif $arg contains 'bedingt' @>[[Bild(Wiki/Vorlagen/StatusIcon/bedingt.png, alt="Bedingt unterstützt", title="Bedingt unterstützt",)]]
<@ elseif $arg contains 'alpha' @>[[Bild(Wiki/Vorlagen/StatusIcon/alpha.png, alt="In Entwicklung", title="In Entwicklung",)]]
<@ elseif $arg contains 'buggy' @>[[Bild(Wiki/Vorlagen/StatusIcon/buggy.png, alt="Fehlerhaft", title="Fehlerhaft",)]]
<@ elseif $arg contains 'manuell' @>[[Bild(Wiki/Vorlagen/StatusIcon/manuell.png, alt="Manuell installierbar", title="Manuell installierbar",)]]
<@ elseif $arg contains 'plugin' @>[[Bild(Wiki/Vorlagen/StatusIcon/plugin.png, alt="Plugin", title="Plugin",)]]
<@ elseif $arg contains 'unbekannt' @>[[Bild(Wiki/Vorlagen/StatusIcon/unbekannt.png, alt="Status unbekannt", title="Status unbekannt",)]]
<@ endif @>
<@ else @><@ $arg @> <@ endif @>
<@ endfor @>
