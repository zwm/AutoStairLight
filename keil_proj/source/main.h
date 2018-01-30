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

// Distance in mm
// LED ON DELAY in second
#define US_DIST_THRES       100     // <100mm led_on
#define LED_ON_DELAY        5       // led on delay
#define PHOTO_SENS_EN       0       // 1 : enable GuangMinDianZu
#define PHOTO_SENS_ACTIVE   0       // GenJu DianLu XiuGai

#define STAIR_LED_ON        1       //

// global control
// To enable keil simulation, change lcd1602.c L20, it will not block simulation
// When download to hardware, must remove this definition! 201801051341
//#define SIMULATE_ONLY
// Print debug info to LCD, it's better to keep it!
//#define PRINT_DEBUG
#define JOY_SYS_COMPAT

// check mode of ultra-sonic echo signal
// only one should be defined! else INT mode will be choosen
//#define CHECK_MODE_INT
#define CHECK_MODE_POLL

#define SYS_TICK_CYC        500         // unit ms
#define MEAS_ERR_GAP          0         // When no echo of ultra-sonic detected, delay 'n' ticks to start the next measurement
                                        // for test, we can set it '0' to enable measure 4 times each second!
//---------------------------------------------------------------------------
// EN_US0 : enable untrasonic 0
// EN_US1 : enable untrasonic 1
//---------------------------------------------------------------------------
//#define EN_US0      1
//#define EN_US1      1

#endif
