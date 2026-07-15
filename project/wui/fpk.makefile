
# 包含skin项目的一些有关包处理的函数
include $(gSKIN_MKFILE)
# 项目的注册信息
PKG_NAME:=$(shell prj-read name)
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(gBUILD_DIR)/$(PKG_NAME)
# 生成项目通用定义
$(eval $(call Package/Define))
# 包编译时的目录
export PKG_NAME PKG_VERSION PROJECT_ID VERSION_ID PKG_BUILD_DIR FPK_BUILD_DIR FPK_LIB_DIR FPK_BIN_DIR FPK_ETC_DIR FPK_INT_DIR FPK_ROOTFS_DIR
#
all:
	$(call Build/Prepare/Default)
	$(call Build/Compile/Default)
	$(INSTALL_DIR) $(FPK_BUILD_DIR)/admin
	cp -fr ./*.ca ./*.crt ./*.key $(FPK_BUILD_DIR)/
	cp -fr ./ace/* $(FPK_BUILD_DIR)/admin
	# 给JS文件添加版本参数，防止IE缓存导致跨项目冲突
	find $(FPK_BUILD_DIR)/admin -name "*.html" -exec sed -i 's|/api/\([^"]*\.js\)"|/api/\1?v=$(PKG_VERSION)"|g' {} \;
	find $(FPK_BUILD_DIR)/admin -name "*.html" -exec sed -i 's|/js/\([^"]*\.js\)"|/js/\1?v=$(PKG_VERSION)"|g' {} \;
	$(call Build/Install/fpk,$(gSTORE_DIR))
install:
	$(call Build/Install/fpk2rootfs,$(gosROOT_DIR))
dep:
clean distclean:

.PHONY: all clean distclean dep install

