#!/bin/sh

export QT_SELECT=qt4

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

if [ ${LINUX} = "1" ]; then
    
    mkdir -p ${PWD_DIR}/build-ace
    cd ${PWD_DIR}/build-ace
    
    #${PWD_DIR}/vlc-${VLC_VERSION}/configure \
    #    --prefix=${PWD_DIR}/build-ace/vlc-${VLC_VERSION} \
     
    cp -fr ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/acestreamplayer.png ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/vlc.png
    cp -fr ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/acestreamplayer.xpm ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/vlc.xpm
    cp -fr ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/acestreamplayer-xmas.xpm ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/vlc-xmas.xpm
   
     ${PWD_DIR}/vlc-${VLC_VERSION}/configure \
        --prefix=/usr \
        --disable-bluray \
        --enable-nls \
        --enable-sdl \
        --enable-dxva2 \
        --enable-faad \
        --enable-flac \
        --enable-theora \
        --enable-live555 \
        --enable-caca \
        --enable-mkv \
        --without-contrib \
        --disable-vcdx \
        --enable-twolame \
        --enable-dvdread \
        --enable-debug \
        --enable-dca \
        --enable-mpc \
        --enable-realrtsp \
        --enable-shout \
        --enable-sse \
        --enable-mmx \
        --disable-upnp \
        --disable-fluidsynth \
        --disable-zvbi \
        --disable-telx \
        --disable-libass \
        --disable-projectm \
        --enable-libva \
        --enable-sqlite
else
    error "Unknown host arch"
fi
