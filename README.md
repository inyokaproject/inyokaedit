# InyokaEdit [![CodeQL](https://github.com/inyokaproject/inyokaedit/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/inyokaproject/inyokaedit/actions/workflows/codeql-analysis.yml)
InyokaEdit is a markup editor for [Inyoka](https://github.com/inyokaproject/inyoka) articles.

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
  * Spell checker (based on [Hunspell](https://hunspell.github.io/) or optionally [Nuspell](https://nuspell.github.io/))
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

AppImage, Flatpak and build for Windows: https://github.com/inyokaproject/inyokaedit/releases

Packages in Arch AUR: https://aur.archlinux.org/packages/?K=inyokaedit

## Build instructions
* InyokaEdit can be compiled with Qt >= 6.0 (optional, but recommended for integrated preview: Including webenginewidgets)
* For compiling spell checker plugin "spellchecker-hunspell" *libhunspell-dev* or for "spellchecker-nuspell" *libnuspell-dev* is needed.
* For running InyokaEdit, community files have to be included. Because of this, it is recommended to include the community files during **cmake install**. For this, the [community branch](https://github.com/inyokaproject/inyokaedit/tree/community) has to be included inside the main branch root folder.

### cmake
Adjust CMAKE_PREFIX_PATH according to your Qt installation. Optionally set `-DINY_PREVIEW=[none/useqtwebengine]` to disable preview / use Qt WebEngine.
```
cmake -B build-cmake -DCMAKE_PREFIX_PATH=/usr/include/qt6 -DINY_COMMUNITY=community/ubuntuusers_de
cmake --build build-cmake -- -j8  # Adjust -j8 according to your available cores.
sudo cmake --install build-cmake  # or DESTDIR=foobar cmake --install build-cmake
```
