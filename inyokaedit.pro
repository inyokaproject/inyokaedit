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
                 plugins/spellchecker \
                 plugins/tabletemplate \
                 application

unix {
    data.path = /usr/share/inyokaedit
    data.files += plugins \
        application/iWikiLinks \
        application/templates \
        GetInyokaStyles \
        GetIWLs \
        ExtendedDict.txt
    lang.path = /usr/share/inyokaedit/lang
    lang.files += application/lang/*.qm
    desktop.path = /usr/share/applications
    desktop.files += inyokaedit.desktop
    pixmap.path = /usr/share/pixmaps
    pixmap.files += application/res/images/inyokaedit_64x64.png
    man.path = /usr/share
    man.files += man
    mime.path = /usr/share/mime/packages
    mime.files += inyokaedit.xml
    target.path = /usr/bin
    INSTALLS += data \
        lang \
        desktop \
        pixmap \
        man \
        mime \
        target
}
