#!/bin/sh

PWD_DIR=$(readlink -f $(dirname $0))
. "${PWD_DIR}/config.sh"
. "${PWD_DIR}/functions.sh"

PARAMS="
      --host=${HOST}
      --disable-projectm
      --disable-skins2
      --disable-update-check
      --enable-sqlite"

if [ "$1" = "ts" ]; then
    PARAMS="${PARAMS} --enable-torrentstream"
    mkdir -p ${PWD_DIR}/build-ts
    cd ${PWD_DIR}/build-ts
else
    mkdir -p ${PWD_DIR}/build-ace
    cd ${PWD_DIR}/build-ace
fi

${PWD_DIR}/vlc-${VLC_VERSION}/extras/package/win32/configure.sh ${PARAMS}