#!/bin/sh

exitCode=1

printf "NNEDI3 deinterlacing available? ... "
test $(uname -p) = "x86_64" && echo "yes" || echo "no"

for v in 4 5 ;
do
    printf "check for QT$v qmake ... "
    qtchooser -run-tool=qmake -qt=$v -v 2>/dev/null 1>/dev/null
    exitCode=$(echo $?)
    test $exitCode -eq 0 && echo "ok" || echo "not found!"
done

for c in make automake ffmpeg MP4Box upx-ucl git pkg-config ;
do
    printf "check for $c ... "
    which $c 2>/dev/null 1>/dev/null
    exitCode=$(echo $?)
    test $exitCode -eq 0 && echo "ok" || echo "not found!"
done

which pkg-config 2>/dev/null 1>/dev/null
exitCode=$(echo $?)
test $exitCode -eq 0 && true || exit 0

for l in zlib gl fdk-aac libavcodec libavformat libavresample libavutil libswscale \
    QtDBus QtOpenGL QtGui QtCore Qt5DBus Qt5OpenGL Qt5Gui Qt5Core ;
do
    printf "check for library $l ... "
    pkg-config --libs $l 2>/dev/null 1>/dev/null
    exitCode=$(echo $?)
    test $exitCode -eq 0 && pkg-config --libs $l || echo "not found!"
done

