#!/bin/bash
rm -rf out/board_tools/gdb/100
rm -rf out/board_tools/gdb/100-nptl
rm -rf out/board_tools/gdb/200

mkdir -p out/board_tools/gdb/100
mkdir -p out/board_tools/gdb/100-nptl
mkdir -p out/board_tools/gdb/200

cp tools/board_tools/gdb/gdb-arm-hisiv100-linux  out/board_tools/gdb/100
cp tools/board_tools/gdb/gdb-arm-hisiv100nptl-linux out/board_tools/gdb/100-nptl
cp tools/board_tools/gdb/gdb-arm-hisiv200-linux out/board_tools/gdb/200
