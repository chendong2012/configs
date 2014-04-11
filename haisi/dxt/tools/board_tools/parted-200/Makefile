TOOLS_DIR=$(PWD)
OSDRV_CROSS=arm-hisiv200-linux
all:
	rm parted-3.0-200 -rf;
	rm parted-3.0 -rf;
	tar -zxf parted-3.0-hisi-bvt.tar.gz;
	mkdir -p $(TOOLS_DIR)/parted-3.0/bin/;
	pushd $(TOOLS_DIR)/parted-3.0/;\
		./configure --host=arm-linux --enable-static --disable-shared CC=$(OSDRV_CROSS)-gcc AR=$(OSDRV_CROSS)-ar LD=$(OSDRV_CROSS)-ld\
		--disable-device-mapper --without-readline --prefix=$(TOOLS_DIR)/bin > /dev/null;\
		make -j 20 > /dev/null;make install > /dev/null;\
	popd

clean:
	rm parted-3.0-200 -rf;
