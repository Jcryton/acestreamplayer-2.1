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

if [ ${WINDOWS} = "1" ]; then
    PARAMS="
        --host=${HOST}
        --disable-projectm
        --enable-skins2
        --enable-qt  
        --disable-update-check
        --enable-debug
        --enable-sqlite
        --enable-update-check
        --enable-lua
        --enable-faad
        --enable-flac
        --enable-theora
        --enable-twolame
        --enable-quicktime
        --enable-avcodec 
        --enable-merge-ffmpeg
        --enable-dca
        --enable-mpc
        --enable-libass
        --enable-x264
        --enable-schroedinger
        --enable-realrtsp
        --enable-live555
        --enable-dvdread
        --enable-shout
        --enable-goom
        --enable-caca
        --disable-portaudio
        --disable-sdl
        --enable-sse 
        --enable-mmx
        --enable-libcddb
        --enable-zvbi 
        --disable-telx
        --disable-dirac"

    if [ "$1" = "ts" ]; then
        PARAMS="${PARAMS} --enable-torrentstream"
        mkdir -p ${PWD_DIR}/build-ts
        cd ${PWD_DIR}/build-ts
    else
        mkdir -p ${PWD_DIR}/build-ace
        cd ${PWD_DIR}/build-ace
    fi

    ${PWD_DIR}/vlc-${VLC_VERSION}/extras/package/win32/configure.sh ${PARAMS}
elif [ ${LINUX} = "1" ]; then
    
    mkdir -p ${PWD_DIR}/build-ace
    cd ${PWD_DIR}/build-ace
    
    #${PWD_DIR}/vlc-${VLC_VERSION}/configure \
    #    --prefix=${PWD_DIR}/build-ace/vlc-${VLC_VERSION} \
    #    --disable-gtk \
     
    cp -vfr ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/acestreamplayer.png ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/vlc.png
    cp -vfr ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/acestreamplayer.xpm ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/vlc.xpm
    cp -vfr ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/acestreamplayer-xmas.xpm ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons/32x32/vlc-xmas.xpm
   
     ${PWD_DIR}/vlc-${VLC_VERSION}/configure \
        --prefix=${PWD_DIR}/build-ace/vlc-${VLC_VERSION} \
        --disable-gtk \
        --disable-bluray \
        --enable-nls \
        --enable-sdl \
        --enable-ffmpeg \
        --enable-dxva2 \
        --with-ffmpeg-mp3lame \
        --with-ffmpeg-faac \
        --with-ffmpeg-zlib \
        --enable-faad \
        --enable-flac \
        --enable-theora \
        --enable-live555 \
        --enable-caca \
        --enable-mkv \
        --without-contrib \
        --disable-cddax \
        --disable-vcdx \
        --enable-twolame \
        --enable-dvdread \
        --enable-real \
        --enable-debug \
        --enable-dca \
        --enable-mpc \
        --enable-realrtsp \
        --enable-shout \
        --enable-portaudio \
        --enable-sse \
        --enable-mmx \
        --disable-upnp \
        --disable-fluidsynth \
        --disable-zvbi \
        --disable-telx \
        --disable-libass \
        --disable-projectm \
	--disable-dvdread \
	--enable-skins2 \
	--enable-qt4 \       
	--enable-libva \
        --enable-sqlite
else
    error "Unknown host arch"
fi
