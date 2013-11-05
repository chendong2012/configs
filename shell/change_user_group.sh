#!/bin/bash
# 将用户添加到附加组。
echo "first para is user, second para is group"
if [ $# != 2 ]; then
echo "first para is user, second para is group"
exit
fi
echo "sudo usermod -a $1 -G $2"
sudo usermod -a $1 -G $2
#sudo usermod -a pi -G audio
