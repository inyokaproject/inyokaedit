#!/bin/bash
if [ ! -d styles ]
  then
    echo "Creating folder \"styles\"..."
    mkdir styles
fi

cd styles

echo "Downloading css files..."
wget -nv -N http://static.cdn.ubuntu-de.org/style/markup.css
mv markup.css markup2.css
wget -nv -N http://static.cdn.ubuntu-de.org/style/wiki.css
wget -nv -N http://static.cdn.ubuntu-de.org/style/main-sprite.css
wget -nv -N http://ubuntuusers.de/markup.css

echo "Replace static paths..."
## Paths
_r1="http://static.cdn.ubuntu-de.org"
_r2="http://media.cdn.ubuntu-de.org/wiki/attachments/[0-9][0-9]/[0-9][0-9]"
_r3="../img/interwiki"
## Escape path for sed using bash find and replace
_r1="${_r1//\//\\/}"
_r2="${_r2//\//\\/}"
_r3="${_r3//\//\\/}"
## Replace paths
sed -e "s/${_r1}/../g" -i markup.css
sed -e "s/${_r2}/${_r3}/g" -i markup.css

cd ..

if [ ! -d img ]
  then
    echo "Creating folder \"img\"..."
    mkdir img
fi

cd img

echo "Downloading images..."
wget -nv -N http://static.cdn.ubuntu-de.org/img/anchor.png

if [ ! -d interwiki ]
  then
    echo "Creating folder \"interwiki\"..."
    mkdir interwiki
fi

cd interwiki

wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/16/askubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/18/bug.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/bug.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/16/canonical.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/09/debian.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/09/debian_de.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/18/ean.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/17/edubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/18/fb.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/09/freshmeat.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/18/getdeb.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/getdeb.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/googlecode.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/18/googlecode.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/44/18/holarse.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/18/identica.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/isbn.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/52/18/isbn.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/17/kubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/16/kubuntu-de.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/launchpad.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/18/launchpad.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/18/linuxgaming.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/lpuser.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/18/lpuser.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/09/osm.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/16/packages.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/09/playdeb.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/18/sourceforge.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/sourceforge.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/16/ticket.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/18/twitter.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/ubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/56/18/ubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/20/17/ubuntustudio.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/16/ubuntu_doc.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/09/ubuntu_fr.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/30/16/ubuntuone.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/09/wikibooks.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/user.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/30/09/wikimedia.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/18/wikipedia.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/search.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/wikipedia.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/interwiki/wikipedia_en.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/01/18/wikipedia_en.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/17/xubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/09/youtube.png

wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/43/ppa.png
cd ..

if [ ! -d flags ]
  then
    echo "Creating folder \"wiki\"..."
    mkdir wiki
fi
cd wiki

wget -nv -N http://static.cdn.ubuntu-de.org/img/wiki/toc.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/17/button.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/38/mouse_left.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/51/38/mouse_right.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/38/mouse_midd.png
wget -nv -N http://static.cdn.ubuntu-de.org/img/wiki/heading.png
cd ..

if [ ! -d flags ]
  then
    echo "Creating folder \"flags\"..."
    mkdir flags
fi

cd flags

wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/download.png
mv download.png dl.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/cz.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/40/eo.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/da.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/gb.png
cp gb.png en.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/fi.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/es.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/de.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/fr.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/hr.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/53/52/hu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/it.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/ja.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/ko.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/lv.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/pt.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/ro.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/pl.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/24/flag-nw.png
mv flag-nw.png nw.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/nl.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/rs.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/ru.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/sk.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/sv.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/tr.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/zh.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/at.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/ch.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/us.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/cl.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/15/overview.png

cd ..
cd ..

if [ ! -d Wiki ]
  then
    echo "Creating folder \"Wiki\"..."
    mkdir Wiki
fi

cd Wiki

if [ ! -d Icons ]
  then
    echo "Creating folder \"Wiki\Icons\"..."
    mkdir Icons
fi

cd Icons

