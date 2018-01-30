#include "proj_lib.h"

//---------------------------------------------------------------------------
// Global Variable Define
//---------------------------------------------------------------------------
volatile u16  time_tick;
volatile u16  time_sec;
volatile u8   fsm;
volatile u8   us0_gap;
volatile u8   us1_gap;
volatile u16  us0_err;
volatile u16  us1_err;
volatile bit  stair_idle;
volatile bit  us0_frash;
volatile bit  us1_frash;
volatile u16  us0_meas[4];
volatile u16  us1_meas[4];
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
    time_tick = 0;
    time_sec = 0;
    fsm = FSM_US01;
    us0_gap = 0;
    us1_gap = 0;
    us0_err = 0;
    us1_err = 0;
    stair_idle = 0;
    us0_trig = 0;
    us1_trig = 0;
    EA = 0;
}

//---------------------------------------------------------------------------
// Trigger one measurement of untrasonic 0
//---------------------------------------------------------------------------
void trig_us0 (void)
{
    DelayUs(20);
    us0_trig = 1;
    DelayUs(20);        // more than 10us
    us0_trig = 0;
}

//---------------------------------------------------------------------------
// Trigger one measurement of untrasonic 1
//---------------------------------------------------------------------------
void trig_us1 (void)
{
    DelayUs(20);
    us1_trig = 1;
    DelayUs(20);        // more than 10us
    us1_trig = 0;
}

//---------------------------------------------------------------------------
// FSM state: FSM_US01 handler
//---------------------------------------------------------------------------
void fsm_us01_proc(void)
{
    u8 i;
    bit ea_bak;
    // debug
    LcdDispChar (19, 0, fsm+'0');
    ea_bak = EA;
#ifdef CHECK_MODE_INT
    mcu_set_exint (INT_IDX_0, INT_MOD_STOP );
    mcu_set_exint (INT_IDX_1, INT_MOD_STOP );
    EA = 1;
#else
    EA = 0;
#endif
    if (us0_gap != 0)
    {
        us0_gap = us0_gap - 1;
    }
    if (us0_gap == 0)
    {
        mcu_set_exint (INT_IDX_0, INT_MOD_START);
        mcu_set_tmr (TMR_IDX_0, TMR_MOD_CAP  );
        trig_us0();
    }
    if (us1_gap != 0)
    {
        us1_gap = us1_gap - 1;
    }
    if (us1_gap == 0)
    {
        mcu_set_exint (INT_IDX_1, INT_MOD_START);
        mcu_set_tmr (TMR_IDX_1, TMR_MOD_CAP  );
        trig_us1();
    }
    // wait finish
    DelayMs(50);
    mcu_set_tmr (TMR_IDX_0, TMR_MOD_STOP );
    mcu_set_tmr (TMR_IDX_1, TMR_MOD_STOP );
#ifdef CHECK_MODE_INT
    // store value
    if (us0_gap == 0)   // ultra-sonic 0
    {
        i = inc_check (&st_x0);
        if (i == 1)
        {
            us0_meas[3] = us0_meas[2];
            us0_meas[2] = us0_meas[1];
            us0_meas[1] = us0_meas[0];
            us0_meas[0] = ((TH0<<8) + TL0);
            us0_frash   = 1;
        }
        else
        {
            us0_err = us0_err + i + 10;
            us0_gap = MEAS_ERR_GAP;
        }
    }
    if (us1_gap == 0)   // ultra-sonic 1
    {
        i = inc_check (&st_x1);
        if (i == 1)
        {
            us1_meas[3] = us1_meas[2];
            us1_meas[2] = us1_meas[1];
            us1_meas[1] = us1_meas[0];
            us1_meas[0] = ((TH1<<8) + TL1);
            us1_frash   = 1;
        }
        else
        {
            us1_err = us1_err + i + 10;
            us1_gap = MEAS_ERR_GAP;
        }
    }
#else       // POLL MODE
    // store value
    if (us0_gap == 0)   // ultra-sonic 0
    {
        if (IE0)
        {
            IE0 = 0;
            us0_meas[3] = us0_meas[2];
            us0_meas[2] = us0_meas[1];
            us0_meas[1] = us0_meas[0];
            us0_meas[0] = ((TH0<<8) + TL0);
            us0_frash   = 1;
        }
        else
        {
            us0_err = us0_err + 1;
            us0_gap = MEAS_ERR_GAP;
        }
    }
    if (us1_gap == 0)   // ultra-sonic 1
    {
        if (IE1)
        {
            IE1 = 0;
            us1_meas[3] = us1_meas[2];
            us1_meas[2] = us1_meas[1];
            us1_meas[1] = us1_meas[0];
            us1_meas[0] = ((TH1<<8) + TL1);
            us1_frash   = 1;
        }
        else
        {
            us1_err = us1_err + 1;
            us1_gap = MEAS_ERR_GAP;
        }
    }
#endif
    mcu_set_exint (INT_IDX_0, INT_MOD_STOP );
    mcu_set_exint (INT_IDX_1, INT_MOD_STOP );
    EA = ea_bak;
    // jump to next state
    fsm = FSM_TICK;
}

