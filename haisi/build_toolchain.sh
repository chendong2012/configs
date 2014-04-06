#!/bin/bash
rm -rf pub/toolchain
mkdir -p pub/toolchain
cp dxt/toolchain/arm-hisiv100-linux/cross.install toolchain/arm-hisiv100-linux/cross.install

cp dxt/toolchain/arm-hisiv200-linux/cross.install toolchain/arm-hisiv200-linux/cross.install

cp dxt/toolchain/arm-hisiv100nptl-linux/cross.install toolchain/arm-hisiv100nptl-linux/cross.install

pushd toolchain/arm-hisiv100-linux
./cross.install
popd

pushd toolchain/arm-hisiv200-linux
./cross.install
popd

pushd toolchain/arm-hisiv100nptl-linux
./cross.install
popd
