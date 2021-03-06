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
    echo check_required_package mingw-w64
    echo check_required_package mingw-w64-dev
    echo check_required_package mingw-w64-tools
fi

# download and unpack
if [ ! -d ${PWD_DIR}/vlc-${VLC_VERSION} ]; then
    cd ${PWD_DIR}
    
    if [ ! -f ${PWD_DIR}/vlc-${VLC_VERSION}.tar.* ]; then
        info "Downloading vlc"       
        if [ ${VLC_VERSION} = "2.1"  ]; then
           git clone https://git.videolan.org/git/vlc/vlc-2.1.git
        else
           download ${VLC_URL} || error "Failed to download vlc"
           unpack ${PWD_DIR}/vlc-${VLC_VERSION}.tar.*
        fi

    fi
    #unpack ${PWD_DIR}/vlc-${VLC_VERSION}.tar.*
fi

if [ ! -f ${PWD_DIR}/._prepare ]; then
    # copy new sources
    rm -r ${PWD_DIR}/vlc-${VLC_VERSION}/share/icons || error ""
    rm ${PWD_DIR}/vlc-${VLC_VERSION}/share/vlc512x512.png || error ""

    cp -rf ${PWD_DIR}/acestreamplayer/include/* ${PWD_DIR}/vlc-${VLC_VERSION}/include || error ""
    cp -rf ${PWD_DIR}/acestreamplayer/lib/* ${PWD_DIR}/vlc-${VLC_VERSION}/lib || error ""
    cp -rf ${PWD_DIR}/acestreamplayer/modules/* ${PWD_DIR}/vlc-${VLC_VERSION}/modules || error ""
    cp -rf ${PWD_DIR}/acestreamplayer/share/* ${PWD_DIR}/vlc-${VLC_VERSION}/share || error ""
    cp -rf ${PWD_DIR}/acestreamplayer/src/* ${PWD_DIR}/vlc-${VLC_VERSION}/src || error ""

    # preparing for patching
    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/vlc.desktop.in ${PWD_DIR}/vlc-${VLC_VERSION}/share/acestreamplayer.desktop.in || error ""

    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/vlc-openvcd.desktop ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/acestreamplayer-openvcd.desktop || error ""
    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/vlc-opendvd.desktop ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/acestreamplayer-opendvd.desktop || error ""
    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/vlc-openbd.desktop ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/acestreamplayer-openbd.desktop || error ""
    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/vlc-opencda.desktop ${PWD_DIR}/vlc-${VLC_VERSION}/share/solid/acestreamplayer-opencda.desktop || error ""

    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/utils/video-vlc-default.sh ${PWD_DIR}/vlc-${VLC_VERSION}/share/utils/video-acestreamplayer-default.sh || error ""
    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/utils/audio-vlc-default.sh ${PWD_DIR}/vlc-${VLC_VERSION}/share/utils/audio-acestreamplayer-default.sh || error ""
    mv ${PWD_DIR}/vlc-${VLC_VERSION}/share/utils/gnome-vlc-default.sh ${PWD_DIR}/vlc-${VLC_VERSION}/share/utils/gnome-acestreamplayer-default.sh || error ""

    mv ${PWD_DIR}/vlc-${VLC_VERSION}/po/vlc.pot ${PWD_DIR}/vlc-${VLC_VERSION}/po/acestreamplayer.pot || error ""
    touch ${PWD_DIR}/._prepare
fi

# patch
check_and_patch()
{
    cd ${PWD_DIR}/vlc-${VLC_VERSION}
    
    filename=$(basename "$1")
    name="${filename%.*}"
    if [ ! -f ${PWD_DIR}/._${name} ]; then
        apply_patch $1
        touch ${PWD_DIR}/._${name}
    fi
    
    cd ${PWD_DIR}
}

for i in `seq 1 24`; do
    if [ "$i" -eq "20" ]; then 
       for i in `seq 1 47`; do
           if [ "$i" -lt "10" ]; then 
               check_and_patch ${PWD_DIR}/patches/qt4/000$i-qt4-modules.patch
           else
               check_and_patch ${PWD_DIR}/patches/qt4/00$i-qt4-modules.patch
           fi
       done   
    else
       if [ "$i" -lt "10" ]; then 
           check_and_patch ${PWD_DIR}/patches/common/0$i-*.patch
       else
           if [ "$i" -eq "12" ]; then 
              check_and_patch ${PWD_DIR}/patches/${VLC_VERSION}/12-root.patch
           else  
              check_and_patch ${PWD_DIR}/patches/common/$i-*.patch
           fi
       fi
    fi
done

check_and_patch ${PWD_DIR}/patches/qt4/0048-qt4-remove-debug-assert-in-inputmanager.patch

if [ ${WINDOWS} = "1" ]; then
    check_and_patch ${PWD_DIR}/patches/win32/0001-win32-remove-npapi.patch
    check_and_patch ${PWD_DIR}/patches/win32/0002-win32-remove-npapi.patch
    check_and_patch ${PWD_DIR}/patches/win32/0003-win32-fix-tsplayer-name.patch
    check_and_patch ${PWD_DIR}/patches/win32/0004-win32-fix-tsplayer-name.patch
    check_and_patch ${PWD_DIR}/patches/win32/0005-win32-libgpg-error-1.12.patch
    check_and_patch ${PWD_DIR}/patches/win32/0006-win32-iconv.patch
    check_and_patch ${PWD_DIR}/patches/win32/0007-win32-update-broken-gmp-version.patch
    check_and_patch ${PWD_DIR}/patches/win32/0008-${VLC_VERSION}-win32-lame-i686-fix.patch
    if [ ${UBUNTU} = "1604" ]; then
        check_and_patch ${PWD_DIR}/patches/win32/0009-win32-dshow-define-fix.patch
        check_and_patch ${PWD_DIR}/patches/win32/0010-win32-dshow_filter-define-fix.patch
        check_and_patch ${PWD_DIR}/patches/win32/0011-win32-directx-remove-assert.patch
    fi
    check_and_patch ${PWD_DIR}/patches/win32/0012-win32-qt4.patch
    check_and_patch ${PWD_DIR}/patches/win32/0013-win32-opensll.patch
    check_and_patch ${PWD_DIR}/patches/win32/0014-win32-sqlite.patch
    check_and_patch ${PWD_DIR}/patches/win32/0015-win32-qt-4.8.6.patch
    check_and_patch ${PWD_DIR}/patches/win32/0016-win32-qt4-before-phtreads.patch
    if [ ${QT_VERSION} = "4.8.5" ]; then
        check_and_patch ${PWD_DIR}/patches/win32/0017-comeback-to-qt-4.8.5.patch
    fi
    check_and_patch ${PWD_DIR}/patches/win32/0018-win32-compile-libqjpeg-before-libjpeg.patch
fi

# private directory
if [ -d  ${PWD_DIR}/private/vlc ]; then
    cp -rf ${PWD_DIR}/private/vlc/* ${PWD_DIR}/vlc-${VLC_VERSION}
fi

info "Ok"
