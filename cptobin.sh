#!/bin/bash
	if [ "$1" == "" ]; then
		echo please input para
		exit
	fi
	echo copy $1 to ~/bin
	cp ./gitcfg.sh ~/bin/
	echo end
