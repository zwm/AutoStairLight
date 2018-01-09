#include "proj_lib.h"

//---------------------------------------------------------------------------
// Global Variable Define
//---------------------------------------------------------------------------
volatile u16  time_ms;
volatile u16  time_qua;
volatile u16  time_sec;
volatile u8   fsm;
volatile u8   us0_frash;
volatile u8   us1_frash;
volatile u16  us0_meas[4];
volatile u16  us1_meas[4];
volatile u16  tt;
volatile STRUCT_INT_CNT data st_x0;
volatile STRUCT_INT_CNT data st_x1;
volatile STRUCT_INT_CNT data st_t0;
volatile STRUCT_INT_CNT data st_t1;
volatile STRUCT_INT_CNT data *st_p;

//---------------------------------------------------------------------------
// Check struct increased by interrupt
//---------------------------------------------------------------------------
u8 inc_check (STRUCT_INT_CNT *p)
{
    u8 cur;
    cur = p->cnt;
    if (cur > p->bak)       // normal increase
    {
        cur = cur - p->bak;
        p->bak = p->cnt;
    }
    else if (cur < p->bak)  // overflow increase
    {
        cur = 256 - p->bak + cur;
        p->bak = p->cnt;
    }
    else                    // no increase
    {
        cur = 0;
    }
    return cur;
}

//---------------------------------------------------------------------------
// Initial System
//---------------------------------------------------------------------------
void sys_init (void)
{
    // device init
    mcu_init ();
    LcdInit ();
    // parameter init
    st_x0.cnt = 0;
    st_x0.bak = 0;
    st_x1.cnt = 0;
    st_x1.bak = 0;
    st_t0.cnt = 0;
    st_t0.bak = 0;
    st_t1.cnt = 0;
    st_t1.bak = 0;
    us0_frash = 0;
    us1_frash = 0;
    us0_meas[0] = 0;
    us0_meas[1] = 0;
    us0_meas[2] = 0;
    us0_meas[3] = 0;
    us1_meas[0] = 0;
    us1_meas[1] = 0;
    us1_meas[2] = 0;
    us1_meas[3] = 0;
    time_ms = 0;
    time_qua = 0;
    time_sec = 0;
    us0_trig = 0;
    us1_trig = 0;
    EA = 1;
}

//---------------------------------------------------------------------------
// Trigger one measurement of untrasonic 0
//---------------------------------------------------------------------------
void trig_us0 (void)
{
    us0_trig = 1;
    DelayUs(20);        // more than 10us
    us0_trig = 0;
}

//---------------------------------------------------------------------------
// Trigger one measurement of untrasonic 1
//---------------------------------------------------------------------------
void trig_us1 (void)
{
    us1_trig = 1;
    DelayUs(20);        // more than 10us
    us1_trig = 0;
}

//---------------------------------------------------------------------------
// FSM state: FSM_US01 handler
//---------------------------------------------------------------------------
void fsm_us01_proc(void)
{
    // set tmr
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_CAP  );
    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_CAP  );
    // trigger
    trig_us0();
    trig_us1();
    // wait finish
    DelayMs(50);
    // stop tmr
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP );
    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_STOP );
    // store value
    us0_meas[3] = us0_meas[2];
    us0_meas[2] = us0_meas[1];
    us0_meas[1] = us0_meas[0];
    us0_meas[0] = ((TH0<<8) + TL0);
    us0_frash   = 1;
    us1_meas[3] = us1_meas[2];
    us1_meas[2] = us1_meas[1];
    us1_meas[1] = us1_meas[0];
    us1_meas[0] = ((TH1<<8) + TL1);
    us1_frash   = 1;
    // jump to next state
    fsm = FSM_TICK;
}

