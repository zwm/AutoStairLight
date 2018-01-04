#include "main.h"

void main (void)
{
    // init
    DelayMs (100);
    sys_init ();

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

