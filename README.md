Yua
===
*"it's about the strength of my desire"*

Homepage: https://forum.speeddemosarchive.com/post/yua3.html


Build
-----
Install build dependencies: `sudo apt-get install build-essential autoconf automake libtool git upx-ucl yasm zlib1g-dev`<br>
`qt5-default qtbase5-dev libqt5opengl5-dev` (Qt5)<br>
`qt4-default libqt4-dev libqt4-opengl-dev` (Qt4)

In most cases a simple `./configure && make && make install` should do it.

If you're cross-building you might have to run something like this:
``` sh
QMAKE=qmake-qt5 ./configure --build=x86_64-pc-linux-gnu \
  --target=i686-w64-mingw32.static \
  --cross-prefix=i686-w64-mingw32.static-
make -j4
```


Copyright
---------
Copyright (C) 2013-2016 Taiga Software LLC

Authors are Nathan 'nate' Jahnke and partially djcj <djcj@gmx.de>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Note: this program makes use of the [Fraunhofer FDK AAC Codec Library](http://opencore-amr.sourceforge.net/).

