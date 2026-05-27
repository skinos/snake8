#
# Toplevel Makefile for the Platform SDK release
#
# Copyright 2009-2020, dimmalex
# All Rights Reserved.

boot: boot_dep
boot_menuconfig: boot_dep
boot_dep:
boot_install:
boot_clean:
boot_distclean: boot_clean
.PHONY: boot boot_dep boot_menuconfig boot_install boot_clean boot_distclean

kernel: kernel_dep
	if [ "X${COMPILE_PROJECT}" != "X" ]; then \
		cd ${gSDK_DIR};make V=s ${COMPILE_PROJECT}; \
	else \
		cd ${gSDK_DIR};make V=s; \
	fi
	cp ${gpUPGRADE_IMAGE} ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade
	cd ${gBUILD_DIR}; \
	if [ -e ${gOEM_DIR} ]; then \
	    if [ -e ${gOEM_SHELL} ]; then \
			cp ${gOEM_SHELL} ${gBUILD_DIR}/${gOEM}.sh; \
		fi; \
	    if [ -e ${gOEM_DIR}/config ]; then \
			cd ${gOEM_DIR}/config; \
			tar -c * -f ${gBUILD_DIR}/${gOEM}.dtar; \
		fi; \
		cd ${gBUILD_DIR}; \
		firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade ${gOEM}.sh ${gOEM}.dtar; \
	else \
		firmware-encode ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade; \
	fi
kernel_dep:
	if [ -f ${gSDK_CFGFILE} ]; then \
		if [ -f ${gSDK_DIR}/.config ]; then \
			tmpdiff=`diff ${gSDK_CFGFILE} ${gSDK_DIR}/.config`; \
			if [ "X$${tmpdiff}" != "X" ]; then \
				cp ${gSDK_CFGFILE} ${gSDK_DIR}/.config; \
			fi \
		else \
			cp ${gSDK_CFGFILE} ${gSDK_DIR}/.config; \
		fi \
	fi
	if [ -f ${gpKERNEL_CFGFILE} ]; then \
		if [ -f ${gpKERNEL_SDK_CFGFILE} ]; then \
			tmpdiff=`diff ${gpKERNEL_CFGFILE} ${gpKERNEL_SDK_CFGFILE}`; \
			if [ "X$${tmpdiff}" != "X" ]; then \
				cp ${gpKERNEL_CFGFILE} ${gpKERNEL_SDK_CFGFILE}; \
			fi \
		else \
			cp ${gpKERNEL_CFGFILE} ${gpKERNEL_SDK_CFGFILE}; \
		fi \
	fi
kernel_menuconfig: kernel_dep
	cd ${gSDK_DIR};make V=s kernel_menuconfig
	if [ -f ${gpKERNEL_CFGFILE} ]; then \
		if [ -f ${gpKERNEL_SDK_CFGFILE} ]; then \
			cp ${gpKERNEL_SDK_CFGFILE} ${gpKERNEL_CFGFILE}; \
		fi \
	fi
kernel_install:
	cd ${gBUILD_DIR} && fpk-indexed ${gSTORE_DIR} ${gSTORE_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.store;
	cd ${gBUILD_DIR} && firmware-log ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ${gVERSION} ${gCUSTOM} ${gSCOPE} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ${gTOP_DIR}/changelog.json ${gPLATFORM_DIR}/changelog.json ${gHARDWARE_DIR}/changelog.json ${gCUSTOM_DIR}/changelog.json ${gSCOPE_DIR}/changelog.json
kernel_clean:
	cd ${gSDK_DIR};make V=s clean
kernel_distclean: kernel_clean
.PHONY: kernel kernel_dep kernel_menuconfig kernel_install kernel_clean kernel_distclean

app: app_dep
	@set -e; \
	if [ "X${COMPILE_PROJECT}" != "X" ]; then \
		pkg="${COMPILE_PROJECT}"; \
		case "$$pkg" in */*) ;; \
		*) \
			for feed in project rice; do \
				if [ -d "${gSDK_DIR}/package/feeds/$$feed/$$pkg" ]; then \
					pkg="feeds/$$feed/$$pkg"; \
					break; \
				fi; \
			done; \
		esac; \
		cd ${gSDK_DIR}; make V=s package/$$pkg/compile; \
	else \
		echo "Nothing be done"; \
	fi
app_dep:
	@echo "Nothing be done"
app_menuconfig: app_dep
	@echo "Nothing be done"
app_install:
	@echo "Nothing be done"
app_clean:
	@set -e; \
	if [ "X${COMPILE_PROJECT}" != "X" ]; then \
		pkg="${COMPILE_PROJECT}"; \
		case "$$pkg" in */*) ;; \
		*) \
			for feed in project rice; do \
				if [ -d "${gSDK_DIR}/package/feeds/$$feed/$$pkg" ]; then \
					pkg="feeds/$$feed/$$pkg"; \
					break; \
				fi; \
			done; \
		esac; \
		cd ${gSDK_DIR}; make V=s package/$$pkg/clean; \
	fi
app_distclean: app_clean
	@echo "Nothing be done"
.PHONY: app app_dep app_menuconfig app_install app_clean app_distclean



