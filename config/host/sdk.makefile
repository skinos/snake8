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
	make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch
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
	make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch dep
app_menuconfig: app_dep
app_install:
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} install
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} install; \
	fi
	make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch install
	make -C ${gTOP_DIR} lay_install
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
		make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch clean; \
	fi
app_distclean: app_clean
	make -f ${gDIR_MAKEFILE} -C ${gPROJECT_DIR} distclean
	if [ -d ${gRICE_DIR} ]; then \
		make -f ${gDIR_MAKEFILE} -C ${gRICE_DIR} distclean; \
	fi
	if [ -d ${gBUILD_DIR}/arch ]; then \
		make -f ${gFPK_MAKEFILE} -C ${gBUILD_DIR}/arch distclean; \
	fi
.PHONY: app app_dep app_menuconfig app_install app_clean app_distclean



sdk_update:
	# 更新fpk
	if [ -e ${gPLATFORM_DIR} ]; then \
		cd ${gPLATFORM_DIR}; rm -fr *.fpk*; \
		cd ${gPLATFORM_DIR}; repo-update host ${gHARDWARE} ${gCUSTOM} fpk; \
	fi
sdk_adjust:
sdk_menu:
sdk_menuconfig:
sdk_clean:
sdk_distclean:
	cd ${gCUSTOM_DIR}; rm -fr *.fpk *.store
.PHONY: sdk_update sdk_adjust sdk_menu sdk_menuconfig sdk_clean sdk_distclean



sdk_install:
	sudo rm -fr /tmp/skin
	sudo rm -fr /usr/prj
	sudo cp -ar ${gosROOT_DIR}/usr/local/bin/* /usr/local/bin
	sudo cp -ar ${gosROOT_DIR}/usr/local/lib/* /usr/local/lib
	sudo cp -ar ${gosROOT_DIR}/usr/prj /usr
	sudo ldconfig
sdk_bootup:
	if [ -f /etc/rc.local ]; then \
		alreadybootup=`cat /etc/rc.local|grep /usr/prj/setup.sh`; \
		if [ "X${alreadybootup} = "X" ]; then \
			sudo "runuser -u ${LOGNAME} /usr/prj/setup.sh"; \
		fi \
	else \
		sudo cp ${gPLATFORM_DIR}/rc.local /etc/; \
	fi
sdk_start:
	if [ -f /usr/prj/setup.sh ]; then \
		/usr/prj/setup.sh; \
	fi
sdk_stop:
	if [ -f /usr/prj/shut.sh ]; then \
		/usr/prj/shut.sh; \
	fi
	sudo rm -fr /tmp/skin
.PHONY: sdk_install sdk_bootup sdk_start sdk_stop



sdk_ftp:
sdk_repo:
	if [ -d ${gSTORE_DIR} ]; then \
		repo-upload host ${gHARDWARE} ${gCUSTOM} ${gSCOPE} ${gSTORE_DIR} fpk; \
	fi
.PHONY: sdk_ftp sdk_repo
