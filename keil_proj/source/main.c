#include "main.h"

void main (void)
{
    // init
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

    sys_init ();
#ifdef PRINT_DEBUG
    LcdDispChar (0, 10, 'i');
#endif

    // main
    while (1)
    {
        switch (fsm)
        {
            case FSM_TUS0:
                fsm_tus0_proc();
                break;
            case FSM_TUS1:
                fsm_tus1_proc();
                break;
            case FSM_WAIT:
                fsm_wait_proc();
                break;
            default:
                fsm_erro_proc();
                break;
        }
    }
}

