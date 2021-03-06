#!/usr/bin/env python
#-*- coding: utf-8 -*-

import os, sys, glob, re

boxs = ["nb4", "cibox"]
builds = ["main", "openwrt"]
actions = ["compile", "install", "clean"]

pkgmkfilename = "include/packages.mk"

######################################################

header = """
# generated by %s
#
# Package build
#
PACKAGE	:= $(subst $(FIRMWARE)-,,$(MAKECMDGOALS))
define Package/Build
	$(call OpenWRT/Build,package/$(1)$(if $(1),/,)$(PACKAGE))
endef

""" % (sys.argv[0],)

header_sub = """
##################################
### %s packages
%s-package: .firmware
	$(call Package/Build,%s)

"""
######################################################

gen = header

def gensubheader(hdname, hdcat):
    global gen
    
    gen += header_sub % (hdname, hdname, hdcat)

def genrule(hdname, pkgname):
    global gen
    
    rule = '# %s\n' % (pkgname,)
    for box in boxs:
        for build in builds:
            for action in actions:
                rule += "%s-%s-%s-%s: %s-package\n" % (box, build, 
                                                      pkgname, action, hdname)
    gen += rule + "\n"

### main ###

# make rule for openwrt packages
filelist = glob.glob('openwrt/package/*')

if len(filelist) == 0:
    print "You need to execute the script in main dir (before openwrt/ directory)"
    sys.exit(1)

gensubheader("openwrt", "")

for file in filelist:
    
    if os.path.isdir(file):
        bfile = os.path.basename(file)
        
        if bfile == "feeds":
            # feeds packages, do after !
            continue

        genrule("openwrt", bfile)

# make rule for feed packages
filelist = glob.glob('openwrt/package/feeds/*')

for file in filelist:
    
    if os.path.isdir(file):
        # feed repo
        bfile = os.path.basename(file)

        gensubheader(bfile, "feeds/%s" % (bfile,))
        
        feedfilelist = glob.glob('%s/*' % (file,) )
        
        for feedfile in feedfilelist:
            if os.path.isdir(feedfile):
                bfeedfile = os.path.basename(feedfile)

                genrule(bfile, bfeedfile)

pkgmkfile = open(pkgmkfilename, "w")

pkgmkfile.write(gen)

pkgmkfile.close()
