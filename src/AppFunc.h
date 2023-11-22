#include "define.h"
#include <EEPROM.h>
#include <TimerOne.h>
#include "AppPriFunc.h"



//                                       // the pointer is used by pushSprite() to push it onto the TFT
#include "Adafruit_GFX.h"     // Библиотека обработчика графики
#include "Adafruit_ILI9341.h" // Программные драйвера для дисплеев ILI9341
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

class lcdScreen
{
  public:
  deviceInfo lastDev; //Last device info
  deviceInfo proDev;  //Processing device info

  void updateScreen(void){
    proDev = dev; //Update proDev
    proDev.ch[1].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[1].scheduleOper[0][1] = 30;          //<------------- DEBUG
    proDev.ch[2].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[2].scheduleOper[0][1] = 30;          //<------------- DEBUG
    proDev.ch[3].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[3].scheduleOper[0][1] = 30;          //<------------- DEBUG
    proDev.ch[4].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[4].scheduleOper[0][1] = 30;          //<------------- DEBUG
    //Print with ANSI YELLOW color
    Serial.println("=== TASK3: LCD UPDATE SCREEN ===");
    if (proDev.loadFullScreenReq == 1){
      Serial.println("=== LCD FIRST UPDATE SCREEN ===");
      lastDev = proDev; //Update lastDev
      loadFullScreenReq();
      proDev.loadFullScreenReq = 0; //Reset loadFullScreenReq
      dev.loadFullScreenReq = 0; //Reset loadFullScreenReq
    }
    else{
    //Update title bar
    updateTitle(NORMAL_UPDATE);
    //Update channel
    updateChannel(1, NORMAL_UPDATE);
    updateChannel(2, NORMAL_UPDATE);
    updateChannel(3, NORMAL_UPDATE);
    updateChannel(4, NORMAL_UPDATE);
    }
  }

  void updateTitleBar(void){
    //Print Time
    DateTime now = rtc.now();
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10,12);
    tft.print("Time: ");
    tft.print(now.hour());
    tft.print(":");
    tft.print(now.minute());

