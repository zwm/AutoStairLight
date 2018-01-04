//*********************************************************
//  File Name   : interrupt.c
//  Created On  : 201801041502
//  Author      : Ming
//  Description : here are interrupt service routines
//*********************************************************
#include	"interrupt_service.h"

// INT0
void ext_int0_servece (void) interrupt 0 using 1
{
    // HW auto clear interrupt flag
    st_x0.cnt++;
}

// INT1
void ext_int1_servece (void) interrupt 2 using 1
{
    // HW auto clear interrupt flag
    st_x1.cnt++;
}

// T0
void t0_service (void) interrupt 1 using 2
{
    // HW auto clear interrupt flag
    // Mode 0: 16-bit auto reload
    st_t0.cnt++;
}

// T1
void t1_service (void) interrupt 3 using 3
{
    // HW auto clear interrupt flag
    // Mode 0: 16-bit auto reload
    st_t1.cnt++;
}

