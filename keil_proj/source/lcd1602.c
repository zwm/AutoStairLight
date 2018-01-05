#include "lcd1602.h"

//**************************************************
//* Internal Func: busy detect
//**************************************************
static void LcdWait (void)
{
    bit ebk;
    ebk = EA;
    EA = 0;

    // start read
    PIN_1602_RS     = 0;
    PIN_1602_RW     = 1;
    PIN_1602_EN     = 1;
    DelayUs (0);

    while (1)
    {
//        PIN_1602_DATA   = 0xFF;
#ifdef SIMULATE_ONLY
        PIN_1602_DATA   = 0x7F;
#else
        PIN_1602_DATA   = 0xFF;
#endif
        PIN_1602_EN     = 0;
        DelayUs (0);
        PIN_1602_EN     = 1;
        DelayUs (0);
        // busy_flag = (bit)(PIN_1602_DATA & 0x80) ;
        if (!(PIN_1602_DATA & 0x80))
        {
            break;
        }
		DelayUs (10); 
    }  

    PIN_1602_EN     = 0;
    EA = ebk;

}

//**************************************************
//* Internal Func: write command
//**************************************************
void LcdWriteCmd (unsigned char cmd)
{
    bit ebk;

    // check
    LcdWait();

    ebk = EA;
    EA = 0;

    // write cmd
    PIN_1602_RS     = 0;
    PIN_1602_RW     = 0;
    PIN_1602_EN     = 0;
    PIN_1602_DATA   = cmd;

    // pulse
    PIN_1602_EN     = 1;
    DelayUs (0);
    PIN_1602_EN     = 0;

    EA = ebk;

    DelayUs (10);
}

//**************************************************
//* Internal Func: write data
//**************************************************
void LcdWriteData (unsigned char dt)
{
    bit ebk;

    // check
    LcdWait();

    ebk = EA;
    EA = 0;

    // write cmd
    PIN_1602_RS     = 1;
    PIN_1602_RW     = 0;
    PIN_1602_EN     = 0;
    PIN_1602_DATA   = dt;
    DelayUs (0);

    // pulse
    PIN_1602_EN     = 1;
    DelayUs (0);
    PIN_1602_EN     = 0;

    EA = ebk;

    DelayUs (10);
}

//**************************************************
//* Internal Func: read status
//**************************************************
unsigned char LcdReadStatus (void)
{
    unsigned char ret;
    bit ebk;
    LcdWait ();

    ebk = EA;
    EA = 0;

    // start read
    PIN_1602_RS     = 0;
    PIN_1602_RW     = 1;
    PIN_1602_EN     = 1;
    PIN_1602_DATA   = 0xFF;
    DelayUs (0);
    ret = PIN_1602_DATA;

    PIN_1602_EN     = 0;
    PIN_1602_RW     = 0;

    EA = ebk;

	return ret;
}

//**************************************************
//* Internal Func: read data
//**************************************************
unsigned char LcdReadData (void)
{
    unsigned char ret;
    bit ebk;
    LcdWait ();

    ebk = EA;
    EA = 0;

    // start read
    PIN_1602_RS     = 1;
    PIN_1602_RW     = 1;
    PIN_1602_EN     = 1;
    PIN_1602_DATA   = 0xFF;
    _nop_();    _nop_(); 
    ret = PIN_1602_DATA;

    PIN_1602_EN     = 0;
    PIN_1602_RW     = 0;

    EA = ebk;

	return ret;
}

//**************************************************
//* Internal Func: AC inc&dec
//**************************************************
void LcdAcInc (void)
{
    LcdWriteCmd (LCD1602_CMD_MM_14);
    DelayUs (50);
}

void LcdAcDec (void)
{
    LcdWriteCmd (LCD1602_CMD_MM_10);
    DelayUs (50);
}

//**************************************************
//* Func: AC inc&dec
//**************************************************
void LcdWriteOneCGRAM (unsigned char addr, char *pval)
{
    unsigned char i;
    LcdWriteCmd (0x40 + (addr << 3));
    for (i = 0; i < 8; i++)
        LcdWriteData (pval[i]);
}

//**************************************************
//* Func: set position
//**************************************************
void LcdSetPos (unsigned char x, unsigned char y)
{
    // 1. format parameter
    x = x & 0x3F;
    y = y & 0x01;

    // 2. cal ddram addr
    if (y) x |= 0x40;

    // 3. write cmd
    LcdWriteCmd (x|0x80);
    DelayUs (50);
}

//**************************************************
//* Func: display char
//**************************************************
void LcdDispChar (unsigned char x, unsigned char y, char c)
{
    LcdSetPos (x, y);
    LcdWriteData (c);
}

//**************************************************
//* Func: display string
//**************************************************
void LcdDispString (unsigned char x, unsigned char y, char *ps)
{
    LcdSetPos (x, y);
    while (*ps != '\0')
	{
        LcdWriteData (*ps);
		ps++;
    }
}

//**************************************************
//* Func: display BCD
//**************************************************
void LcdDispBCD (unsigned char x, unsigned char y, unsigned char bcd, unsigned char mode)
{
    // higher byte
    LcdSetPos (x, y);
    if ((bcd & 0xF0) == 0x00)
    {
        if ((mode & 0xF0) == 0)
            LcdWriteData ('0');
        else
            LcdWriteData (' ');
    }
    else
    {
        LcdWriteData (((bcd >> 4) & 0x0F) + '0');
    }

    // lower byte
    LcdSetPos ((x + 1), y);
    if ((bcd & 0x0F) == 0x00)
    {
        if ((mode & 0x0F) == 0)
            LcdWriteData ('0');
        else
            LcdWriteData (' ');
    }
    else
    {
        LcdWriteData (((bcd) & 0x0F) + '0');
    }
}

//******************************************************
//* LCD Init
//******************************************************
void LcdInit (void)
{
    // 1. Bus Mode 38: 8b, 2 lines
    DelayMs (50);
    LcdWriteCmd (LCD1602_CMD_BM_38);        // 39us
    DelayUs (100);

    // 5. Display Mode 0C: open screen, no cursor, no splash
    LcdWriteCmd (LCD1602_CMD_DM_0C);
    DelayUs (100);

    // 2. Display Mode 08: close screen
    //LcdWriteCmd (LCD1602_CMD_DM_08);      // 39us
    //DelayUs (100);

    // 3. Clear Screen 01
    LcdWriteCmd (LCD1602_CMD_CLS);          // 1.53ms
    DelayMs (10);

    // 4. Enter Mode 06: addr+1, no scroll
    LcdWriteCmd (LCD1602_CMD_EM_06);        // 39us
    DelayUs (100);

    // 5. Display Mode 0C: open screen, no cursor, no splash
    //LcdWriteCmd (LCD1602_CMD_DM_0C);
    //DelayUs (250);
}

//**************************************************

