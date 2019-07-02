#!/bin/sh

PWD_DIR=$(readlink -f $(dirname $0))
. "${PWD_DIR}/config.sh"
. "${PWD_DIR}/functions.sh"


# change into vlc dir
cd ${PWD_DIR}/vlc-${VLC_VERSION}

# create and change into contrib dir
mkdir -p contrib/win32 && cd contrib/win32

#run the bootstrap
../bootstrap --host=i686-w64-mingw32

echo "##################################"
echo "### steps after build contrib: ###"
echo "##################################"
echo " "
echo "### remove the 64 bit binaries"
echo "rm -f ../i686-w64-mingw32/bin/moc ../i686-w64-mingw32/bin/uic ../i686-w64-mingw32/bin/rcc"
echo " "
echo "### go back and run the bootstrap"
echo "cd ${PWD_DIR}/vlc-${VLC_VERSION}"
echo "./bootstrap"
echo " "
echo "### create the dir we'll be compiling in"
echo "mkdir win32 && cd win32"
echo " "
echo "### tell the system where to find the pkgconfig dir"
echo "xport PKG_CONFIG_LIBDIR=${PWD_DIR}/vlc-${VLC_VERSION}/contrib/i686-w64-mingw32/lib/pkgconfig"
echo " "
echo "### run configure"
echo "../extras/package/win32/configure.sh --host=i686-w64-mingw32 --enable-sqlite --disable-bluray"
echo " "
echo "### compile"
echo "make"

read -p "Press key to continue.. " -n1 -s

make

