#ifndef	__MCU__
#define	__MCU__

#include "stc15fxxxx.h"
#include "proj_lib.h"

//---------------------------------------------------------------------------
// CLOCK: 6MHz
//---------------------------------------------------------------------------
#define MAIN_Fosc		 6000000L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟
#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

// type define
typedef unsigned short int uint16;
typedef unsigned char uchar;

//---------------------------------------------------------------------------
// Function Declearation
//---------------------------------------------------------------------------
void mcu_init (void);
void mcu_set_exint (void);
void mcu_set_tmr (unsigned char idx, unsigned char mode);

#endif
