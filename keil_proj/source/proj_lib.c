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
#ifdef EN_US0
void fsm_tus0_proc(void)
{
    u8 i, j;
    // set 
    time_ms = 0;
    mcu_set_exint (INT_IDX_0, INT_MOD_START);
    mcu_set_exint (INT_IDX_1, INT_MOD_STOP );
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_CAP  );
    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_50MS );
    // start us0
    us0_trig = 1;
    DelayUs(100);
    us0_trig = 0;
    // detect
    while (1)
    {
        // check response
        i = inc_check (&st_x0);
        if (i>0)
        {
            // stop
            mcu_set_exint (INT_IDX_0, INT_MOD_STOP);
            mcu_set_exint (INT_IDX_1, INT_MOD_STOP);
            mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
            mcu_set_tmr   (TMR_IDX_1, TMR_MOD_STOP);
            if (i==1)   // normal response
            {
                // backup response
                for(j=0; j<3; j++)
                {
                    us0_meas[j+1] = us0_meas[j];
                }
                us0_meas[0] = (TH0<<8) + TL0;
                us0_frash = 1;
                // record time
                time_ms = time_ms + (((TH1<<8) + TL1)*2)/1000;
                // jump to next state
                fsm = FSM_TUS1;
                break;
            }
            else
            {
                // record time
                time_ms = time_ms + (((TH1<<8) + TL0)*2)/1000;
                // jump to error state
                fsm = FSM_ERR_US0_MULTI_RESP;
                break;
            }
        }
        // check timeout
        i = inc_check (&st_t1);
        if (i>0)
        {
            // stop
            mcu_set_exint (INT_IDX_0, INT_MOD_STOP);
            mcu_set_exint (INT_IDX_1, INT_MOD_STOP);
            mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
            mcu_set_tmr   (TMR_IDX_1, TMR_MOD_STOP);
            time_ms = time_ms + 50;
            fsm = FSM_ERR_US0_TIMEOUT;
            break;
        }
    }
}
#else
void fsm_tus0_proc(void)
{
    time_ms = 0;
    fsm = FSM_TUS1;
}
#endif

//---------------------------------------------------------------------------
// Trigger one measurement of untrasonic 1
//---------------------------------------------------------------------------
#ifdef EN_US1
void fsm_tus1_proc(void)
{
    u8 i=0;
    // set 
//    mcu_set_exint (INT_IDX_0, INT_MOD_STOP );
//    mcu_set_exint (INT_IDX_1, INT_MOD_START);
//    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_50MS );
//    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_CAP  );
    mcu_set_exint (INT_IDX_0, INT_MOD_STOP );
    mcu_set_exint (INT_IDX_1, INT_MOD_STOP );
    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_CAP  );
    TR1 = 1;
    // start us1
    us1_trig = 1;
    DelayUs(80);
    us1_trig = 0;
    //
/*    while (1)
    {
//        us1_echo = 1;
//        i = i+1;
        if (us1_echo)
        {
            TR1 = 1;
            break;
        }
    }*/
    // 
    DelayMs(50);
    while (1)
    {
        if (~us1_echo)
        {
            us1_meas[0] = ((TH1<<8) + TL1);
            fsm = FSM_WAIT;
            TR1 = 0;
            break;
        }
        if (TF1)
        {
            fsm = FSM_ERR_US1_TIMEOUT;
            TR1 = 0;
            break;
        }
    }

/*
    // detect
    while (1)
    {
        // check response
        i = inc_check (&st_x1);
        if (i>0)
        {
            // stop
            if (i==1)   // normal response
            {
                // backup response
                for(j=0; j<3; j++)
                {
                    us1_meas[j+1] = us1_meas[j];
                }
                us1_meas[0] = (TH1<<8) + TL1;
                us1_frash = 1;
                // record time
                time_ms = time_ms + (((TH0<<8) + TL0)*2)/1000;
                // jump to next state
                mcu_set_exint (INT_IDX_0, INT_MOD_STOP);
                mcu_set_exint (INT_IDX_1, INT_MOD_STOP);
                mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
                mcu_set_tmr   (TMR_IDX_1, TMR_MOD_STOP);
                fsm = FSM_WAIT;
                break;
            }
            else
            {
                // record time
                time_ms = time_ms + (((TH0<<8) + TL0)*2)/1000;
                // jump to error state
                fsm = FSM_ERR_US1_MULTI_RESP;
                break;
            }
        }
        // check timeout
        i = inc_check (&st_t0);
        if (i>0)
        {
            // stop
            mcu_set_exint (INT_IDX_0, INT_MOD_STOP);
            mcu_set_exint (INT_IDX_1, INT_MOD_STOP);
            mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
            mcu_set_tmr   (TMR_IDX_1, TMR_MOD_STOP);
            time_ms = time_ms + 50;
            fsm = FSM_ERR_US1_TIMEOUT;
            break;
        }
    }
    */
}
#else
void fsm_tus1_proc(void)
{
    time_ms = time_ms;
    fsm = FSM_WAIT;
}
#endif

//---------------------------------------------------------------------------
// Wait for 250ms, and process the result of measurement
//---------------------------------------------------------------------------
void fsm_wait_proc(void)
{
    u8 i, j;
    u16 len;
    // 250ms timer
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_10MS);
    TR0 = 1;
    ET0 = 1;
    while (1)
    {
        i = inc_check (&st_t0);
        time_ms = time_ms + i*10;
//        if (time_ms > 250)
        if (time_ms > 1000)
        {
            time_ms = 0;
            mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
//            time_qua = time_qua + 1;
            time_qua = 4;
            break;
        }
    }
#ifdef JOY_SYS_COMPAT
//    LCD_BKL = ~LCD_BKL;
#endif
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
        len = us0_meas[0]*2;    // time in us
        len = len/2;        // single trace
        len = len/1000;     // time in ms
        len = len * 340;    // length in mm
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

//        len = us1_meas[0]*2;    // time in us
//        len = len/2;        // single trace
//        len = len/1000;     // time in ms
//        len = len * 340;    // length in mm
        len = us1_meas[0];
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

