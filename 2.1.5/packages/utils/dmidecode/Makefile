# 
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dmidecode
PKG_VERSION:=2.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.savannah.gnu.org/releases/dmidecode/
PKG_MD5SUM:=3c9c4d55a40b78600f3b43bfa64616f9

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/dmidecode
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_x86
  TITLE:=Displays BIOS informations.
  URL:=http://www.nongnu.org/dmidecode/
endef

define Package/dmidecode/description
	Dmidecode reports information about your system's hardware
	as described in your system BIOS according to the SMBIOS/DMI
	standard.
endef

define Package/dmidecode/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin
endef

$(eval $(call BuildPackage,dmidecode))
