Ace Stream Player
-----------------

![](https://o9.icdn.ru/j/jcryton/9/64309749PrO.jpg)

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

`./make.sh`

`su`

`./pre-install.sh`

`cd build-ace`

`make install`

`cd ..`

`./post-install.sh`


**Crosscompile for Windows:**

Download [ubuntu-13.10-desktop-amd64.iso] or [ubuntu-mate-16.04-desktop-amd64.iso], and instal on VirtualBox
For Ubuntu 13.10 change [source.list]


***Ubuntu 13.10:***
`apt-get install -y \
gcc-mingw-w64-i686 \
g++-mingw-w64-i686 \
mingw-w64-tools \
build-essential \
lua5.2 \
libtool \
automake \
autoconf \
autopoint \
make \
gettext \
pkg-config \
qt4-dev-tools \
git \
subversion \
cmake \
cvs \
wine-dev \
zip \
p7zip-full \
nsis \
bzip2 \
vim-gnome \
vim-gui-common \
vim-runtime \
yasm`

`git clone https://github.com/Jcryton/acestreamplayer-2.1.git`

`cd acestreamplayer-2.1`

edit config.sh (set ubuntu qt and vlc version)

`./win32prepare.sh`

`./win32build.sh`

***Ubuntu 16.04:***
`apt-get install -y \
build-essential \
qt4-dev-tools qt4-default git \
subversion cmake cvs \
zip p7zip nsis \
yasm cvs cmake ragel autopoint \
ant default-jdk protobuf-compiler \
dos2unix`

`git clone https://github.com/Jcryton/acestreamplayer-2.1.git`

`cd acestreamplayer-2.1`

edit config.sh (set ubuntu qt and vlc version)

`./win32prepare.sh`

`./win32build.sh`

[libraries]:https://wiki.videolan.org/Contrib_Status/
[ubuntu-13.10-desktop-amd64.iso]:http://old-releases.ubuntu.com/releases/13.10/ubuntu-13.10-desktop-amd64.iso
[ubuntu-mate-16.04-desktop-amd64.iso]:https://mirror.yandex.ru/ubuntu-cdimage/ubuntu-mate/releases/16.04/release/ubuntu-mate-16.04-desktop-amd64.iso
[source.list]:https://askubuntu.com/questions/91815/how-to-install-software-or-upgrade-from-an-old-unsupported-release

