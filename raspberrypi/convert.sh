#!/bin/bash
#引脚本用于把mp4格式转为mp4格式
echo 引脚本用于把mp4格式转为mp4格式
#ls | while read line; do
find . -type f \( -name "*.MPG" -o -name "*.mpg" \) | while read line; do
echo $line
mencoder "$line" -o ${line}.mp4 -oac mp3lame -ovc x264 -of lavf -vf lavcdeint
done
#mencoder 20130706-广东话+才艺表演.MPG -o m001.mp4 -oac mp3lame -ovc x264 -of lavf -vf lavcdeint

