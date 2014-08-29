#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2014 The InyokaEdit developers
#
#  InyokaEdit is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  InyokaEdit is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE       = subdirs
CONFIG        += ordered
SUBDIRS        = libs/templates \
                 libs/parser \
                 plugins/spellchecker \
                 plugins/tabletemplate \
                 plugins/knowledgebox \
                 application

unix {
    data.path      = /usr/share/inyokaedit
    data.files    += data/GetInyokaStyles \
                     data/GetIWLs \
                     data/ExtendedDict.txt
    desktop.path   = /usr/share/applications
    desktop.files += data/inyokaedit.desktop
    pixmap.path    = /usr/share/pixmaps
    pixmap.files  += application/res/images/inyokaedit_64x64.png \
                     application/res/images/inyokaedit.xpm
    icons.path     = /usr/share/icons
    icons.files   += application/res/images/hicolor
    man.path       = /usr/share
    man.files     += man
    mime.path      = /usr/share/mime/packages
    mime.files    += data/inyokaedit.xml
    INSTALLS      += data \
                     desktop \
                     pixmap \
                     man \
                     mime
}
