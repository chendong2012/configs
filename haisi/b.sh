#!/bin/bash

#task [7]       build rootfs
##########################################################################################
hirootfs_build:
        @echo "---------task [7] build rootfs"
        chmod 777 $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/*
        chmod 777 $(OSDRV_DIR)/pub/bin/pc/*
        rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/* -rf
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/console c 5 1
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/ttyAMA0 c 204 64
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/ttyAMA1 c 204 65
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/ttyS000 c 204 64
        pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS);ln -s sbin/init init;popd
        pushd $(OSDRV_DIR)/pub/bin/pc; ./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x40000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_256K);popd
        pushd $(OSDRV_DIR)/pub/bin/pc; ./mkfs.jffs2 -d $(OSDRV_DIR)/pub/$(PUB_ROOTFS) -l -e 0x10000 -o $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(JFFS2_IMAGE_BIN_64K);popd
ifneq ($(CHIP),hi3520d)
        pushd $(OSDRV_DIR)/pub/bin/pc; ./mkyaffs2image $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(YAFFS2_IMAGE_BIN) 1 1;popd
endif
        find $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -name '*svn' | xargs rm -rf
        pushd $(OSDRV_DIR)/pub;tar czf $(PUB_ROOTFS).tgz $(PUB_ROOTFS);rm $(PUB_ROOTFS) -rf;popd
        @echo "---------finish osdrv work"

hirootfs_mini_build:
        @echo "---------task [7] build rootfs"
        chmod 777 $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/*
        chmod 777 $(OSDRV_DIR)/pub/bin/pc/*
        rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/* -rf
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/console c 5 1
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/ttyAMA0 c 204 64
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/ttyAMA1 c 204 65
#       mknod $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/dev/ttyS000 c 204 64
        pushd $(OSDRV_DIR)/pub/$(PUB_ROOTFS);ln -s sbin/init init;popd
        pushd $(OSDRV_DIR)/pub;find $(PUB_ROOTFS)/ -perm +700 ! -name "*.ko" -exec $(OSDRV_CROSS)-strip {} \;;popd
        pushd $(OSDRV_DIR)/pub/bin/pc; ./mksquashfs $(OSDRV_DIR)/pub/$(PUB_ROOTFS) $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(SQUASHFS_IMAGE_BIN_256K) -b 256K -comp xz;popd
        pushd $(OSDRV_DIR)/pub/bin/pc; ./mksquashfs $(OSDRV_DIR)/pub/$(PUB_ROOTFS) $(OSDRV_DIR)/pub/$(PUB_IMAGE)/$(SQUASHFS_IMAGE_BIN_64K) -b 64K -comp xz;popd
        find $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -name '*svn' | xargs rm -rf
        pushd $(OSDRV_DIR)/pub;tar czf $(PUB_ROOTFS).tgz $(PUB_ROOTFS);rm $(PUB_ROOTFS) -rf;popd
        @echo "---------finish osdrv work"

hirootfs_clean:
        rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS)/ -rf

##########################################################################################
#task [8]       clean pub
##########################################################################################
pub_clean:
        rm $(OSDRV_DIR)/pub/$(PUB_IMAGE)/ -rf
        rm $(OSDRV_DIR)/pub/bin/$(PUB_BOARD)/ -rf
        rm $(OSDRV_DIR)/pub/bin/pc/ -rf
        rm $(OSDRV_DIR)/pub/$(PUB_ROOTFS).tgz -f


