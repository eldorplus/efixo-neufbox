#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mercurial
PKG_VERSION:=1.6.2
PKG_RELEASE:=2

PKG_SOURCE_URL:=http://mercurial.selenic.com/release
PKG_SOURCE:=mercurial-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=847359d7488caf8074a69d4b87ca64d2
PKG_BUILD_DEPENDS:=python-mini

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/mercurial
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+python
  TITLE:=Mercurial Source Control Management (SCM) system
  URL:=http://mercurial.selenic.com/
  SUBMENU:=Version Control Systems
endef

define Package/mercurial/description
  A fast, lightweight Source Control Management system designed for efficient handling of very large distributed projects.
endef

define Build/Compile
	$(call Build/Compile/PyMod,, \
		install --prefix="$(PKG_INSTALL_DIR)", \
    )
endef

define Package/mercurial/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/bin $(1)/usr
	$(CP) $(PKG_INSTALL_DIR)/lib $(1)/usr
endef

$(eval $(call BuildPackage,mercurial))
