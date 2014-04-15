#!/bin/bash
echo "SPI Flash 烧写方法如下:"
echo "1. 在内存中运行起来之后在超级终端中输入:"
hisilicon# mw.b 0x82000000 ff 0x100000
echo "/* 对内存初始化*/"
hisilicon# tftp 0x82000000 u-boot-hi3521_930MHz.bin
echo "/*U-boot下载到内存*/"

hisilicon# sf probe 0
echo "/*探测并初始化SPI flash, 擦除０　到１Ｍ的空间*/"
hisilicon# sf erase 0x0 0x100000
echo "/*擦除 1M大小*/"
hisilicon# sf write 0x82000000 0x0 0x100000
echo "/*从内存写入SPI Flash*/"
echo "2. 上述步骤操作完成后,重启系统可以看到 U-boot 烧写成功。"


3、烧写映像文件到SPI Flash
    以16M SPI Flash为例。
    1）地址空间说明
        |      1M       |      4M       |      11M      |

        |---------------|---------------|---------------|

        |     boot      |     kernel    |     rootfs    |

        以下的操作均基于图示的地址空间分配，您也可以根据实际情况进行调整。
    2）烧写u-boot
        sf probe 0
        mw.b 82000000 ff 100000
        tftp 0x82000000 u-boot-hi3520D.bin
        sf probe 0
        sf erase 0 100000
        sf write 82000000 0 100000	
        reset    
    3）烧写内核
        mw.b 82000000 ff 400000
        tftp 82000000 uImage_3520d
        sf probe 0
        sf erase 100000 400000
        sf write 82000000 100000 400000
    4)烧写文件系统
        mw.b 82000000 ff b00000
        tftp 0x82000000 rootfs_3520d_64k.jffs2
        sf erase 500000 b00000
        sf write 82000000 500000 b00000
    5）设置启动参数
        setenv bootargs 'mem=64M console=ttyAMA0,115200 root=/dev/mtdblock2 rootfstype=jffs2 mtdparts=hi_sfc:1M(boot),4M(kernel),11M(rootfs)'
        setenv bootcmd 'sf probe 0;sf read 0x82000000 0x100000 0x400000;bootm 0x82000000'
        sa
4、启动新系统
    reset	# 重启进入新系统。
