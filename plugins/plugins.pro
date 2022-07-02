#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2022 The InyokaEdit developers
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
#  along with InyokaEdit.  If not, see <https://www.gnu.org/licenses/>.

TEMPLATE = subdirs
CONFIG  += ordered
SUBDIRS  = spellchecker-hunspell \
#          spellchecker-nuspell \
           uu_tabletemplate \
           uu_knowledgebox \
           highlighter \
           hotkey
