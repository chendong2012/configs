#!/bin/bash
mkdir -p out/e2fsprogs/bin_200
mkdir -p out/e2fsprogs/bin_nptl
cp dxt/tools/board_tools/e2fsprogs/Makefile tools/board_tools/e2fsprogs/Makefile 
pushd tools/board_tools/e2fsprogs
make
popd

rm -rf out/boardtools/e2fsprogs
mkdir -p out/boardtools/e2fsprogs/100/bin
mkdir -p out/boardtools/e2fsprogs/100-nptl/bin
mkdir -p out/boardtools/e2fsprogs/200/bin

cp tools/board_tools/e2fsprogs  tools/board_tools/e2fsprogs-100 -a
cp tools/board_tools/e2fsprogs  tools/board_tools/e2fsprogs-100-nptl -a
cp tools/board_tools/e2fsprogs  tools/board_tools/e2fsprogs-200 -a

cp dxt/tools/board_tools/e2fsprogs-100/Makefile  	tools/board_tools/parted-100/ 
cp dxt/tools/board_tools/e2fsprogs-100-nptl/Makefile 	tools/board_tools/parted-100-nptl/
cp dxt/tools/board_tools/e2fsprogs-200/Makefile 	tools/board_tools/parted-100-nptl/

pushd tools/board_tools/e2fsprogs-100
make
popd

pushd tools/board_tools/e2fsprogs-100-nptl
make
popd

pushd tools/board_tools/e2fsprogs-200
make
popd
