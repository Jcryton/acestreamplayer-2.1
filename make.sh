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

cd build-ace
export QT_SELECT=qt4
make

