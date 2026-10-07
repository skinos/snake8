# 包含一些Openwrt的编译函数
include $(TOPDIR)/rules.mk

# 定义项目基本信息
PROJECT_ID:=ifname
PKG_NAME:=skinos_$(PROJECT_ID)
PKG_VERSION:=$(shell prj-read version)
INTRO_STR:=$(shell prj-read intro)
DESC_STR:=$(shell prj-read desc)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_DEPENDS:=land network arch

# 包含Openwrt的一些有关包处理的函数
include $(INCLUDE_DIR)/package.mk
include $(gSKIN_MKFILE)
# 生成项目通用定义
$(eval $(call Package/Define))

PKG_CONFIG_DEPENDS := CONFIG_PACKAGE_skinos_ifname_ipv6

# menuconfig的选项的位置及说明
define Package/$(PKG_NAME)
  CATEGORY:=Skin system
  TITLE:=$(INTRO_STR)
  DEPENDS:=+PACKAGE_skinos_arch:skinos_arch +PACKAGE_skinos_land:skinos_land +PACKAGE_skinos_network:skinos_network +libevent2 +libopenssl \
	+PACKAGE_skinos_ifname_ipv6:odhcp6c \
	+PACKAGE_skinos_ifname_ipv6:odhcpd-ipv6only \
	+PACKAGE_skinos_ifname_ipv6:ip6tables-mod-nat
endef
define Package/$(PKG_NAME)/config
config PACKAGE_skinos_ifname_ipv6
	bool "IPv6 (odhcp6c / odhcpd / masq6)"
	default n
	depends on PACKAGE_skinos_ifname
	select IPV6
	select PACKAGE_odhcp6c
	select PACKAGE_odhcpd-ipv6only
	select PACKAGE_ip6tables-mod-nat
	help
	  Enable IPv6 on ifname (ethcon/ltecon/...). Selects Global IPv6,
	  WAN/LTE client (odhcp6c), LAN RA/DHCPv6 (odhcpd-ipv6only),
	  and masq6 (ip6tables-mod-nat).
endef
# menuconfig的界面上的帮助
define Package/$(PKG_NAME)/description
  $(INTRO_STR):$(DESC_STR)
endef
define Package/$(PKG_NAME)/extra_provides
    echo libskin.so; echo libskinm.so; echo libskinnet.so;
endef

# 安装到系统运行目录中( 注意必须要有这个函数才会执行Build/Compile )
define Package/$(PKG_NAME)/install
	$(call Build/Install/doc)
	# ########## 在此可添加额外的安装指示: 开始 ########### # 
	# ########## 在此可添加额外的安装指示: 结束 ########### # 
	$(call Build/Install/fpk,$(gSTORE_DIR))
	$(call Build/Install/fpk2rootfs,$(1))
endef

# 调用编译安装
$(eval $(call BuildPackage,$(PKG_NAME)))

