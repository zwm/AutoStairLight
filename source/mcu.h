#ifndef	__MCU__
#define	__MCU__

#include "stc15fxxxx.h"
#include "proj_lib.h"

// Function Declearation
void mcu_init (void);
void mcu_set_exint (void);
void mcu_set_tmr (unsigned char idx, unsigned char mode);

#endif
