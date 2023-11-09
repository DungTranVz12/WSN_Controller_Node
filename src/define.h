#pragma once

//*************Define GPIO*************
//Output//
#define O_RELAY_1_PWM     8 //Control CH1 relay by PWM
#define O_RELAY_1_LEVEL   9 //Control CH1 relay by level
#define O_RELAY_2_PWM     6 //Control CH2 relay by PWM
#define O_RELAY_2_LEVEL   7 //Control CH2 relay by level
#define O_RELAY_3_PWM     4 //Control CH3 relay by PWM
#define O_RELAY_3_LEVEL   5 //Control CH3 relay by level
#define O_RELAY_4_PWM     2 //Control CH4 relay by PWM
#define O_RELAY_4_LEVEL   3 //Control CH4 relay by level

#define O_BUZZER         36 //Control buzzer
#define O_LED_PWR        39 //Control power LED of the box inside cabinet.
#define O_LED_4G         30 //Control 4G LED of the box inside cabinet.
#define O_LED_RF         28 //Control RF LED of the box inside cabinet.
#define O_LED_WIFI       26 //Control WIFI LED of the box inside cabinet.
#define O_LED_BAT        32 //Control battery LED of the box inside cabinet.
#define O_LED_SLA        34 //Control SLA LED of the box inside cabinet.
//Input//
#define I_SW_CH1         65 //Monitor CH1 switch of the lid outside cabinet.
#define I_SW_CH2         66 //Monitor CH2 switch of the lid outside cabinet.
#define I_SW_CH3         67 //Monitor CH3 switch of the lid outside cabinet.
#define I_SW_CH4         68 //Monitor CH4 switch of the lid outside cabinet.
   
#define I_Vmon_CH1       A0 //Monitor CH1's output voltage.
#define I_Vmon_CH2       A2 //Monitor CH2's output voltage.
#define I_Vmon_CH3       A4 //Monitor CH3's output voltage.
#define I_Vmon_CH4       A6 //Monitor CH4's output voltage.
#define I_Imon_CH1       A1 //Monitor CH1's output current.
#define I_Imon_CH2       A3 //Monitor CH2's output current.
#define I_Imon_CH3       A5 //Monitor CH3's output current.
#define I_Imon_CH4       A7 //Monitor CH4's output current.
   
#define I_Vmon_DC_IN     A8 //Monitor DC input voltage.
#define I_Vmon_SS_SYS    A9 //Monitor voltage of the sensor which measures the current of whole system.
#define I_Vmon_DOOR     A15 //Monitor voltage status of the door.
   
#define BUTTON_BACK      33 //Button BACK of the box inside cabinet.
#define BUTTON_DOWN      31 //Button DOWN of the box inside cabinet.
#define BUTTON_UP        29 //Button UP of the box inside cabinet.
#define BUTTON_ENTER     27 //Button ENTER of the box inside cabinet.

//***************************************
//EEPROM address
#define ADDR_CH1_OPER_STS      0
#define ADDR_CH1_OPER_MODE     0
#define ADDR_CH1_SCHDLE_TIME[] 0
#define ADDR_CH2_OPER_STS 1
#define ADDR_CH3_OPER_STS 2
#define ADDR_CH4_OPER_STS 3




