Ace Stream Player
-----------------

Tested with VLC version *2.1.4   2.1.5   2.1.6   2.1.6-git*

Depends: acestreamengine, libgcrypt < 1.6, ffmpeg < 2.0
Optimal: acestreamengine-3.1.35, libgcrypt-1.5.4[5,6], ffmpeg-1.2.12

**Compile for Linux:** 

Install build tools *libtool*, *build-essential*, *pkg-config*, *autoconf*

Checkout all required [libraries] to build vlc

Also install *libqt4-webkit*, *openssl*, *sqlite3*

Edit config.sh, set VLC_VERSION="2.1.4", "2.1.5". "2.1.6" to build this release,
 or "2.1" to build last 2.1.6-git version

`./bootstrap.sh`

`./configure.sh`

`cd build-ace`

`make`

`make install`

[libraries]:https://wiki.videolan.org/Contrib_Status/
