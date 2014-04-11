#!/bin/bash
. env.sh
rm -rf tools/board_tools/parted-100
rm -rf tools/board_tools/parted-100-nptl
rm -rf tools/board_tools/parted-200

cp tools/board_tools/parted  tools/board_tools/parted-100 -a
cp tools/board_tools/parted  tools/board_tools/parted-100-nptl -a
cp tools/board_tools/parted  tools/board_tools/parted-200 -a

cp dxt/tools/board_tools/parted-100/Makefile tools/board_tools/parted-100/ 
cp dxt/tools/board_tools/parted-100-nptl/Makefile tools/board_tools/parted-100-nptl/
cp dxt/tools/board_tools/parted-200/Makefile tools/board_tools/parted-200/

pushd tools/board_tools/parted-100
make
popd

pushd tools/board_tools/parted-100-nptl
make
popd

pushd tools/board_tools/parted-200
make
popd

rm -rf out/board_tools/parted

mkdir -p out/board_tools/parted/100/sbin
mkdir -p out/board_tools/parted/100-nptl/sbin
mkdir -p out/board_tools/parted/200/sbin

cp tools/board_tools/parted-100/bin/sbin/parted out/board_tools/parted/100/sbin/
cp tools/board_tools/parted-100/bin/sbin/partprobe  out/board_tools/parted/100/sbin/

cp tools/board_tools/parted-100-nptl/bin/sbin/parted out/board_tools/parted/100-nptl/sbin/
cp tools/board_tools/parted-100-nptl/bin/sbin/partprobe  out/board_tools/parted/100-nptl/sbin/

cp tools/board_tools/parted-200/bin/sbin/parted out/board_tools/parted/200/sbin/
cp tools/board_tools/parted-200/bin/sbin/partprobe  out/board_tools/parted/200/sbin/
