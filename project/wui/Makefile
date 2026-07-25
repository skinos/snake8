# 包含一些Openwrt的编译函数
include $(TOPDIR)/rules.mk

# 定义项目基本信息
PROJECT_ID:=wui
PKG_NAME:=skinos_$(PROJECT_ID)
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_DEPENDS:=land network arch

# 包含Openwrt的一些有关包处理的函数
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk
include $(gSKIN_MKFILE)
# 生成项目通用定义
$(eval $(call Package/Define))

# menuconfig的选项的位置及说明
define Package/$(PKG_NAME)
  CATEGORY:=Skin system
  TITLE:=$(INTRO_STR)
  DEPENDS:=+PACKAGE_skinos_arch:skinos_arch +PACKAGE_skinos_land:skinos_land +PACKAGE_skinos_network:skinos_network +PACKAGE_skinos_webs:skinos_webs +libevent2 +libopenssl
endef
# menuconfig的界面上的帮助
define Package/$(PKG_NAME)/description
  $(INTRO_STR):$(DESC_STR)
endef
define Package/$(PKG_NAME)/extra_provides
    echo libskin.so; echo libskinnet.so;
endef

# 安装到系统运行目录$(1)中( 注意必须要有这个函数才会执行Build/Compile )
define Package/$(PKG_NAME)/install
	$(call Build/Install/doc)
	# ########## 在此可添加额外的安装指示: 开始 ########### #
	$(CP) ./*.ca ./*.crt ./*.key $(FPK_BUILD_DIR)/
	$(INSTALL_DIR) $(FPK_BUILD_DIR)/admin
	$(CP) ./ace/* $(FPK_BUILD_DIR)/admin
	# 给JS/lang文件添加版本参数，防止IE缓存导致跨项目冲突
	$(FIND) $(FPK_BUILD_DIR)/admin -name "*.html" -exec $(SED) 's|/api/\([^"]*\.js\)"|/api/\1?v=$(PKG_VERSION)"|g' {} \;
	$(FIND) $(FPK_BUILD_DIR)/admin -name "*.html" -exec $(SED) 's|/js/\([^"]*\.js\)"|/js/\1?v=$(PKG_VERSION)"|g' {} \;
	$(SED) 's|__WUI_VERSION__|$(PKG_VERSION)|g' $(FPK_BUILD_DIR)/admin/api/util.js
	# ########## 在此可添加额外的安装指示: 结束 ########### #
	$(call Build/Install/fpk,$(gSTORE_DIR))
	$(call Build/Install/fpk2rootfs,$(1))
endef

# 调用编译安装
$(eval $(call BuildPackage,$(PKG_NAME)))

