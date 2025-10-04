#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.

boot: boot_dep
boot_dep:
boot_menuconfig: boot_dep
boot_install:
boot_clean:
boot_distclean: boot_clean
.PHONY: boot boot_dep boot_menuconfig boot_install boot_clean boot_distclean

kernel: kernel_dep
kernel_dep:
kernel_menuconfig: kernel_dep
kernel_install:
kernel_clean:
kernel_distclean: kernel_clean
.PHONY: kernel kernel_dep kernel_menuconfig kernel_install kernel_clean kernel_distclean

app: app_dep
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR}
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR}; \
	fi
	make -C ${gBUILD_DIR}/arch
app_dep:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} dep
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} dep; \
	fi
	if [ ! -d ${gBUILD_DIR}/arch ]; then \
		cp -Lr ${gPLATFORM_DIR}/arch ${gBUILD_DIR}; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gPLATFORM} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gPLATFORM}/* ${gBUILD_DIR}/arch; \
		fi; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gHARDWARE} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gHARDWARE}/* ${gBUILD_DIR}/arch; \
		fi; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gCUSTOM} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gCUSTOM}/* ${gBUILD_DIR}/arch; \
		fi; \
		if [ -d ${gPLATFORM_DIR}/arch/odm/${gCUSTOM}/${gSCOPE} ]; then \
			$(CP) ${gPLATFORM_DIR}/arch/odm/${gCUSTOM}/${gSCOPE}/* ${gBUILD_DIR}/arch; \
		fi; \
	fi
	make -C ${gBUILD_DIR}/arch dep
app_menuconfig: app_dep
app_install:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} install
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} install; \
	fi
	make -C ${gBUILD_DIR}/arch install
	make -C ${gTOP_DIR} rootfs_install
	cd ${gosROOT_DIR};if [ -f needless.sh ]; then \
		chmod a+rwx needless.sh;./needless.sh; \
	fi
	cd ${gBUILD_DIR} && rm -fr ${gpFIRMWARE_FILE} ${gpSTORE_FILE} ${gpFIRMWARE_LOG}
	cd ${gosROOT_DIR} && tar jcf ../${gpFIRMWARE_FILE} *
	cd ${gSTORE_DIR} && fpk-indexed ../${gpSTORE_FILE} ${gSTORE_DIR}
	cd ${gBUILD_DIR} && firmware-log ${gpFIRMWARE_LOG} ${gVERSION} ${gCUSTOM} ${gSCOPE} ${gpFIRMWARE_FILE} ${gTOP_DIR}/log.json ${gPLATFORM_DIR}/log.json ${gHARDWARE_DIR}/log.json ${gCUSTOM_DIR}/log.json ${gSCOPE_DIR}/log.json
app_clean:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} clean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} clean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -C ${gBUILD_DIR}/arch clean; \
	fi
app_distclean: app_clean
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} distclean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} distclean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -C ${gBUILD_DIR}/arch distclean; \
	fi
.PHONY: app app_dep app_menuconfig app_install app_clean app_distclean



OPENWRT_DL_NAME:=dl-raw.tar.xz
OPENWRT_FEED_NAME:=feeds-raw.tar.gz
OPENWRT_SDK_NAME:=openwrt-raw.tar.xz
sdk_update:
	# 下载或更新底层SDK
	# 更新dl目录, 避免每次一个一个下载
	# 更新并安装所有的菜单项
	# 更新fpk
	if [ -e ${gPLATFORM_DIR} ]; then \
		cd ${gPLATFORM_DIR}; rm -fr *.fpk*; \
		cd ${gPLATFORM_DIR}; repo-update slave ${gHARDWARE} ${gCUSTOM} fpk; \
	fi
sdk_adjust:
	# 对底层SDK打补丁
	if [ -e ${gPLATFORM_DIR}/adjust/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch.sh; \
	fi
sdk_menu:
	# 更新并安装所有的菜单项
	# 对底层SDK打补丁
sdk_menuconfig: kernel_dep
	# 显示菜单供用户配置
sdk_clean:
	# 清除所有菜单安装
	# distclean整个SDK
sdk_distclean: sdk_clean
	# 清除所有download
	cd ${gCUSTOM_DIR}; rm -fr *.fpk *.store
.PHONY: sdk_update sdk_adjust sdk_menu sdk_menuconfig sdk_clean sdk_distclean



sdk_install:
	sudo rm -fr /tmp/skinos
	sudo rm -fr /usr/share/skinos
	sudo cp -ar ${gosROOT_DIR}/usr/share/skinos /usr/share
	sudo cp -ar ${gosROOT_DIR}/usr/local/bin/* /usr/local/bin
	sudo cp -ar ${gosROOT_DIR}/usr/local/lib/* /usr/local/lib
	sudo chown root:root /usr/share/skinos/land/bin/he
	sudo chmod u+s /usr/share/skinos/land/bin/he
	sudo chown root:root /usr/share/skinos/land/bin/daemon
	sudo chmod u+s /usr/share/skinos/land/bin/daemon
	sudo chown root:root /usr/share/skinos/center/bin/hh
	sudo chmod u+s /usr/share/skinos/center/bin/hh
	sudo ldconfig
sdk_start:
	if [ -f /usr/share/skinos/setup.sh ]; then \
		/usr/share/skinos/setup.sh; \
	fi
sdk_stop:
	if [ -f /usr/share/skinos/shut.sh ]; then \
		/usr/share/skinos/shut.sh; \
	fi
	sudo rm -fr /tmp/skinos
sdk_uninstall:
	sudo rm -fr /tmp/skinos
	sudo rm -fr /var/skinos
	sudo rm -fr /usr/share/skinos
	sudo ldconfig
.PHONY: sdk_install sdk_bootup sdk_start sdk_stop sdk_uninstall



sdk_ftp:
sdk_repo:
	# 上传到FTP库
	if [ -d ${gSTORE_DIR} ]; then \
		repo-upload slave ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gSTORE_DIR} fpk; \
	fi
.PHONY: sdk_ftp sdk_repo



