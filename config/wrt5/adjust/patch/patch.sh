#!/bin/bash

# copy file only when content differs
copy_if_diff() {
    local src="$1"
    local dst="$2"

    if [ ! -f "${src}" ]; then
        echo "skip: source file not found: ${src}"
        return 1
    fi

    mkdir -p "$(dirname "${dst}")"

    if [ -f "${dst}" ] && cmp -s "${src}" "${dst}"; then
        #echo "skip: same file: ${dst}"
        return 0
    fi

    cp "${src}" "${dst}"
    echo "copy: ${src} -> ${dst}"
}

# copy directory files only when different, and map package.make to Makefile
copy_dir_if_diff() {
    local src_dir="$1"
    local dst_dir="$2"
    local src=""
    local rel_path=""
    local dst=""

    shopt -s globstar nullglob
    for src in "${src_dir}"/**/*; do
        [ -f "${src}" ] || continue
        rel_path="${src#${src_dir}/}"
        if [ "${rel_path}" = "package.make" ]; then
            dst="${dst_dir}/Makefile"
        else
            dst="${dst_dir}/${rel_path}"
        fi
        copy_if_diff "${src}" "${dst}"
    done
    shopt -u globstar nullglob
}

# get the ubuntu release
UB_CODE=`lsb_release -s -r`
UB_REL=${UB_CODE:0:2}
echo "Current Ubuntu ${UB_CODE} Release: ${UB_REL}"

if [ -z "${gPLATFORM}" ] || [ -z "${gSDK_DIR}" ] || [ -z "${gPLATFORM_DIR}" ]; then
    echo "error: gPLATFORM, gSDK_DIR and gPLATFORM_DIR must be set" >&2
    exit 1
fi
if [ "${gPLATFORM}" = "/" ] || [ "${gPLATFORM}" = "." ] || [ "${gPLATFORM}" = ".." ]; then
    echo "error: invalid gPLATFORM: ${gPLATFORM}" >&2
    exit 1
fi

# include the config/swrt5 to build
rm -fr "${gSDK_DIR}/package/${gPLATFORM}"
if [ ! -e ${gSDK_DIR}/package/${gPLATFORM} ]; then
    ln -s ${gPLATFORM_DIR}  ${gSDK_DIR}/package/${gPLATFORM}
fi

# feeds modify
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/feeds.conf" "${gSDK_DIR}/feeds.conf"

# patch for gosROOT_DIR to image
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/makefile/image.mk" "${gSDK_DIR}/include/image.mk"
# patch call the kernel/custom.sh to override the kernel source code before kernel compile
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/makefile/kernel-defaults.mk" "${gSDK_DIR}/include/kernel-defaults.mk"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/makefile/rootfs.mk" "${gSDK_DIR}/include/rootfs.mk"

