cforth
======

# Smartfusion additions

added subdirectories in build, src/app, src/platform for the SmartFusion A2F200M3 Eval Kit.
To build:
```
cd build/arm-smartfusion
make clean
make
```

Requires arm-none-eabi toolchain:
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
built with Version 9-2019-q4-major

## Adding Forth words -- in Forth 
see: src/app/arm-smartfusion/app.fth
* added Z.R, timertest and xdump
  * xdump has was patterned after the ldump word in  src/cforth/dump.fth.
* ldump cannot be used in the Fabric Space because the Ascii display attempts 8 bit reads.
  * THe Fabric address space only accepts 32 bit read/write.

## Adding new Forth words in C 
see: src/platform/arm-smartfusion/extend.c

* added timer1-init,timer1-enabled,timer1-start,timer1-stop,fabric-enable,fabric-disable
* examples for adding ISRs are in src/platform/arm-smartfusion/extend.c
  * aded examples for Timer1 and Fabric Interrupts

## Build options:
in src/platform/arm-smatfusion/targets.mk there are three options for selecting the .ld file used for the build
comment/uncomment as desired:
* debug-in-actel-smartfusion-envm.ld -- for load via JLink to Flash @0x60000000 - mirrored to 0x00000000
* production-execute-in-place.ld  --- for building into Libero design - loads at 0x00000000  -- cannot load via JLink
* production-relocateable-executable.ld --  not used -- kept as an example must alos comment/uncomment objcopy command a few lines below as noted,

## Loading via JLink:
connect terminal session via USB
screen /dev/ttyUSB0 57600

### JLink EDU (full sized)
```
JLinkExe
connect
Device: A2F200M3F
Target Interface: JTAG -- accept default
JTConf - Auto-Detect -- accept default
Speed 4000 kHz  -- accept default
 after connect:
 loadfile app.hex
 setPC 0
 g
 code should now be running
```

### JLink mini EDU
```
JLinkExe
connect
Device: A2F200M3F
Target Interface: SWD 
Speed 4000 kHz  -- accept default
 after connect:
 loadfile app.hex
 setPC 0
 g
 code should now be running
``` 

## using GDB

connect terminal session via USB
screen /dev/ttyUSB0 57600

### full sized EDU
```
JLinkGDBServer -i JTAG -device A2F2000M3F
```
### mini EDU
```
JLinkGDBServer -i SWD -device A2F2000M3F
```
it will connect and listen on port 2331

open another session
```
arm-none-eabi-gdb
tar extended-remote:2331
file app.elf
(respond y - to prompts for loading file and symbols)
load
c
```

the code should be running in your terminal window.
