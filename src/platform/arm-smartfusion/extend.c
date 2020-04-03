// Edit this file to include C routines that can be called as Forth words.
// See "ccalls" below.

#include "forth.h"

// Prototypes


#define DECLARE_REGS \
    volatile unsigned long *fifo = (volatile unsigned long *)0xd4035010; \
    volatile unsigned long *stat = (volatile unsigned long *)0xd4035008


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


void set_control_reg(cell arg)
{
    __asm__ __volatile__ (
        "mcr	p15, 0, %0, c1, c0, 0\n\t"
        : : "r" (arg));
}

cell get_control_reg()
{
    unsigned long value;
    __asm__ __volatile__ (
        "mrc	p15, 0, %0, c1, c0, 0\n\t"
        : "=r" (value));
    return value;
}

cell get_tcm_size()
{
    unsigned long value;
    __asm__ __volatile__ (
        "mrc	p15, 0, %0, c0, c0, 2\n\t"
        : "=r" (value));
    return value;
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

#define GPIO71_MASK 0x80
#define GPIO72_MASK 0x100
cell kbd_bit_in()
{
    volatile unsigned long *kbdgpio = (unsigned long *)0xd4019008;
    unsigned long regval;
    unsigned long bitval;
    do {
	regval = *kbdgpio;
    } while ((regval & GPIO71_MASK) != 0);

    bitval = (regval & GPIO72_MASK) ? 0x100 : 0;
    
    do {
	regval = *kbdgpio;
    } while ((regval & GPIO71_MASK) == 0);

    return bitval;
}
#define DIR_OUT 0x15
#define DIR_IN 0x18
cell kbd_bit_out(cell bitval)
{
    volatile unsigned long *kbdgpio = (unsigned long *)0xd4019008;
    unsigned long regval;
    do {
	regval = *kbdgpio;
    } while ((regval & GPIO71_MASK) != 0);

    // Seting direction IN pulls up to 1, OUT drives to 0
    kbdgpio[bitval ? DIR_IN : DIR_OUT] = GPIO72_MASK;
    
    do {
	regval = *kbdgpio;
    } while ((regval & GPIO71_MASK) == 0);

    return bitval;
}


cell ((* const ccalls[])()) = {
// Add your own routines here
  C(lfill)           //c lfill           { a.adr i.len i.value -- }
  C(lcheck)          //c lcheck          { a.adr i.len i.value -- i.erraddr }
  C(incfill)         //c inc-fill        { a.adr i.len -- }
  C(inccheck)        //c inc-check       { a.adr i.len -- i.erraddr }
  C(randomfill)      //c random-fill     { a.adr i.len -- }
  C(randomcheck)     //c random-check    { a.adr i.len -- i.erraddr }
  C(get_control_reg) //c control@        { -- i.value }
  C(set_control_reg) //c control!        { i.value -- }
  C(get_tcm_size)    //c tcm-size@       { -- i.value }
  C(byte_checksum)   //c byte-checksum   { a.adr i.len -- i.checksum }
  C(kbd_bit_in)      //c kbd-bit-in      { -- i.value }
  C(kbd_bit_out)     //c kbd-bit-out     { i.value -- }
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
