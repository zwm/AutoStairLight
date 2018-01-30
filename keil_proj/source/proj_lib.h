#ifndef __PROJ_LIB_H__
#define __PROJ_LIB_H__

#include "mcu.h"
#include "delay.h"
#include "lcd1602.h"
#include "main.h"

// FSM
// 0 : FSM_US01    start measurement of two ultrasonic modules
// 1 : FSM_TICK    process time tick of 1/4 second
// 2 : FSM_ERRO    no echo received process
#define FSM_US01                0
#define FSM_TICK                1
#define FSM_ERRO                2
//#define FSM_ERR_US0_MULTI_RESP  3
//#define FSM_ERR_US1_MULTI_RESP  4
//#define FSM_ERR_US0_TIMEOUT     5
//#define FSM_ERR_US1_TIMEOUT     6
// Index of timer
// 0: T0
// 1: T0
#define TMR_IDX_0       0
#define TMR_IDX_1       1
// Mode of timer
// 0: Pulse Width Test Mode
// 1: 10 ms timer mode
// 2: 50 ms timer mode
#define TMR_MOD_CAP     0
#define TMR_MOD_10MS    1
#define TMR_MOD_50MS    2
#define TMR_MOD_STOP    3
// Init value of timer
// MCU  : 6MHz 
// T0/1 : 6/12MHz
// 1cyc : 2us
#define TL_INIT_10MS    0x78
#define TH_INIT_10MS    0xEC
#define TL_INIT_50MS    0x58
#define TH_INIT_50MS    0x9E
// Index of int
// 0: INT0
// 1: INT1
#define INT_IDX_0       0
#define INT_IDX_1       1
// Mode of timer
// 0: Enable
// 1: Disable
#define INT_MOD_START   0
#define INT_MOD_STOP    1

#define BKL_ON          0
#define BKL_OFF         1

typedef struct{
    u8 cnt;
    u8 bak;
} STRUCT_INT_CNT;

//---------------------------------------------------------------------------
// Global Variable Declearation
//---------------------------------------------------------------------------
extern volatile u16  time_tick;
extern volatile u16  time_sec;
extern volatile u8   fsm;
extern volatile u8   us0_gap;
extern volatile u8   us1_gap;
extern volatile u16  us0_err;
extern volatile u16  us1_err;
extern volatile bit  stair_idle;
extern volatile bit  us0_frash;
extern volatile bit  us1_frash;
extern volatile u16  us0_meas[4];
extern volatile u16  us1_meas[4];
extern volatile STRUCT_INT_CNT data st_x0;
extern volatile STRUCT_INT_CNT data st_x1;
extern volatile STRUCT_INT_CNT data st_t0;
extern volatile STRUCT_INT_CNT data st_t1;
extern volatile STRUCT_INT_CNT data *st_p;

u8 inc_check(STRUCT_INT_CNT *p);
void sys_init (void);
void trig_us0 (void);
void trig_us1 (void);
void led_light (unsigned char sw);
void fsm_us01_proc(void);
void fsm_tick_proc(void);
void fsm_erro_proc(void);

#endif
