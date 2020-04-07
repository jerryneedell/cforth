// Character I/O stubs

#include "mss_uart.h"
#include "mss_watchdog.h"

#ifndef ACTEL_STDIO_BAUD_RATE
#define ACTEL_STDIO_BAUD_RATE  MSS_UART_57600_BAUD
#endif

/*------------------------------------------------------------------------------
 * Global flag used to indicate if the UART driver needs to be initialized.
 */
static int g_stdio_uart_init_done = 0;




void raw_putchar(char c)
{
  
/*--------------------------------------------------------------------------
     * Output text to the UART.
     */
    MSS_UART_polled_tx( &g_mss_uart0, (uint8_t *)&c, 1 );
    
}

int kbhit() {
  uint8_t key;

  return   MSS_UART_get_rx(&g_mss_uart0, &key, 1);
}

int getkey()
{
  uint8_t key;
  uint8_t rx_size;
  do
        {
            //Reload watchdog.
            MSS_WD_reload();
            rx_size = MSS_UART_get_rx(&g_mss_uart0, &key, 1);
        }while(rx_size == 0);



  return  key;
}

void init_io()
{

   /*--------------------------------------------------------------------------
     * Initialize the UART driver if it is the first time this function is
     * called.
     */
    if ( !g_stdio_uart_init_done )
    {
        MSS_UART_init( &g_mss_uart0, ACTEL_STDIO_BAUD_RATE, (MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY));
        g_stdio_uart_init_done = 1;
    }

 /*
      * Reload watchdog.
      */
     MSS_WD_reload();


}

int spins(int i)
{
  while(i--)
    asm("");  // The asm("") prevents optimize-to-nothing
}

int strlen(const char *s)
{
	const char *p;
	for (p=s; *p != '\0'; *p++) {
	}
	return p-s;
}

int __errno;
