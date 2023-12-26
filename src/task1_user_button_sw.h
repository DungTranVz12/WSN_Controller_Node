#ifndef APPFUNC
#include "appfunc.h"
#endif

/**
 * @brief This function performs various tasks related to button, switch, and relay status updates.
 * 
 * It reads the button status and updates the button status based on the door installation status.
 * If the door is installed and the voltage reading is greater than or equal to 1, it scans the button status and turns on the LCD backlight.
 * If the door is not installed, it scans the button status and turns off the LCD backlight.
 * 
 * It also reads the mode switch status and updates the switch status for each channel.
 * 
 * Furthermore, it reads the relay status and updates the relay status for each channel.
 * 
 * @note This function assumes the availability of certain input pins and variables.
 */
void task1 () {
  // Serial.println("=== TASK 1 ===");

  dev.V_Door   = analogRead(I_Vmon_DOOR)*(5.0/1023.0)*2;
  // 1.  Read button status and update button status
  if (DOOR_INSTALLED == 1 && dev.V_Door >= 1) {
    buttonScan();               //Scan button status and update button status
    dev.doorStatus = 1; //Door is opened
  }
  else if (DOOR_INSTALLED == 0){
    buttonScan();              //Scan button status and update button status
    dev.doorStatus = 1; //Door is closed
  }
  else {
    dev.doorStatus = 0; //Door is closed
  }
  //2. Read mode switch status and update switch status
  dev.ch[1].i_switchMode = digitalRead(I_SW_CH1); //1: AUTO, 0: MANUAL
  dev.ch[2].i_switchMode = digitalRead(I_SW_CH2); //1: AUTO, 0: MANUAL
  dev.ch[3].i_switchMode = digitalRead(I_SW_CH3); //1: AUTO, 0: MANUAL
  dev.ch[4].i_switchMode = digitalRead(I_SW_CH4); //1: AUTO, 0: MANUAL

  //3. Read relay status and update relay status and 1 is OFF, 0 is ON
  dev.ch[1].i_contactorStatus = 1-digitalRead(I_RL_CH1); //0: OFF, 1: ON
  dev.ch[2].i_contactorStatus = 1-digitalRead(I_RL_CH2); //0: OFF, 1: ON
  dev.ch[3].i_contactorStatus = 1-digitalRead(I_RL_CH3); //0: OFF, 1: ON
  dev.ch[4].i_contactorStatus = 1-digitalRead(I_RL_CH4); //0: OFF, 1: ON

  delay(10);
}

