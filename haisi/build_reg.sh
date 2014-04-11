#!/bin/bash

function create
{
	cp -af btools $1 
	cp -af him* $1
	cp -af hiddrs $1
	cp -af hil2s $1
	cp -af hie* $1
}

out_reg_tool100="$PWD/out/boardtools/reg_tool/100/bin"
out_reg_tool100nptl="$PWD/out/boardtools/reg_tool/100-nptl/bin"
out_reg_tool200="$PWD/out/boardtools/reg_tool/200/bin"

rm -rf out/boardtools/reg_tool

mkdir -p "$out_reg_tool100"
mkdir -p "$out_reg_tool100nptl"
mkdir -p "$out_reg_tool200"

if [ ! -d tools/board_tools/reg-tools-1.0.0-100 ]; then
	cp tools/board_tools/reg-tools-1.0.0 tools/board_tools/reg-tools-1.0.0-100 -a
fi

if [ ! -d tools/board_tools/reg-tools-1.0.0-100-nptl ]; then
	cp tools/board_tools/reg-tools-1.0.0 tools/board_tools/reg-tools-1.0.0-100-nptl -a
fi

if [ ! -d tools/board_tools/reg-tools-1.0.0-200 ]; then
	cp tools/board_tools/reg-tools-1.0.0 tools/board_tools/reg-tools-1.0.0-200 -a
fi

cp dxt/tools/board_tools/reg-tools-1.0.0-100/Makefile  tools/board_tools/reg-tools-1.0.0-100/

cp dxt/tools/board_tools/reg-tools-1.0.0-100-nptl/Makefile tools/board_tools/reg-tools-1.0.0-100-nptl/

cp dxt/tools/board_tools/reg-tools-1.0.0-200/Makefile tools/board_tools/reg-tools-1.0.0-200/

pushd tools/board_tools/reg-tools-1.0.0-100
make clean
make
create $out_reg_tool100
popd

pushd tools/board_tools/reg-tools-1.0.0-100-nptl
make clean
make
create $out_reg_tool100nptl
popd

pushd tools/board_tools/reg-tools-1.0.0-200
make clean
make
create $out_reg_tool200
popd
