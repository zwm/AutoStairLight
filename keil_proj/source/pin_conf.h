#ifndef __PIN_CONF_H__
#define __PIN_CONF_H__

//******************************************************
//* headers
//******************************************************
#include <STC15Fxxxx.H>
//******************************************************

//******************************************************
//* LCD 1602
//******************************************************
#define def_pin_1602_data               P1
//******************************************************
sbit sbit_pin_1602_rs               =   P2^5;
sbit sbit_pin_1602_rw               =   P2^4;
sbit sbit_pin_1602_en               =   P2^7;
//******************************************************

//******************************************************
//* DS12C887
//******************************************************
#define def_pin_rtc_data                P1
//******************************************************
sbit sbit_pin_rtc_as                =   P2^1;
sbit sbit_pin_rtc_ds                =   P2^3;
sbit sbit_pin_rtc_rw                =   P2^2;
sbit sbit_pin_rtc_irq               =   P3^6;
//******************************************************

//******************************************************
//* sht1x pins
//******************************************************
sbit sbit_pin_sht1x_data            =   P5^5;
sbit sbit_pin_sht1x_clk             =   P5^4;
//******************************************************

//******************************************************
sbit us0_trig                       =   P3^7;
sbit us1_trig                       =   P3^4;
sbit us0_echo                       =   P3^2;
sbit us1_echo                       =   P3^3;
//******************************************************

sbit LCD_BKL    = P3^5;

#endif