OPENWRT_DL_NAME:=openwrt-25.12.2-dl.tar.xz
OPENWRT_FEED_NAME:=openwrt-25.12.2-feeds.tar.xz
OPENWRT_SDK_NAME:=openwrt-25.12.2.tar.xz
sdk_update:
	@set -e; \
	if [ ! -d ${gPLATFORM_DIR}/dl ]; then \
		mkdir ${gPLATFORM_DIR}/dl; \
	fi; \
	if [ ! -d ${gSDK_DIR} ]; then \
		cd ${gPLATFORM_DIR}/dl; repo-update wrt5 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_SDK_NAME} || exit 1; \
		cd ${gTOP_DIR}; tar -Jxvf ${gPLATFORM_DIR}/dl/${OPENWRT_SDK_NAME} || exit 1; \
		mv openwrt-25.12.2 ${gPLATFORM} || exit 1; \
	fi; \
	if [ ! -d ${gSDK_DIR}/dl ]; then \
		cd ${gPLATFORM_DIR}/dl; repo-update wrt5 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_DL_NAME} || exit 1; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/dl/${OPENWRT_DL_NAME} || exit 1; \
	fi; \
	if [ ! -d ${gSDK_DIR}/feeds ]; then \
		cd ${gPLATFORM_DIR}/dl; repo-update wrt5 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_FEED_NAME} || exit 1; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/dl/${OPENWRT_FEED_NAME} || exit 1; \
	fi; \
	cd ${gPLATFORM_DIR}/dl; rm -fr *.fpk*; \
	cd ${gPLATFORM_DIR}/dl; repo-update wrt5 ${gHARDWARE} ${gCUSTOM} fpk || exit 1
sdk_adjust:
	# 对底层SDK打补丁
	if [ -e ${gPLATFORM_DIR}/adjust/patch/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch/patch.sh; \
	fi
sdk_menu:
	@set -e; \
	if [ ! -d ${gSDK_DIR}/feeds ]; then \
		cd ${gPLATFORM_DIR}/dl; repo-update wrt5 ${gHARDWARE} ${gCUSTOM} ${OPENWRT_FEED_NAME} || exit 1; \
		cd ${gSDK_DIR}; tar -Jxvf ${gPLATFORM_DIR}/dl/${OPENWRT_FEED_NAME} || exit 1; \
	fi; \
	if [ -e ${gPLATFORM_DIR}/adjust/patch/patch.sh ]; then \
		${gPLATFORM_DIR}/adjust/patch/patch.sh; \
	fi; \
	cd ${gSDK_DIR}; ./scripts/feeds update project; \
	cd ${gSDK_DIR}; ./scripts/feeds update rice; \
	cd ${gSDK_DIR}; ./scripts/feeds install -a -f
sdk_menuconfig: kernel_dep
	# 显示菜单供用户配置
	cd ${gSDK_DIR};make menuconfig
	if [ -f ${gSDK_DIR}/.config ]; then \
		cp ${gSDK_DIR}/.config ${gSDK_CFGFILE}; \
	fi
sdk_clean:
	if [ -d ${gSDK_DIR} ]; then \
		cd ${gSDK_DIR}; \
		./scripts/feeds uninstall -a; \
		./scripts/feeds clean -a; \
		make V=s distclean; \
		rm -fr ${gSDK_DIR}/dl; \
	fi
sdk_distclean: sdk_clean
	# 清除所有download
	rm -fr ${gPLATFORM_DIR}/dl
.PHONY: sdk_update sdk_adjust sdk_menu sdk_menuconfig sdk_clean sdk_distclean



sdk_sz:
	# 通过tftp协议发送固件到本地
	if [ -e ${gOEM_DIR} ]; then \
		sz ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz; \
	else \
		sz ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz; \
	fi
sdk_tftp:
	# 通过xmodem协议发送固件到本地
	cd ${gBUILD_DIR}; \
	if [ -e ${gOEM_DIR} ]; then \
		${gTFTPD} -m binary -c put ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz; \
	else \
		${gTFTPD} -m binary -c put ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz; \
	fi
	cd ${gBUILD_DIR}; \
	${gTFTPD} -m binary -c put ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.upgrade
sdk_tar:
	if [ "X${gPACK_MAKEFILE}" != "X" ]; then \
		if [ -f ${gPACK_MAKEFILE} ]; then \
			make -f ${gPACK_MAKEFILE}; \
		fi; \
	fi
	cd ${gBUILD_DIR}; \
	if [ -e ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.bb ] && [ -e ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz ]; then \
		rm -fr ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
		tar jcvf ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2 ${gHARDWARE}_${gCUSTOM}_${gSCOPE}*; \
		sz ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.tar.bz2; \
	fi
.PHONY: sdk_sz sdk_tftp sdk_tar



sdk_ftp:
	# 上传到FTP目录
	if [ -e ${gOEM_DIR} ]; then \
		repo-upload wrt5 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}_${gOEM}.zz; \
	else \
		repo-upload wrt5 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}_${gVERSION}.zz; \
		if [ -f ${gBUILD_DIR}/${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt ]; then\
			repo-upload wrt5 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gBUILD_DIR} ${gHARDWARE}_${gCUSTOM}_${gSCOPE}.txt; \
		fi \
	fi
sdk_repo:
	# 上传到FTP库
	if [ -d ${gSTORE_DIR} ]; then \
		repo-upload wrt5 ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gSTORE_DIR} fpk; \
	fi
.PHONY: sdk_ftp sdk_repo



