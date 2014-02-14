#!/bin/bash
if [ $1 -eq "" ]; then
	echo "please input the dir for broadcast infos"
fi

cd $1
allitems=`ls`

for item in $allitems
do
	/opt/hub/configs/raspberrypi/bc_output.sh "$item"
	sleep 1
done
