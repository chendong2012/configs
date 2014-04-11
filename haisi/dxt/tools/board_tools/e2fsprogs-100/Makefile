OSDRV_CROSS_nptl=arm-hisiv100nptl-linux
OSDRV_CROSS_200=arm-hisiv200-linux
OUT_DIR=../../../out/boardtools/e2fsprogs

OSDRV_CROSS_CFLAGS=-march=armv7-a -mcpu=cortex-a9
INST=$(PWD)/e2fsprogs-1.41.11/inst
all:200 nptl
200:
	rm e2fsprogs-1.41.11 -rf
	tar xzf e2fsprogs-1.41.11.tar.gz
	mkdir $(INST) -p
	rm -rf $(OUT_DIR)/bin_200
	mkdir -p $(OUT_DIR)/bin_200
	pushd e2fsprogs-1.41.11;./configure --host=$(OSDRV_CROSS_200) CFLAGS="$(OSDRV_CROSS_CFLAGS)" --prefix=$(INST) --disable-tls >/dev/null;make -j 20 >/dev/null;make install >/dev/null;popd
	cp $(INST)/sbin/mkfs.ext2 $(OUT_DIR)/bin_200
	cp $(INST)/sbin/mkfs.ext3 $(OUT_DIR)/bin_200
	cp $(INST)/sbin/mkfs.ext4 $(OUT_DIR)/bin_200
	cp $(INST)/sbin/mke2fs $(OUT_DIR)/bin_200
	rm e2fsprogs-1.41.11 -rf

nptl:
	rm e2fsprogs-1.41.11 -rf
	tar xzf e2fsprogs-1.41.11.tar.gz
	mkdir $(INST) -p
	rm -rf $(OUT_DIR)/bin_nptl
	mkdir -p $(OUT_DIR)/bin_nptl
	pushd e2fsprogs-1.41.11;./configure --host=$(OSDRV_CROSS_nptl) CFLAGS="$(OSDRV_CROSS_CFLAGS)" --prefix=$(INST) --disable-tls >/dev/null;make -j 20 >/dev/null;make install >/dev/null;popd
	cp $(INST)/sbin/mkfs.ext2 $(OUT_DIR)/bin_nptl
	cp $(INST)/sbin/mkfs.ext3 $(OUT_DIR)/bin_nptl
	cp $(INST)/sbin/mkfs.ext4 $(OUT_DIR)/bin_nptl
	cp $(INST)/sbin/mke2fs $(OUT_DIR)/bin_nptl
	rm e2fsprogs-1.41.11 -rf
clean:
	rm e2fsprogs-1.41.11 -rf
