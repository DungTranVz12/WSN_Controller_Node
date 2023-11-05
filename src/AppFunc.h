#pragma once
#include "define.h"
#include "AppPriFunc.h"








void controllerInit(void){
  Serial.begin(115200);
  Serial.println("Controller is initializing...");

  // 1. Initialize IO
  pinMode(O_RELAY_1_LEVEL, OUTPUT);
  pinMode(O_RELAY_2_LEVEL, OUTPUT);
  pinMode(O_RELAY_3_LEVEL, OUTPUT);
  pinMode(O_RELAY_4_LEVEL, OUTPUT);
  pinMode(O_RELAY_1_PWM, OUTPUT);
  pinMode(O_RELAY_2_PWM, OUTPUT);
  pinMode(O_RELAY_3_PWM, OUTPUT);
  pinMode(O_RELAY_4_PWM, OUTPUT);
  pinMode(O_BUZZER, OUTPUT);
  pinMode(O_LED_PWR, OUTPUT);
  pinMode(O_LED_4G, OUTPUT);
  pinMode(O_LED_RF, OUTPUT);
  pinMode(O_LED_WIFI, OUTPUT);
  pinMode(O_LED_BAT, OUTPUT);
  pinMode(O_LED_SLA, OUTPUT);
  pinMode(I_SW_CH1, INPUT);
  pinMode(I_SW_CH2, INPUT);
  pinMode(I_SW_CH3, INPUT);
  pinMode(I_SW_CH4, INPUT);
  pinMode(BUTTON_BACK, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_ENTER, INPUT);

  // 2. Load config from EEPROM

  // 3. Return last state to GPIO


  //4. Initialize RTC
  rtcInit(); //Initialize RTC
  rtcPrintDatetime(); //Print current datetime




  //5. Initialize EEPROM
  //
}