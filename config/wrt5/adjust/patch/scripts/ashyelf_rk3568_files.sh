#!/bin/sh
# by qingchengrd@outlook.com
set -e -x
if [ $# -ne 5 ] && [ $# -ne 6 ]; then
    echo "SYNTAX: $0 <file>  <kernel size> <kernel directory> <rootfs size> <rootfs image> [<align>]"
    exit 1
fi


OUTPUT="$1"
KERNELSIZE="$2"
KERNELDIR="$3"
KERNELPARTTYPE=${KERNELPARTTYPE:-83}
ROOTFSSIZE="$4"
ROOTFSIMAGE="$5"
ROOTFSPARTTYPE=${ROOTFSPARTTYPE:-83}
ALIGN="$6"


KERNELSIZE="$(($2*1024*1024))"

cp "$ROOTFSIMAGE" "$OUTPUT.boot/../ashyelf_img.files/ashyelf.rootfs"

make_ext4fs -J -L kernel -l "$KERNELSIZE" ${SOURCE_DATE_EPOCH:+-T ${SOURCE_DATE_EPOCH}} "$OUTPUT.boot/../ashyelf_img.files/ashyelf.kernel" "$KERNELDIR"

cd $OUTPUT.boot/../;tar czvf $OUTPUT ashyelf_img.files

#echo "Output file: $OUTPUT "

