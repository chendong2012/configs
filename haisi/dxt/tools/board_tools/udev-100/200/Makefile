OSDRV_CROSS=arm-hisiv200-linux
all:
	tar xzf udev-100.tgz
	make -C udev-100 CROSS_COMPILE=$(OSDRV_CROSS)- 
clean:
	rm udev-100 -rf
 
