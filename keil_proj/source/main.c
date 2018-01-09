#include "main.h"

void main (void)
{
    // init
#ifdef JOY_SYS_COMPAT
    LCD_BKL = 0;
    DelayMs (200);
    DelayMs (200);
    DelayMs (200);
    DelayMs (200);
    DelayMs (200);
    LCD_BKL = ~LCD_BKL;
    DelayMs (200);
    DelayMs (200);
    DelayMs (200);
    DelayMs (200);
    DelayMs (200);
    LCD_BKL = ~LCD_BKL;
#endif
    DelayMs (200);

    sys_init ();
#ifdef PRINT_DEBUG
    LcdDispChar (10, 0, 'i');
#endif

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

