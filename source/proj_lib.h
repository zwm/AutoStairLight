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
// Init value of timer
// MCU  : 6MHz 
// T0/1 : 6/12MHz
// 1cyc : 2us
#define TL_INIT_10MS    0x78
#define TH_INIT_10MS    0xEC
#define TL_INIT_50MS    0x58
#define TH_INIT_50MS    0x9E

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
extern STRUCT_INT_CNT data st_x0;
extern STRUCT_INT_CNT data st_x1;
extern STRUCT_INT_CNT data st_t0;
extern STRUCT_INT_CNT data st_t1;

unsigned char inc_check (STRUCT_INT_CNT *p);

