#!/bin/sh
. ./build.env
datestr=`date +%Y-%m-%dT%H.%M.%S`
logfile=build_${datestr}.log
echo "Building, log: $logfile"
exec > $logfile 2>&1

make