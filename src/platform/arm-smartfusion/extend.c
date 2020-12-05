// Edit this file to include C routines that can be called as Forth words.
// See "ccalls" below.

#include "forth.h"
#include "mss_watchdog.h"
#include "mss_timer.h"

// Prototypes

// FPGA registers - relative to *fgpabase - 0x40050000
#define FPGADATE 0x80/4
#define FPGAVERSION 0x84/4
#define FORTHVERSION 0x0000
#define FORTHDATE 0x7e40c05
#define LED 0
#define CICCLEAR 0x200C/4
#define COUNTER 0xA00000
#define MAJOR_FRAME 0x1
#define MINOR_FRAME 0x2
#define  FABRIC_INT_CTRL_BASE_ADDR 0x40051000
#define  FABRIC_GPIO_BASE_ADDR 0x40050100
// enable Major Frame and Minor Interrupt
#define  FABRIC_INTR_ENBL 0x00000003
#define  FABRIC_INTR_CLR 0xFFFFFFFF
#define  FABRIC_INTR_ENBL_OFF_ADDR 0x00000020
#define  FABRIC_INTR_STAT_OFF_ADDR 0x0000002C
#define  FABRIC_INTR_CLR_OFF_ADDR 0x00000080

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

cell getforthdate()
{
    return FORTHDATE;
}

cell getforthversion()
{
    return FORTHVERSION;
}

cell timer1_init(cell counter)
{
    // initialize timer 1
    MSS_TIM1_init(MSS_TIMER_PERIODIC_MODE);
    MSS_TIM1_load_background(counter);
}

cell fabric_enable()
{
     /* Clear Pending Fabric Interrupts*/
    NVIC_ClearPendingIRQ(Fabric_IRQn);
\
     /* Enable Fabric Interrupt*/
    NVIC_EnableIRQ(Fabric_IRQn);


    /* Configuring the Interrupt control register in FPGA fabric
     * This will enable the Interrupt in the FPGA*/
    (*((uint32_t volatile *)(FABRIC_INT_CTRL_BASE_ADDR + FABRIC_INTR_ENBL_OFF_ADDR)) = FABRIC_INTR_ENBL);

}

cell fabric_disable()
{
    /* Configuring the Interrupt control register in FPGA fabric
     * This will disable the Interrupt in the FPGA*/
    (*((uint32_t volatile *)(FABRIC_INT_CTRL_BASE_ADDR + FABRIC_INTR_CLR_OFF_ADDR)) = FABRIC_INTR_CLR);
     /* Disabling Fabric Interrupt*/
    NVIC_DisableIRQ(Fabric_IRQn);
     /* Clear Pending Fabric Interrupts*/
    NVIC_ClearPendingIRQ(Fabric_IRQn);

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
  C(getforthdate)    //c forthdate@      { -- i.forthdate }
  C(getforthversion) //c forthversion@   { -- i.forthversion }
  C(timer1_init)     //c timer1-init     { i.value -- }
  C(timer1_start)    //c timer1-start    { -- }
  C(timer1_stop)     //c timer1-stop     { -- }
  C(timer1_enable)   //c timer1-enable   { -- }
  C(timer1_disable)  //c timer1-disable  { -- }
  C(fabric_enable)   //c fabric-enable   { -- }
  C(fabric_disable)  //c fabric-disable  { -- }
};



// Timer1 ISR
void Timer1_IRQHandler(void)
{
    // pulse counters
    fpgabase[0x4080/4] = 0x1fffff;
    // toggle LEDs
    fpgabase[LED] ^= 0xff;
    MSS_TIM1_clear_irq();
}



/* Fabric Interrupt Handler */
/* This Interrupt handler executes upon the occurrence of
   Fabric interrupt, which is from the FPGA Fabric.
   This function reads the status register from the FPGA fabric
   and checks for the Interrupt source.
   */
void Fabric_IRQHandler( void )
{
    uint32_t read_irq;

    /* reading Interrupt Status register from Interrupt controller in FPGA fabric */
    read_irq = (*((uint32_t volatile *)(FABRIC_INT_CTRL_BASE_ADDR + FABRIC_INTR_STAT_OFF_ADDR)));
    /* Identifying the source of the interrupt */
    if(read_irq&1)
    {
        // toggle LEDs
        fpgabase[LED] ^= 0xaa;
        // clear the interrupt
        fpgabase[CICCLEAR] = MAJOR_FRAME;
        read_irq &= 0xFFFFFFFE;
    }
    if(read_irq&2)
    {
        // toggle LEDs
        fpgabase[LED] ^= 0x55;
        // clear the interrupt
        fpgabase[CICCLEAR] = MINOR_FRAME;
        read_irq &= 0xFFFFFFFD;
    }
        read_irq = 0;

    /* Clearing the Interrupts in the FPGA */
    (*((uint32_t volatile *)(FABRIC_GPIO_BASE_ADDR + FABRIC_INTR_CLR_OFF_ADDR)) = FABRIC_INTR_CLR);

    NVIC_ClearPendingIRQ( Fabric_IRQn );

  }

