#!/bin/bash
PATH=$PATH:$PWD/out/toolchain/arm-hisiv100-linux/target/bin:$PWD/out/toolchain/nptl/arm-hisiv100-linux/target/bin:$PWD/out/toolchain/arm-hisiv200-linux/target/bin:$PWD/out/pc_tools/bin
export PATH

#match=`rpm -q libstdc++-statics | grep -o "not installed"`
#if [ "$match" == "not installed" ]; then
#	exit 1
#fi
#
if [ ! -d out/toolchain ]; then
	./build_toolchain.sh
fi
#sudo yum install ld-linux.so.2
#sudo yum install libz.so.1



cross=`cat ~/.bashrc | grep arm-hisiv`
if [ "$cross" == "" ]; then
echo "PATH=$PATH:$PWD/out/toolchain/arm-hisiv100-linux/target/bin:$PWD/out/toolchain/nptl/arm-hisiv100-linux/target/bin:$PWD/out/toolchain/arm-hisiv200-linux/target/bin:$PWD/out/pc_tools/bin" >> ~/.bashrc
echo "export PATH" >> ~/.bashrc
fi

echo $PATH