//---------------------------------------------------------------------------
// FSM: FSM_TICK state handler, process each 1/4 second
// JOY_SYS TEST DISPLAY:
// char_index  |012345678901234567890123|
// label       |len       err_num  time |
// line0       |xxxxx     xxxxx         |
// line1       |xxxxx     xxxxx    xxxxx|
//---------------------------------------------------------------------------
void fsm_tick_proc (void)
{
    u16 i, j;
    u8  k;
    bit ea_bak;
    ea_bak = EA;
    i = 0;
    j = 0;
    k = 0;
    LcdDispChar (19, 0, fsm+'0');
    // 250ms timer
    EA = 1;
    mcu_set_tmr (TMR_IDX_0, TMR_MOD_10MS);
    while (1)
    {
        k = k+1;
        LcdDispChar (22, 0, (k%100)/10+'0');
        LcdDispChar (23, 0, (k%100)%10+'0');

//        LcdDispChar (16, 0, (j%100)/10+'0');
//        LcdDispChar (17, 0, (j%100)%10+'0');

        LcdDispChar (16, 0, (st_t0.cnt%100)/10+'0');
        LcdDispChar (17, 0, (st_t0.cnt%100)%10+'0');

        LcdDispChar (16, 1, (SP/100)+'0');
        LcdDispChar (17, 1, (SP%100)/10+'0');
        LcdDispChar (18, 1, (SP%100)%10+'0');

        if (ET0)
            LcdDispChar (20, 0, '1');
        else
            LcdDispChar (20, 0, '0');
        if (TF0)
            LcdDispChar (21, 0, '1');
        else
            LcdDispChar (21, 0, '0');

/*        if (EA)
            LcdDispChar (7, 0, '1');
        else
            LcdDispChar (7, 0, '0');
        if (TR0)
            LcdDispChar (8, 0, '1');
        else
            LcdDispChar (8, 0, '0');*/

        LcdDispChar (7, 0, (TH0%100)+'0');
        LcdDispChar (8, 0, (TH0%100)/10+'0');
        LcdDispChar (9, 0, (TH0%100)%10+'0');

        LcdDispChar (7, 1, (TL0%100)+'0');
        LcdDispChar (8, 1, (TL0%100)/10+'0');
        LcdDispChar (9, 1, (TL0%100)%10+'0');



        EA = 0;
        i = inc_check (&st_t0);
        EA = 1;
        j = j + i;
        if (j >= (SYS_TICK_CYC/10))
        {
            mcu_set_tmr (TMR_IDX_0, TMR_MOD_STOP);
            break;
        }
    }
    EA = ea_bak;
    time_tick = time_tick + 2;
    // display time of second
    if (time_tick%4 == 0)
    {
        time_sec = time_sec + 1;
        LcdDispInt  (19, 1, time_sec);
    }
    // display distance
    if (us0_frash == 1)
    {
        us0_frash = 0;
        i = us0_meas[0]*0.34;
        LcdDispInt  (0, 0, i);
        LcdDispChar (5, 0, 'm');
        LcdDispChar (6, 0, 'm');
    }
    if (us1_frash == 1)
    {
        us1_frash = 0;
        i = us1_meas[0]*0.34;
        LcdDispInt  (0, 1, i);
        LcdDispChar (5, 1, 'm');
        LcdDispChar (6, 1, 'm');
    }
    j = 0;
    if (us0_gap == 0 && us0_frash == 0)
    {
        j = j + 1;
    }
    if (us1_gap == 0 && us1_frash == 0)
    {
        j = j + 1;
    }
    if (j != 0)
    {
        fsm = FSM_ERRO;
    }
    else 
    {
        fsm = FSM_US01;
    }
}

//---------------------------------------------------------------------------
// FSM: FSM_ERRO handler
//---------------------------------------------------------------------------
void fsm_erro_proc(void)
{
    // debug
    LcdDispChar (19, 0, fsm+'0');

    LcdDispInt  (10, 0, us0_err);
    LcdDispInt  (10, 1, us1_err);
    fsm = FSM_US01;
}

