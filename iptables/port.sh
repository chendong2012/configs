#!/bin/bash
if [[ "$2" == "on" ]]; then
	echo "open the port:$1"
	sudo /sbin/iptables -I INPUT -p tcp --dport $1 -j ACCEPT
fi

if [[ "$2" == "off" ]]; then
	echo "close the port:$1"
	sudo /sbin/iptables -I INPUT -p tcp --dport $1 -j REJECT
fi
