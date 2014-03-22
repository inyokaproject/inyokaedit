## Macro=[[Vorlage(Tasten, %%TASTE%%)]]
<@ for $key in ($arguments join_with ' ') split_by "+" @>
<@ if $loop.first @><@ else @> + <@ endif @>
<@ if ['lmb','lmt','linke Maustaste'] contains ($key as stripped) @>[[Bild(Wiki/Vorlagen/Tasten/mouse_left.png, alt="linke Maustaste")]]
<@ elseif ['mmb','mmt','mittlere Maustaste'] contains ($key as stripped) @>[[Bild(Wiki/Vorlagen/Tasten/mouse_midd.png, alt="mittlere Maustaste")]]
<@ elseif ['rmb','rmt','rechte Maustaste'] contains ($key as stripped) @>[[Bild(Wiki/Vorlagen/Tasten/mouse_right.png, alt="rechte Maustaste")]]
<@ elseif ['pfeiltasten'] contains ($key as stripped) as lowercase @>[[Bild(Wiki/Icons/pfeiltasten.png, 66, alt="Pfeiltasten")]]
<@ elseif ['wasd'] contains ($key as stripped) as lowercase @>[[Bild(Wiki/Icons/tasten_wasd.png, 66, alt="W A S D Tasten")]]
<@ elseif true @>
[[SPAN("<@ if ($key as stripped) matches_regex '^[a-zA-Z0-9äüö]{1}$' @><@ ($key as stripped) as uppercase @>
<@ elseif ($key as stripped) as lowercase matches_regex '^f([1-9]|1[0-6])$' @><@ ($key as stripped) as uppercase @>
<@ elseif ['hash','raute','rautenzeichen','#'] contains ($key as stripped) as lowercase @>#
<@ elseif ['^', '.', '>', '<', ',', 'alt', 'fn'] contains ($key as stripped) as lowercase @><@ ($key as stripped) as title @>
<@ elseif ['pause','break'] contains ($key as stripped) as lowercase @>Pause
<@ elseif (($key as stripped) as lowercase) == 'plus' @>+
<@ elseif ['*'] contains ($key as stripped) as lowercase @>*
<@ elseif ['ende','end'] contains ($key as stripped) as lowercase @>Ende
<@ elseif ['num','num-taste','num-lock-taste'] contains ($key as stripped) as lowercase @>⇩ num
<@ elseif ['rollen','bildlauf'] contains ($key as stripped) as lowercase @>⇩ Rollen
<@ elseif ['menue','menue-taste'] contains ($key as stripped) as lowercase @>Menue
<@ elseif ['pos1','pos 1','home'] contains ($key as stripped) as lowercase @>Pos1
<@ elseif ['return','enter','eingabe'] contains ($key as stripped) as lowercase @>⏎
<@ elseif ['at','@'] contains ($key as stripped) as lowercase @>@
<@ elseif ['space','leerschritt','leerzeichen','leer','leertaste'] contains ($key as stripped) as lowercase @>        
<@ elseif ['up','hoch','rauf','pfeil hoch','pfeil-hoch','auf'] contains ($key as stripped) as lowercase @>↑
<@ elseif ['backspace','löschen','rückschritt'] contains ($key as stripped) as lowercase @>⌫
<@ elseif ['down','runter','pfeil-runter','pfeil runter','pfeil-ab','pfeil ab','ab'] contains ($key as stripped) as lowercase @>↓
<@ elseif ['left','links','pfeil links','pfeil-links'] contains ($key as stripped) as lowercase @>←
<@ elseif ['right','rechts','pfeil rechts','pfeil-rechts'] contains ($key as stripped) as lowercase @>→
<@ elseif ['bild auf','bild-auf','bild-rauf','bild rauf'] contains ($key as stripped) as lowercase @>Bild ↑
<@ elseif ['bild ab','bild-ab','bild-runter','bild runter'] contains ($key as stripped) as lowercase @>Bild ↓
<@ elseif ['strg','ctrl','steuerung'] contains ($key as stripped) as lowercase @>Strg
<@ elseif ['rstrg','rctrl','rsteuerung'] contains ($key as stripped) as lowercase @>RStrg
<@ elseif ['alt gr','altgr'] contains ($key as stripped) as lowercase @>Alt Gr
<@ elseif ['umschalt','umsch','shift'] contains ($key as stripped) as lowercase @>⇧
<@ elseif ['feststell','feststelltaste','groß','caps'] contains ($key as stripped) as lowercase @>⇩
<@ elseif ['entf','delete','entfernen','del'] contains ($key as stripped) as lowercase @>Entf
<@ elseif ['win','windows'] contains ($key as stripped) as lowercase @>Windows
<@ elseif ['ubuntu'] contains ($key as stripped) as lowercase @>Ubuntu
<@ elseif ['super'] contains ($key as stripped) as lowercase @>Super
<@ elseif ['tux'] contains ($key as stripped) as lowercase @>Tux
<@ elseif ['any', 'anykey'] contains ($key as stripped) as lowercase @>ANY
<@ elseif ['panic'] contains ($key as stripped) as lowercase @>PANIC
<@ elseif ['koelsch','kölsch'] contains ($key as stripped) as lowercase @>Kölsch
<@ elseif ['keyman','cherry'] contains ($key as stripped) as lowercase @>Keym@n
<@ elseif ['tab','tabulator'] contains ($key as stripped) as lowercase @>Tab ⇆
<@ elseif ['esc','escape'] contains ($key as stripped) as lowercase @>Esc
<@ elseif ['druck','prtsc','print','sysrq'] contains ($key as stripped) as lowercase @>Druck
<@ elseif ['minus','-'] contains ($key as stripped) as lowercase @>-
<@ elseif ['apple','mac','apfel','command','cmd'] contains ($key as stripped) as lowercase @>⌘
<@ elseif ['wahl','alternate','optionen'] contains ($key as stripped) as lowercase @>⌥
<@ elseif ['einfg','ins','einfügen','insert'] contains ($key as stripped) as lowercase @>Einfg
<@ elseif ['ß','sz'] contains ($key as stripped) as lowercase @>ß
<@ elseif ['`','\'','grave','gravis'] contains ($key as stripped) @>`
<@ elseif ['[','eckig_auf','squared open','squared-open'] contains ($key as stripped) @>[
<@ elseif [']','eckig_zu','squared close','squared-close'] contains ($key as stripped) @>]
<@ elseif ['{','geschweift_auf','curly open','curly-open'] contains ($key as stripped) @>{
<@ elseif ['}','geschweift_zu','curly close','curly-close'] contains ($key as stripped) @>}
<@ elseif ['gleich','equal','='] contains ($key as stripped) @>=
<@ elseif ['schrägstrich','slash','/'] contains ($key as stripped) as lowercase @>/
<@ elseif ['rückstrich','backslash', '\\'] contains ($key as stripped) @>\\
<@ elseif ['?','fragezeichen','question mark','question-mark'] contains ($key as stripped) as lowercase@>?
<@ elseif ['shift2','umschalt2','umsch2'] contains ($key as stripped) as lowercase @>⇧   
<@ elseif ['akut','acute','´'] contains ($key as stripped) as lowercase @>´
<@ elseif [':','doppelpunkt'] contains ($key as stripped) as lowercase @>:
<@ elseif ['!','ausrufezeichen'] contains ($key as stripped) as lowercase @>!
<@ elseif ['%','prozent'] contains ($key as stripped) as lowercase @>%
<@ elseif ['&','und'] contains ($key as stripped) as lowercase @>&
<@ elseif ['compose'] contains ($key as stripped) as lowercase @>⎄
<@ elseif ['anfuehrungsstriche','doublequote'] contains ($key as stripped) as lowercase @>\"
<@ endif @>"class_='key')]] 
<@ endif @>
<@ endfor @>
