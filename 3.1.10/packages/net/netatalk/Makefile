#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=netatalk
PKG_VERSION:=2.1.3
PKG_RELEASE:=1
PKG_SOURCE:=netatalk-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/netatalk/
PKG_BUILD_PARALLEL:=1
PKG_MD5SUM:=785e287f5b5e5c9cce9397f9bce02a22

include $(INCLUDE_DIR)/package.mk

define Package/netatalk
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libdb47 +libgcrypt +libopenssl
  TITLE:=netatalk
  URL:=http://netatalk.sourceforge.net
  MAINTAINER:=W. Michael Petullo <mike@flyn.org>
endef

define Package/netatalk/decription
  Netatalk is a freely-available Open Source AFP fileserver.
  It also provides a kernel level implementation of the AppleTalk
  Protocol Suite.
endef

EXTRA_CFLAGS += -I$(STAGING_DIR)/usr/include

CONFIGURE_VARS += \
	CFLAGS="$(TARGET_CFLAGS) -std=gnu99 -DNO_DEBUG "

CONFIGURE_ARGS += \
	--disable-afs \
	--enable-hfs \
	--disable-debugging \
	--disable-shell-check \
	--disable-timelord \
	--disable-a2boot \
	--disable-cups \
	--disable-tcp-wrappers \
	--with-cnid-default-backend=dbd \
	--with-bdb="$(STAGING_DIR)/usr/" \
	--with-libgcrypt-dir="$(STAGING_DIR)/usr" \
	--with-ssl-dir="$(STAGING_DIR)/usr" \
	--with-uams-path="/usr/lib/uams" \
	--disable-admin-group \
	--disable-srvloc \

define Package/netatalk/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/uams/
	$(INSTALL_DIR) $(1)/etc/netatalk/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(CP) $(PKG_BUILD_DIR)/etc/afpd/afpd $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/bin/afppasswd/afppasswd $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/etc/cnid_dbd/cnid_dbd $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/etc/cnid_dbd/cnid_metad $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/etc/uams/.libs/*.so $(1)/usr/lib/uams/
	$(CP) ./files/AppleVolumes.* $(1)/etc/netatalk/
	$(INSTALL_CONF) ./files/afpd.conf $(1)/etc/netatalk/
	$(INSTALL_BIN) ./files/afpd.init $(1)/etc/init.d/afpd
endef

$(eval $(call BuildPackage,netatalk))
