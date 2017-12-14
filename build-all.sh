#!/bin/sh

pwd=$PWD
# build toolchain
echo "Building Toolchain..."
cd $pwd/smp86xx_toolchain.20080505
./build.sh

# build rootfs
#echo "Building Rootfs..."
#cd $pwd/smp86xx_rootfs
#./build.sh

# build linux
#echo "Building Kernel..."
#cd $pwd/linux-2.6.15
#./build.sh