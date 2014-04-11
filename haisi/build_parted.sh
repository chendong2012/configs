#!/bin/bash
rm -rf tools/board_tools/parted-100
rm -rf tools/board_tools/parted-100-nptl
rm -rf tools/board_tools/parted-200

cp tools/board_tools/parted  tools/board_tools/parted-100 -a
cp tools/board_tools/parted  tools/board_tools/parted-100-nptl -a
cp tools/board_tools/parted  tools/board_tools/parted-200 -a

cp dxt/tools/boardtools/parted-100/Makefile tools/board_tools/parted-100/ 
cp dxt/tools/boardtools/parted-100-nptl/Makefile tools/board_tools/parted-100-nptl/
cp dxt/tools/boardtools/parted-200/Makefile tools/board_tools/parted-100-nptl/

pushd tools/board_tools/parted-100
make
popd

pushd tools/board_tools/parted-100-nptl
make
popd

pushd tools/board_tools/parted-200
make
popd

rm -rf out/boardtools/parted

mkdir -p out/boardtools/parted/100/sbin
mkdir -p out/boardtools/parted/100-nptl/sbin
mkdir -p out/boardtools/parted/200/sbin

cp tools/board_tools/parted-100/parted-3.0/bin/sbin/parted out/boardtools/parted/100/sbin/
cp tools/board_tools/parted-100/parted-3.0/bin/sbin/partprobe  out/boardtools/parted/100/sbin/

cp tools/board_tools/parted-100-nptl/parted-3.0/bin/sbin/parted out/boardtools/parted/100-nptl/sbin/
cp tools/board_tools/parted-100-nptl/parted-3.0/bin/sbin/partprobe  out/boardtools/parted/100-nptl/sbin/

cp tools/board_tools/parted-200/parted-3.0/bin/sbin/parted out/boardtools/parted/200/sbin/
cp tools/board_tools/parted-200/parted-3.0/bin/sbin/partprobe  out/boardtools/parted/200/sbin/
