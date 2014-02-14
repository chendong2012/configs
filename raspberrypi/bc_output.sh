#!/bin/bash
today="今天;"
tomo="明天;"
info=""

for line in `cat "$1"`
do
	((i++))
	if [ "$i" -eq 1 ]; then
		temp=`echo $line |  sed 's/^|//g'`
		info="$temp"
	else

		if [ "$i" -eq 2 ]; then
			ctx=`echo "$line" | sed -n 's/\(.*:\)\(.*\)/\2/p'`
		info="$info"";""$today""$line"
		fi

		if [ "$i" -eq 3 ]; then
			ctx=`echo "$line" | sed -n 's/\(.*:\)\(.*\)/\2/p'`
		info="$info"";""$tomo""$line"
		fi
	fi

done
echo $info
#speech.sh "$info"
