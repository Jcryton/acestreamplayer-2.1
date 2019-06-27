#!/bin/sh

PWD_DIR=$(readlink -f $(dirname $0))
. "${PWD_DIR}/config.sh"
. "${PWD_DIR}/functions.sh"

LINUX="0"
WINDOWS="0"
case "${HOST}" in
  *mingw32*)
    WINDOWS="1"
  ;;
  *)
    LINUX="1"
  ;;
esac

if [ ${WINDOWS} = "1" ]; then
    echo "For compile AcestreamPlayer for windows"
    echo "Use sripts:"
    echo "win32prepare.sh win32compile.sh"
    exit
fi

echo Fix Acestream files "/usr/bin/acestreamplayer,/usr/bin/acestreamplayer-wrapper,/usr/lib/pkgconfig/libacestreamplayer.pc,/usr/lib/pkgconfig/acestreamplayer-plugin.pc"
cd /usr/bin
mv vlc acestreamplayer 2>/dev/null
mv vlc-wrapper acestreamplayer-wrapper 2>/dev/null
cd /usr/lib/pkgconfig
mv libvlc.pc acestreamplayervlc.pc 2>/dev/null
mv vlc-plugin.pc acestreamplayer-plugin.pc 2>/dev/null

echo Restore VLC files "/usr/bin/vlc,/usr/bin/vlc-wrapper,/usr/lib/pkgconfig/libvlc.pc,/usr/lib/pkgconfig/vlc-plugin.pc"
cd /usr/bin
mv _vlc vlc 2>/dev/null
mv _vlc-wrapper vlc-wrapper 2>/dev/null
cd /usr/lib/pkgconfig
mv _libvlc.pc libvlc.pc 2>/dev/null
mv _vlc-plugin.pc vlc-plugin.pc 2>/dev/null


