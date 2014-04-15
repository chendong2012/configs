#!/bin/bash
. env.sh
MKBOOT="mkboot-hi3520d.sh"
REG_INFO_BIN="reg_info_Hi3520D-bvt_No1_660_330_660_ddr_innerFEPHY.bin"
REG_INFO_BIN1="reg_info_Hi3515A-bvt_No1_600_300_600_ddr_innerFEPHY.bin"
UBOOT_IMAGE_BIN="u-boot_hi3520d.bin"
CHIP_UBOOT_CFG="hi3520d_config"

compile_100="CROSS_COMPILE=arm-hisiv100-linux-"
compile_100_nptl="CROSS_COMPILE=arm-hisiv100nptl-linux-"
compile_200="CROSS_COMPILE=arm-hisiv200-linux-"

help="Usage: $0 cfg{new|remake|r|n}"

if [ "$1" == "--help" ] || [ "$1" == "-h" ] || [ "$#" == "0" ]; then
	echo $help
	exit 0
fi

#$1 100 or 100-nptl or 200
#$2 operate new or remake or other ..
#$3  cross-compile
function create
{
	if [ ! -d uboot/"$1" ]; then
		pushd uboot
		tar xzvf u-boot-2010.06.tgz
		mv u-boot-2010.06 "$1"
		popd
	fi

	mkdir -p out/uboot/$1

	cp tools/pc_tools/uboot_tools/$MKBOOT out/uboot/"$1"
	cp tools/pc_tools/uboot_tools/$REG_INFO_BIN out/uboot/"$1"
	cp tools/pc_tools/uboot_tools/$REG_INFO_BIN1 out/uboot/"$1"

	case "$2" in
	n*)
		pushd uboot/$1
		echo $PWD
		make ARCH=arm "$3" "$CHIP_UBOOT_CFG"
		make ARCH=arm "$3" -j 20
		cp u-boot.bin ../../out/uboot/$1
		popd
		pushd out/uboot/$1
		chmod a+x ./$MKBOOT
		./$MKBOOT $REG_INFO_BIN $REG_INFO_BIN1 $UBOOT_IMAGE_BIN
		popd
		;;
	*)
		echo $help
		return
	esac
	echo "ok!"
}


create "100" "$1" "$compile_100"
create "100-nptl" "$1" "$compile_100_nptl"
create "200" "$1" "$compile_200"
echo =====================end========================