    //Print connection status via a circle on the top right corner of the screen
    tft.fillCircle(290, 20, 10, ILI9341_GREEN);
  }

  void updateTitle (uint8_t forcedUpdate = 0){
    if (lastDev.time != proDev.time || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_WHITE); //Blue is the background color
      tft.setTextSize(2);
      tft.setCursor(10,12);
      tft.print("Time: ");
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.print(lastDev.time.hour());
      tft.print(":");
      //print minute with leading zero
      if (lastDev.time.minute() < 10)
        tft.print("0");
      tft.print(lastDev.time.minute());
      //Print Time
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(10,12);
      tft.print("Time: ");
      tft.print(proDev.time.hour());
      tft.print(":");
      //print minute with leading zero
      if (proDev.time.minute() < 10)
        tft.print("0");
      tft.print(proDev.time.minute());
      lastDev.time = proDev.time;
    }
    if (lastDev.rfConnStatus != proDev.rfConnStatus || forcedUpdate == FORCED_UPDATE){
      if (proDev.rfConnStatus == 1)
        tft.fillCircle(290, 20, 10, ILI9341_GREEN);
      else
        tft.fillCircle(290, 20, 10, ILI9341_RED);
      lastDev.rfConnStatus = proDev.rfConnStatus;
    }
  }

  void updateChannel(uint8_t chNum, uint8_t forcedUpdate = 0){

    uint16_t titleH = 50;
    uint16_t titleV = 57;
    uint16_t hItemName = 20;
    uint16_t hItemValue = 100;
    uint16_t vContent = 80;
    uint16_t vSpace = 12;

    //Channel 1
    if (chNum == 1){
      titleH = 50;
      titleV = 57;
      hItemName = 20;
      hItemValue = 100;
      vContent = 80;
      vSpace = 12;
    }
    //Channel 2
    else if (chNum == 2){
      titleH = 205;
      titleV = 57;
      hItemName = 175;
      hItemValue = 265;
      vContent = 80;
      vSpace = 12;
    }
    //Channel 3
    else if (chNum == 3){
      titleH = 50;
      titleV = 147;
      hItemName = 20;
      hItemValue = 100;
      vContent = 170;
      vSpace = 12;
    }
    //Channel 4
    else if (chNum == 4){
      titleH = 205;
      titleV = 147;
      hItemName = 175;
      hItemValue = 265;
      vContent = 170;
      vSpace = 12;
    }
    else{
      return;
    }

    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.setCursor(titleH, titleV);
    //1. Update Channel
    if (lastDev.ch[chNum].operStatus != proDev.ch[chNum].operStatus || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(titleH, titleV);
      tft.print("CH"+String(chNum)+" ON");
      tft.setCursor(titleH, titleV);
      tft.print("CH"+String(chNum)+" OFF");
      //Print Channel status
      tft.setCursor(titleH, titleV);
      if (proDev.ch[chNum].operStatus == 1){
        tft.setTextColor(ILI9341_GREEN);
        tft.print("CH"+String(chNum)+" ON");
      }
      else {
        tft.setTextColor(ILI9341_RED);
        tft.print("CH"+String(chNum)+" OFF");
      }
      lastDev.ch[chNum].operStatus = proDev.ch[chNum].operStatus;
    }

    tft.setTextSize(1);
    //2. Update Voltage
    if (lastDev.ch[chNum].Vout != proDev.ch[chNum].Vout || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(hItemValue, vContent+0*vSpace);
      tft.print(lastDev.ch[chNum].Vout,0);
      tft.print(" V");
      //Print Item name
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemName, vContent+0*vSpace);
      tft.print("Voltage ");
      //Print Voltage
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemValue, vContent+0*vSpace);
      tft.print(proDev.ch[chNum].Vout,0);
      tft.print(" V");
      lastDev.ch[chNum].Vout = proDev.ch[chNum].Vout;
    }
    //3. Update Current
    if (lastDev.ch[chNum].Iout != proDev.ch[chNum].Iout || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(hItemValue, vContent+1*vSpace);
      tft.print(lastDev.ch[chNum].Iout,1);
      tft.print(" A");
      //Print Item name
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemName, vContent+1*vSpace);
      tft.print("Current ");
      //Print Current
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemValue, vContent+1*vSpace);
      tft.print(proDev.ch[chNum].Iout,1);
      tft.print(" A");
      lastDev.ch[chNum].Iout = proDev.ch[chNum].Iout;
    }
    //4. Update Mode
    if (lastDev.ch[chNum].operMode != proDev.ch[chNum].operMode || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(hItemValue, vContent+2*vSpace);
      tft.print("MANUAL");
      tft.setCursor(hItemValue, vContent+2*vSpace);
      tft.print("AUTO");
      //Print Item name
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemName, vContent+2*vSpace);
      tft.print("Mode ");
      //Print Mode
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemValue, vContent+2*vSpace);
      if (proDev.ch[chNum].operMode == MANUAL_MODE) {
        tft.setTextColor(ILI9341_WHITE);
        tft.print("MANUAL");
      }
      else{
        tft.setTextColor(ILI9341_GREEN);
        tft.print("AUTO");
      }
      lastDev.ch[chNum].operMode = proDev.ch[chNum].operMode;
    }
    //5. Update Schedule
    if (lastDev.ch[chNum].scheduleOper[0] != proDev.ch[chNum].scheduleOper[0] || forcedUpdate == FORCED_UPDATE){
      if (proDev.ch[chNum].operMode == AUTO_MODE) {
        //Clear the old text
        tft.setTextColor(ILI9341_BLUE); //Blue is the background color
        tft.setCursor(hItemValue, vContent+3*vSpace);
        tft.print(lastDev.ch[chNum].scheduleOper[0][0]);
        tft.print(":");
        tft.print(lastDev.ch[chNum].scheduleOper[0][1]);
        //Print Item name
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(hItemName, vContent+3*vSpace);
        tft.print("Next OFF ");
        //Print Schedule
        tft.setTextColor(ILI9341_GREEN);
        tft.setCursor(hItemValue, vContent+3*vSpace);
        tft.print(proDev.ch[chNum].scheduleOper[0][0]);
        tft.print(":");
        tft.print(proDev.ch[chNum].scheduleOper[0][1]);
        lastDev.ch[chNum].scheduleOper[0][0] = proDev.ch[chNum].scheduleOper[0][0];
        lastDev.ch[chNum].scheduleOper[0][1] = proDev.ch[chNum].scheduleOper[0][1];
      }
      else{
        //Draw a blue rectangle to cover the old text and item name
        tft.fillRect(hItemName, vContent+3*vSpace, 130, 8, ILI9341_BLUE);
      }
    }
  }




  //   tft.print("CH1 ON");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setTextSize(1);
  //   tft.setCursor(hItemName, vContent+0*vSpace);
  //   tft.print("Voltage: ");
  //   tft.setCursor(hItemValue, vContent+0*vSpace);
  //   tft.print("110V");
  //   tft.setCursor(hItemName, vContent+1*vSpace);
  //   tft.print("Current: ");
  //   tft.setCursor(hItemValue, vContent+1*vSpace);
  //   tft.print("0.5A");
  //   tft.setCursor(hItemName, vContent+2*vSpace);
  //   tft.print("Mode: ");
  //   tft.setCursor(hItemValue, vContent+2*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("AUTO");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setCursor(hItemName, vContent+3*vSpace);
  //   tft.print("Next OFF: ");
  //   tft.setCursor(hItemValue, vContent+3*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("12:00");
  //   tft.setTextColor(ILI9341_WHITE);
  // }




  void loadFullScreenReq (void){
    //Draw Frame
    tft.fillRect(0, 0, 320, 40, ILI9341_BLUE);
    tft.fillRect(0, 40, 320, 240, ILI9341_WHITE);
    tft.fillRoundRect( 10,  50, 150, 85,10, ILI9341_BLUE); //Channel 1
    tft.fillRoundRect(165,  50, 150, 85,10, ILI9341_BLUE); //Channel 2
    tft.fillRoundRect( 10, 140, 150, 85,10, ILI9341_BLUE); //Channel 3
    tft.fillRoundRect(165, 140, 150, 85,10, ILI9341_BLUE); //Channel 4

    //Update title bar
    updateTitle(FORCED_UPDATE);

    //Update channel
    updateChannel(1, FORCED_UPDATE);
    updateChannel(2, FORCED_UPDATE);
    updateChannel(3, FORCED_UPDATE);
    updateChannel(4, FORCED_UPDATE);
  }



  //   //Channel 1
  //   uint8_t titleH = 50;
  //   uint8_t titleV = 57;
  //   uint8_t hItemName = 20;
  //   uint8_t hItemValue = 100;
  //   uint8_t vContent = 80;
  //   uint8_t vSpace = 12;
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.setTextSize(2);
  //   tft.setCursor(titleH, titleV);
  //   tft.print("CH1 ON");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setTextSize(1);
  //   tft.setCursor(hItemName, vContent+0*vSpace);
  //   tft.print("Voltage: ");
  //   tft.setCursor(hItemValue, vContent+0*vSpace);
  //   tft.print("110V");
  //   tft.setCursor(hItemName, vContent+1*vSpace);
  //   tft.print("Current: ");
  //   tft.setCursor(hItemValue, vContent+1*vSpace);
  //   tft.print("0.5A");
  //   tft.setCursor(hItemName, vContent+2*vSpace);
  //   tft.print("Mode: ");
  //   tft.setCursor(hItemValue, vContent+2*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("AUTO");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setCursor(hItemName, vContent+3*vSpace);
  //   tft.print("Next OFF: ");
  //   tft.setCursor(hItemValue, vContent+3*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("12:00");
  //   tft.setTextColor(ILI9341_WHITE);


  //   //Channel 2
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setTextSize(3);
  //   tft.setCursor(175, 70);
  //   tft.print("CH2");
  //   tft.setTextSize(2);
  //   tft.setCursor(175, 100);
  //   tft.print("OFF");
  //   //Channel 3 ON
  //   tft.setTextSize(3);
  //   tft.setCursor(20, 160);
  //   tft.print("CH3");
  //   tft.setTextSize(2);
  //   tft.setCursor(20, 190);
  //   tft.print("OFF");
  //   //Channel 4
  //   tft.setTextSize(3);
  //   tft.setCursor(175, 160);
  //   tft.print("CH4");
  //   tft.setTextSize(2);
  //   tft.setCursor(175, 190);
  //   tft.print("OFF");
  // }


};
lcdScreen lcd = lcdScreen();