//---------------------------------------------------------------------------
// FSM: FSM_TICK state handler, process each 1/4 second
//---------------------------------------------------------------------------
void fsm_tick_proc (void)
{
    u16 i, j;
    u16 len;
    time_ms = 0;
    // 250ms timer
    mcu_set_tmr (TMR_IDX_0, TMR_MOD_10MS);
    while (1)
    {
        i = inc_check (&st_t0);
        time_ms = time_ms + i;
        if (time_ms > 19)       // each 250ms
        {
            mcu_set_tmr (TMR_IDX_0, TMR_MOD_STOP);
            time_qua = time_qua + 1;
            break;
        }
    }
    // frash flag
    us0_frash = 0;
    us1_frash = 0;
    // refrash display each second
    if (time_qua == 4)
    {
        time_qua = 0;
        time_sec = time_sec + 1;
        // display time
        i = time_sec/10000 + '0';
        j = time_sec%10000;
        LcdDispChar (10, 1, 's');
        LcdDispChar (11, 1, 'e');
        LcdDispChar (12, 1, 'c');
        LcdDispChar (13, 1, ':');
        LcdDispChar (14, 1, i);
        i = j/1000 + '0';
        j = j%1000;
        LcdDispChar (15, 1, i);
        i = j/100 + '0';
        j = j%100;
        LcdDispChar (16, 1, i);
        i = j/10 + '0';
        j = j%10 + '0';
        LcdDispChar (17, 1, i);
        LcdDispChar (18, 1, j);
        // display us0
//        len = (us0_meas[0] + us0_meas[1] + us0_meas[2] + us0_meas[3])/2;    // time in us
//        len = us0_meas[0]*2;    // time in us
//        len = len/2;        // single trace
//        len = len/1000;     // time in ms
//        len = len * 340;    // length in mm
        len = us0_meas[0];
        len = len * 0.34;
        i = len/10000 + '0';
        j = len%10000;
        LcdDispChar (0, 0, i);
        i = j/1000 + '0';
        j = j%1000;
        LcdDispChar (1, 0, i);
        i = j/100 + '0';
        j = j%100;
        LcdDispChar (2, 0, i);
        i = j/10 + '0';
        j = j%10 + '0';
        LcdDispChar (3, 0, i);
        LcdDispChar (4, 0, j);
        LcdDispChar (5, 0, 'm');
        LcdDispChar (6, 0, 'm');
        // display us1
//        len = (us1_meas[0] + us1_meas[1] + us1_meas[2] + us1_meas[3])/2;    // time in us

        len = us1_meas[0]*2;    // time in us
        len = len/2;        // single trace
//        len = len/1000;     // time in ms
//        len = len * 340;    // length in mm
        len = len * 0.34;    // length in mm
//        len = us1_meas[0];
        i = len/10000 + '0';
        j = len%10000;
        LcdDispChar (0, 1, i);
        i = j/1000 + '0';
        j = j%1000;
        LcdDispChar (1, 1, i);
        i = j/100 + '0';
        j = j%100;
        LcdDispChar (2, 1, i);
        i = j/10 + '0';
        j = j%10 + '0';
        LcdDispChar (3, 1, i);
        LcdDispChar (4, 1, j);
        LcdDispChar (5, 1, 'm');
        LcdDispChar (6, 1, 'm');
    }
    fsm = FSM_TUS0;
}

//---------------------------------------------------------------------------
// Display Error Message
//---------------------------------------------------------------------------
void fsm_erro_proc(void)
{
    u8 i;
    while (1)
    {
        i = inc_check (&st_t0);
        time_ms = time_ms + i*10;
        if (time_ms > 1000)
        {
            time_ms = 0;
            mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
            time_qua = 4;
            break;
        }
    }
    // 1. clear screen
    for (i=0; i<16; i++)
        LcdDispChar (i, 0, ' ');
    for (i=0; i<16; i++)
        LcdDispChar (i, 1, ' ');
    // 2. display "FSM ERROR!"
    LcdDispString (0, 0, "FSM ERROR!!!");
    LcdDispChar (0, 1, 'f');
    LcdDispChar (1, 1, 's');
    LcdDispChar (2, 1, 'm');
    LcdDispChar (3, 1, ':');
    LcdDispChar (4, 1, ('0'+fsm/100));
    LcdDispChar (5, 1, ('0'+(fsm%100)/10));
    LcdDispChar (6, 1, ('0'+fsm%10));
    LcdDispChar (7, 1, 'd');
    // 3. stay here
    fsm = fsm;
}

