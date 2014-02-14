#!/bin/bash
cd /media/usb0/mygithub/hub/configs/raspberrypi/
git pull

cp  /home/xbian/bin/bc* /media/usb0/mygithub/hub/configs/raspberrypi/
git add .
git commit -m "update"
git push
