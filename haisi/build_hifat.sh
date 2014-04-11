#!/bin/bash
. env.sh
rm -rf out/board_tools/hifat

mkdir -p out/board_tools/hifat/100/sbin
mkdir -p out/board_tools/hifat/100-nptl/sbin
mkdir -p out/board_tools/hifat/200/sbin

mkdir -p out/board_tools/hifat/100/lib
mkdir -p out/board_tools/hifat/100-nptl/lib
mkdir -p out/board_tools/hifat/200/lib

cp tools/board_tools/hifat/uclibc/static/himount out/board_tools/hifat/100/sbin/
cp tools/board_tools/hifat/uclibc/static/libhimount_api.a out/board_tools/hifat/100/lib/

cp tools/board_tools/hifat/uclibc/static/himount out/board_tools/hifat/100-nptl/sbin/
cp tools/board_tools/hifat/uclibc/static/libhimount_api.a out/board_tools/hifat/100-nptl/lib/

cp tools/board_tools/hifat/glibc/static/himount out/board_tools/hifat/200/sbin/
cp tools/board_tools/hifat/glibc/static/libhimount_api.a out/board_tools/hifat/200/lib/
