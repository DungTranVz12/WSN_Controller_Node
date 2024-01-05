#pragma once
#include <ArduinoQueue.h>
#define QUEUE_SIZE_ITEMS 5
ArduinoQueue<String> rxQueue(QUEUE_SIZE_ITEMS);
// ArduinoQueue<String> txQueue(QUEUE_SIZE_ITEMS);
char UID[6]; //Unique ID of the controller
String UIDStr = ""; //Unique ID of the controller
String nodeType = "03" ; //Node type: 03: Controller
String fwVer = "V0.1"; //Firmware version
bool joinRequestSent = false; //Flag to check if join request has been sent
unsigned long lastJoinRequestTime = millis() - 60000; //Last time join request was sent
bool setupDoneFlag = false; //Flag to check if setup process is done
bool receivedA1Flag = false; //Reset receivedA1Flag
bool joinReqDoneFlag = false; //Join request done flag
bool gotB0RequestFlag = false; //Flag to check if got B0 request
#define WAIT_TIME_PACKET_SEC        30 //Wait time for packet in seconds
#define SEND_STATUS_PERIOD_MS   120000 //Send status period 120s = 2min
#define SYNC_SCHEDULE_PERIOD_MS 900000 //Sync schedule period 900s = 15min
String rfRxData = ""; // Data received from serial port


// #define WAIT_TIME_PACKET_SEC         5 //Wait time for packet in seconds
// #define SEND_STATUS_PERIOD_MS    10000 //Send status period 120s = 2min
// #define SYNC_SCHEDULE_PERIOD_MS  20000 //Sync schedule period 900s = 15min

 
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
#define O_LED_4G         30 //Control 4G LED of the box inside cabinet.
#define O_LED_RF         28 //Control RF LED of the box inside cabinet.
#define O_LED_WIFI       26 //Control WIFI LED of the box inside cabinet.
#define O_LED_BAT        32 //Control battery LED of the box inside cabinet.
#define O_LED_SLA        34 //Control SLA LED of the box inside cabinet.
#define O_LED_PWR        39 //Control power LED of the box inside cabinet.
//Input//
#define I_SW_CH1         65 //Monitor CH1 switch of the lid outside cabinet.
#define I_SW_CH2         66 //Monitor CH2 switch of the lid outside cabinet.
#define I_SW_CH3         67 //Monitor CH3 switch of the lid outside cabinet.
#define I_SW_CH4         68 //Monitor CH4 switch of the lid outside cabinet.
#define I_RL_CH1         37 //Monitor Relay of CH1.
#define I_RL_CH2         41 //Monitor Relay of CH2.
#define I_RL_CH3         40 //Monitor Relay of CH3.
#define I_RL_CH4         22 //Monitor Relay of CH4.
   
#define I_Vmon_CH1       A2 //Monitor CH1's output voltage.
#define I_Vmon_CH2       A0 //Monitor CH2's output voltage.
#define I_Vmon_CH3       A6 //Monitor CH3's output voltage.
#define I_Vmon_CH4       A4 //Monitor CH4's output voltage.
#define I_Imon_CH1       A3 //Monitor CH1's output current.
#define I_Imon_CH2       A1 //Monitor CH2's output current.
#define I_Imon_CH3       A7 //Monitor CH3's output current.
#define I_Imon_CH4       A5 //Monitor CH4's output current.
   
#define I_Vmon_DC_IN     A8 //Monitor DC input voltage.
#define I_Vmon_SS_SYS    A9 //Monitor voltage of the sensor which measures the current of whole system.
#define I_Vmon_DOOR     A15 //Monitor voltage status of the door.
#define I_Slave_RDY      11 //Monitor Slave RDY pin.
   
#define BUTTON_BACK      33 //Button BACK of the box inside cabinet.
#define BUTTON_DOWN      31 //Button DOWN of the box inside cabinet.
#define BUTTON_UP        29 //Button UP of the box inside cabinet.
#define BUTTON_SELECT    27 //Button ENTER of the box inside cabinet.

#define DOOR_INSTALLED   0 //0: Door sensor is not installed, 1: Door sensor is installed.
#define MANUAL_MODE      0
#define AUTO_MODE        1
#define CONTROL_ON       1
#define CONTROL_OFF      0
#define ON_STATUS        1
#define OFF_STATUS       0
#define UNKNOWN        255
#define MANUAL_OFF       0
#define MANUAL_ON        1
#define AUTO_OFF         2
#define AUTO_ON          3

//***************************************
#define TFT_DC          49
#define TFT_CS          53
#define TFT_RST         48
#define TFT_MISO        50
#define TFT_MOSI        51
#define TFT_CLK         52
#define TFL_BACKLIGHT   35
#define LCD_ON           1
#define LCD_OFF          0
#define FORCED_UPDATE    1
#define NORMAL_UPDATE    0
//***************************************
//EEPROM address
// #define ADDR_CH1_OPER_STS      0
// #define ADDR_CH1_OPER_MODE     0
// #define ADDR_CH1_SCHDLE_TIME[] 0
// #define ADDR_CH2_OPER_STS 1
// #define ADDR_CH3_OPER_STS 2
// #define ADDR_CH4_OPER_STS 3

#define LED_DEBUG1  digitalWrite(O_LED_4G  , LOW);
#define LED_DEBUG2  digitalWrite(O_LED_RF  , LOW);
#define LED_DEBUG3  digitalWrite(O_LED_WIFI, LOW);
#define LED_DEBUG4  digitalWrite(O_LED_BAT , LOW);
#define LED_DEBUG5  digitalWrite(O_LED_SLA , LOW);
#define LED_DEBUG6  digitalWrite(O_LED_PWR , LOW);

#define LED_DEBUG7  digitalWrite(O_LED_4G  , HIGH);
#define LED_DEBUG8  digitalWrite(O_LED_RF  , HIGH);
#define LED_DEBUG9  digitalWrite(O_LED_WIFI, HIGH);
#define LED_DEBUG10 digitalWrite(O_LED_BAT , HIGH);
#define LED_DEBUG11 digitalWrite(O_LED_SLA , HIGH);
#define LED_DEBUG12 digitalWrite(O_LED_PWR , HIGH);

