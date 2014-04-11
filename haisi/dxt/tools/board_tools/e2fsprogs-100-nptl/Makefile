OSDRV_CROSS=arm-hisiv100nptl-linux
OSDRV_CROSS_CFLAGS=-march=armv7-a -mcpu=cortex-a9
e2fdir=e2fsprogs-1.41.11

INST=$(PWD)/$(e2fdir)/inst
all:
	rm $(e2fdir) -rf
	tar xzf e2fsprogs-1.41.11.tar.gz
	mkdir $(INST) -p
	pushd $(e2fdir); ./configure --host=$(OSDRV_CROSS) CFLAGS="$(OSDRV_CROSS_CFLAGS)" --prefix=$(INST) --disable-tls >/dev/null;make -j 20 >/dev/null;make install >/dev/null; popd
clean:
	rm $(e2fdir) -rf
