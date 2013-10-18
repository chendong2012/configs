#!/bin/bash
echo "looking .........$1"
if [ $# != 1 ]; then
echo "lookport.sh port"
exit
fi
sudo netstat -altp |grep $1
