#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/saket/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/home/saket/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/home/saket/Program/ti/ccsv7/ccs_base
override XDCROOT = /home/saket/Program/ti/xdctools_3_50_02_20_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/saket/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/home/saket/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/home/saket/Program/ti/ccsv7/ccs_base;/home/saket/Program/ti/xdctools_3_50_02_20_core/packages;..
HOSTOS = Linux
endif
