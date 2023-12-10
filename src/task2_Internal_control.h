#ifndef APPFUNC
#include "appfunc.h"
#endif

// Run every 100ms
void task2 () {
  // Serial.println("=== TASK 2 ===");
  static uint16_t sht21TimeCounter = 0;   //Counter for SHT21 timer
  static uint16_t rtcTimeCounter = 0;     //Counter for RTC timer
  static uint16_t findMaxVoltCounter = 0; //Counter for findMaxVoltage function
  static uint16_t sendWdtCounter = 0;     //Counter for WDT timer
  sht21TimeCounter++;
  rtcTimeCounter++;
  findMaxVoltCounter++;
  sendWdtCounter++;
  
  //1. Update RTC timer every minute
  if (rtcTimeCounter >= 600) {
    dev.time = rtc.now(); //Get current datetime
    rtcTimeCounter = dev.time.second()*10; //Reset RTC timer counter
  }

  //2. Update RTC timer every minute
  if (sendWdtCounter >= 600) {
    rfSend("10","TASK1_OK");
    rfSend("10","TASK2_OK");
    sendWdtCounter = 0;
  }

  //3. Update SH21 value and store to variable every 10 seconds
  if (sht21TimeCounter >= 100) {
    SHT21_Read();
    // Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
    // Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
    sht21TimeCounter = 0;
  }
  
  //4. Update Current/Voltage of every Channel and store to variable every second
  //4.1 Sample voltage of all channels every 100ms
  dev.ch[1].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH1);
  dev.ch[2].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH2);
  dev.ch[3].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH3);
  dev.ch[4].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH4);
  dev.ch[1].listVoltIndex++; if (dev.ch[1].listVoltIndex >= 40) dev.ch[1].listVoltIndex = 0;
  //4.2 Find max voltage of all channels
  if (findMaxVoltCounter >= 10) {
    findMaxVoltage(1); //Find max voltage of channel 1
    findMaxVoltage(2); //Find max voltage of channel 2
    findMaxVoltage(3); //Find max voltage of channel 3
    findMaxVoltage(4); //Find max voltage of channel 4
    findMaxVoltCounter = 0;
  }
  //4.3 voltCurrMonUpdate();


  //5. Cảnh báo nếu kênh đang được mở mà dòng bằng 0 hoặc áp bằng 0
  // if (operStatusCh1 == 1 && (Vout1 == 0 || Iout1 == 0)) {
  //   Serial.println("Channel 1 is opened but current or voltage is 0");
  //   operStatusCh1 = 0;
  //   EEPROM.write(ADDR_CH1_OPER_STS, operStatusCh1);
  // }

  //6. Nếu SW đang ở chế độ AUTO_MODE thì kiểm tra thời gian và thực hiện mở/đóng kênh
  /***
  if (operModeCh1 == AUTO_MODE) {
    //Kiểm tra thời gian hiện tại có trùng với thời gian được lưu trong EEPROM hay không?
    if (rtcGetDatetime().hour == EEPROM.read(ADDR_CH1_SCHDLE_TIME[0]) && rtcGetDatetime().minute == EEPROM.read(ADDR_CH1_SCHDLE_TIME[1])) {
      //Nếu trùng thì thực hiện mở/đóng kênh
      if (operStatusCh1 == 0) {
        //Nếu kênh đang đóng thì mở kênh
        digitalWrite(O_RELAY_1_LEVEL, HIGH);
        operStatusCh1 = 1;
        EEPROM.write(ADDR_CH1_OPER_STS, operStatusCh1);
      }
      else {
        //Nếu kênh đang mở thì đóng kênh
        digitalWrite(O_RELAY_1_LEVEL, LOW);
        operStatusCh1 = 0;
        EEPROM.write(ADDR_CH1_OPER_STS, operStatusCh1);
      }
    }
  }
  ***/

}