#!/bin/sh

pwd=$PWD

# clean linux
#echo "Cleaning Kernel..."
#cd $pwd/smp86xx_kernel_source
#./clean.sh

# clean rootfs
#echo "Cleaning Rootfs..."
#cd $pwd/smp86xx_rootfs
#./clean.sh

# clean toolchain
echo "Cleaning Toolchain..."
cd $pwd/smp86xx_toolchain.20080505
./clean.sh