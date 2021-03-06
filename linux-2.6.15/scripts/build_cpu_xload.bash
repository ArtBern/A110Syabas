#!/bin/bash
set -e
#
if [ $# != 3 ]; then 
    cat <<EOF

Syntax: $0 cpupkg 000b ES4_dev

The cpupkg came with .zbf extension. You have to specify cpu binary xtask to wrap, 
the certificate id you plan to use, and the chip revision (ES1_dev or ES4_dev) .
also make sure that the scripts below are accessible and in your PATH variable
EOF
    exit -1 
fi
#
if [ -z "$XSDK_ROOT" ]; then echo "*** You need to define the XSDK_ROOT variable ***"; exit -1; fi
#
CPUPKG=$1
CERTID=$2
REV=$3
BNAME=`basename $1`
DNAME=`dirname $1`
TMPADDR=0x13000000

CPUPKG_BIN=${DNAME}/${BNAME}.zbf
CPUPKG_SIG=${DNAME}/${BNAME}.8634_${REV}_${CERTID}.bin
XLOAD_BIN=${DNAME}/${BNAME}_${REV}.xload
XRPC_BIN=xrpc_xload_${BNAME}_${REV}.bin
PRIVATE_KEY=$XSDK_ROOT/dummy_private_keys/8634_${REV}_${CERTID}_keyboth.pem

echo Using $CERT_BIN

# aes128 pad!
zeropad.bash $CPUPKG_BIN 16
SIZE=`wc -c $CPUPKG_BIN | awk '{print $1}'`

echo CPUPKG Image Map : size=$SIZE

openssl sha1 -sign $PRIVATE_KEY < $CPUPKG_BIN | revbytes.pl > $CPUPKG_SIG
mkxload.bash $XSDK_ROOT $REV $CERTID $CPUPKG_BIN $CPUPKG_SIG $XLOAD_BIN
buildxrpc.bash XRPC_CALLERID_IGNORED XRPC_ID_XLOAD $SIZE $TMPADDR 2 3 4 $XLOAD_BIN ${DNAME}/$XRPC_BIN

