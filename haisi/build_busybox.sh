#!/bin/bash
BUSYBOX_CFG="busybox_cfg_godarm_nptl"
#BUSYBOX_CFG="busybox_cfg_godarm_v200"
#BUSYBOX_CFG="busybox_cfg_hi3520d_nptl_mini"

if [ ! -d out/busybox ]; then
	mkdir -p out/busybox
fi

echo "build busybox......................."
if [ ! -d busybox/busybox-1.16.1 ]; then
	tar xzf busybox/busybox-1.16.1.tgz -C busybox
fi

cp dxt/busybox/busybox-1.16.1/Makefile busybox/busybox-1.16.1/Makefile

cp busybox/busybox-1.16.1/$BUSYBOX_CFG busybox/busybox-1.16.1/.config

pushd busybox/busybox-1.16.1
make -j 20 >/dev/null;
popd

make -C busybox/busybox-1.16.1 install
cp -af busybox/busybox-1.16.1/_install/* out/busybox
