#!/bin/bash
function create
{
	mkdir -p $1	
	cp tools/board_tools/$2/bin/arm/* $1
	rm $1/ubi* -rf
}

rm -rf tools/board_tools/mtd-utils-100
rm -rf tools/board_tools/mtd-utils-100-nptl
rm -rf tools/board_tools/mtd-utils-200

cp tools/board_tools/mtd-utils tools/board_tools/mtd-utils-100 -a
cp tools/board_tools/mtd-utils tools/board_tools/mtd-utils-100-nptl -a
cp tools/board_tools/mtd-utils tools/board_tools/mtd-utils-200 -a

cp dxt/tools/board_tools/mtd-utils-100/Makefile tools/board_tools/mtd-utils-100/Makefile
cp dxt/tools/board_tools/mtd-utils-100-nptl/Makefile tools/board_tools/mtd-utils-100-nptl/Makefile
cp dxt/tools/board_tools/mtd-utils-200/Makefile tools/board_tools/mtd-utils-200/Makefile

pushd tools/board_tools/mtd-utils-100
find . -name 'configure' | xargs chmod +x
make
popd

pushd tools/board_tools/mtd-utils-100-nptl
find . -name 'configure' | xargs chmod +x
make
popd

pushd tools/board_tools/mtd-utils-200
find . -name 'configure' | xargs chmod +x
make
popd

out_dir100=out/boardtools/mtd-utils/100/bin
out_dir100nptl=out/boardtools/mtd-utils/100-nptl/bin
out_dir200=out/boardtools/mtd-utils/200/bin
create $out_dir100  mtd-utils-100
create $out_dir100nptl  mtd-utils-100-nptl
create $out_dir200  mtd-utils-200
