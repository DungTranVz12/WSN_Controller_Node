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
  uint16_t listVolt[40] = {0}; //List of latest 20 values of voltage of each channel.
  uint8_t  listVoltIndex = 0; //Index of listVolt array.
  uint8_t operStatus = 0; //0: OFF, 1: ON
  uint8_t autoControl = 0; //0: OFF, 1: ON
  uint8_t operMode = MANUAL_MODE; //MANUAL_MODE/AUTO_MODE
  uint32_t scheduleOper[20][2]; //Schedule operation time of each channel.
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
  uint8_t firstLoadMem = 0; //0: Not loaded, 1: Loaded
  
  // DateTime time; //RTC time
  DateTime time; //RTC time

  // Channel info
  channelInfo ch[5]; //Channel 0,1,2,3,4. Channel 0 is not used.
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

  dev.ch[1].Vout = analogRead(I_Vmon_CH1);
  dev.ch[2].Vout = analogRead(I_Vmon_CH2);
  dev.ch[3].Vout = analogRead(I_Vmon_CH3);
  dev.ch[4].Vout = analogRead(I_Vmon_CH4);
  dev.ch[1].Iout = analogRead(I_Imon_CH1);
  dev.ch[2].Iout = analogRead(I_Imon_CH2);
  dev.ch[3].Iout = analogRead(I_Imon_CH3);
  dev.ch[4].Iout = analogRead(I_Imon_CH4);

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
  // 4.1 Set datetime to RTC with PC time
  // rtcSetDatetime(2000, 1, 1, 0, 0, 0); //Manual set datetime
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //PC set datetime
  rtcPrintDatetime(); //Print current datetime
  // 4.2 Set datetime to dev.time
  dev.time = rtc.now(); //Get current datetime
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


/**
 * Finds the maximum voltage value from the list of voltage readings for a given channel.
 * 
 * @param chNum The channel number for which to find the maximum voltage.
 */
void findMaxVoltage(uint8_t chNum){
  uint16_t maxValue = 0;
  uint8_t  sampleNum = 40;
  double VmaxD=0;
  double VeffD;
  double Veff;
  double Voutput;

  //Find max value
  for (int i = 0; i < sampleNum; i++)
  {
    if (dev.ch[chNum].listVolt[i] > maxValue)
    {
      maxValue = dev.ch[chNum].listVolt[i];
    }
  }
  VmaxD=maxValue;
  VeffD=VmaxD/sqrt(2);
  Veff=(((VeffD-420.76)/-90.24)*-210.2)+210.2;
  Voutput = Veff*222.0/70.0;
  if (Voutput < 0) Voutput = 0;
  dev.ch[chNum].Vout = int(Voutput);
}

void pwmControlCheck () {
  static uint8_t pwmStateCh1 = LOW;
  static uint8_t pwmStateCh2 = LOW;
  static uint8_t pwmStateCh3 = LOW;
  static uint8_t pwmStateCh4 = LOW;

  if (dev.firstLoadMem == 0) return; //If memory is not loaded at the first time => return.

  //Channel 1
  if (dev.ch[1].operMode == AUTO_MODE) {
    if (dev.ch[1].autoControl == CONTROL_ON) {
      digitalWrite(O_RELAY_1_LEVEL, HIGH);
      pwmStateCh1 = !pwmStateCh1;
      digitalWrite(O_RELAY_1_PWM, pwmStateCh1);
    }
    else{
      digitalWrite(O_RELAY_1_LEVEL, LOW);
    }
  }
  else{
    digitalWrite(O_RELAY_1_LEVEL, LOW);
  }
  //Channel 2
  if (dev.ch[2].operMode == AUTO_MODE) {
    if (dev.ch[2].autoControl == CONTROL_ON) {
      digitalWrite(O_RELAY_2_LEVEL, HIGH);
      pwmStateCh2 = !pwmStateCh2;
      digitalWrite(O_RELAY_2_PWM, pwmStateCh2);
    }
    else{
      digitalWrite(O_RELAY_2_LEVEL, LOW);
    }
  }
  else{
    digitalWrite(O_RELAY_2_LEVEL, LOW);
  }
  //Channel 3
  if (dev.ch[3].operMode == AUTO_MODE) {
    if (dev.ch[3].autoControl == CONTROL_ON) {
      digitalWrite(O_RELAY_3_LEVEL, HIGH);
      pwmStateCh3 = !pwmStateCh3;
      digitalWrite(O_RELAY_3_PWM, pwmStateCh3);
    }
    else{
      digitalWrite(O_RELAY_3_LEVEL, LOW);
    }
  }
  else{
    digitalWrite(O_RELAY_3_LEVEL, LOW);
  }
  //Channel 4
  if (dev.ch[4].operMode == AUTO_MODE) {
    if (dev.ch[4].autoControl == CONTROL_ON) {
      digitalWrite(O_RELAY_4_LEVEL, HIGH);
      pwmStateCh4 = !pwmStateCh4;
      digitalWrite(O_RELAY_4_PWM, pwmStateCh4);
    }
    else{
      digitalWrite(O_RELAY_4_LEVEL, LOW);
    }
  }
  else{
    digitalWrite(O_RELAY_4_LEVEL, LOW);
  }
}