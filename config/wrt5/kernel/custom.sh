#!/bin/bash

src2kernel()
{
    if [ -e "${gSCOPE_DIR}/kernel/${1}" ]; then
        echo "cp ${gSCOPE_DIR}/kernel/${1} ${2}"
        cp -fr ${gSCOPE_DIR}/kernel/${1} ${2}
    elif [ -e "${gCUSTOM_DIR}/kernel/${1}" ]; then
        echo "cp ${gCUSTOM_DIR}/kernel/${1} ${2}"
        cp -fr ${gCUSTOM_DIR}/kernel/${1} ${2}
    elif [ -e "${gHARDWARE_DIR}/kernel/${1}" ]; then
        echo "cp ${gHARDWARE_DIR}/kernel/${1} ${2}"
        cp -fr ${gHARDWARE_DIR}/kernel/${1} ${2}
    elif [ -e "${gPLATFORM_DIR}/kernel/${1}" ]; then
        echo "cp ${gPLATFORM_DIR}/kernel/${1} ${2}"
        cp -fr ${gPLATFORM_DIR}/kernel/${1} ${2}
    fi
}

# rootfs recuse
src2kernel do_mounts.c ${LINUX_DIR}/init/do_mounts.c

# 7981 Control
src2kernel pinctrl-moore.c ${LINUX_DIR}/drivers/pinctrl/mediatek/pinctrl-moore.c
src2kernel winbond.c ${LINUX_DIR}/drivers/mtd/nand/spi/winbond.c

# 7621 Control
src2kernel net_phy_jl31x3.c ${LINUX_DIR}/drivers/net/phy/jl31x3.c
src2kernel net_phy_Kconfig ${LINUX_DIR}/drivers/net/phy/Kconfig
src2kernel net_phy_Makefile ${LINUX_DIR}/drivers/net/phy/Makefile
src2kernel gpio_gpio-mt7621.c ${LINUX_DIR}/drivers/gpio/gpio-mt7621.c

# 7628 Control
src2kernel arch_mips_pci_pci-mt7620.c ${LINUX_DIR}/arch/mips/pci/pci-mt7620.c

# hardware sp706 watchdog
src2kernel watchdog_sp706_wdt.c ${LINUX_DIR}/drivers/watchdog/sp706_wdt.c
src2kernel watchdog_kconfig ${LINUX_DIR}/drivers/watchdog/Kconfig
src2kernel watchdog_makefile ${LINUX_DIR}/drivers/watchdog/Makefile
