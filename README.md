Yua
===
*"it's about the strength of my desire"*

Homepage: https://forum.speeddemosarchive.com/post/yua3.html

This code is meant to be used to build distribution specific GNU/Linux packages,
especially for Debian based systems. Checkout the 'original' branch to build statically linked
binaries on all three major OSes.

Build dependencies: `build-essential libqt4-dev libqt4-opengl-dev libavcodec-dev libavformat-dev libavresample-dev libavutil-dev libswscale-dev zlib1g-dev`

Additional runtime dependencies: FFmpeg (preferably with fdk-aac enabled) and MP4Box binaries (`ffmpeg gpac`)

Compile and install: `make && make install`<br>
Default prefix: /usr/local<br>
Install in different prefix: `make install PREFIX=/usr`

Compile a statically linked binary with embedded FFmpeg and MP4Box: `make static`
Build dependencies for a static build: `build-essential git libqt4-dev libqt4-opengl-dev upx-ucl zlib1g-dev`

Copyright (C) 2013-2014 Taiga Software LLC

Your use of this software is governed by the terms of the GPL version 2
or, at your option, any later version.
