# InyokaEdit
InyokaEdit is a markup editor for Inyoka articles.

Features:
* Download raw text of Inyoka article (including pictures)
* Upload article into Inyoka wiki
* Integrated article preview
* Tab support for editing multiple articles in parallel
* Simple code completion
* All Inyoka templates and InterWiki links available via menu entries
* Plug-ins:
  * Syntax highlighting of Inyoka elements
  * Spell checker (based on Hunspell)
  * Knowledge block dialog
  * Table generator / converter
  * Hotkyes for own text blocks

## Installation
PPA for Ubuntu:
```
sudo add-apt-repository ppa:elthoro/inyokaedit
sudo apt-get update
sudo apt-get install inyokaedit inyokaedit-plugins
```

## Compiling
* InyokaEdit can be compiled with Qt 4 (including webkit) or Qt 5 (< 5.6 webkitwidgets / >= 5.6 webenginewidgets)
* For compiling spell checker plugin: Hunspell
