# Builds CForth for RKT SMARTFUSION

TOPDIR=../..

CONFIG += -DBITS32 -DT16

ifneq "$(shell getconf LONG_BIT)" "32"
CFLAGS += -m32
endif

default: app.elf app.bin app.nm app.dump



include $(TOPDIR)/src/app/arm-smartfusion2/targets.mk

