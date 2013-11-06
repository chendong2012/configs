#!/bin/bash
mailfile="/home/chend/mail.html"
TAG="[mail_init.sh]"
echo "$tag:begin `date`" 
mkdir -p /home/chend/.mailinfo

if [ -e /home/chend/.mailinfo/finish ]; then
	echo "$TAG:end(for:run more than one times) `date`" 
	exit
fi
echo 0 > /home/chend/.mailinfo/finish
html.sh 								> "$mailfile"
body.sh 								>> "$mailfile"
br.sh 									>> "$mailfile"
echo "$TAG:end(create html head...) `date`" 
