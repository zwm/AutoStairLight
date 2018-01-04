#include "mcu.h"
#include "delay.h"
#include "lcd1602.h"

// FSM
// 0 : FSM_TUS0
// 1 : FSM_TUS1
// 2 : FSM_WAIT
#define FSM_TUS0                0
#define FSM_TUS1                1
#define FSM_WAIT                2
#define FSM_US0_MULTI_RESP      3
#define FSM_US1_MULTI_RESP      3
#define FSM_US0_TIMEOUT         3
#define FSM_US1_TIMEOUT         3
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
#define INT_START       0
#define INT_STOP        1

//******************************************************
//* Structure define
//******************************************************
typedef struct {
    unsigned char cnt;
    unsigned char bak;
} STRUCT_INT_CNT;

//---------------------------------------------------------------------------
// Global Variable Declearation
//---------------------------------------------------------------------------
extern uint16  time_ms;
extern uint16  time_qua;
extern uint16  time_sec;
extern uchar   fsm;
extern uchar   us0_frash;
extern uchar   us1_frash;
extern uint16  us0_meas[4];
extern uint16  us1_meas[4];
extern STRUCT_INT_CNT data st_x0;
extern STRUCT_INT_CNT data st_x1;
extern STRUCT_INT_CNT data st_t0;
extern STRUCT_INT_CNT data st_t1;
extern STRUCT_INT_CNT data *st_p;

unsigned char inc_check (STRUCT_INT_CNT *p);
void fsm_tus0_proc(void);
void fsm_tus1_proc(void);
void fsm_wait_proc(void);
void fsm_erro_proc(void);
