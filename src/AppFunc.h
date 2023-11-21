#include "define.h"
#include <EEPROM.h>
#include "AppPriFunc.h"



//                                       // the pointer is used by pushSprite() to push it onto the TFT
#include "Adafruit_GFX.h"     // Библиотека обработчика графики
#include "Adafruit_ILI9341.h" // Программные драйвера для дисплеев ILI9341
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

class lcdScreen
{
  public:
  deviceInfo lastDev;
  void updateScreen(void){
    //Print with ANSI YELLOW color
    Serial.println("\1b[31m=== TASK3: LCD RUNNING ===\1b[0m");
    if (dev.loadFullScreenReq == 1){
      Serial.println("Load full screen request");
      lastDev = dev; //Update lastDev
      loadFullScreenReq();
      dev.loadFullScreenReq = 0; //Reset loadFullScreenReq
    }
    else{
      Serial.println("======DEBUG1======");
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

  void loadFullScreenReq (void){
    tft.fillRect(0, 0, 320, 40, ILI9341_BLUE);
    tft.fillRect(0, 40, 320, 240, ILI9341_WHITE);
    tft.fillRoundRect( 10,  50, 150, 85,10, ILI9341_BLUE); //Channel 1
    tft.fillRoundRect(165,  50, 150, 85,10, ILI9341_BLUE); //Channel 2
    tft.fillRoundRect( 10, 140, 150, 85,10, ILI9341_BLUE); //Channel 3
    tft.fillRoundRect(165, 140, 150, 85,10, ILI9341_BLUE); //Channel 4

    //Print Time
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10,12);
    tft.print("Time: ");
    tft.print(dev.time.hour());
    tft.print(":");
    tft.print(dev.time.minute());

    //Print connection status via a circle on the top right corner of the screen
    tft.fillCircle(290, 20, 10, ILI9341_GREEN);
    //Channel 1
    uint8_t titleH = 50;
    uint8_t titleV = 57;
    uint8_t hItemName = 20;
    uint8_t hItemValue = 100;
    uint8_t vContent = 80;
    uint8_t vSpace = 12;
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.setCursor(titleH, titleV);
    tft.print("CH1 ON");
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setCursor(hItemName, vContent+0*vSpace);
    tft.print("Voltage: ");
    tft.setCursor(hItemValue, vContent+0*vSpace);
    tft.print("110V");
    tft.setCursor(hItemName, vContent+1*vSpace);
    tft.print("Current: ");
    tft.setCursor(hItemValue, vContent+1*vSpace);
    tft.print("0.5A");
    tft.setCursor(hItemName, vContent+2*vSpace);
    tft.print("Mode: ");
    tft.setCursor(hItemValue, vContent+2*vSpace);
    tft.setTextColor(ILI9341_GREEN);
    tft.print("AUTO");
    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(hItemName, vContent+3*vSpace);
    tft.print("Next OFF: ");
    tft.setCursor(hItemValue, vContent+3*vSpace);
    tft.setTextColor(ILI9341_GREEN);
    tft.print("12:00");
    tft.setTextColor(ILI9341_WHITE);


    //Channel 2
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(175, 70);
    tft.print("CH2");
    tft.setTextSize(2);
    tft.setCursor(175, 100);
    tft.print("OFF");
    //Channel 3 ON
    tft.setTextSize(3);
    tft.setCursor(20, 160);
    tft.print("CH3");
    tft.setTextSize(2);
    tft.setCursor(20, 190);
    tft.print("OFF");
    //Channel 4
    tft.setTextSize(3);
    tft.setCursor(175, 160);
    tft.print("CH4");
    tft.setTextSize(2);
    tft.setCursor(175, 190);
    tft.print("OFF");
  }


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
  //2. Read switch status and update switch status

  vTaskDelay(10/portTICK_PERIOD_MS);
}

// Run every second
void task2 () {
  static uint8_t sht21TimeCounter = 0;
  sht21TimeCounter++;
  //Print with ANSI YELLOW color
  Serial.println("=== TASK 2 ===");
  //1. Update SH21 value and store to variable
  if (sht21TimeCounter == 10) {
    SHT21_Read();
    // Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
    // Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
    sht21TimeCounter = 0;
  }
  
  //2. Update Current/Voltage of every Channel and store to variable every second
  voltCurrMonUpdate();

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