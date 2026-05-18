#!/bin/sh
# Copyright (C) 2006-2012 OpenWrt.org
set -e -x
if [ $# -ne 8 ] && [ $# -ne 9 ]; then
    echo "SYNTAX: $0 <file> <kernel size> <kernel directory> <rootfs size> <rootfs image> <config size> <oem size> <factory_size>"
    exit 1
fi

OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"
CONFIGSSIZE=$6
OEMSIZE=$7
FACTORYSIZE=$8
# define mmc blocksize 4096M
MMCALLSIZE=4092

UBOOTSIZE=8
UBOOTOFFSET=8
KERNELOFFSET="$(($UBOOTOFFSET + $UBOOTSIZE))"
ROOTFSOFFSET="$(($KERNELSIZE + $KERNELOFFSET))"
CONFIGSOFFSET="$(($ROOTFSSIZE + $ROOTFSOFFSET))"
OEMOFFSET="$(($CONFIGSSIZE + $CONFIGSOFFSET))"
FACTORYOFFSET="$(($OEMSIZE + $OEMOFFSET))"
RECOVERYOFFSET="$(($FACTORYSIZE + $FACTORYOFFSET))"
ROOTFSBAKOFFSET="$(($KERNELSIZE + $RECOVERYOFFSET))"
USERDATAOFFSET="$(($ROOTFSSIZE + $ROOTFSBAKOFFSET))"
USERDATASIZE="$(($MMCALLSIZE - $USERDATAOFFSET))"


rm -f "$OUTPUT"

# create partition table
set $(ptgen -g -o "$OUTPUT" -a 2 -N uboot -p "$UBOOTSIZE"M@"$UBOOTOFFSET"M -N kernel -p "$KERNELSIZE"M@"$KERNELOFFSET"M -N rootfs -p "$ROOTFSSIZE"M@"$ROOTFSOFFSET"M -N configs -p "$CONFIGSSIZE"M@"$CONFIGSOFFSET"M -N oem -p "$OEMSIZE"M@"$OEMOFFSET"M -N factory -p "$FACTORYSIZE"M@"$FACTORYOFFSET"M -N recovery -p "$KERNELSIZE"M@"$RECOVERYOFFSET"M -N rootfsbak -p "$ROOTFSSIZE"M@"$ROOTFSBAKOFFSET"M -N userdata -p "$USERDATASIZE"M@"$USERDATAOFFSET"M)

KERNELOFFSET="$(($3 / 512))"
KERNELSIZE="$4"
ROOTFSOFFSET="$(($5 / 512))"
ROOTFSSIZE="$(($6 / 512))"

[ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc count="$ROOTFSSIZE"
dd if="$ROOTFSIMAGE" of="$OUTPUT" bs=512 seek="$ROOTFSOFFSET" conv=notrunc

if [ -n "$GUID" ]; then
    [ -n "$PADDING" ] && dd if=/dev/zero of="$OUTPUT" bs=512 seek="$((ROOTFSOFFSET + ROOTFSSIZE))" conv=notrunc count="$sect"
    mkfs.fat -n kernel -C "$OUTPUT.kernel" -S 512 "$((KERNELSIZE / 1024))"
    mcopy -s -i "$OUTPUT.kernel" "$KERNELDIR"/* ::/
else
    make_ext4fs -J -L kernel -l "$KERNELSIZE" "$OUTPUT.kernel" "$KERNELDIR"
fi
dd if="$OUTPUT.kernel" of="$OUTPUT" bs=512 seek="$KERNELOFFSET" conv=notrunc

KERNEL_TMP_DIR=`dirname $OUTPUT`
cp "$OUTPUT.kernel" "$KERNEL_TMP_DIR/../../../../bin/targets/rockchip/armv8-glibc/"
rm -f "$OUTPUT.kernel"
