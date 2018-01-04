#ifndef __LCD1602_H__
#define __LCD1602_H__

//******************************************************
//* headers
//******************************************************
#include <intrins.h>
#include "pin_conf.h"
#include "delay.h"
//******************************************************

//******************************************************
//* pin definitions
//******************************************************
#define PIN_1602_RS                     sbit_pin_1602_rs
#define PIN_1602_RW                     sbit_pin_1602_rw
#define PIN_1602_EN                     sbit_pin_1602_en
#define PIN_1602_DATA                   def_pin_1602_data
//******************************************************
// rs : 0, command; 1, data
// rw : 0, write; 1, read
//******************************************************

//******************************************************
//* command definitions
//******************************************************
//------------------------------------------------------
// Class 1: Clear Screen
//------------------------------------------------------
#define LCD1602_CMD_CLS                 0x01
//------------------------------------------------------
// Class 2: Reset Cursor
//------------------------------------------------------
#define LCD1602_CMD_RSTCUR              0x02
//------------------------------------------------------
// Class 3: Enter Mode
//      04H : after write ddram, address-1, scroll none
//      05H : after write ddram, address-1, scroll right
//      06H : after write ddram, address+1, scroll none
//      07H : after write ddram, address+1, scroll left
//------------------------------------------------------
#define LCD1602_CMD_EM_04               0x04
#define LCD1602_CMD_EM_05               0x05
#define LCD1602_CMD_EM_06               0x06
#define LCD1602_CMD_EM_07               0x07
//------------------------------------------------------
// Class 4: Display Mode
//      08H : close screen
//      09H : close screen
//      0AH : close screen
//      0BH : close screen
//      0CH : open screen, close cursor, close splash
//      0DH : open screen, close cursor, open splash
//      0EH : open screen, open cursor, close splash
//      0FH : open screen, open cursor, open splash
//------------------------------------------------------
#define LCD1602_CMD_DM_08               0x08
#define LCD1602_CMD_DM_09               0x09
#define LCD1602_CMD_DM_0A               0x0A
#define LCD1602_CMD_DM_0B               0x0B
#define LCD1602_CMD_DM_0C               0x0C
#define LCD1602_CMD_DM_0D               0x0D
#define LCD1602_CMD_DM_0E               0x0E
#define LCD1602_CMD_DM_0F               0x0F
//------------------------------------------------------
// Class 5: Move Mode
//      10H : AC-1, Cursor-1, no scroll
//      14H : AC+1, Cursor+1, no scroll
//      18H : scroll left
//      1CH : scroll right
//------------------------------------------------------
#define LCD1602_CMD_MM_10               0x10
#define LCD1602_CMD_MM_14               0x14
#define LCD1602_CMD_MM_18               0x18
#define LCD1602_CMD_MM_1C               0x1C
//------------------------------------------------------
// Class 6: Bus Mode
//      20H : bus 4 bits, single line, 5x7
//      24H : bus 4 bits, single line, 5x10
//      28H : bus 4 bits, double line, 5x7
//      2CH : bus 4 bits, double line, 5x10
//      30H : bus 8 bits, single line, 5x7
//      34H : bus 8 bits, single line, 5x10
//      38H : bus 8 bits, double line, 5x7
//------------------------------------------------------
#define LCD1602_CMD_BM_20               0x20
#define LCD1602_CMD_BM_24               0x24
#define LCD1602_CMD_BM_28               0x28
#define LCD1602_CMD_BM_2C               0x2C
#define LCD1602_CMD_BM_30               0x30
#define LCD1602_CMD_BM_34               0x34
#define LCD1602_CMD_BM_38               0x38
//------------------------------------------------------
// Class 7: CGRAM Addr
//      01xx_xxxx: set 6 bits of CGRAM address
//------------------------------------------------------
#define LCD1602_CMD_ADDR_CGRAM          0x40
//------------------------------------------------------
// Class 8: DDRAM Addr
//      1xxx_xxxx: set 7 bits of CGRAM address
//------------------------------------------------------
#define LCD1602_CMD_ADDR_DDRAM          0x80
//******************************************************
//******************************************************
//******************************************************

//***************************************************************
//* func declearations
//***************************************************************
static void LcdWait (void);
void LcdWriteCmd (unsigned char cmd);
void LcdWriteData (unsigned char dt);
unsigned char LcdReadStatus (void);
unsigned char LcdReadData (void);
void LcdAcInc (void);
void LcdAcDec (void);
void LcdWriteOneCGRAM (unsigned char addr, char *pval);

void LcdSetPos (unsigned char x, unsigned char y);
void LcdDispChar (unsigned char x, unsigned char y, char c);
void LcdDispString (unsigned char x, unsigned char y, char *ps);
void LcdDispBCD (unsigned char x, unsigned char y, unsigned char bcd, unsigned char mode);
void LcdInit (void);
//***************************************************************

#endif
