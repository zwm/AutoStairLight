#ifndef __MAIN_H__
#define __MAIN_H__

//******************************************************
//* headers
//******************************************************
#include "delay.h"
#include "lcd1602.h"
#include "proj_lib.h"
#include "pin_conf.h"
#include "interrupt_service.h"
//******************************************************

// global control
// To enable keil simulation, change lcd1602.c L20, it will not block simulation
// When download to hardware, must remove this definition! 201801051341
//#define SIMULATE_ONLY
// Print debug info to LCD, it's better to keep it!
#define PRINT_DEBUG
#define JOY_SYS_COMPAT
//---------------------------------------------------------------------------
// EN_US0 : enable untrasonic 0
// EN_US1 : enable untrasonic 1
//---------------------------------------------------------------------------
//#define EN_US0      1
#define EN_US1      1

#endif
