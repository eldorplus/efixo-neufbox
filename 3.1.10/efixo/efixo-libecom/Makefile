# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10749 2008-04-07 00:24:19Z libnbu $

include $(TOPDIR)/rules.mk

PKG_NAME:=efixo-libecom
PKG_RELEASE:=1
PKG_INSTALL:=1

PKG_FIXUP:=libtool

include $(INCLUDE_DIR)/package-bin-ipkg.mk

define Package/efixo-libecom
  TITLE:=Neufbox 3G support (com library)
  SECTION:=efixo
  CATEGORY:=Efixo applications
  DEPENDS:=+efixo-libestuff
  MAINTAINER:=XXX
endef

define Package/efixo-libecom/description
 This package contains the com library used for 3Gbackup
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/efixo-libecom/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libecom.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,efixo-libecom))
