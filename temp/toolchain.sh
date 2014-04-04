#!/bin/bash
arm100="arm-hisiv100-linux"
armnptl="arm-hisiv100nptl-linux"
arm200="arm-hisiv200-linux"


if [ $# -ne 2 ]; then
	echo "the first param is the toolchain source dir"
	echo "the second param is the toochain setup   dir"
	exit 1
fi

echo "$1 is the toolchain source dir"
echo "$2 is the toochain setup   dir"

export TOP_DIR="$2"
export TOP_DIR_NPTL="$2/nptl"

script_path=/home/$USER/bin
echo "delete all files in $2"
sleep 2
#sudo rm -rf "$2/*"

cd $1
echo "install $arm100..."
pushd "$arm100"
cp "$script_path/cross.install-$arm100" ./cross.install
./cross.install
popd
exit 0



echo "install $armnptl..."
pushd "$armnptl"
cp "$script_path/cross.install-$armnptl" ./cross.install
./cross.install
popd


echo "install $arm200..."
pushd "$arm200"
cp "$script_path/cross.install-$arm200" ./cross.install
./cross.install
popd