# dts modify
# mt7981
for src in "${gPLATFORM_DIR}"/adjust/patch/mt7981/dts/*.dts; do
    [ -e "${src}" ] || continue
    copy_if_diff "${src}" "${gSDK_DIR}/target/linux/mediatek/dts/$(basename "${src}")"
done

# mt7621
for src in "${gPLATFORM_DIR}"/adjust/patch/mt7621/dts/*.dts; do
    [ -e "${src}" ] || continue
    copy_if_diff "${src}" "${gSDK_DIR}/target/linux/ramips/dts/$(basename "${src}")"
done

# mt7628
for src in "${gPLATFORM_DIR}"/adjust/patch/mt7628/dts/*.dts; do
    [ -e "${src}" ] || continue
    copy_if_diff "${src}" "${gSDK_DIR}/target/linux/ramips/dts/$(basename "${src}")"
done

# rk3568
mkdir -p ${gSDK_DIR}/target/linux/rockchip/files-6.12/arch/arm64/boot/dts/rockchip
for src in "${gPLATFORM_DIR}"/adjust/patch/rk3568/dts/*.dts; do
    [ -e "${src}" ] || continue
    copy_if_diff "${src}" "${gSDK_DIR}/target/linux/rockchip/files-6.12/arch/arm64/boot/dts/rockchip/$(basename "${src}")"
done

# board target
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/mt7981/target/image_filogic.mk" "${gSDK_DIR}/target/linux/mediatek/image/filogic.mk"
#copy_if_diff "${gPLATFORM_DIR}/adjust/patch/mt7981/target/image_makefile" "${gSDK_DIR}/target/linux/mediatek/image/Makefile" move to smtk3 because only smtk2/smtk3/srock make the flash image
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/mt7621/target/image_mt7621.mk" "${gSDK_DIR}/target/linux/ramips/image/mt7621.mk"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/mt7628/target/image_mt76x8.mk" "${gSDK_DIR}/target/linux/ramips/image/mt76x8.mk"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/rk3568/target/image_armv8.mk" "${gSDK_DIR}/target/linux/rockchip/image/armv8.mk"

# one board.d script for all ashyelf / custom boards (mt7628 swconfig + DSA)
for dest in \
    "${gSDK_DIR}/target/linux/ramips/mt76x8/base-files/etc/board.d/02_network_ashyelf" \
    "${gSDK_DIR}/target/linux/ramips/mt7621/base-files/etc/board.d/02_network_ashyelf" \
    "${gSDK_DIR}/target/linux/mediatek/filogic/base-files/etc/board.d/02_network_ashyelf"
do
    copy_if_diff "${gPLATFORM_DIR}/adjust/patch/board.d/02_network_ashyelf" "${dest}"
done

# package
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/base-files-Makefile" "${gSDK_DIR}/package/base-files/Makefile"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/wifi-scripts-Makefile" "${gSDK_DIR}/package/network/config/wifi-scripts/Makefile"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/perl-111-glibc-GNU_SOURCE-cloexec-prototypes.patch" "${gSDK_DIR}/feeds/packages/lang/perl/patches/111-glibc-GNU_SOURCE-cloexec-prototypes.patch"

# patch to busybox for udhcpc support ifnameid and exit when renewip failed
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/busybox/udhcpc-defconfig-ifnameid-renewexit.patch" "${gSDK_DIR}/package/utils/busybox/patches/900-udhcpc-defconfig-ifnameid-renewexit.patch"
# do not syslog "password for 'user' changed" from chpasswd
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/busybox/chpasswd-no-changed-syslog.patch" "${gSDK_DIR}/package/utils/busybox/patches/901-chpasswd-no-changed-syslog.patch"

# patch to mt7613 cannot connect to 160M AP
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/010-mt7663-disable-vht160-sta-compat.patch" "${gSDK_DIR}/package/kernel/mt76/patches/010-mt7663-disable-vht160-sta-compat.patch"

# patch mt76 rmmod/insmod: disable rx/tx napi before netif_napi_del
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/020-mt76-dma-cleanup-disable-rx-napi.patch" "${gSDK_DIR}/package/kernel/mt76/patches/020-mt76-dma-cleanup-disable-rx-napi.patch"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/021-mt7603-unregister-stop-irq-before-dma-cleanup.patch" "${gSDK_DIR}/package/kernel/mt76/patches/021-mt7603-unregister-stop-irq-before-dma-cleanup.patch"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/022-mt7603-mcu-reload-recovery.patch" "${gSDK_DIR}/package/kernel/mt76/patches/022-mt7603-mcu-reload-recovery.patch"

# proftpd
if [ ! -e ${gSDK_DIR}/package/network/services/proftpd ]; then
    copy_dir_if_diff "${gPLATFORM_DIR}/adjust/patch/proftpd" "${gSDK_DIR}/package/network/services/proftpd"
fi

# hostapd/wpad: no ucode (see hostapd-Makefile CORE_DEPENDS / CONFIG_UCODE)
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/hostapd-Makefile" "${gSDK_DIR}/package/network/services/hostapd/Makefile"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/linux-modules-netfilter.mk" "${gSDK_DIR}/package/kernel/linux/modules/netfilter.mk"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/iptables-600-shared-libext.patch" "${gSDK_DIR}/package/network/utils/iptables/patches/600-shared-libext.patch"
# xtables-nft: always depend on libiptext6 (needed when IPV6 is disabled)
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/iptables-Makefile" "${gSDK_DIR}/package/network/utils/iptables/Makefile"

# disable the print error when no ubusd
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/package/hostapd_wpa_supplicant.uc" "${gSDK_DIR}/package/network/services/hostapd/files/wpa_supplicant.uc"

# scripts
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/scripts/gen_fw_gpt.sh" "${gSDK_DIR}/scripts/gen_fw_gpt.sh"

# config
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/config/Config-images.in" "${gSDK_DIR}/config/Config-images.in"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/makefile/Config-build.in" "${gSDK_DIR}/config/Config-build.in"
copy_if_diff "${gPLATFORM_DIR}/adjust/patch/makefile/netfilter.mk" "${gSDK_DIR}/include/netfilter.mk"


