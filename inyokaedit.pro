#  This file is part of InyokaEdit.
#  Copyright (C) 2011-2020 The InyokaEdit developers
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

lessThan(QT_MAJOR_VERSION, 5) {
  error("InyokaEdit requires Qt 5.4 or greater")
}
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 4) {
  error("InyokaEdit requires Qt 5.4 or greater")
}

TEMPLATE = subdirs
CONFIG  += ordered
SUBDIRS  = plugins \
           application
