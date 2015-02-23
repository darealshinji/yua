Yua
===
*"it's about the strength of my desire"*

Homepage: https://forum.speeddemosarchive.com/post/yua3.html


Build
-----
Install build dependencies: `sudo apt-get install build-essential libfdk-aac-dev libavcodec-dev libavformat-dev libavresample-dev libavutil-dev libswscale-dev zlib1g-dev`<br>
`qtbase5-dev libqt5opengl5-dev` (Qt5)<br>
`libqt4-dev libqt4-opengl-dev` (Qt4)


Install FFmpeg (preferably with fdk-aac enabled) and MP4Box as additional runtime dependencies:<br>
`sudo apt-get install ffmpeg gpac`

Compile and install (Qt5): `make && make install`<br>
Build Qt4 version: `make qt4`<br>
Default prefix: /usr/local<br>
Install in different prefix: `make install PREFIX=/usr`

You can check your build dependencies by running `./check.sh`.


Static build
------------
To compile a binary with embedded FFmpeg and MP4Box binaries and embedded encoding libraries run `make static` (Qt5) or `make static-qt4`

Install build dependencies for a static build:<br>
`sudo apt-get install build-essential automake git upx-ucl zlib1g-dev`<br>
`qtbase5-dev libqt5opengl5-dev` (Qt5)<br>
`libqt4-dev libqt4-opengl-dev` (Qt4)


Copyright
---------
Copyright (C) 2013-2015 Taiga Software LLC

Authors are Nathan 'nate' Jahnke and djcj <djcj@gmx.de>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA, or visit
http://www.gnu.org/copyleft/gpl.html
