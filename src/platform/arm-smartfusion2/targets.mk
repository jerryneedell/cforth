# Makefile fragment for the final target application

SRC=$(TOPDIR)/src

# Target compiler definitions
CROSS ?= arm-none-eabi-
CPU_VARIANT=-mthumb -mcpu=cortex-m3
include $(SRC)/cpu/arm/compiler.mk


# extra definitions
DEFS += 

DICTIONARY=ROM

DICTSIZE ?= 0x3000

TCFLAGS += -Os --specs=nano.specs

# Use thumb instruction set to save a little space
TCFLAGS += -mthumb

# Omit unreachable functions from output

TCFLAGS += -ffunction-sections -fdata-sections
TCFLAGS += -ffunction-sections -fdata-sections
TLFLAGS += --gc-sections

VPATH += $(SRC)/platform/arm-smartfusion2
VPATH += $(SRC)/lib

# This directory, including board information
INCS += -I$(SRC)/platform/arm-smartfusion2

include $(SRC)/common.mk
include $(SRC)/cforth/targets.mk

# Platform-specific object files for low-level startup and platform I/O


# FIRST_OBJ = tstartup_stm32f10x_mdp.o
FIRST_OBJ = tstartup_m2sxxx.o

PLAT_OBJS += ttmain.o mallocembed.o
PLAT_OBJS += tconsoleio.o 
PLAT_OBJS += tmss_uart.o
PLAT_OBJS += tmss_rtc.o
PLAT_OBJS += tmss_ethernet_mac.o
PLAT_OBJS += tnewlib_stubs.o
PLAT_OBJS += tcore_cm3.o
PLAT_OBJS += tsystem_m2sxxx.o
PLAT_OBJS += tsys_config.o
PLAT_OBJS += tsys_config_SERDESIF_0.o

ttmain.o: vars.h


PLAT_OBJS += $(EXTEND_OBJS)

# Object files for the Forth system and application-specific extensions

FORTH_OBJS = tembed.o textend.o


# Recipe for linking the final image

# use this for a version that cah be uplaoded via J-Link 
LDSCRIPT = $(SRC)/platform/arm-smartfusion2/debug-in-microsemi-smartfusion2-envm.ld
# use this for a version to be embedded in to Libero build
#LDSCRIPT = $(SRC)/platform/arm-smartfusion2/production-smartfusion2-execute-in-place.ld
#  not currently used  -- keeping as an example
#LDSCRIPT = $(SRC)/platform/arm-smartfusion2/production-smartfusion2-relocate-executable.ld


TLDBODY = -T$(LDSCRIPT) $(FIRST_OBJ) $(PLAT_OBJS) $(FORTH_OBJS)
LDCMD := $(TLD) $(TLFLAGS) \
	   $(TLDBODY) 

app.elf: $(FIRST_OBJ) $(PLAT_OBJS) $(FORTH_OBJS)
	@echo Linking $@ ... 
	arm-none-eabi-g++ $(FIRST_OBJ) $(PLAT_OBJS) $(FORTH_OBJS) -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T$(LDSCRIPT) -Xlinker --gc-sections -Wl,-Map,"app.map" --specs=nano.specs -o $@ $(OBJS) $(USER_OBJS) $(LIBS)
	arm-none-eabi-objcopy -O ihex app.elf app.hex
	arm-none-eabi-size --format=berkeley app.elf
#  use this only with the production-relocatable-executable.ld
#	arm-none-eabi-objcopy -O ihex app.elf app.hex --change-section-lma *-0x60000000



# LIBM := $(shell $(TCC) --print-file-name=libm.a)
# LIBC := $(shell $(TCC) --print-file-name=libc.a)

# This rule extracts the executable bits from an ELF file, yielding raw binary.

%.img: %.elf
	@$(TOBJCOPY) -O binary $< $@
	date  "+%F %H:%M" >>$@
	@ls -l $@

# Override the default .dump rule to include the interrupt vector table

%.dump: %.elf
	@$(TOBJDUMP) -s  $< >$@
	@$(TOBJDUMP) --disassemble $< >>$@

# This rule builds a date stamp object that you can include in the image
# if you wish.

EXTRA_CLEAN += *.bin *.elf *.dump *.nm *.img date.c *.map *.hex $(FORTH_OBJS) $(PLAT_OBJS)

include $(SRC)/cforth/embed/targets.mk
