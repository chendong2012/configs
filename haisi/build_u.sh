#!/bin/bash
MKBOOT="mkboot-hi3520d.sh"
REG_INFO_BIN="reg_info_Hi3520D-bvt_No1_660_330_660_ddr_innerFEPHY.bin"
REG_INFO_BIN1="reg_info_Hi3515A-bvt_No1_600_300_600_ddr_innerFEPHY.bin"
CHIP_BOOT_CFG="hi3520d_config"
UBOOT_IMAGE_BIN="u-boot_hi3520d.bin"
CHIP_UBOOT_CFG="hi3520d_config"
compile="CROSS_COMPILE=arm-hisiv200-linux-"

help="Usage: $0 cfg {new|remake|r|n}"

if [ "$1" == "--help" ] || [ "$1" == "-h" ] || [ "$#" == "0" ]; then
	echo $help
	exit 0
fi

if [ ! -d uboot/u-boot-2010.06 ]; then
	pushd uboot
	tar xzvf u-boot-2010.06.tgz
	popd
fi

if [ ! -d pub/uboot ]; then
	mkdir -p pub/uboot
fi

cp tools/pc_tools/uboot_tools/$MKBOOT pub/uboot/
cp tools/pc_tools/uboot_tools/$REG_INFO_BIN pub/uboot/
cp tools/pc_tools/uboot_tools/$REG_INFO_BIN1 pub/uboot/

case "$1" in
n*)
	pushd uboot/u-boot-2010.06
	make -C ARCH=arm "$compile" "$CHIP_UBOOT_CFG"
	make ARCH=arm "$compile" -j 20
	cp u-boot.bin pub/uboot/
	popd
	pushd pub/uboot
	chmod a+x ./$MKBOOT
	./$MKBOOT $REG_INFO_BIN $REG_INFO_BIN1 $UBOOT_IMAGE_BIN
	popd
	;;
*)
	echo $help
	exit 1
esac
echo "ok!"
