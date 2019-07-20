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
	echo "#################################################"
	echo "### RUN: win32compile.sh before win32build.sh ###"
	echo "#################################################"
    exit
fi

# download contrib
wget https://raw.githubusercontent.com/Jcryton/ace-contrib/master/2.1/ace-contrib-i686-w64-mingw32-qt-${QT_VERSION}-ubuntu${UBUNTU}.tar.bz2

# change into vlc dir
cd ${PWD_DIR}/vlc-${VLC_VERSION}

# create and change into contrib dir
mkdir -p contrib/win32 && cd contrib/win32

#run the bootstrap
../bootstrap --host=i686-w64-mingw32

#create symlink
ln -s ../../../ace-contrib-i686-w64-mingw32-qt-${QT_VERSION}-ubuntu${UBUNTU}.tar.bz2 vlc-contrib-i686-w64-mingw32-latest.tar.bz2

### prebuilt
make prebuilt

### remove the 64 bit binaries"
rm -f ../i686-w64-mingw32/bin/moc ../i686-w64-mingw32/bin/uic ../i686-w64-mingw32/bin/rcc

### go back and run the bootstrap
cd ${PWD_DIR}/vlc-${VLC_VERSION}
./bootstrap

### create the dir we'll be compiling in"
mkdir -p ${PWD_DIR}/vlc-${VLC_VERSION}/win32
cd ${PWD_DIR}/vlc-${VLC_VERSION}/win32

### tell the system where to find the pkgconfig dir"
export PKG_CONFIG_LIBDIR=${PWD_DIR}/vlc-${VLC_VERSION}/contrib/i686-w64-mingw32/lib/pkgconfig

### run configure
if [ ${UBUNTU} = "1604" ]; then
../extras/package/win32/configure.sh --host=i686-w64-mingw32 --enable-sqlite --disable-bluray
else
../extras/package/win32/configure.sh --host=i686-w64-mingw32 --enable-sqlite
fi

### compile
make

####create the package
make package-win32-zip

cp ${PWD_DIR}/vlc-${VLC_VERSION}/win32/vlc-${VLC_VERSION}-win32.zip ${PWD_DIR}/acestreamplyaer-${VLC_VERSION}-win32.zip 

echo "#################################################################"
echo "### Finally: acestreamplyaer-${VLC_VERSION}-win32.zip is ready###"
echo "#################################################################"
