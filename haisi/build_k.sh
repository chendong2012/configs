#!/bin/bash
help="Usage: $0 cfg {new|menuconfig|touch|cpcfg|new|remake|r|n}"

compile="CROSS_COMPILE=arm-hisiv200-linux-"
CHIP_KERNEL_CFG="hi3520d_full_defconfig"
echo $CHIP_KERNEL_CFG $compile

if [ "$1" == "--help" ] || [ "$1" == "-h" ] || [ "$#" == "0" ]; then
	echo $help
	exit 0
fi


if [ ! -d linux-3.0.y ]; then
	tar xzvf linux-3.0.y.tgz
fi

case "$1" in
mini)
	CHIP_KERNEL_CFG="hi3520d_mini_defconfig"
	;;
full)
	CHIP_KERNEL_CFG="hi3520d_full_defconfig"
	;;
*)
	echo $help
	exit 1
esac

pushd linux-3.0.y


case "$2" in
  clean)
        make ARCH=arm "$compile" clean
	;;
  r*)
	echo "<======================  make ARCH=arm "$compile" -j 20  ==========================>"
        make ARCH=arm "$compile" -j 20

	echo "make new step 3:"
	echo "<======================  make ARCH=arm "$compile" uImage -j 20  ==========================>"
        make ARCH=arm "$compile" uImage -j 20
	;; 	
  n*)
        make ARCH=arm "$compile" clean
	echo "<======================  make ARCH=arm "$compile" $CHIP_KERNEL_CFG  ==========================>"
        make ARCH=arm "$compile" "$CHIP_KERNEL_CFG"

	echo "<======================  make ARCH=arm "$compile" -j 20  ==========================>"
        make ARCH=arm "$compile" -j 20

	echo "make new step 3:"
	echo "<======================  make ARCH=arm "$compile" uImage -j 20  ==========================>"
        make ARCH=arm "$compile" uImage -j 20
	;;
  menuconfig)
	if [ ! -f .config ]; then
		echo "[no config file]: make menuconfig error!!!"
		popd
		exit 1
	fi
        make ARCH=arm "$compile" menuconfig
	;;
  touch)
        find linux-3.0.y | xargs touch
	;;
  cpcfg)
  	if [ $1 == "mini" ]; then
	echo "cp arch/arm/configs/hi3520d_mini_defconfig .config"
	cp arch/arm/configs/hi3520d_mini_defconfig .config
	fi

  	if [ $1 == "full" ]; then
	echo "cp arch/arm/configs/hi3520d_full_defconfig .config"
	cp arch/arm/configs/hi3520d_full_defconfig .config
	fi
	;;
unpack)
	popd
	tar xzvf linux-3.0.y.tgz
	exit 1
	;;
  *)
	echo $help
	popd
	exit 1
esac
popd
