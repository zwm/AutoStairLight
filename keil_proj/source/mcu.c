//*********************************************************
//  File Name   : mcu.c
//  Created On  : 201801041429
//  Author      : Ming
//  Description : STC15F204EA related function
//*********************************************************
#include	"mcu.h"

//---------------------------------------------------------------------------
// INIT MCU
//---------------------------------------------------------------------------
void mcu_init (void)
{
    // STC15F204EA Configure
    //************************************
    //* T0x12, T1x12
    //************************************
    AUXR        = 0x00;

#ifdef JOY_SYS_COMPAT
    AUXR1       = 0x00;
    AUXR2       = 0x00;
    IAP_CONTR   = 0x00;
    WKTCL       = 0xFF;
    WKTCH       = 0xEF;
#endif

    //************************************
    //* MCLKO_S1, MCLKO_S0, ADRJ, Tx_Rx, MCLKO_2
    //* CLK_S2, CLK_S1, CLK_S0
    //*     CLK do not output;
    //*     Uart work mode: normal
    //*     Main_CLK: div1
    //************************************
    CLK_DIV     = 0x00;
    //************************************
    //* WDT_FLAG, EN_WDT, CLR_WDT, IDLE_WDT
    //* PS2, PS1, PS0
    //*     WDT: disable
    //************************************
    WDT_CONTR   = 0x00;
    //************************************
    //* SMOD, SMOD0, LVDF, POF, GF1, GF0
    //* PD, DIL
    //************************************
    PCON        = 0x00;
    //************************************
    //* P1, P2, P3, I/O mode
    //*     dual-dir
    //************************************
    P0M1        = 0x00;
    P0M0        = 0x00;
    P1M1        = 0x00;
    P1M0        = 0x00;
    P2M1        = 0x00;
    P2M0        = 0x00;
    P3M1        = 0x00;
    P3M0        = 0x00;
    //************************************
    // interrupt
    IE = 0x00;
    TCON = 0x00;
    mcu_set_exint (INT_IDX_0, INT_MOD_STOP);
    mcu_set_exint (INT_IDX_1, INT_MOD_STOP);
    mcu_set_tmr   (TMR_IDX_0, TMR_MOD_STOP);
    mcu_set_tmr   (TMR_IDX_1, TMR_MOD_STOP);
}

//---------------------------------------------------------------------------
// SET Interrupt INT0 and INT1
//---------------------------------------------------------------------------
void mcu_set_exint (unsigned char idx, unsigned char mode)
{
    // disable interrupt
    bit ea_bak;
    ea_bak = EA;
    EA = 0;
    if (idx == INT_IDX_0)
    {
        EX0 = 0;        // disable int0
        IE0 = 0;        // clear int0 flag
        IT0 = 1;        // int0 falling edge trigger int
        if (mode == INT_MOD_START)
        {
            EX0 = 1;        // enable int0
        }
        else if (mode == INT_MOD_STOP)
        {
        }
    }
    else if (idx == INT_IDX_1)
    {
        EX1 = 0;        // disable int1
        IE1 = 0;        // clear int1 flag
        IT1 = 1;        // int1 falling edge trigger int
        if (mode == INT_MOD_START)
        {
            EX1 = 1;        // enable int1
        }
        else if (mode == INT_MOD_STOP)
        {
        }
    }
    EA = ea_bak;
}

//---------------------------------------------------------------------------
// SET T0 and T1
//---------------------------------------------------------------------------
void mcu_set_tmr (unsigned char idx, unsigned char mode)
{
    // init reg
    bit ea_bak;
    ea_bak = EA;
    EA = 0;     // disable all interrupt
    // T0
    if (idx == TMR_IDX_0)
    {
        TR0 = 0;    // stop T0
        ET0 = 0;    // disable T0 overflow interrupt
        TF0 = 0;    // clear T0 overflow interrupt flag
        // conf timer
        if (mode == TMR_MOD_CAP)
        {
            TMOD = (TMOD & 0xF0) + 0x08;    // GATE=1, C/T=0, M1=0, M0=0
            TL0 = 0;    // reset TL0
            TH0 = 0;    // reset TH0
        }
        else if (mode == TMR_MOD_50MS)
        {
            TMOD = (TMOD & 0xF0) + 0x00;    // GATE=0, C/T=0, M1=0, M0=0
            TL0 = TL_INIT_50MS;     // load TL0
            TH0 = TH_INIT_50MS;     // load TH0
        }
        else if (mode == TMR_MOD_10MS)
        {
            TMOD = (TMOD & 0xF0) + 0x00;    // GATE=0, C/T=0, M1=0, M0=0
            TL0 = TL_INIT_10MS;     // load TL0
            TH0 = TH_INIT_10MS;     // load TH0
        }
        else if (mode == TMR_MOD_STOP)
        {
            EA = ea_bak;    // enable all interrupt
            return;
        }
        // set reg
        TF0 = 0;        // clear T0 overflow interrupt flag
        ET0 = 1;        // enable T0 overflow interrupt
        EA = ea_bak;    // enable all interrupt
        TR0 = 1;        // start T0
    }
    // T1
    else if (idx == TMR_IDX_1)
    {
        TR1 = 0;    // stop T1
        ET1 = 0;    // disable T1 overflow interrupt
        TF1 = 0;    // clear T1 overflow interrupt flag
        // conf timer
        if (mode == TMR_MOD_CAP)
        {
            TMOD = (TMOD & 0x0F) + 0x80;    // GATE=1, C/T=0, M1=0, M0=0
            TL1 = 0;                // reset TL1
            TH1 = 0;                // reset TH1
        }
        else if (mode == TMR_MOD_50MS)
        {
            TMOD = (TMOD & 0x0F) + 0x00;    // GATE=0, C/T=0, M1=0, M0=0
            TL1 = TL_INIT_50MS;     // load TL1
            TH1 = TH_INIT_50MS;     // load TH1
        }
        else if (mode == TMR_MOD_10MS)
        {
            TMOD = (TMOD & 0x0F) + 0x00;    // GATE=0, C/T=0, M1=0, M0=0
            TL1 = TL_INIT_10MS;     // load TL1
            TH1 = TH_INIT_10MS;     // load TH1
        }
        else if (mode == TMR_MOD_STOP)
        {
            EA = ea_bak;    // enable all interrupt
            return;
        }
        // set reg
        TF1 = 0;        // clear T1 overflow interrupt flag
        ET1 = 1;        // enable T1 overflow interrupt
        EA = ea_bak;    // enable all interrupt
        TR1 = 1;        // start T1
    }
}

