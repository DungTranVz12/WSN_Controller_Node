#include "define.h"
#include <EEPROM.h>
#include "AppPriFunc.h"
#include <SHT2x.h>  
SHT2x sht;

void SHT21_Read(){
  sht.read();
  boxTemp = sht.getTemperature();
  boxHumi = sht.getHumidity();
}

//004_***********************ADC reading**************************
float V_DC_IN, V_SS_SYS, V_Door;
float Vout1, Vout2, Vout3, Vout4, Iout1, Iout2, Iout3, Iout4;
void ADC_Read(){
  V_DC_IN  = analogRead(I_Vmon_DC_IN)*(5.0/1023.0)*(47.0+10.0)/10.0;
  V_SS_SYS = analogRead(I_Vmon_SS_SYS)*(5.0/1023.0)*2/20/0.1*1000;
  V_Door   = analogRead(I_Vmon_DOOR)*(5.0/1023.0)*2;

  Vout1 = analogRead(I_Vmon_CH1);
  Vout2 = analogRead(I_Vmon_CH2);
  Vout3 = analogRead(I_Vmon_CH3);
  Vout4 = analogRead(I_Vmon_CH4);
  Iout1 = analogRead(I_Imon_CH1); 
  Iout2 = analogRead(I_Imon_CH2); 
  Iout3 = analogRead(I_Imon_CH3); 
  Iout4 = analogRead(I_Imon_CH4); 

  Serial.print("V_DC_IN : ");Serial.print(V_DC_IN,1);Serial.println("V");
  Serial.print("V_SS_SYS: ");Serial.print(V_SS_SYS,1);Serial.println("mA");
  Serial.print("V_Door  : ");Serial.print(V_Door,1);Serial.println("V");
  Serial.print("V_CH1   : ");Serial.print(Vout1);Serial.println(" Step");
  Serial.print("V_CH2   : ");Serial.print(Vout2);Serial.println(" Step");
  Serial.print("V_CH3   : ");Serial.print(Vout3);Serial.println(" Step");
  Serial.print("V_CH4   : ");Serial.print(Vout4);Serial.println(" Step");
  Serial.print("I_CH1   : ");Serial.print(Iout1);Serial.println(" Step");
  Serial.print("I_CH2   : ");Serial.print(Iout2);Serial.println(" Step");
  Serial.print("I_CH3   : ");Serial.print(Iout3);Serial.println(" Step");
  Serial.print("I_CH4   : ");Serial.print(Iout4);Serial.println(" Step");
  Serial.println("-----------------------------");
}

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
  // operStatusCh1 = EEPROM.read();

  // 3. Return last state to GPIO


  //4. Initialize RTC
  rtcInit(); //Initialize RTC
  // rtcPrintDatetime(); //Print current datetime
  // rtcSetDatetime(2023, 11, 9, 0, 0, 0); //Set datetime

  //5. LCD init

  //6. SHT21
  sht.begin();




  //5. Initialize EEPROM
  //
}