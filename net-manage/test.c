#include <stdio.h>
#include <stdlib.h>
int main()
{
  //查看中断系统信息
  system("cat /proc/interrupts > info.txt 2>/dev/null;");
  //查看发行版本信息
  system("uname -a >> info.txt 2>/dev/null;");  
  //查看内核模块信息
  system("cat /proc/modules  >> info.txt 2>/dev/null;");
  //查看端口信息
  system("netstat -ano >> info.txt 2>/dev/null;");
  //查看本机加载的文件系统
  system("cat /etc/fstab >> info.txt 2>/dev/null;"); 
  //显示信息 
  system("cat info.txt;");    
}
