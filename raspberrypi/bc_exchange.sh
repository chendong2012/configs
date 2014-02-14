#!/bin/bash
if [ "$1" == "" ]; then
	echo "please input the dir for broadcast infos"
	exit
fi

cd "$1"
allitems=`ls`

exit
for item in $allitems
do
	/home/xbian/bin/bc_output.sh "$item"
	sleep 1
done
