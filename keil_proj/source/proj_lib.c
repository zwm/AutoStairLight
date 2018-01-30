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
volatile u16  led_sec;
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
    led_sec = 0;
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

void led_light (unsigned char sw)
{
#ifdef JOY_SYS_COMPAT
    if (sw == 0)
    {
        LcdDispChar (11, 0, '0');
    }
    if (sw == 1)
    {
        LcdDispChar (11, 0, '1');
    }
#else
    if (sw == 0)
    {
        stair_led = ~STAIR_LED_ON;
    }
    if (sw == 1)
    {
        stair_led = STAIR_LED_ON;
    }
#endif
}

//---------------------------------------------------------------------------
// FSM state: FSM_US01 handler
//---------------------------------------------------------------------------
void fsm_us01_proc(void)
{
    u16 i;
    bit ea_bak;
    ea_bak = EA;
    EA = 0;
    TR0 = 0;    // stop T0
    ET0 = 0;    // disable T0 overflow interrupt
    TF0 = 0;    // clear T0 overflow interrupt flag
    TMOD = 0;   // 16-bit auto reload
    TL0 = 0;    // reset TL0
    TH0 = 0;    // reset TH0
    us0_echo = 1;
    trig_us0();
    while (1)
    {
        if (us0_echo)
        {
            TR0 = 1;
            break;
        }
        else
        {
            ;
        }
    }
    while (1)
    {
        if (~us0_echo)
        {
            TR0 = 0;
            break;
        }
        else
        {
            ;
        }
    }
    // us0 store
    i = ((TH0<<8) + TL0)*0.34;
    if ((i>20) && (i < 4500))   // range 2cm to 4.5m
    {   // valid measurement
        us0_meas[3] = us0_meas[2];
        us0_meas[2] = us0_meas[1];
        us0_meas[1] = us0_meas[0];
        us0_meas[0] = i;
        us0_frash   = 1;
    }
    DelayUs(100);

    EA = 0;
    TR0 = 0;    // stop T0
    ET0 = 0;    // disable T0 overflow interrupt
    TF0 = 0;    // clear T0 overflow interrupt flag
    TMOD = 0;   // 16-bit auto reload
    TL0 = 0;    // reset TL0
    TH0 = 0;    // reset TH0
    us1_echo = 1;
    trig_us1();
    while (1)
    {
        if (us1_echo)
        {
            TR0 = 1;
            break;
        }
        else
        {
            ;
        }
    }
    while (1)
    {
        if (~us1_echo)
        {
            TR0 = 0;
            break;
        }
        else
        {
            ;
        }
    }
    // us1 store
    i = ((TH0<<8) + TL0)*0.34;
    if ((i>20) && (i < 4500))   // range 2cm to 4.5m
    {   // valid measurement
        us1_meas[3] = us1_meas[2];
        us1_meas[2] = us1_meas[1];
        us1_meas[1] = us1_meas[0];
        us1_meas[0] = i;
        us1_frash   = 1;
    }
    DelayUs(100);
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
    // 250ms timer
    DelayMs(200);
    time_tick = time_tick + 1;
    // display time of second
    if (time_tick%4 == 0)
    {
        time_tick = 0;
        time_sec = time_sec + 1;
        LcdDispInt  (19, 1, time_sec);
        if (led_sec > 0)
        {
            led_sec = led_sec - 1;
        }
    }
    // display distance
    if (us0_frash == 1)
    {
        us0_frash = 0;
        LcdDispInt  (0, 0, us0_meas[0]);
        LcdDispChar (5, 0, 'm');
        LcdDispChar (6, 0, 'm');
        i = (us0_meas[0] + us0_meas[1] + us0_meas[2] + us0_meas[3])/4;
        if (i < US_DIST_THRES)
        {
            led_sec = LED_ON_DELAY;
        }
    }
    if (us1_frash == 1)
    {
        us1_frash = 0;
        LcdDispInt  (0, 1, us1_meas[0]);
        LcdDispChar (5, 1, 'm');
        LcdDispChar (6, 1, 'm');
        i = (us1_meas[0] + us1_meas[1] + us1_meas[2] + us1_meas[3])/4;
        if (i < US_DIST_THRES)
        {
            led_sec = LED_ON_DELAY;
        }
    }
    // led light
    if (led_sec > 0)
    {
        if (PHOTO_SENS_EN)
        {
            if (photo_sens == PHOTO_SENS_ACTIVE)
            {
                led_light(1);
            }
            else
            {
                led_light(0);
            }
        }
        else
        {
            led_light(1);
        }
    }
    else
    {
        led_light(0);
    }
    fsm = FSM_US01;
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

