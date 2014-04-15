#!/bin/bash
. ./env.sh
e2fname="e2fsprogs-1.41.11"

function create
{
	mkdir -p $1	
#	cp tools/board_tools/$2/e2fsprogs-1.41.11/inst/sbin/mkfs.ext2 $1
#	cp tools/board_tools/$2/e2fsprogs-1.41.11/inst/sbin/mkfs.ext3 $1
#	cp tools/board_tools/$2/e2fsprogs-1.41.11/inst/sbin/mkfs.ext4 $1
#	cp tools/board_tools/$2/e2fsprogs-1.41.11/inst/sbin/mke2fs $1

	cp tools/board_tools/$2/mkfs.ext2 $1
	cp tools/board_tools/$2/mkfs.ext3 $1
	cp tools/board_tools/$2/mkfs.ext4 $1
	cp tools/board_tools/$2/mke2fs $1
}

rm -rf tools/board_tools/e2fsprogs-100
rm -rf tools/board_tools/e2fsprogs-100-nptl
rm -rf tools/board_tools/e2fsprogs-200

cp tools/board_tools/e2fsprogs tools/board_tools/e2fsprogs-100 -a
cp tools/board_tools/e2fsprogs tools/board_tools/e2fsprogs-100-nptl -a
cp tools/board_tools/e2fsprogs tools/board_tools/e2fsprogs-200 -a

cp dxt/tools/board_tools/e2fsprogs-100/Makefile tools/board_tools/e2fsprogs-100/Makefile
cp dxt/tools/board_tools/e2fsprogs-100-nptl/Makefile tools/board_tools/e2fsprogs-100-nptl/Makefile
cp dxt/tools/board_tools/e2fsprogs-200/Makefile tools/board_tools/e2fsprogs-200/Makefile

pushd tools/board_tools/e2fsprogs-100
make
popd

pushd tools/board_tools/e2fsprogs-100-nptl
make
popd

pushd tools/board_tools/e2fsprogs-200
make
popd
out_dir100="out/board_tools/e2fsprogs/100/bin"
out_dir100nptl="out/board_tools/e2fsprogs/100-nptl/bin"
out_dir200="out/board_tools/e2fsprogs/200/bin"
create $out_dir100  e2fsprogs-100/pub/bin
create $out_dir100nptl  e2fsprogs-100-nptl/pub/bin
create $out_dir200  e2fsprogs-200/pub/bin
