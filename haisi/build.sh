#!/bin/bash
echo "=======================build pc tools========================"
./build_pctools.sh

echo "=======================build board tools========================"
./build_e2f.sh
./build_mtd-utils.sh
./build_gdb.sh
./build_parted.sh
./build_reg.sh
./build_udev.sh
./build_hifat.sh

echo "=======================build busybox tools========================"
./build_busybox.sh

echo "=======================build rootfs and package rootfs ========================"
./build_rootfs.sh
./package_rootfs.sh