# Linux
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/ubuntu_.png
mv ubuntu_.png ubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/kubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/28/edubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/28/gobuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/02/01/lubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/06/online-community.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/51/28/ubuntu64.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/54/28/xubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/28/ubuntustudio.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/01/28/kxubuntu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/05/idea-logo.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/20/28/gnome.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/gnu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/28/debian.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/28/redhat_.png
mv redhat_.png redhat.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/tux.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/28/suse.png

# Programme
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/53/28/abiword.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/aim.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/28/baobab.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/52/28/calc.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/28/ekiga.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/28/firefox.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/gnome-emacs.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/icq.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/msn.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/43/28/ooo_draw.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/43/28/ooo_math.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/44/28/ooo_writer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/synaptic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/34/28/thunderbird.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/vlc.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/28/wine2.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/28/xmms.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/28/pidgin.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/acroread.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/amule.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/bmp.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/28/dtp.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/file-manager.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/gimp.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/28/gnome-word.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/jabber.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/ooo_calc.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/43/28/ooo_impress.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/44/28/ooo_web.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/51/28/stickynotes.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/34/28/text-editor.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/28/vanity.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/wine.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/28/wine3.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/28/gajim.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/09/guayadeque.png

# Hardware
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/28/2monitors.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/computer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/games.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/28/hd-linux.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/ipod-mini-blue.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/28/joystick.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/28/mouse2.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/23/28/pci.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/28/printer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/scanner.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/28/tv2.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/28/CPU.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/17/keyboard.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/28/backup.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/28/harddrive.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/32/28/network_local.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/28/removable.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/camera.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/floppy.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/28/hd.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/28/hd-windows.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/ipod-photos.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/21/28/mouse.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/28/notebook.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/portablemp3.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/24/28/printer2.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/23/28/tv.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/usb.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/40/bluetooth.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/17/mouse3.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/download_manager.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/removable-usb.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/networksettings.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/removable-1394.png

# System
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/24/28/access.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/button_cancel.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/28/develop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/28/globe.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/gnome-reclevel.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/44/28/help.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/30/28/internet.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/multimedia.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/search2.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/19/28/security.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/sound.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/terminal.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/28/trash.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/advancedsettings.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/32/28/config.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/dvd.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/gnome-cd.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/gnome-warning.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/home.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/konsole2.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/optical.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/secure_non.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/settings.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/34/28/soundcontrol.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/volume.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/28/service.png

# Sonstige
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/ati.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/blackbox.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/27/28/burning.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/contact.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/email.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/19/28/improvements.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/movies.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/19/28/panel.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/28/rss.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/time.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/53/28/unknown.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/users.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/28/window.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/yahoo.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/windows_users.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/06/openstreetmap.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/28/wiki.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/save_all.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/28/pencil.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/package.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/32/28/looknfeel.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/28/hint.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/27/28/date.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/community-help.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/23/28/box.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/background.png

if [ ! -d Tango ]
  then
    echo "Creating folder \"Wiki\Icons\Tango\"..."
    mkdir Tango
fi

cd Tango

# Tango - Dokument
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/accessories-text-editor.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/document-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/document-print.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/document-save.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/edit-copy.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/edit-delete.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/edit-paste.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/folder-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/28/folder-remote.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/28/user-home.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/08/contact-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/bookmark-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/document-open.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/document-save-as.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/edit-clear.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/edit-cut.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/edit-find.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/edit-select-all.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/folder.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/28/folder-saved-search.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/28/document-properties.png

# Tango - Dateityp
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/audio-x-generic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/text-x-generic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/image-x-generic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/video-x-generic.png

# Tango - Navigation
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/go-bottom.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/go-first.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/go-jump.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/go-next.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/go-top.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/go-down.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/go-home.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/go-last.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/go-previous.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/go-up.png

# Tango - Internet
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/internet-mail.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/internet-news-reader.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/internet-group-chat.png

# Tango Netzwerk
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/network-error.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-offline.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-transmit-receive.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-wired.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-wireless.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/network-idle.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-receive.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-transmit.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/network-wireless-encrypted.png

# Tango - Listen
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/list-add.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/list-remove.png

