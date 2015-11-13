#!/bin/sh

build="$(sh ./config.guess)"

case $build in
  i?86-*mingw*)
    host=win32 ;;
  x86_64-*mingw*)
    host=win64 ;;
  *linux*)
    host=linux ;;
  *darwin*)
    host=mac ;;
  *)
    host=unknown ;;
esac

echo $host
exit 0
