#!/bin/bash
mkdir -p out/e2fsprogs/bin_200
mkdir -p out/e2fsprogs/bin_nptl
cp dxt/tools/board_tools/e2fsprogs/Makefile tools/board_tools/e2fsprogs/Makefile 
pushd tools/board_tools/e2fsprogs
make 200
popd
