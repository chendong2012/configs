#!/bin/bash
. env.sh
dir_nptl="busybox-1.16.1_nptl"
dir_200="busybox-1.16.1_200"
dir_nptl_mini="busybox-1.16.1_nptl_mini"

outdir_nptl="100-nptl"
outdir_nptl_mini="100-nptl_mini"
outdir_200="200"

BUSYBOX_CFG_nptl="busybox_cfg_godarm_nptl"
BUSYBOX_CFG_200="busybox_cfg_godarm_v200"
BUSYBOX_CFG_nptl_mini="busybox_cfg_hi3520d_nptl_mini"

function create
{
	echo "build busybox......................."
	if [ ! -d busybox/"$1" ]; then
		tar xzf busybox/busybox-1.16.1.tgz -C busybox
		echo "mv busybox-1.16.1 "$1""
		mv busybox/busybox-1.16.1 busybox/"$1"
	fi

	cp dxt/busybox/"$1"/Makefile busybox/"$1"/Makefile

	cp busybox/"$1"/"$2" busybox/"$1"/.config

	pushd busybox/"$1"
	make -j 20 >/dev/null
	popd

	make -C busybox/"$1" install
	mkdir -p out/busybox/"$3"
	cp -af busybox/"$1"/_install/* out/busybox/"$3"
}

if [ ! -d out/busybox ]; then
	mkdir -p out/busybox
fi

create $dir_nptl $BUSYBOX_CFG_nptl $outdir_nptl
create $dir_nptl_mini $BUSYBOX_CFG_nptl_mini $outdir_nptl_mini
create $dir_200 $BUSYBOX_CFG_200 $outdir_200