//////////////////////////////////////
// Initialize for controller node   //
//////////////////////////////////////
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
  pinMode(O_LED_4G, OUTPUT);
  pinMode(O_LED_RF, OUTPUT);
  pinMode(O_LED_WIFI, OUTPUT);
  pinMode(O_LED_BAT, OUTPUT);
  pinMode(O_LED_SLA, OUTPUT);
  pinMode(O_LED_PWR, OUTPUT);
  pinMode(I_SW_CH1, INPUT);
  pinMode(I_SW_CH2, INPUT);
  pinMode(I_SW_CH3, INPUT);
  pinMode(I_SW_CH4, INPUT);
  pinMode(I_RL_CH1, INPUT);
  pinMode(I_RL_CH2, INPUT);
  pinMode(I_RL_CH3, INPUT);
  pinMode(I_RL_CH4, INPUT);
  pinMode(BUTTON_BACK, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_SELECT, INPUT);

  digitalWrite(O_LED_4G  , HIGH);
  digitalWrite(O_LED_RF  , HIGH);
  digitalWrite(O_LED_WIFI, HIGH);
  digitalWrite(O_LED_BAT , LOW );
  digitalWrite(O_LED_SLA , HIGH);
  digitalWrite(O_LED_PWR , LOW );

  Timer1.initialize(1000); // 1 Millisecond
  Timer1.attachInterrupt(pwmControlCheck); // blinkLED to run every 0.15 seconds

  // 2. Load config from EEPROM
  // operStatusCh1 = EEPROM.read();

  // 3. Return last state to GPIO


  //4. Initialize RTC
  rtcInit(); //Initialize RTC
  dev.time = rtc.now(); //Get current datetime

  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Set datetime
  // rtcPrintDatetime(); //Print current datetime
  // rtcSetDatetime(2023, 11, 9, 0, 0, 0); //Set datetime

  //5. Button init
  buttonInit(); //Initialize button: BACK, DOWN, UP, SELECT
  
  //6. LCD init
  pinMode(TFL_BACKLIGHT, OUTPUT);
  digitalWrite(TFL_BACKLIGHT, HIGH); //Turn on LCD backlight
  tft.begin();
  tft.setRotation(3);

  //7. SHT21
  sht.begin();
}

