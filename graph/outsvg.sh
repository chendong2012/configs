#!/bin/bash
#功能介绍
echo "第一参数：函数名称:$1"
echo "第二参数：输出名称(不带扩展名):$2"

if [ $# -ne 2 ]; then
	echo "error params"
	exit
fi

calltree list=$1 -b -np `find . -name "*.c"` | tree2dot.sh > "$2.dot"
dot -Tsvg "$2.dot" -o "$2.svg"
dia "$2.svg"
