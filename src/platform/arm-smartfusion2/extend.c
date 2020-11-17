// Edit this file to include C routines that can be called as Forth words.
// See "ccalls" below.

#include "forth.h"
#include "mss_rtc.h"

// Prototypes
#define FORTHVERSION 0x0001
#define FORTHDATE 0x7e40b07
// FPGA registers - relative to *fgpabase - 0x30000000
#define FPGADATE 0x80/4
#define FPGAVERSION 0x84/4
#define PPS_CLEAR 0x10000C/4
#define LED 0

volatile unsigned long *fpgabase = (volatile unsigned long *)0x30000000;


void lfill(cell value, cell len, cell adr)
{
    unsigned long *p = (unsigned long *)adr;
    while(len>0) {
        *p++ = value;
        len -= sizeof(long);
    }
}
cell lcheck(cell value, cell len, cell adr)
{
    unsigned long *p = (unsigned long *)adr;
    while(len>0) {
        if (*p != value)
            return (cell)p;
        p++;
        len -= sizeof(long);
    }
    return -1;
}
void incfill(cell len, cell adr)
{
    unsigned long *p = (unsigned long *)adr;
    while(len>0) {
        *p = (unsigned long)p;
        p++;
        len -= sizeof(long);
    }
}
cell inccheck(cell len, cell adr)
{
    unsigned long *p = (unsigned long *)adr;
    while(len>0) {
        if (*p != (cell)p)
            return (cell)p;
        p++;
        len -= sizeof(long);
    }
    return -1;
}
#define NEXTRAND(n) ((n*1103515245+12345) & 0x7fffffff)
void randomfill(cell len, cell adr)
{
    unsigned long *p = (unsigned long *)adr;
    unsigned long value = 0;
    while(len>0) {
        value = NEXTRAND(value);
        *p = value;
        p++;
        len -= sizeof(long);
    }
}
cell randomcheck(cell len, cell adr)
{
    unsigned long *p = (unsigned long *)adr;
    unsigned long value = 0;
    while(len>0) {
        value = NEXTRAND(value);
        if (*p != value)
            return (cell)p;
        p++;
        len -= sizeof(long);
    }
    return -1;
}



cell byte_checksum(cell len, cell adr)
{
    unsigned char *p = (unsigned char *)adr;
    unsigned long value = 0;
    while(len--) {
        value += *p++;
    }
    return value;
}


cell getfpgadate()
{
    unsigned long int fpgadate;
    fpgadate = fpgabase[FPGADATE];
    return fpgadate;
}

cell getfpgaversion()
{
    unsigned long int fpgaversion;
    fpgaversion = fpgabase[FPGAVERSION];
    return fpgaversion;
}


cell getforthdate()
{
    return FORTHDATE;
}

cell getforthversion()
{
    return FORTHVERSION;
}



cell rtc_init()
{
    // initialize RTC
    MSS_RTC_init(MSS_RTC_BINARY_MODE, 32767);
}

cell rtc_start()
{
    // start  RTC
    MSS_RTC_start();
}

cell rtc_stop()
{
    // stop  RTC
    MSS_RTC_stop();
}

cell rtc_read()
{
    unsigned long long value;
    value = MSS_RTC_get_binary_count();
    return (unsigned long)value;
}

cell pps_enable()
{
     /* Clear Pending PPS Interrupt*/
    NVIC_ClearPendingIRQ(FabricIrq0_IRQn);

     /* Enable Fabric Interrupt*/
    NVIC_EnableIRQ(FabricIrq0_IRQn);

}

cell pps_disable()
{
     /* Disabling PPS Interrupt*/
    NVIC_DisableIRQ(FabricIrq0_IRQn);
     /* Clear Pending Fabric Interrupts*/
    NVIC_ClearPendingIRQ(FabricIrq0_IRQn);

}


cell ((* const ccalls[])()) = {
// Add your own routines here
  C(lfill)           //c lfill           { a.adr i.len i.value -- }
  C(lcheck)          //c lcheck          { a.adr i.len i.value -- i.erraddr }
  C(incfill)         //c inc-fill        { a.adr i.len -- }
  C(inccheck)        //c inc-check       { a.adr i.len -- i.erraddr }
  C(randomfill)      //c random-fill     { a.adr i.len -- }
  C(randomcheck)     //c random-check    { a.adr i.len -- i.erraddr }
  C(byte_checksum)   //c byte-checksum   { a.adr i.len -- i.checksum }
  C(getforthdate)    //c forthdate@      { -- i.forthdate }
  C(getforthversion) //c forthversion@   { -- i.forthversion }
  C(getfpgadate)     //c fpgadate@       { -- i.fpgadate }
  C(getfpgaversion)  //c fpgaversion@    { -- i.fpgaversion }
  C(rtc_init)        //c rtc-init        { -- }
  C(rtc_start)       //c rtc-start       { -- }
  C(rtc_stop)        //c rtc-stop        { -- }
  C(rtc_read)        //c rtc-read        { -- i.rtc_binary }
  C(pps_enable)   //c pps-enable   { -- }
  C(pps_disable)  //c pps-disable  { -- }

};


void FabricIrq0_IRQHandler(void)
{
    // reset the PPS signal
    fpgabase[PPS_CLEAR] = 0;
    // toggle LEDs
    fpgabase[LED] ^= 0xff;
    NVIC_ClearPendingIRQ(FabricIrq0_IRQn);

}
