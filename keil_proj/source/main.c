#include "main.h"

void main (void)
{
    // init
    DelayMs (200);
    sys_init ();
    LCD_BKL = BKL_ON;

    // main
    while (1)
    {
        switch (fsm)
        {
            case FSM_US01:
                fsm_us01_proc();
                break;
            case FSM_TICK:
                fsm_tick_proc();
                break;
            default:
                fsm_erro_proc();
                break;
        }
    }
}

