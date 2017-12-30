# InyokaEdit
InyokaEdit is a markup editor for Inyoka articles.

Features:
* Download raw text of Inyoka article (including pictures)
* Upload article into Inyoka wiki
* Integrated article preview
* Tab support for editing multiple articles in parallel
* Code completion for templates
* Simple Inyoka syntax check (parenthesis, known templates)
* All Inyoka templates and InterWiki links available via menu entries
* Save article together with images in one file
* Plug-ins:
  * Syntax highlighting of Inyoka elements
  * Spell checker (based on Hunspell)
  * Hotkyes for own text blocks
  * Knowledge block dialog (ubuntuusers.de)
  * Table generator / converter (ubuntuusers.de)

## Installation
PPA for Ubuntu:
```
sudo add-apt-repository ppa:elthoro/inyokaedit
sudo apt-get update
sudo apt-get install inyokaedit inyokaedit-data-ubuntuusersde inyokaedit-plugins
```

## Compiling
* InyokaEdit can be compiled with Qt 5 (including webkitwidgets or webenginewidgets)
* For compiling spell checker plugin, libhunspell-dev is needed.
