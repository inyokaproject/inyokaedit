# InyokaEdit [![CodeQL](https://github.com/inyokaproject/inyokaedit/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/inyokaproject/inyokaedit/actions/workflows/codeql-analysis.yml)
InyokaEdit is a markup editor for Inyoka articles.

![Screenshot](https://user-images.githubusercontent.com/26674558/110246608-8df67e00-7f68-11eb-93b5-a6802b2f37a7.png)

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

AppImage and build for Windows: https://github.com/inyokaproject/inyokaedit/releases

Packages in Arch AUR: https://aur.archlinux.org/packages/?K=inyokaedit

## Compiling
* InyokaEdit can be compiled with Qt >= 5.9 (optional, but recommended: Including webkitwidgets or webenginewidgets)
* For compiling spell checker plugin, libhunspell-dev is needed.

### Manual installation
For executing **make install** successfully, one has to include the [community branch](https://github.com/inyokaproject/inyokaedit/tree/community) inside the master branch root folder.
