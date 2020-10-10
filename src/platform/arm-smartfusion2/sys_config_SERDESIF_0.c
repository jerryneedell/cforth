/*-----------------------------------------------------------*/
/* SERDESIF_0 Initialization */
/*-----------------------------------------------------------*/

#include <stdint.h>
#include "../../CMSIS/sys_init_cfg_types.h"

const cfg_addr_value_pair_t g_m2s_serdes_0_config[] =
{
    { (uint32_t*)( 0x40028000 + 0x2028 ), 0x80F },
    { (uint32_t*)( 0x40028000 + 0x1c28 ), 0x0 },
    { (uint32_t*)( 0x40028000 + 0x1e00 ), 0x1 },
    { (uint32_t*)( 0x40028000 + 0x2028 ), 0xf0F }
};

