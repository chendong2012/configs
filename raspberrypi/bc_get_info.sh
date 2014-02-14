#!/bin/bash
if [ "$2" == "" ]; then
	echo "please input paras area dir"
	exit
fi

if [ ! -e "$2" ]; then
mkdir -p "$2"
fi
cd $2
webaddr="http://61.135.159.21/cgi-bin/weather?""$1"
name=`echo "$webaddr" | sed 's/.*\///g'`
if [ -e "$name" ]; then
	rm -rf "$name"
fi

wget "$webaddr"
iconv -f GB2312 -t UTF-8 "$name" > temp
rm -rf "$name"
mv temp "$name"
cat "$name"
