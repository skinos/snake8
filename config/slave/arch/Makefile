
# 包含skinos项目的一些有关包处理的函数
include $(gSKIN_MKFILE)
# 项目的注册信息
PKG_NAME:=$(shell prj-read name)
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(gBUILD_DIR)/$(PKG_NAME)
# 生成项目通用定义
$(eval $(call Package/Define))
FPK_CONFIG_DIR:=$(PKG_BUILD_DIR)/.fpk/config
# 包编译时的目录
export PKG_NAME PKG_VERSION PROJECT_ID VERSION_ID PKG_BUILD_DIR FPK_BUILD_DIR FPK_LIB_DIR FPK_BIN_DIR FPK_ROOTFS_DIR
#
all:
	$(call Build/Prepare/Default)
	# copy custom to config
	$(INSTALL_DIR) $(FPK_CONFIG_DIR)
	if [ -d ${gPLATFORM_DIR}/config ]; then \
		$(CP) ${gPLATFORM_DIR}/config/* $(FPK_CONFIG_DIR); \
	fi
	if [ -d ${gHARDWARE_DIR}/config ]; then \
		$(CP) ${gHARDWARE_DIR}/config/* $(FPK_CONFIG_DIR); \
	fi
	if [ -d ${gCUSTOM_DIR}/config ]; then \
		$(CP) ${gCUSTOM_DIR}/config/* $(FPK_CONFIG_DIR); \
	fi
	if [ -d ${gSCOPE_DIR}/config ]; then \
		$(CP) ${gSCOPE_DIR}/config/* $(FPK_CONFIG_DIR); \
	fi
	$(call Build/Compile/Default)
	$(call Build/Install/fpk,$(gSTORE_DIR))
install:
	$(call Build/Install/fpk2rootfs,$(gosROOT_DIR))
dep:
clean distclean:

.PHONY: all clean distclean dep install

