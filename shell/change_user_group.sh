#!/bin/bash
echo "first para is user, second para is group"
if [ $# != 2 ]; then
echo "first para is user, second para is group"
exit
fi
echo "sudo usermod -a $1 -G $2"
sudo usermod -a $1 -G $2
#sudo usermod -a pi -G audio
