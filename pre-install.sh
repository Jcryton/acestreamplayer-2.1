#!/bin/shPWD_DIR=$(readlink -f $(dirname $0))
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

echo Backup VLC files "/usr/bin/vlc,/usr/bin/vlc-wrapper,/usr/lib/pkgconfig/libvlc.pc,/usr/lib/pkgconfig/vlc-plugin.pc"
cd /usr/bin
mv vlc _vlc 2>/dev/null
mv vlc-wrapper _vlc-wrapper 2>/dev/null
cd /usr/lib/pkgconfig
mv libvlc.pc _libvlc.pc 2>/dev/null
mv vlc-plugin.pc _vlc-plugin.pc 2>/dev/null


