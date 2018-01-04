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
sbit sbit_pin_1602_rs               =   P2^3;
sbit sbit_pin_1602_rw               =   P2^2;
sbit sbit_pin_1602_en               =   P2^1;
//******************************************************

//******************************************************
//* DS12C887
//******************************************************
#define def_pin_rtc_data                P1
//******************************************************
sbit sbit_pin_rtc_as                =   P2^4;
sbit sbit_pin_rtc_ds                =   P2^7;
sbit sbit_pin_rtc_rw                =   P2^5;
sbit sbit_pin_rtc_irq               =   P3^2;
//******************************************************

//******************************************************
//* sht1x pins
//******************************************************
sbit sbit_pin_sht1x_data            =   P3^1;
sbit sbit_pin_sht1x_clk             =   P3^0;
sbit us0_trig                       =   P3^6;
sbit us1_trig                       =   P3^7;
//******************************************************

#endif
