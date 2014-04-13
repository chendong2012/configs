#!/bin/bash
#e2fsprogs  gdb  hifat  mtd-utils  parted  reg_tool  udev
#1
pushd out/rootfs
rm -rf .git
git init
git add .
git commit -m "init rootfs"
popd

function create
{
cp out/board_tools/e2fsprogs/$1/sbin/* out/rootfs/$1/sbin/

#2
cp out/board_tools/gdb/$1/* out/rootfs/$1/sbin/

#3
cp out/board_tools/hifat/$1/lib/* out/rootfs/$1/lib/
cp out/board_tools/hifat/$1/sbin/* out/rootfs/$1/sbin/

#4
cp out/board_tools/mtd-utils/$1/bin/* out/rootfs/$1/bin/

#5
cp out/board_tools/parted/$1/sbin/* out/rootfs/$1/sbin/

#6
cp -af out/board_tools/reg_tool/$1/bin/* out/rootfs/$1/bin/

#7
mkdir -p out/board_tools/udev/$1/etc/udev/rules.d
mkdir -p out/rootfs/$1/etc/udev/rules.d
cp out/board_tools/udev/$1/bin/* out/rootfs/$1/bin/
cp out/board_tools/udev/$1/etc/udev/udev.conf out/rootfs/$1/etc/udev/
cp out/board_tools/udev/$1/etc/udev/rules.d/* out/rootfs/$1/etc/udev/rules.d/
}
create 100
create 100-nptl
create 200
