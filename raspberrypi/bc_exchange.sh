#!/bin/bash
if [ "$1" == "" ]; then
	echo "please input the dir for broadcast infos"
	exit
fi

cd "$1"
allitems=(`ls`)
echo ${#allitems[*]}
for ((i=0;i<${#allitems[*]};i++)) {
	echo "item is ${allitems[$i]}"

	bc_output.sh "${allitems[$i]}" "$1"
	sleep 1
}
