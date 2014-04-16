#!/bin/bash
PATH=$PATH:$PWD/out/toolchain/arm-hisiv100-linux/target/bin:$PWD/out/toolchain/nptl/arm-hisiv100-linux/target/bin:$PWD/out/toolchain/arm-hisiv200-linux/target/bin:$PWD/out/pc_tools/bin
export PATH

match=`rpm -q libstdc++-statics | grep -o "not installed"`
if [ $match == "not installed" ]; then
	exit 1
#sudo yum install ld-linux.so.2
#sudo yum install libz.so.1
fi
echo $PATH
exit 0
