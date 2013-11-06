#!/bin/bash
#第一个参数表示服务器目录(只有一个参数，如果有二个服务器
#那么重新调这个函数了),这个目录主要是提供所有编译结果信息
#完成的工作:在指定服务器中，把编出来的各个项目进行分析，
#生成MAIL要求的表格
#处理完后，会做好标记，表示已处理。再次调用将不再有任何处理
mailfile="/home/chend/mail.html"
TAG="[mail_handle.sh]"
logfile=/home/chend/.mailinfo

#第一个表格有8行, 如果要修改在这里修改
itemrows=8
ctxdir="$1"
num="$ctxdir/prjs-num"
finish="$ctxdir/prjs-finish"

if [ -e "$ctxdir/end" ];then
	echo "$TAG:服务器已处理完毕!!!"
fi
if [ ! -e "$num" ] || [ ! -e "$finish" ]; then
	echo "$TAG:服务器:$ctxdir 目录下没有找到prjs-num或是prjs-finish 这二个文件,退出先!!!"
	exit
fi

temp1=`cat $num`
temp2=`cat $finish`

if [ "$temp1" == "0" ]; then
	echo "$TAG:这个服务器,没有要发EMAIL的项目!!!"
	exit
fi
#用来检查这个指定服务器项目是否已经编完
if [ "$temp1" != "$temp2" ]; then
	echo "$TAG:这个服务器还在编译中.....,先退出"
	exit
fi

ifnull=`find "$ctxdir" -name "ok" -o -name "fail"`
if [ "$ifnull" == "" ]; then
	echo "$TAG:cannot find ok or fail files"
	echo "$TAG:服务器已编完毕，但是没有发现ok和fail文件"
	touch "$ctxdir/end"
	exit
fi

###############################################################################################
results=(`find "$ctxdir" -name "ok" -o -name "fail"`)
for maildetail in ${results[*]}
do
#	infolines=`cat "$maildetail" | wc -l`
	items=(`cat "$maildetail"`)
#	echo "${items[@]}"
	table.sh 								>> "$mailfile"
	row_green.sh "项目" 		"${items[0]}" 				>> "$mailfile"
	row.sh       "映像文件" 	"${items[3]}"				>> "$mailfile"
	row.sh       "版本下载时间" 	"${items[4]}" 				>> "$mailfile"
	row.sh       "编译结果" 	"${items[5]}" 				>> "$mailfile"
	row.sh       "ANDROID 流地址" 	"${items[1]}" 				>> "$mailfile"
	row.sh       "PACKAGE 流地址" 	"${items[2]}" 				>> "$mailfile"
	row.sh       "FTP 地址"		"${items[6]}" 				>> "$mailfile"
	row.sh       "FTP 登陆信息"	"用户名：reldn 密码：ksjfsf@off" 	>> "$mailfile"
	row.sh       "备注"		"${items[7]}" 				>> "$mailfile"
	table_end.sh 								>> "$mailfile"
	br.sh 									>> "$mailfile"
done
##############################################################################################
#整个服务器编完加1
currnum=`cat "$logfile/finish"`
((currnum++))
echo "$currnum" > "$logfile/finish"
touch "$ctxdir/end"

echo "$TAG:========================================="
echo "$TAG:这个服务器的所有编的项目已生成表格"
echo "$TAG:编完的服务器总计:$currnum(从1开始)"
echo "$TAG:服务器地址目录:$ctxdir"
echo "$TAG:========================================="
