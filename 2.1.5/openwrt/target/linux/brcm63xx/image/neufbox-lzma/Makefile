# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 4439 2006-08-04 17:14:36Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME := neufbox-lzma
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

$(PKG_BUILD_DIR)/.prepared:
	mkdir $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	touch $@

$(PKG_BUILD_DIR)/lzma: $(PKG_BUILD_DIR)/.prepared
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(HOSTCC)" CXX="$(HOSTCXX)" lzma

download: 
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(PKG_BUILD_DIR)/lzma
install:

ifneq ($(TARGET),)
install: compile
	cp $(PKG_BUILD_DIR)/lzma $(TARGET)
endif

clean:
	rm -rf $(PKG_BUILD_DIR)
