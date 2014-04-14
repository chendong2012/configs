#!/bin/bash
#e2fsprogs  gdb  hifat  mtd-utils  parted  reg_tool  udev
#1
if [ ! -d out/board_tools/e2fsprogs ] || \
	[ ! -d out/board_tools/hifat ] || \
	[ ! -d out/board_tools/mtd-utils ] || \
	[ ! -d out/board_tools/parted ] || \
	[ ! -d out/board_tools/reg_tool ] || \
	[ ! -d out/board_tools/udev ]; then
	echo "please install tools ...!!!!!"
	exit
fi
pushd out/rootfs
git clean -f
rm -rf .git
git init
git add .
git commit -m "init rootfs"
popd

function create
{
	c_dir=$1
	c_dir1=$1
	if [ "$c_dir1" == "100-nptl_mini" ]; then
		c_dir="100-nptl"
	fi
echo 1
	cp -af out/board_tools/e2fsprogs/$c_dir/sbin out/rootfs/"$c_dir1"/
echo 2
#cp out/board_tools/gdb/$c_dir/* out/rootfs/$c_dir1/sbin/
echo 3
	cp -af out/board_tools/hifat/$c_dir/lib out/rootfs/$c_dir1/
	cp -af out/board_tools/hifat/$c_dir/sbin out/rootfs/$c_dir1/
echo 4
	cp -af out/board_tools/mtd-utils/$c_dir/bin out/rootfs/$c_dir1/
echo 5
	cp -af out/board_tools/parted/$c_dir/sbin out/rootfs/$c_dir1/
echo 6
	cp -af out/board_tools/reg_tool/$c_dir/bin out/rootfs/$c_dir1/
echo 7
	mkdir -p out/board_tools/udev/$c_dir/etc/udev/rules.d
	mkdir -p out/rootfs/$c_dir1/etc/udev/rules.d
	cp -af out/board_tools/udev/$c_dir/bin out/rootfs/$c_dir1/
	cp -af out/board_tools/udev/$c_dir/etc/udev/udev.conf out/rootfs/$c_dir1/etc/udev/
	cp -af out/board_tools/udev/$c_dir/etc/udev/rules.d out/rootfs/$c_dir1/etc/udev/

	cp -af out/busybox/$c_dir1 out/rootfs/
}
#create 100
create 100-nptl
create 100-nptl_mini
create 200
