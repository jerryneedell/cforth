// Edit this file to include C routines that can be called as Forth words.
// See "ccalls" below.

#include "forth.h"
#include "mss_watchdog.h"
#include "mss_timer.h"

// Prototypes

// FPGA registers - relative to *fgpabase - 0x40050000
#define FPGADATE 0x80/4
#define FPGAVERSION 0x84/4
#define FORTHVERSION 0x0005
#define FORTHDATE 0x7e40c08
#define LED 0
#define SCRATCH0 0x00
#define SCRATCH1 0x04/4
#define SCRATCH2 0x08/4
#define SCRATCH3 0x0C/4
#define SCRATCH4 0x10/4
#define SCRATCH5 0x14/4
#define SCRATCH6 0x18/4
#define SCRATCH7 0x1C/4
#define DIP_SWITCH 0x40/4
#define INT_1KHZ 0x1
#define INT_128HZ 0x2
#define INT_2HZ 0x4
#define ACI_SYNC 0x20
#define MAG_SYNC 0x40
#define ACE_SYNC 0x80
#define INTR 0x20/4
#define INTR_CLR 0x20/4
#define  FABRIC_INT_CTRL_BASE_ADDR 0x40051000
#define  GPIO_INTR_CTRL_BASE_ADDR 0x40050100
// enable 2Hz Interrupt
#define  FABRIC_INTR_ENBL 0x00000007
#define  FABRIC_INTR_CLR 0xFFFFFFFF
#define  FABRIC_INTR_SOFT_ENBL_OFF_ADDR 0x00000018
#define  FABRIC_INTR_SOFT_CLR_OFF_ADDR 0x0000001C
#define  FABRIC_INTR_ENBL_OFF_ADDR 0x00000020
#define  FABRIC_INTR_CLR_OFF_ADDR 0x00000024
#define  FABRIC_INTR_RAW_STAT_OFF_ADDR 0x00000028
#define  FABRIC_INTR_STAT_OFF_ADDR 0x0000002C
#define  GPIO_INTR_CLR_OFF_ADDR 0x00000080
#define  FIQ_SOFT_INTR 0x00/4
#define  FIQ_SOFT_INTR_CLR 0x04/4
#define  FIQ_ENABLE 0x08/4
#define  FIQ_ENABLE_CLR 0x0C/4
#define  FIQ_RAW_STATUS 0x10/4
#define  FIQ_STATUS 0x14/4
#define  IRQ_SOFT_INTR 0x18/4
#define  IRQ_SOFT_INTR_CLR 0x1C/4
#define  IRQ_ENABLE 0x20/4
#define  IRQ_ENABLE_CLR 0x24/4
#define  IRQ_RAW_STATUS 0x28/4
#define  IRQ_STATUS 0x2C/4
#define  ACE_COMMAND 0x2000/4
#define  ACE_COMMAND_INTR 0x2004/4
#define  ACE_COMMAND_INTR_CLR 0x2004/4
volatile uint32_t *fpgabase = (volatile uint32_t *)0x40050000;
volatile uint32_t *fabricbase = (volatile uint32_t *)0x40051000;
uint32_t counter = 0;

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

cell fabric_enable()
{

    /* Configuring the Interrupt control register in FPGA fabric
     * This will enable the Interrupt in the FPGA*/
//    (*((uint32_t volatile *)(FABRIC_INT_CTRL_BASE_ADDR + FABRIC_INTR_ENBL_OFF_ADDR)) = FABRIC_INTR_ENBL);
    //fpgabase[INTR_CLR] = 7;

     /* Clear Pending Fabric Interrupts*/
    NVIC_ClearPendingIRQ(Fabric_IRQn);

     /* Enable Fabric Interrupt*/
    NVIC_EnableIRQ(Fabric_IRQn);
    fabricbase[IRQ_ENABLE] = FABRIC_INTR_ENBL;


}

cell fabric_disable()
{
    /* Configuring the Interrupt control register in FPGA fabric
     * This will disable the Interrupt in the FPGA*/
//    (*((uint32_t volatile *)(FABRIC_INT_CTRL_BASE_ADDR + FABRIC_INTR_CLR_OFF_ADDR)) = FABRIC_INTR_CLR);
   /* Disabling Fabric Interrupt*/
    fabricbase[IRQ_ENABLE_CLR] = FABRIC_INTR_CLR;
    NVIC_DisableIRQ(Fabric_IRQn);
     /* Clear Pending Fabric Interrupts*/
    NVIC_ClearPendingIRQ(Fabric_IRQn);
    //fpgabase[INTR_CLR] = 7;

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
  C(fabric_enable)   //c fabric-enable   { -- }
  C(fabric_disable)  //c fabric-disable  { -- }
};



/* Fabric Interrupt Handler */
/* This Interrupt handler executes upon the occurrence of
   Fabric interrupt, which is from the FPGA Fabric.
   This function reads the status register from the FPGA fabric
   and checks for the Interrupt source.
   */
void Fabric_IRQHandler( void )
{
    volatile uint32_t read_irq;
    fpgabase[LED] ^= 8;
    fpgabase[SCRATCH1]++;

    /* reading Interrupt Status register from Interrupt controller in FPGA fabric */
    read_irq = fabricbase[IRQ_STATUS];
    fpgabase[SCRATCH2] = read_irq;
    /* Identifying the source of the interrupt */
   if(read_irq&INT_1KHZ)
   {
        // toggle LEDs
        fpgabase[LED] ^=INT_1KHZ;
        fpgabase[INTR_CLR] = INT_1KHZ;
   }
   if(read_irq&INT_128HZ)
   {
        // toggle LEDs
        fpgabase[LED] ^=INT_128HZ;
        fpgabase[INTR_CLR] = INT_128HZ;
   }
   if(read_irq&INT_2HZ)
   {
        // toggle LEDs
        fpgabase[LED] ^=INT_2HZ;
        fpgabase[INTR_CLR] = INT_2HZ;
   }


   NVIC_ClearPendingIRQ( Fabric_IRQn );



  }

