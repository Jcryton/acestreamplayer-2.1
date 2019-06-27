#!/bin/sh

PWD_DIR=$(readlink -f $(dirname $0))
. "${PWD_DIR}/config.sh"
. "${PWD_DIR}/functions.sh"

# Downloading contribs
#download ${CONTRIBS_URL} || error "Failed to download contribs"

# Prepare contribs
#tar -xvfz i686-w64-mingw32.tar.gz 
#tar -cjvpf acestreamplayer-contrib-i686-w64-mingw32-2015.tar.bz2 i686-w64-mingw32
#rm i686-w64-mingw32.tar.gz
#rm -rf i686-w64-mingw32

# change into vlc dir
cd ${PWD_DIR}/vlc-${VLC_VERSION}

# create and change into contrib dir
mkdir -p contrib/win32 && cd contrib/win32

#export PKG_CONFIG=/usr/bin/pkg-config

#run the bootstrap
../bootstrap --host=i686-w64-mingw32

# symlink in the contrib as make prebuilt tries to download contrib and
# fails (plus the latest one isn't supported by vlc 2.1)
#ln -s ../../../acestreamplayer-contrib-i686-w64-mingw32-2014.tar.bz2 vlc-contrib-i686-w64-mingw32-latest.tar.bz2
#ln -s ${PWD_DIR}/contrib/acestreamplayer-contrib-i686-w64-mingw32-2014.tar.bz2 vlc-contrib-i686-w64-mingw32-latest.tar.bz2
#make prebuilt
make

# remove the 64 bit binaries
rm -f ../i686-w64-mingw32/bin/moc ../i686-w64-mingw32/bin/uic ../i686-w64-mingw32/bin/rcc

# go back and run the bootstrap
cd ${PWD_DIR}/vlc-${VLC_VERSION}
./bootstrap

# create the dir we'll be compiling in
mkdir win32 && cd win32

# tell the system where to find the pkgconfig dir
export PKG_CONFIG_LIBDIR=${PWD_DIR}/vlc-${VLC_VERSION}/contrib/i686-w64-mingw32/lib/pkgconfig

# run configure
../extras/package/win32/configure.sh --host=i686-w64-mingw32 --enable-sqlite --disable-bluray

# compile
make

