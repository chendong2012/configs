#!/bin/bash
. env.sh
match=`which mkimage 2>&1 | grep -o "no mkimage"`
if [ "$match" != "" ]; then
	./build_pctools.sh
fi
help="Usage: $0 cfg{mini|full} {new|menuconfig|touch|cpcfg|new|remake|r|n}"

compile_100="CROSS_COMPILE=arm-hisiv100-linux-"
compile_100_nptl="CROSS_COMPILE=arm-hisiv100nptl-linux-"
compile_200="CROSS_COMPILE=arm-hisiv200-linux-"
CHIP_KERNEL_CFG="hi3520d_full_defconfig"

if [ "$1" == "--help" ] || [ "$1" == "-h" ] || [ "$#" == "0" ]; then
	echo $help
	exit 0
fi

#$1 100 or 100_nptl or 200
#$2 mini or full

function create
{
	pushd kernel
	if [ ! -d "$1"-"$2" ]; then
		tar xzvf linux-3.0.y.tgz 
		mv linux-3.0.y "$1"-"$2"
	fi
	popd

	case "$2" in
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

	pushd kernel/$1-$2


	case "$3" in
	  clean)
		make ARCH=arm "$4" clean
		;;
	  r*)
		make ARCH=arm "$4" -j 20
		make ARCH=arm "$4" uImage -j 20
		mkdir -p ../../out/kernel/$1-$2
		cp arch/arm/boot/uImage ../../out/kernel/$1-$2/
		;; 	
	  n*)
		make ARCH=arm "$4" clean
		make ARCH=arm "$4" "$CHIP_KERNEL_CFG"
		make ARCH=arm "$4" -j 20
		make ARCH=arm "$4" uImage -j 20
		mkdir -p ../../out/kernel/$1-$2
		cp arch/arm/boot/uImage ../../out/kernel/$1-$2/
		;;
	  menuconfig)
		if [ ! -f .config ]; then
			echo "[no config file]: make menuconfig error!!!"
			popd
			exit 1
		fi
		make ARCH=arm "$4" menuconfig
		;;
	  touch)
		find . | xargs touch
		;;
	  cpcfg)
		if [ $2 == "mini" ]; then
			echo "cp arch/arm/configs/hi3520d_mini_defconfig .config"
			cp arch/arm/configs/hi3520d_mini_defconfig .config
		fi

		if [ $2 == "full" ]; then
			echo "cp arch/arm/configs/hi3520d_full_defconfig .config"
			cp arch/arm/configs/hi3520d_full_defconfig .config
		fi
		;;
	unpack)
		popd
		rm -rf kernel/"$1"-"$2"
		tar xzvf kernel/linux-3.0.y.tgz -C kernel/
		mv linux-3.0.y "$1"-"$2"
		exit 1
		;;
	  *)
		echo $help
		popd
		exit 1
	esac
	popd
}
create "100" $1 $2 "$compile_100"
create "100_nptl" $1 $2 "$compile_100_nptl"
create "200" $1 $2 "$compile_200"
