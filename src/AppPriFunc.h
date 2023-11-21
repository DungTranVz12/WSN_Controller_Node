#include "define.h"
#include "RTClib.h"
#include <SHT2x.h>  
#include <AceButton.h>

using namespace ace_button;
AceButton buttonBack(BUTTON_BACK);
AceButton buttonDown(BUTTON_DOWN);
AceButton buttonUp(BUTTON_UP);
AceButton buttonSelect(BUTTON_SELECT);
void handleEvent(AceButton*, uint8_t, uint8_t);
SHT2x sht;

struct channelInfo {
  float Vout = 0; //Voltage of each channel.
  float Iout = 0; //Current of each channel.
  uint8_t operStatus = 0; //0: OFF, 1: ON
  uint8_t operMode = 0; //0: Manual, 1: Schedule
  uint32_t scheduleOper[20]; //Schedule operation time of each channel.
};

struct  deviceInfo
{
  /* data */
  float boxTemp = 0; //Temperature inside the box.
  float boxHumi = 0; //Humidity inside the box.
  float V_DC_IN = 0; //Voltage of DC input.
  float V_SS_SYS = 0; //Voltage of sensor system.
  float V_Door = 0; //Voltage of door sensor.
  uint8_t rfConnStatus = 0; //0: Not connected, 1: Connected
  uint8_t doorStatus = 0; //0: Closed, 1: Opened
  uint8_t loadFullScreenReq = 1; //0: No request, 1: Request
  
  // DateTime time; //RTC time
  DateTime time; //RTC time

  // Channle info
  channelInfo ch1; //Channel 1
  channelInfo ch2; //Channel 2
  channelInfo ch3; //Channel 3
  channelInfo ch4; //Channel 4
} dev;

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool rtcHwOperFlag = false; //RTC hardware operation flag: True: RTC is working, False: RTC is not working.

void SHT21_Read(){
  sht.read();
  dev.boxTemp = sht.getTemperature();
  dev.boxHumi = sht.getHumidity();
}

//***********************ADC reading**************************
void voltCurrMonUpdate(){
  dev.V_DC_IN  = analogRead(I_Vmon_DC_IN)*(5.0/1023.0)*(47.0+10.0)/10.0;
  dev.V_SS_SYS = analogRead(I_Vmon_SS_SYS)*(5.0/1023.0)*2/20/0.1*1000;
  // V_Door   = analogRead(I_Vmon_DOOR)*(5.0/1023.0)*2;

  dev.ch1.Vout = analogRead(I_Vmon_CH1);
  dev.ch2.Vout = analogRead(I_Vmon_CH2);
  dev.ch3.Vout = analogRead(I_Vmon_CH3);
  dev.ch4.Vout = analogRead(I_Vmon_CH4);
  dev.ch1.Iout = analogRead(I_Imon_CH1);
  dev.ch2.Iout = analogRead(I_Imon_CH2);
  dev.ch3.Iout = analogRead(I_Imon_CH3);
  dev.ch4.Iout = analogRead(I_Imon_CH4);

  // Serial.print("V_DC_IN : ");Serial.print(V_DC_IN,1);Serial.println("V");
  // Serial.print("V_SS_SYS: ");Serial.print(V_SS_SYS,1);Serial.println("mA");
  // // Serial.print("V_Door  : ");Serial.print(V_Door,1);Serial.println("V");
  // Serial.print("V_CH1   : ");Serial.print(Vout1);Serial.println(" Step");
  // Serial.print("V_CH2   : ");Serial.print(Vout2);Serial.println(" Step");
  // Serial.print("V_CH3   : ");Serial.print(Vout3);Serial.println(" Step");
  // Serial.print("V_CH4   : ");Serial.print(Vout4);Serial.println(" Step");
  // Serial.print("I_CH1   : ");Serial.print(Iout1);Serial.println(" Step");
  // Serial.print("I_CH2   : ");Serial.print(Iout2);Serial.println(" Step");
  // Serial.print("I_CH3   : ");Serial.print(Iout3);Serial.println(" Step");
  // Serial.print("I_CH4   : ");Serial.print(Iout4);Serial.println(" Step");
  // Serial.println("-----------------------------");
}

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

// Setup button pins and event handler
void buttonInit(){
  pinMode(BUTTON_BACK  , INPUT_PULLUP);
  pinMode(BUTTON_DOWN  , INPUT_PULLUP);
  pinMode(BUTTON_UP    , INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  // Configure the ButtonConfig with the event handler, and enable all higher
  // level events.
  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  // buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
}

//Scan button status and update button status
void buttonScan(){
  buttonBack.check();
  buttonDown.check();
  buttonUp.check();
  buttonSelect.check();
}
