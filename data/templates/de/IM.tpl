## Macro={{{#!vorlage IM, "Überschrift"\n+++\nIcon\nArtikel\nBeschreibung\nProtokolle\nMerkmal ja/nein/...\n}}}

## Vorlage für die Übersicht der Instant Messenger, basierend auf der Vorlage Projekte
<@ for $arg in $arguments split_by '+++
' @>
<@ if $loop.first @>
||<tablestyle="width:96%; background-color:#f2f2f2;" -2 rowclass="titel"><@ $arguments.0 @>||
||<rowclass="kopf">Programm||Beschreibung||
<@ else @>
<@ for $attr in $arg split_by '
' @>
<@ if $loop.index == 1 @>
||<cellstyle="font-weight:bold;" :>
<@ if $attr == "" @>
<@ else @>[:<@ $attr @>:[[Bild(<@ $attr @>.png)]]][[BR]]
<@ endif @>
<@ elseif $loop.index == 2 @>[:<@ $attr @>:]
<@ elseif $loop.index == 3 @>||<cellstyle="padding-top:0.8em;"> <@ $attr @>
<@ elseif $loop.index == 4 @>{{|<style="color:#333333;">~-(
<@ for $atom in $attr split_by ', ' @>
<@ if ['aim', 'bonjour', 'gadu-gadu', 'icq', 'sametime', 'irc', 'meanwhile', 'mrim', 'msn', 'mxit', 'myspace', 'netsoul', 'novell', 'qq', 'rss', 'silc', 'simple', 'skype', 'sms', 'twitter', 'vkontakte','winpopup', 'yahoo', 'xmpp', 'zephyr'] contains $atom @> 
<@ if $atom as lowercase contain 'aim' @>[[Bild(Wiki/Icons/Protokolle/aim-24.png, alt= "Aim", title="Aim")]]
<@ elseif $atom as lowercase contain 'bonjour' @>[:Avahi:[[Bild(Wiki/Icons/Protokolle/bonjour-24.png, alt= "Bonjour (Avahi, Zeroconf)", title="Bonjour (Avahi, Zeroconf)")]]]
<@ elseif $atom as lowercase contain 'gadu-gadu' @>[[Bild(Wiki/Icons/Protokolle/gadu-gadu-24.png, alt= "Gadu-Gadu", title="Gadu-Gadu")]]
<@ elseif $atom as lowercase contain 'icq' @>[[Bild(Wiki/Icons/Protokolle/icq-24.png, alt= "ICQ", title="ICQ")]]
<@ elseif $atom as lowercase contain 'sametime' @>[[Bild(Wiki/Icons/Protokolle/sametime-24.png, alt= "Sametime", title="Sametime")]]
<@ elseif $atom as lowercase contain 'irc' @>[:IRC:[[Bild(Wiki/Icons/Protokolle/irc-24.png, alt= "IRC", title="IRC")]]]
<@ elseif $atom as lowercase contain 'meanwhile' @>[[Bild(Wiki/Icons/Protokolle/meanwhile-24.png, alt= "Meanwhile", title="Meanwhile")]]
<@ elseif $atom as lowercase contain 'mrim' @>[[Bild(Wiki/Icons/Protokolle/mrim-24.png, alt= "mail.ru Agent", title="mail.ru Agent")]]
<@ elseif $atom as lowercase contain 'msn' @>[[Bild(Wiki/Icons/Protokolle/msn-24.png, alt= "MSN", title="MSN")]]
<@ elseif $atom as lowercase contain 'mxit' @>[[Bild(Wiki/Icons/Protokolle/mxit-24.png, alt= "MXit", title="MXit")]]
<@ elseif $atom as lowercase contain 'myspace' @>[[Bild(Wiki/Icons/Protokolle/myspace-24.png, alt= "MyspaceIM", title="MyspaceIM")]]
<@ elseif $atom as lowercase contain 'netsoul' @>[[Bild(Wiki/Icons/Protokolle/netsoul-24.png, alt= "Netsoul", title="Netsoul")]]
<@ elseif $atom as lowercase contain 'novell' @>[[Bild(Wiki/Icons/Protokolle/novell-24.png, alt= "Groupwise", title="Groupwise")]]
<@ elseif $atom as lowercase contain 'qq' @>[[Bild(Wiki/Icons/Protokolle/qq-24.png, alt= "QQ", title="QQ")]]
<@ elseif $atom as lowercase contain 'rss' @>[[Bild(Wiki/Icons/Protokolle/rss-24.png, alt= "RSS", title="RSS")]]
<@ elseif $atom as lowercase contain 'silc' @>[[Bild(Wiki/Icons/Protokolle/silc-24.png, alt= "SILC", title="SILC")]]
<@ elseif $atom as lowercase contain 'simple' @>[[Bild(Wiki/Icons/Protokolle/simple-24.png, alt= "Simple / SIP", title="Simple / SIP")]]
<@ elseif $atom as lowercase contain 'skype' @>[[Bild(Wiki/Icons/Protokolle/skype-24.png, alt= "Skype", title="Skype")]]
<@ elseif $atom as lowercase contain 'sms' @>[[Bild(Wiki/Icons/Protokolle/sms-24.png, alt= "SMS", title="SMS")]]
<@ elseif $atom as lowercase contain 'twitter' @>[[Bild(Wiki/Icons/Protokolle/twitter-24.png, alt= "Twitter", title="Twitter")]]
<@ elseif $atom as lowercase contain 'vkontakte' @>[[Bild(Wiki/Icons/Protokolle/vkontakte-24.png, alt= "vkontakte.ru", title="vkontakte.ru")]]
<@ elseif $atom as lowercase contain 'winpopup' @>[[Bild(Wiki/Icons/Protokolle/winpopup-24.png, alt= "WinPopUp", title="WinPopUp")]]
<@ elseif $atom as lowercase contain 'xmpp' @>[:XMPP:[[Bild(Wiki/Icons/Protokolle/xmpp-24.png, alt= "XMPP (Jabber)", title="XMPP (Jabber)")]]]
<@ elseif $atom as lowercase contain 'yahoo' @>[[Bild(Wiki/Icons/Protokolle/yahoo-24.png, alt= "YahooIM", title="YahooIM")]]
<@ elseif $atom as lowercase contain 'zephyr' @>[[Bild(Wiki/Icons/Protokolle/zephyr-24.png, alt= "Zephyr", title="Zephyr")]]
<@ endif @>
<@ else @>
<@ $atom @>
<@ endif @>
<@ endfor @>[[BR]]
<@ elseif $loop.index == 5 @>
<@ for $atom in $attr split_by ', ' @> [[Vorlage(StatusIcon, <@ $atom @>)]]
<@ if $loop.revindex > 1 @> | 
<@ endif @>
<@ endfor @>)-~ |}}
<@ elseif $loop.last @> ||
<@ endif @>
<@ endfor @>
<@ endif @>
<@ endfor @>
