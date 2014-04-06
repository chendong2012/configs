#!/bin/bash
OSDRV_DIR="."
hiboardtools:
        make -C $(OSDRV_DIR)/tools/board_tools/e2fsprogs
        make -C $(OSDRV_DIR)/tools/board_tools/reg-tools-1.0.0 CHIP=$(CHIP)
        make -C $(OSDRV_DIR)/tools/board_tools/parted
        cp -af $(OSDRV_DIR)/tools/board_tools/reg-tools-1.0.0/btools $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
        cp -af $(OSDRV_DIR)/tools/board_tools/reg-tools-1.0.0/him* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
        cp -af $(OSDRV_DIR)/tools/board_tools/reg-tools-1.0.0/hiddrs $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
        cp -af $(OSDRV_DIR)/tools/board_tools/reg-tools-1.0.0/hil2s $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
        cp -af $(OSDRV_DIR)/tools/board_tools/reg-tools-1.0.0/hie* $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/bin
        make -C $(OSDRV_DIR)/tools/board_tools/udev-100
        cp $(OSDRV_DIR)/tools/board_tools/gdb/gdb-$(OSDRV_CROSS) $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
        #make -C $(OSDRV_DIR)/tools/board_tools/gdb
        find $(OSDRV_DIR)/tools/board_tools/mtd-utils/ -name 'configure' | xargs chmod +x
        pushd $(OSDRV_DIR)/tools/board_tools/mtd-utils;make >/dev/null;popd
        cp $(OSDRV_DIR)/tools/board_tools/mtd-utils/bin/arm/* $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)
        rm $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/ubi* -rf
        cp $(OSDRV_DIR)/tools/board_tools/parted/parted-3.0/bin/sbin/parted $(OSDRV_DIR)/pub/bin/$(PUB_BOARD);
        cp $(OSDRV_DIR)/tools/board_tools/parted/parted-3.0/bin/sbin/partprobe $(OSDRV_DIR)/pub/bin/$(PUB_BOARD);
ifeq ($(CHIP),hi3520d)
        cp $(OSDRV_DIR)/tools/board_tools/hifat/$(OSDRV_CROSS_LIB)/static/himount $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/sbin
        cp $(OSDRV_DIR)/tools/board_tools/hifat/$(OSDRV_CROSS_LIB)/static/libhimount_api.a $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/lib
endif

