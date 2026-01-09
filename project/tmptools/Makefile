# 包含一些Openwrt的编译函数
include $(TOPDIR)/rules.mk

# 项目名称(替换名称即可)
PROJECT_ID:=tmptools
# 定义项目基本信息
PKG_NAME:=skinos_$(PROJECT_ID)
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

# 包含Openwrt的一些有关包处理的函数
include $(INCLUDE_DIR)/package.mk
include $(gSKIN_MKFILE)
# 生成项目通用定义
$(eval $(call Package/Define))

# menuconfig的选项的位置及说明
define Package/$(PKG_NAME)
  CATEGORY:=Skin Application
  TITLE:=$(INTRO_STR)
  DEPENDS:=+PACKAGE_skinos_arch:skinos_arch +PACKAGE_skinos_land:skinos_land
endef
# menuconfig的界面上的帮助
define Package/$(PKG_NAME)/description
  $(INTRO_STR):$(DESC_STR)
endef

# 安装到系统运行目录中( 注意必须要有这个函数才会执行Build/Compile )
define Package/$(PKG_NAME)/install
	$(call Build/Install/doc)
	# More installation instructions can be added here
	$(call Build/Install/fpk,$(gSTORE_DIR))
	$(call Build/Install/fpk2rootfs,$(1))
endef

# 调用编译安装
$(eval $(call BuildPackage,$(PKG_NAME)))

