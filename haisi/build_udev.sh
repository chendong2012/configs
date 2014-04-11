#!/bin/bash
function create
{
	mkdir -p $1	
	cp tools/board_tools/$2/udev-100/udevd $1
	cp tools/board_tools/$2/udev-100/udevstart $1
	cp tools/board_tools/$2/udev-100/udevinfo $1
	cp -af tools/board_tools/$2/rootfs/. $1/../
}

rm -rf tools/board_tools/udev-100-100
rm -rf tools/board_tools/udev-100-100-nptl
rm -rf tools/board_tools/udev-100-200

cp tools/board_tools/udev-100 tools/board_tools/udev-100-100 -a
cp tools/board_tools/udev-100 tools/board_tools/udev-100-100-nptl -a
cp tools/board_tools/udev-100 tools/board_tools/udev-100-200 -a

cp dxt/tools/board_tools/udev-100/100/Makefile tools/board_tools/udev-100-100/Makefile
cp dxt/tools/board_tools/udev-100/100-nptl/Makefile tools/board_tools/udev-100-100-nptl/Makefile
cp dxt/tools/board_tools/udev-100/200/Makefile tools/board_tools/udev-100-200/Makefile

pushd tools/board_tools/udev-100-100
make
popd

pushd tools/board_tools/udev-100-100-nptl
make
popd

pushd tools/board_tools/udev-100-200
make
popd

out_dir100=out/boardtools/udev/100/bin
out_dir100nptl=out/boardtools/udev/100-nptl/bin
out_dir200=out/boardtools/udev/200/bin

rm -rf out/boardtools/udev
create $out_dir100 "udev-100-100"
create $out_dir100nptl "udev-100-100-nptl"
create $out_dir200 "udev-100-200"
