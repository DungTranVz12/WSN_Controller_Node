#ifndef APPFUNC
#include "appfunc.h"
#endif
uint8_t findMaxVoltageDone = 0; //Flag to check if findMaxVoltage function is done or not
extern void rfRxCheck();

// Run every 100ms
void task2 () {
  // Serial.println("=== TASK 2 ===");
  static uint16_t sht21TimeCounter = 0;   //Counter for SHT21 timer
  static uint16_t rtcTimeCounter = 0;     //Counter for RTC timer
  static uint16_t findMaxVoltCurrentCounter = 0; //Counter for findMaxVoltage function and findMaxCurrent
  static uint16_t sendWdtCounter = 0;     //Counter for WDT timer
  sht21TimeCounter++;
  rtcTimeCounter++;
  findMaxVoltCurrentCounter++;
  sendWdtCounter++;
  dev.rfStatusCounter++; //Counter for RF status
  
  //Change 4G LED status every second
  static bool led4GStatus = false;
  if (led4GStatus == false) { //4G is connected
    digitalWrite(O_LED_4G, HIGH); //Turn on LED 4G
    led4GStatus = true;
  }
  else { //4G is not connected
    digitalWrite(O_LED_4G, LOW); //Turn off LED 4G
    led4GStatus = false;
  }

  //1. Update RTC timer every minute
  if (rtcTimeCounter >= 600) {
    dev.time = rtc.now(); //Get current datetime
    rtcTimeCounter = dev.time.second()*10; //Reset RTC timer counter
  }

  //2. Update RTC timer every minute
  if (sendWdtCounter >= 600) {
    rfSendToGateway("10","TASK2_OK");
    sendWdtCounter = 0;
  }

  //3. Update SH21 value and store to variable every 10 seconds
  if (sht21TimeCounter >= 100) {
    SHT21_Read();
    // Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
    // Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
    sht21TimeCounter = 0;
  }
  
  //3. Check and control RF status
  if (dev.rfStatusCounter >= 18000){ //18000*100ms = 30min
    dev.rfConnStatus = 0; //0: Not connected, 1: Connected
    digitalWrite(O_LED_RF, HIGH); //Turn off LED RF
  }

  //4. Update Current/Voltage of every Channel and store to variable every second
  //4.1 Sample voltage of all channels every 100ms
  dev.ch[1].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH1);
  dev.ch[2].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH2);
  dev.ch[3].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH3);
  dev.ch[4].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH4);
  dev.ch[1].listVoltIndex++; if (dev.ch[1].listVoltIndex >= 40) dev.ch[1].listVoltIndex = 0;
  //4.2 Find max voltage and current of all channels
  uint8_t MAX_VALUE;
  if (dev.doorStatus == 1) { //Door is opened
    MAX_VALUE = 10; //Khi cửa mở. Giây đầu tìm áp lớn nhất. Giây sau tìm dòng lớn nhất.
  }
  else { //Door is closed
    MAX_VALUE = 50; //Khi cửa đóng. Giây thứ 5 tìm áp lớn nhất. Giây thứ 10 tìm dòng lớn nhất.
  }
  if (findMaxVoltCurrentCounter >= MAX_VALUE && findMaxVoltageDone == 0) {
    findMaxVoltage(1); //Find max voltage of channel 1
    findMaxVoltage(2); //Find max voltage of channel 2
    findMaxVoltage(3); //Find max voltage of channel 3
    findMaxVoltage(4); //Find max voltage of channel 4
    findMaxVoltageDone = 1; //Set flag to 1
  }
  if (findMaxVoltCurrentCounter >= 2*MAX_VALUE) {
    findMaxCurrent(1); //Find max current of channel 1
    findMaxCurrent(2); //Find max current of channel 2
    findMaxCurrent(3); //Find max current of channel 3
    findMaxCurrent(4); //Find max current of channel 4
    findMaxVoltageDone = 0; //Set flag to 1
    findMaxVoltCurrentCounter = 0; //Reset counter
  }
  


  //5. Cảnh báo nếu kênh đang được mở mà dòng bằng 0 hoặc áp bằng 0
  // if (i_contactorStatusCh1 == 1 && (Vout1 == 0 || Iout1 == 0)) {
  //   Serial.println("Channel 1 is opened but current or voltage is 0");
  //   i_contactorStatusCh1 = 0;
  //   EEPROM.write(ADDR_CH1_OPER_STS, i_contactorStatusCh1);
  // }

  //6. Nếu SW đang ở chế độ AUTO_MODE thì kiểm tra thời gian và thực hiện mở/đóng kênh
  /***
  if (i_switchModeCh1 == AUTO_MODE) {
    //Kiểm tra thời gian hiện tại có trùng với thời gian được lưu trong EEPROM hay không?
    if (rtcGetDatetime().hour == EEPROM.read(ADDR_CH1_SCHDLE_TIME[0]) && rtcGetDatetime().minute == EEPROM.read(ADDR_CH1_SCHDLE_TIME[1])) {
      //Nếu trùng thì thực hiện mở/đóng kênh
      if (i_contactorStatusCh1 == 0) {
        //Nếu kênh đang đóng thì mở kênh
        digitalWrite(O_RELAY_1_LEVEL, HIGH);
        i_contactorStatusCh1 = 1;
        EEPROM.write(ADDR_CH1_OPER_STS, i_contactorStatusCh1);
      }
      else {
        //Nếu kênh đang mở thì đóng kênh
        digitalWrite(O_RELAY_1_LEVEL, LOW);
        i_contactorStatusCh1 = 0;
        EEPROM.write(ADDR_CH1_OPER_STS, i_contactorStatusCh1);
      }
    }
  }
  ***/

}