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

if [ ${WINDOWS} = "0" ]; then
    echo "For compile AcestreamPlayer for linux"
    echo "Use sripts:"
    echo "bootstrap.sh configure.sh"
    exit
fi


if [ ! -f ${PWD_DIR}/._prepare ]; then
	echo "####################################################"
	echo "### RUN: win32prepare.sh before win32tarballs.sh ###"
	echo "####################################################"
    exit
fi

# change into vlc dir
cd ${PWD_DIR}/vlc-${VLC_VERSION}
mkdir -p contrib/tarballs && cd contrib/tarballs


# download tarballs

wget http://master.qt.io/archive/qt/4.8/${QT_VERSION}/qt-everywhere-opensource-src-${QT_VERSION}.tar.gz
mv qt-everywhere-opensource-src-${QT_VERSION}.tar.gz qt-${QT_VERSION}.tar.gz

wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/a52dec-0.7.4.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/chromaprint-0.7.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/crystalhd_lgpl_includes_v1.zip
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/faad2-2.7.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/ffmpeg-2dcaa1b9d142ae113b28bffdbf7f8f8900b5e770.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/ffmpeg-469de4f58317e121644c4cf9a2824ccbbf7763ca.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/flac-1.3.0.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.10.2.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.11.0.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.11.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.12.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.12.3.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.12.4.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fontconfig-2.12.6.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/freetype-2.5.0.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/freetype-2.5.3.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/fribidi-0.19.6.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/game-music-emu-0.6.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gettext-0.18.3.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gettext-0.19.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/glew-1.7.0.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gmp-4.2.1.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gmp-5.0.2.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gmp-6.0.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gmp-6.1.2.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gnutls-3.1.17.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/gnutls-3.1.25.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/goom-2k4-0-src.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/harfbuzz-0.9.19.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/harfbuzz-0.9.5.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/jpegsrc.v9a.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/jpegsrc.v9.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/lame-3.99.5.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libass-0.10.2.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libass-0.11.2.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libbluray-0.4.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libbluray-0.5.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libcaca-0.99.beta17.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libcddb-1.3.2.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdca-0.0.5.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdvbpsi-0.2.2.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdvdcss-1.2.13.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdvdnav-4.2.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdvdnav-git.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdvdread-4.2.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libdvdread-git.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libebml-1.3.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libgcrypt-1.6.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libgpg-error-1.12.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libgpg-error-1.13.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libgsm_1.0.13.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libiconv-1.14.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libkate-0.4.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libmad-0.15.1b.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libmatroska-1.4.1.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libmodplug-0.8.8.5.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libmpeg2-0.5.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libogg-1.3.1.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libogg-1.3.2.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libpng-1.6.12.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libpng-1.6.8.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libsamplerate-0.1.8.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libshout-2.3.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libtheora-1.1.1.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libupnp-1.6.18.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/LibVNCServer-0.9.9.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libvorbis-1.3.4.tar.xz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libvpx-v1.1.0.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/libxml2-2.9.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/live.2014.01.21.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/live.2014.05.27.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/lua-5.1.4.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/modplug-xmms-git-bc8cb8248788c05b77da7d653f4c677354339a21.zip
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/musepack_src_r481.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/nettle-2.6.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/openjpeg-1.5.0.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/openssl-1.0.1e.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/opus-1.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/orc-0.4.18.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/postproc-git.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/projectM-2.0.1-Source.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/pthreads-w32-2-9-1-release.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/regex-0.13.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/schroedinger-1.0.11.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/SDL-1.2.15.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/SDL_image-1.2.12.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/sidplay-libs-2.1.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/speex-git.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/sqlite-3.6.20.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/taglib-1.9.1.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/tiff-4.0.3.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/twolame-0.3.13.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/twolame-winutil.h
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/x26410b-git.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/x264-git.tar.bz2
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/x264-git.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/zlib-1.2.8.tar.gz
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/tarballs/zvbi-0.2.35.tar.bz2
