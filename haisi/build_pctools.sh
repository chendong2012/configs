#!/bin/bash
mkdir -p out/pc_tools/bin
make -C tools/pc_tools/mkyaffs2image301 clean
make -C tools/pc_tools/mkyaffs2image301

cp tools/pc_tools/mkyaffs2image301/mkyaffs2image out/pc_tools/bin/
cp tools/pc_tools/mkfs.jffs2 out/pc_tools/bin/
cp tools/pc_tools/mkfs.cramfs out/pc_tools/bin/
cp tools/pc_tools/mkimage out/pc_tools/bin/

pushd tools/pc_tools/squashfs4.2
make clean
make
popd

cp tools/pc_tools/squashfs4.2/mksquashfs out/pc_tools/bin/