//////////////////////////
//Button Handler        //
//////////////////////////
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  // Print out a message for all events, for both buttons.
  Serial.print(F("handleEvent(): pin: "));
  Serial.print(button->getPin());
  Serial.print(F("; eventType: "));
  Serial.print(AceButton::eventName(eventType));
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  // switch (eventType) {
  //   case AceButton::kEventPressed:
  //     if (button->getPin() == BUTTON1_PIN) {
  //       digitalWrite(LED_PIN, LED_ON);
  //     }
  //     break;
  //   case AceButton::kEventReleased:
  //     if (button->getPin() == BUTTON1_PIN) {
  //       digitalWrite(LED_PIN, LED_OFF);
  //     }
  //     break;
  //   case AceButton::kEventClicked:
  //     if (button->getPin() == BUTTON2_PIN) {
  //       Serial.println(F("Button 2 clicked!"));
  //     } else if (button->getPin() == BUTTON3_PIN) {
  //       Serial.println(F("Button 3 clicked!"));
  //     }
  //     break;
  // }
}

//===========================================================================================================//
void task1 () {
  // Serial.println("=== TASK 1 ===");

  dev.V_Door   = analogRead(I_Vmon_DOOR)*(5.0/1023.0)*2;
  // 1.  Read button status and update button status
  if (DOOR_INSTALLED == 1 && dev.V_Door >= 1) {
    buttonScan();               //Scan button status and update button status
    digitalWrite(TFL_BACKLIGHT, LCD_ON);     //Turn on LCD backlight
  }
  else if (DOOR_INSTALLED == 0){
    buttonScan();              //Scan button status and update button status
    digitalWrite(TFL_BACKLIGHT, LCD_ON);     //Turn OFF LCD backlight
  }
  else {
    digitalWrite(TFL_BACKLIGHT, LCD_OFF);     //Turn OFF LCD backlight
  }
  //2. Read mode switch status and update switch status
  dev.ch[1].operMode = digitalRead(I_SW_CH1); //1: AUTO, 0: MANUAL
  dev.ch[2].operMode = digitalRead(I_SW_CH2); //1: AUTO, 0: MANUAL
  dev.ch[3].operMode = digitalRead(I_SW_CH3); //1: AUTO, 0: MANUAL
  dev.ch[4].operMode = digitalRead(I_SW_CH4); //1: AUTO, 0: MANUAL

  //3. Read relay status and update relay status and 1 is OFF, 0 is ON
  dev.ch[1].operStatus = 1-digitalRead(I_RL_CH1); //0: OFF, 1: ON
  dev.ch[2].operStatus = 1-digitalRead(I_RL_CH2); //0: OFF, 1: ON
  dev.ch[3].operStatus = 1-digitalRead(I_RL_CH3); //0: OFF, 1: ON
  dev.ch[4].operStatus = 1-digitalRead(I_RL_CH4); //0: OFF, 1: ON

  delay(10);
}

