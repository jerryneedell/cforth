cforth
======

# Smartfusion2 additions

added subdirectories in build, src/app, src/platform for the Smartfusion2  Eval Kit.
To build:
```
cd build/arm-smartfusion2
make clean
make
```


Requires arm-none-eabi toolchain:
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
built with Version 10-2020-q4-major

## Adding Forth words -- in Forth 
see: src/app/arm-smartfusion2/app.fth
* added Z.R, timertest and xdump
  * xdump has was patterned after the ldump word in  src/cforth/dump.fth.
* ldump cannot be used in the Fabric Space because the Ascii display attempts 8 bit reads.
  * THe Fabric address space only accepts 32 bit read/write.

## Adding new Forth words in C 
see: src/platform/arm-smartfusion2/extend.c

* examples for adding ISRs are in src/platform/arm-smartfusion2/extend.c
  * aded example for Fabric Interrupt for PPS

## Build options:
in src/platform/arm-smatfusion2/targets.mk there are three options for selecting the .ld file used for the build
comment/uncomment as desired:
* debug-in-microsemi-smartfusion2-envm.ld -- for load via JLink to Flash @0x60000000 - mirrored to 0x00000000
* production-smartfusion2-execute-in-place.ld  --- for building into Libero design - loads at 0x00000000  -- cannot load via JLink
* production-production-smartfusion2-relocate-to-external-ram.ld --  not used -- kept as an example must alos comment/uncomment objcopy command a few lines below as noted,

## Loading via JLink:
connect terminal session via USB
screen /dev/ttyUSB0 57600

### JLink EDU (full sized)
```
JLinkExe
connect
Device: M2S090
Target Interface: JTAG -- accept default
JTConf - Auto-Detect -- accept default
Speed 4000 kHz  -- accept default
 after connect:
 loadfile app.hex
 setPC 0
 g
 code should now be running

or exit JLinkEXe after loadfile (q) and press RESET on the board
```

### JLink mini EDU
```
JLinkExe
connect
Device: M2S090
Target Interface: SWD 
Speed 4000 kHz  -- accept default
 after connect:
 loadfile app.hex
 setPC 0
 g
 code should now be running

or exit JLinkEXe after loadfile (q) and press RESET on the board
``` 

## using GDB

connect terminal session via USB
screen /dev/ttyUSB0 57600

### full sized EDU
```
JLinkGDBServer  -device M2S090
```
### mini EDU
```
JLinkGDBServer -i SWD -device M2S090
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
