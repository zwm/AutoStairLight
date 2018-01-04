//---------------------------------------------------------------------------
// Global Variable Define
//---------------------------------------------------------------------------
uint16  time_ms;
uint16  time_qua;
uint16  time_sec;
uchar   fsm;
uchar   us0_frash;
uchar   us1_frash;
uint16  us0_meas[4];
uint16  us1_meas[4];
STRUCT_INT_CNT data st_x0;
STRUCT_INT_CNT data st_x1;
STRUCT_INT_CNT data st_t0;
STRUCT_INT_CNT data st_t1;
STRUCT_INT_CNT data *st_p;

//---------------------------------------------------------------------------
// Global Variable Define
//---------------------------------------------------------------------------
unsigned char inc_check (STRUCT_INT_CNT *p)
{
    unsigned char cur;
    cur = p->cnt;
    if (cur > p->bak)       // normal increase
    {
        p->bak = cur;
        cur = cur - p->bak;
    }
    else if (cur < p->bak)  // overflow increase
    {
        p->bak = cur;
        cur = 256 - p->bak + cur;
    }
    else                    // no increase
    {
        cur = 0;
    }
    return cur;
}

void sys_init (void)
{
    // device init
    LcdInit ();
    mcu_init ();
    // parameter init
    st_x0 = {0, 0};
    st_x1 = {0, 0};
    st_t0 = {0, 0};
    st_t1 = {0, 0};
    us0_frash = 0;
    us1_frash = 0;
    us0_meas[] = {0, 0, 0, 0};
    us1_meas[] = {0, 0, 0, 0};
    time_ms = 0;
    time_qua = 0;
    time_sec = 0;
    us0_trig = 0;
    us1_trig = 0;
    EA = 1;
}

void fsm_tus0_proc(void)
{
    unsigned char i, j;
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
void fsm_tus1_proc(void)
{
    unsigned char i, j;
    // set 
    mcu_set_exint (INT_IDX_0, INT_MOD_STOP );
    mcu_set_exint (INT_IDX_1, INT_MOD_START);
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_50MS );
    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_CAP  );
    // start us0
    us1_trig = 1;
    DelayUs(100);
    us1_trig = 0;
    // detect
    while (1)
    {
        // check response
        i = inc_check (&st_x1);
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
                    us1_meas[j+1] = us1_meas[j];
                }
                us1_meas[0] = (TH1<<8) + TL1;
                us1_frash = 1;
                // record time
                time_ms = time_ms + (((TH0<<8) + TL0)*2)/1000;
                // jump to next state
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
}
void fsm_wait_proc(void)
{
    uchar i, j;
    uint16 len;
    // 250ms timer
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_10MS);
    while (1)
    {
        i = inc_check (&st_t0);
        time_ms = time_ms + i*10;
        if (time_ms > 250)
        {
            mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
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
        LcdDispChar (1, 10, i);
        i = j/1000 + '0';
        j = j%1000;
        LcdDispChar (1, 11, i);
        i = j/100 + '0';
        j = j%100;
        LcdDispChar (1, 12, i);
        i = j/10 + '0';
        j = j%10;
        LcdDispChar (1, 13, i);
        LcdDispChar (1, 14, j);
        // display us0
        len = (us0_meas[0] + us0_meas[1] + us0_meas[2] + us0_meas[3])/2;    // time in us
        len = len/2;        // single trace
        len = len/1000;     // time in ms
        len = len * 340;    // length in mm
        i = len/10000 + '0';
        j = len%10000;
        LcdDispChar (0, 0, i);
        i = j/1000 + '0';
        j = j%1000;
        LcdDispChar (0, 1, i);
        i = j/100 + '0';
        j = j%100;
        LcdDispChar (0, 2, i);
        i = j/10 + '0';
        j = j%10;
        LcdDispChar (0, 3, i);
        LcdDispChar (0, 4, j);
        // display us1
        len = (us1_meas[0] + us1_meas[1] + us1_meas[2] + us1_meas[3])/2;    // time in us
        len = len/2;        // single trace
        len = len/1000;     // time in ms
        len = len * 340;    // length in mm
        i = len/10000 + '0';
        j = len%10000;
        LcdDispChar (1, 0, i);
        i = j/1000 + '0';
        j = j%1000;
        LcdDispChar (1, 1, i);
        i = j/100 + '0';
        j = j%100;
        LcdDispChar (1, 2, i);
        i = j/10 + '0';
        j = j%10;
        LcdDispChar (1, 3, i);
        LcdDispChar (1, 4, j);
    }
    fsm = FSM_TUS0;
}

// fall in error state
void fsm_erro_proc(void)
{
    unsigned char i;
    // 1. clear screen
    for (i=0; i<16; i++)
        LcdDispChar (0, i, " ");
    for (i=0; i<16; i++)
        LcdDispChar (1, i, " ");
    // 2. display "FSM ERROR!"
    LcdDispString (0, 0, "FSM ERROR!!!");
    LcdDispChar (1, 0, 'f');
    LcdDispChar (1, 1, 's');
    LcdDispChar (1, 2, 'm');
    LcdDispChar (1, 3, ':');
    LcdDispChar (1, 4, ('0'+fsm/100));
    LcdDispChar (1, 5, ('0'+(fsm%100)/10));
    LcdDispChar (1, 6, ('0'+fsm%10));
    LcdDispChar (1, 7, 'd');
    // 3. stay here
    fsm = fsm;
}

