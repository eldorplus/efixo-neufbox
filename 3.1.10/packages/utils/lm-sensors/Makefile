#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lm-sensors
PKG_VERSION:=3.2.0
PKG_RELEASE:=2

PKG_SOURCE:=lm_sensors-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://dl.lm-sensors.org/lm-sensors/releases/
PKG_MD5SUM:=829d88fb09d67723fbf42853eb84d1fd

PKG_BUILD_DIR:=$(BUILD_DIR)/lm_sensors-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/lm-sensors/Default
  DEPENDS:=+sysfsutils
  URL:=http://www.lm-sensors.org/
endef

define Package/lm-sensors
  $(call Package/lm-sensors/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=lm-sensors
  DEPENDS+=+libsensors
endef

define Package/libsensors
  $(call Package/lm-sensors/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libsensors
endef

define Package/lm-sensors/description
	Utility to read hardware sensor data
endef

define Package/libsensors/description
	lm-sensors libraries
endef

define Package/lm-sensors/conffiles
/etc/sensors.conf
endef

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KERNELVERSION="$(LINUX_VERSION)" \
		LINUX="$(LINUX_DIR)" \
		CC="$(TARGET_CC)" \
		STAGING_DIR="$(STAGING_DIR)" \
		PREFIX="/usr" \
		MACHINE="$(ARCH)" \
		user
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/sensors
	$(CP) \
		$(PKG_BUILD_DIR)/lib/sensors.h \
		$(1)/usr/include/sensors
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_BUILD_DIR)/lib/libsensors.{a,so*} \
		$(1)/usr/lib/
endef

define Package/lm-sensors/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/prog/sensors/sensors $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/prog/detect/sensors-detect $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/sensors.conf $(1)/etc/sensors.conf
endef

define Package/libsensors/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libsensors.so* $(1)/usr/lib/
endef


$(eval $(call BuildPackage,lm-sensors))
$(eval $(call BuildPackage,libsensors))
