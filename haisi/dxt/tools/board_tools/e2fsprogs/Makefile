OSDRV_CROSS_nptl=arm-hisiv100nptl-linux
OSDRV_CROSS_200=arm-hisiv200-linux

OSDRV_CROSS_CFLAGS=-march=armv7-a -mcpu=cortex-a9
INST=$(PWD)/e2fsprogs-1.41.11/inst
all:200 nptl
200:
	rm e2fsprogs-1.41.11 -rf
	tar xzf e2fsprogs-1.41.11.tar.gz
	mkdir $(INST) -p
	rm -rf ../../../out/e2fsprogs/bin_200
	mkdir -p ../../../out/e2fsprogs/bin_200
	pushd e2fsprogs-1.41.11;./configure --host=$(OSDRV_CROSS_200) CFLAGS="$(OSDRV_CROSS_CFLAGS)" --prefix=$(INST) --disable-tls >/dev/null;make -j 20 >/dev/null;make install >/dev/null;popd
	cp $(INST)/sbin/mkfs.ext2 ../../../out/e2fsprogs/bin_200
	cp $(INST)/sbin/mkfs.ext3 ../../../out/e2fsprogs/bin_200
	cp $(INST)/sbin/mkfs.ext4 ../../../out/e2fsprogs/bin_200
	cp $(INST)/sbin/mke2fs ../../../out/e2fsprogs/bin_200
	rm e2fsprogs-1.41.11 -rf

nptl:
clean:
	rm e2fsprogs-1.41.11 -rf
