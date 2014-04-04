#!/bin/bash
OSDRV_DIR="."
TOOLCHAIN_RUNTIME_LIB="armv7a_soft"

help="Usage:$0 new|clean"
OUT_DIR="out"

function unpack()
{
	tar xzf $OSDRV_DIR/rootfs_scripts/rootfs.tgz -C $OSDRV_DIR/$OUT_DIR
	mv $OSDRV_DIR/$OUT_DIR/rootfs $OSDRV_DIR/$OUT_DIR/$PUB_ROOTFS
	tar xzf $OSDRV_DIR/toolchain/$OSDRV_CROSS/runtime_lib/$TOOLCHAIN_RUNTIME_LIB/$TOOLCHAIN_RUNTIME_LIB_C \
		-C $OSDRV_DIR/$OUT_DIR/$PUB_ROOTFS
	tar xzf $OSDRV_DIR/toolchain/$OSDRV_CROSS/runtime_lib/$TOOLCHAIN_RUNTIME_LIB/lib.stdc++.tgz \
		-C $OSDRV_DIR/$OUT_DIR/$PUB_ROOTFS
}

echo "rm -rf $OUT_DIR/rootfs"

rm -rf $OUT_DIR/rootfs*
PUB_ROOTFS="rootfs_200"
OSDRV_CROSS="arm-hisiv200-linux"
TOOLCHAIN_RUNTIME_LIB_C="lib.glibc.tgz"
unpack

PUB_ROOTFS="rootfs_100"
OSDRV_CROSS="arm-hisiv100-linux"
TOOLCHAIN_RUNTIME_LIB_C="lib.uClibc.tgz"
unpack

PUB_ROOTFS="rootfs_100nptl"
OSDRV_CROSS="arm-hisiv100nptl-linux"
TOOLCHAIN_RUNTIME_LIB_C="lib.uClibc.tgz"
unpack
echo "ok"
