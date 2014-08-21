Yua
===
*"it's about the strength of my desire"*

Homepage: https://forum.speeddemosarchive.com/post/yua3.html


Build
-----
Install build dependencies: `sudo apt-get install build-essential qtbase5-dev libqt5opengl5-dev libavcodec-dev libavformat-dev libavresample-dev libavutil-dev libswscale-dev zlib1g-dev`

Install FFmpeg (preferably with fdk-aac enabled) and MP4Box as additional runtime dependencies:<br>
`sudo apt-get install ffmpeg gpac`

Compile and install: `make && make install`<br>
Default prefix: /usr/local<br>
Install in different prefix: `make install PREFIX=/usr`


Static build
------------
To compile a statically linked binary with embedded FFmpeg and MP4Box run `make static`

Install build dependencies for a static build:<br>
`sudo apt-get install build-essential git qtbase5-dev libqt5opengl5-dev upx-ucl zlib1g-dev`


Ubuntu
------
Ubuntu users can install Yua via PPA.
Note that this will also install newer FFmpeg libraries.
```
sudo add-apt-repository ppa:djcj/yua
sudo apt-get update
sudo apt-get install yua
sudo apt-get upgrade
```


Copyright
---------
Copyright (C) 2013-2014 Taiga Software LLC

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