// Run every second
void task2 () {
  Serial.println("=== TASK 2 ===");
  static uint8_t sht21TimeCounter = 0; //Counter for SHT21 timer
  static uint8_t rtcTimeCounter = 0; //Counter for RTC timer
  static uint8_t findMaxVoltCounter = 0; //Counter for findMaxVoltage function
  sht21TimeCounter++;
  rtcTimeCounter++;
  findMaxVoltCounter++;
  
  //0. Update RTC timer every minute
  if (rtcTimeCounter >= 600) {
    dev.time = rtc.now(); //Get current datetime
    rtcTimeCounter = dev.time.second();
  }

  //1. Update SH21 value and store to variable every 10 seconds
  if (sht21TimeCounter >= 100) {
    SHT21_Read();
    Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
    Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
    sht21TimeCounter = 0;
  }
  
  //2. Update Current/Voltage of every Channel and store to variable every second
  //2.1 Sample voltage of all channels every 100ms
  dev.ch[1].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH1);
  dev.ch[2].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH2);
  dev.ch[3].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH3);
  dev.ch[4].listVolt[dev.ch[1].listVoltIndex] = analogRead(I_Vmon_CH4);
  dev.ch[1].listVoltIndex++; if (dev.ch[1].listVoltIndex >= 40) dev.ch[1].listVoltIndex = 0;
  //2.2 Find max voltage of all channels
  if (findMaxVoltCounter >= 10) {
    findMaxVoltage(1); //Find max voltage of channel 1
    findMaxVoltage(2); //Find max voltage of channel 2
    findMaxVoltage(3); //Find max voltage of channel 3
    findMaxVoltage(4); //Find max voltage of channel 4
    findMaxVoltCounter = 0;
  }
  //2.3 voltCurrMonUpdate();


  //3. Cảnh báo nếu kênh đang được mở mà dòng bằng 0 hoặc áp bằng 0
  // if (operStatusCh1 == 1 && (Vout1 == 0 || Iout1 == 0)) {
  //   Serial.println("Channel 1 is opened but current or voltage is 0");
  //   operStatusCh1 = 0;
  //   EEPROM.write(ADDR_CH1_OPER_STS, operStatusCh1);
  // }

  //4. Nếu SW đang ở chế độ AUTO_MODE thì kiểm tra thời gian và thực hiện mở/đóng kênh
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