# Tango - Multimedia
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/media-eject.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/media-playback-start.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/media-record.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/media-seek-forward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/media-skip-forward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/media-playback-pause.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/28/media-playback-stop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/media-seek-backward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/media-skip-backward.png

# Tango - System
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/process-stop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/system-search.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/user-desktop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/system-log-out.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/system-shutdown.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/28/user-trash.png

# Tango - Ansichtsmodi
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/28/view-fullscreen.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/zoom-best-fit.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/zoom-original.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/view-refresh.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/zoom-in.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/zoom-out.png

# Tango - Diverse
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/camera-photo.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/computer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/drive-optical.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/input-gaming.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/28/input-mouse.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/media-floppy.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/multimedia-player.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/audio-card.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/battery.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/help-browser.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/28/preferences-desktop-locale.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/preferences-desktop-assistive-technology.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/preferences-desktop-keyboard-shortcuts.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/preferences-desktop-wallpaper.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/emblem-favorite.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/camera-video.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/drive-harddisk.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/drive-removable-media.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/input-keyboard.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/media-flash.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/28/media-optical.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/28/printer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/28/audio-input-microphone.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/28/office-calendar.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/28/preferences-desktop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/preferences-desktop-accessibility.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/preferences-desktop-font.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/17/28/preferences-desktop-theme.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/28/X.png

cd ..

if [ ! -d Oxygen ]
  then
    echo "Creating folder \"Wiki\Icons\Oxygen\"..."
    mkdir Oxygen
fi

cd Oxygen

# Oxygen - Dokument
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/accessories-text-editor.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/28/document-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/30/28/document-print.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/30/28/document-save.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/12/edit-copy.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/edit-delete.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/edit-paste.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/folder-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/folder-remote.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/34/28/document-properties.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/12/bookmark-new.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/document-open.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/33/28/document-save-as.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/edit-cut.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/31/28/edit-clear.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/12/edit-find.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/32/28/edit-select-all.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/folder.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/user-home.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/06/datashow.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/18/contents.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/46/18/fileclose.png

# Oxygen - Dateityp
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/27/28/audio-x-generic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/27/28/text-x-generic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/27/28/image-x-generic.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/27/28/video-x-generic.png

# Oxygen - Navigation
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/24/28/go-bottom.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/go-first.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/go-jump.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/go-next.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/go-top.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/24/28/go-down.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/go-home.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/go-last.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/28/go-previous.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/28/go-up.png

# Oxygen - Internet
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/28/internet-mail.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/internet-web-browser.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/28/internet-telephony.png

# Oxygen - Netzwerk
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/network-wired.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/34/28/network-wireless.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/11/network-connect.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/11/network-disconnect.png

# Oxygen - Listen
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/list-add.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/list-remove.png

# Oxygen - Multimedia
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/35/28/media-eject.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/media-playback-start.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/media-record.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/28/media-seek-forward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/28/media-skip-forward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/11/media-playlist-repeat.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/media-playback-pause.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/28/media-playback-stop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/28/media-seek-backward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/28/media-skip-backward.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/11/media-playlist-shuffle.png

# Oxygen - System
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/process-stop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/system-search.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/28/user-desktop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/12/system-restart.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/12/06/tools_report_bug.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/system-log-out.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/28/system-shutdown.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/28/user-trash.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/05/system_users.png

# Oxygen - Ansichtsmodi
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/39/28/view-fullscreen.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/zoom-best-fit.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/zoom-original.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/view-refresh.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/zoom-in.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/zoom-out.png

# Oxygen - Hardware
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/28/audio-card.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/audio-input-line.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/28/camera-photo.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/08/12/computer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/28/cpu.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/28/drive-optical.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/28/drive-removable-media-usb-pendrive.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/input-gaming.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/input-mouse.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/52/28/media-flash-memory-stick.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/53/28/media-flash-sd-mmc.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/43/28/media-floppy.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/43/28/media-optical.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/54/28/media-tape.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/54/28/multimedia-player-apple-ipod.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/28/pda.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/28/printer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/12/video-display.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/56/28/video-television.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/49/28/audio-headset.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/28/audio-input-microphone.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/28/camera-web.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/50/28/computer-laptop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/28/drive-harddisk.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/drive-removable-media.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/51/28/drive-removable-media-usb.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/input-keyboard.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/52/28/input-tablet.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/28/media-flash.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/53/28/media-flash-smart-media.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/54/28/media-optical-audio.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/54/28/media-optical-recordable.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/54/28/modem.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/44/28/multimedia-player.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/12/phone.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/28/scanner.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/56/28/video-projector.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/28/battery.png

