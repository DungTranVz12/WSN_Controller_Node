#include "define.h"
#include <EEPROM.h>
#include "AppPriFunc.h"

// #include "logo.h"
// #include <TFT_eSPI.h>
// TFT_eSPI    tft = TFT_eSPI();         // Create object "tft"
// TFT_eSprite img = TFT_eSprite(&tft);  // Create Sprite object "img" with pointer to "tft" object
//                                       // the pointer is used by pushSprite() to push it onto the TFT
#include "Adafruit_GFX.h"     // Библиотека обработчика графики
#include "Adafruit_ILI9341.h" // Программные драйвера для дисплеев ILI9341
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
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
  // rtcPrintDatetime(); //Print current datetime
  // rtcSetDatetime(2023, 11, 9, 0, 0, 0); //Set datetime

  //5. Button init
  buttonInit(); //Initialize button: BACK, DOWN, UP, SELECT
  
  //6. LCD init
  pinMode(TFL_BACKLIGHT, OUTPUT);
  digitalWrite(TFL_BACKLIGHT, HIGH); //Turn on LCD backlight
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(85,25);
  tft.print("HELLO WSN");
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(25,200);
  tft.setTextSize(2);
  tft.print("CM Engineering Vietnam");    // Выводим текст
  // tft.drawRGBBitmap(0, 0, logo, 240, 320);

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
  V_Door   = analogRead(I_Vmon_DOOR)*(5.0/1023.0)*2;
  // 1.  Read button status and update button status
  if (DOOR_INSTALLED == 1 && V_Door >= 1) {
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
  //1. Update SH21 value and store to variable
  if (sht21TimeCounter == 10) {
    SHT21_Read();
    // Serial.print("Temperature   : ");Serial.print(boxTemp,1);Serial.println("oC");
    // Serial.print("Humidity      : ");Serial.print(boxHumi,1);Serial.println("%");
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