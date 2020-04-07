// Edit this file to include C routines that can be called as Forth words.
// See "ccalls" below.

#include "forth.h"
#include "mss_watchdog.h"
#include "mss_timer.h"

// Prototypes

// FPGA registers - relative to *fgpabase - 0x40050000
#define FPGADATE 0x80/4
#define FPGAVERSION 0x84/4


volatile unsigned long *fpgabase = (volatile unsigned long *)0x40050000;

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

cell wdog()
{
    // reload watchdog
    MSS_WD_reload();
}

cell wdog_disable()
{
    // disable watchdog
    MSS_WD_disable();
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

cell timer1_init(cell counter)
{
    // initialize timer 1
    MSS_TIM1_init(MSS_TIMER_PERIODIC_MODE);
    MSS_TIM1_load_background(counter);
}

cell timer1_start()
{
    // start timer 1
    MSS_TIM1_start();
}

cell timer1_stop()
{
    // start timer 1
    MSS_TIM1_stop();
}

cell timer1_enable()
{
    // enable timer 1 IRQ
    MSS_TIM1_enable_irq();
}

cell timer1_disable()
{
    // disable timer 1 IRQ
    MSS_TIM1_disable_irq();
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
  C(wdog)            //c wdog            { -- }
  C(wdog_disable)    //c wdog-disable    { -- }
  C(getfpgadate)     //c fpgadate@       { -- i.fpgadate }
  C(getfpgaversion)  //c fpgaversion@    { -- i.fpgaversion }
  C(timer1_init)     //c timer1-init     { i.value -- }
  C(timer1_start)    //c timer1-start    { -- }
  C(timer1_stop)     //c timer1-stop     { -- }
  C(timer1_enable)   //c timer1-enable   { -- }
  C(timer1_disable)  //c timer1-disable  { -- }
};


// Forth words to call the above routines may be created by:
//
// system also
// 0 ccall: spi-send     { i.adr i.len -- }

//  1 ccall: byterev  { s.in -- s.out }
//
// and could be used as follows:
//
//  5 6 sum .
//  p" hello"  byterev  count type


// Timer1 ISR
void Timer1_IRQHandler(void)
{
    // pulse counters
    fpgabase[0x4080/4] = 0x1fffff;
    // toggle LEDs
    fpgabase[0] ^= 0xff;
    MSS_TIM1_clear_irq();
}
