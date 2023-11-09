#include "define.h"
#include "RTClib.h"

// #include "SPI.h"
// #include "Adafruit_GFX.h"
// #include "Adafruit_ILI9341.h"
// // For the Adafruit shield, these are the default.
// #define TFT_DC 9
// #define TFT_CS 10

// // Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool rtcHwOperFlag = false; //RTC hardware operation flag: True: RTC is working, False: RTC is not working.

// Initializes the RTC module and sets the rtcHwOperFlag to true if the hardware is operational.
void rtcInit(){
  //Kiểm tra tối đa 3 lần xem rtc phần cứng có chạy không? Nếu chạy thì bật cờ rtcHwOperFlag = true rồi thoát khỏi vòng lặp.
  for (int i = 0; i < 3; i++)
  {
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      delay(100);
    }else{
      rtcHwOperFlag = true;
      break;
    }
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

// Returns the current date and time in the format: "yyyy/mm/dd (day) hh:mm:ss"
void rtcPrintDatetime (){
  if (!rtcHwOperFlag) {
    Serial.println("=== RTC is not working! ===");
    return;
  }
  DateTime now = rtc.now();
  Serial.print("RTC: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

// Sets the date and time for the RTC module.
void rtcSetDatetime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
  if (!rtcHwOperFlag) {
    Serial.println("=== RTC is not working! ===");
    return;
  }
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
  Serial.println("=== RTC is set: " + String(year) + "/" + String(month) + "/" + String(day) + " " + String(hour) + ":" + String(minute) + ":" + String(second) + " ===");
}
