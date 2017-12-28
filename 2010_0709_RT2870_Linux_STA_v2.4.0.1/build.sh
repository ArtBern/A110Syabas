#!/bin/sh
. ./build.env
datestr=`date +%Y-%m-%dT%H.%M.%S`
logfile=build_${datestr}.log
#echo "Building, log: $logfile"
#exec > $logfile 2>&1

export LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/3.4.6/

make