# Oxygen - Einstellungen
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/11/45/preferences-other.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/57/28/preferences-desktop.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/28/preferences-contact-list.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/28/preferences-desktop-color.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/28/preferences-desktop-default-applications.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/28/preferences-desktop-font.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/28/preferences-desktop-icons.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/01/28/preferences-desktop-locale.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/02/28/preferences-desktop-notification.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/01/28/preferences-desktop-printer.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/28/preferences-desktop-sound.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/28/preferences-desktop-theme.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/28/preferences-desktop-user.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/28/preferences-kcalc-constants.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/28/preferences-system-bluetooth.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/28/preferences-system-network-sharing.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/preferences-system-time.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/57/28/preferences-desktop-personal.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/22/preferences-desktop-filetype-association.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/22/user-identity.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/22/preferences-system-session-services.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/01/12/preferences-system-windows.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/28/preferences-system-power-management.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/05/28/preferences-system-login.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/28/preferences-plugin.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/28/preferences-desktop-wallpaper.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/28/preferences-desktop-user-password.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/28/preferences-desktop-text-to-speech.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/02/28/preferences-desktop-screensaver.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/57/28/preferences-desktop-peripherals.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/01/28/preferences-desktop-mouse.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/28/preferences-desktop-keyboard.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/28/preferences-desktop-gaming.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/12/preferences-desktop-display.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/28/preferences-desktop-cryptography.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/46/28/preferences-desktop-accessibility.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/28/preferences-system-network.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/28/preferences-system.png

# Oxygen - Anwendungen
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/52/12/k3b.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/59/12/konversation.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/23/06/partitionmanager.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/56/12/kaffeine.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/28/16/hwinfo.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/24/12/nepomuk.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/22/k3b.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/57/12/kontact.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/18/12/knotes.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/57/12/digikam.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/12/akonadi.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/45/05/konsole.png

# Oxygen - Diverses
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/46/28/office-calendar.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/28/X.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/58/12/utilities-terminal.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/41/kde.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/41/plasma.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/14/11/security-high.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/41/11/security-low.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/12/applications-games.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/06/12/system-run.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/34/13/face-smile.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/40/22/view-calendar-day.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/46/28/help-browser.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/28/emblem-favorite.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/41/utilities-file-archiver.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/41/applications-internet.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/11/applications-other.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/11/security-medium.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/25/24/dialog-ok-apply.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/42/06/object-locked.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/12/system-software-update.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/07/22/get-hot-new-stuff.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/47/06/applications_engineering.png

cd ..

if [ ! -d Portal ]
  then
    echo "Creating folder \"Wiki\Icons\Portal\"..."
    mkdir Portal
fi

cd Portal

wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/48/04/go-home.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/00/04/internet-news-reader.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/55/04/ubuntuusers.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/29/04/community.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/22/03/usercp-preferences.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/36/03/usercp-profile.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/51/03/usercp-wikipage.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/06/anchor.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/08/download.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/16/06/mark.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/53/06/picture.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/15/07/topic-solved-locked.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/10/04/system-users.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/04/internet.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/13/wiki_icon_retouched24_braun.png
mv wiki_icon_retouched24_braun.png wiki.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/26/09/wochenrueckblick.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/09/03/usercp-password.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/04/03/usercp-subscriptions.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/23/06/pre.png
mv pre.png code.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/03/07/download.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/13/06/user_link.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/38/06/wikilink.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/21/06/expand.png
wget -nv -N http://media.cdn.ubuntu-de.org/wiki/attachments/37/49/online-community.png

cd ..
cd ..
cd ..
