#!/bin/bash
TAG="[mail_post.sh]"
logfile="/home/chend/.mailinfo"
mailfile="/home/chend/mail.html"
#$1 表示服务器的信息来源目录
serversdir=($*)
#总共有多少个服务器
nums=$#
j=0
temp=0
#用来判断，每个服务器是否有mail要发送,如果有就加1
for((i=0;i<$nums;i++)) {
	grep mailenable "${serversdir[$i]}" -nr
	temp=`echo $?`
	if [ "$temp" == "0" ]; then
		((temp+=1))
	fi
}

#等每个服务器都编译完毕以及，已生成表格, 如果和上面统计出来的数字是一样的，表示所有都已编完了
#最后生成html并发送出去
if [ -e "$logfile/finish" ]; then
	currnum=`cat "$logfile/finish"`
	if [ "$temp" == "$currnum" ]; then
		body_end.sh 		>> "$mailfile"
		html_end.sh 		>> "$mailfile"
		mutt -s 项目编译结果 -e 'set content_type=text/html' chend@gionee.com < /home/chend/mail.html
	fi
fi
rm -rf /home/chend/.mailinfo
echo "$TAG------------------------end-----`date`"
