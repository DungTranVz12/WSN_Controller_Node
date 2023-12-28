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
  uint16_t listVolt[20] = {0}; //List of latest 20 values of voltage of each channel.
  uint8_t  listVoltIndex = 0; //Index of listVolt array.
  uint16_t listCurrent[20] = {0}; //List of latest 20 values of current of each channel.
  uint8_t  listCurrentIndex = 0; //Index of listCurrent array.
  uint8_t i_contactorStatus = OFF_STATUS; //0: OFF, 1: ON. Directly read from relay/Contactor.
  bool    remoteControlFlag = false; //Khi nhận được yêu cầu remote control trong chế độ AUTO MODE thì cờ này bật lên và chiếm quyền Schedule.  
  uint8_t autoControl = CONTROL_OFF; //0: OFF, 1: ON. Control in AUTO MODE bởi schedule hoặc remote control.
  uint8_t i_switchMode = MANUAL_MODE; //MANUAL_MODE/AUTO_MODE. Directly read from switch.
  bool    inScheduleFlag = false; //0: Not in schedule, 1: In schedule
  uint8_t  lastControlStatus = 0; //0: Unknow, 1: MANUAL_OFF, 2: MANUAL_ON, 3: AUTO_OFF, 4: AUTO_ON
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
  uint8_t  rfConnStatus = 0; //0: Not connected, 1: Connected
  uint16_t rfStatusCounter = 0; //Counter for RF status
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

void delayForLowPrioTask(uint32_t delayMs){
  unsigned long startTimeMs = millis();
  unsigned long i = 0;
  while (millis()-startTimeMs<=delayMs){
    i += 1;
  };
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
    //Serial.println(F("=== RTC is not working! ==="));
    return;
  }
  //DateTime now = rtc.now();
  //Serial.print(F("RTC: "));
  //Serial.print(now.year(), DEC);
  //Serial.print(F("/"));
  //Serial.print(now.month(), DEC);
  //Serial.print(F("/"));
  //Serial.print(now.day(), DEC);
  //Serial.print(F(" ("));
  //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  //Serial.print(F(") "));
  //Serial.print(now.hour(), DEC);
  //Serial.print(F(":"));
  //Serial.print(now.minute(), DEC);
  //Serial.print(F(":"));
  //Serial.print(now.second(), DEC);
  //Serial.println();
}

// Initializes the RTC module and sets the rtcHwOperFlag to true if the hardware is operational.
void rtcInit(){
  //Kiểm tra tối đa 3 lần xem rtc phần cứng có chạy không? Nếu chạy thì bật cờ rtcHwOperFlag = true rồi thoát khỏi vòng lặp.
  for (int i = 0; i < 3; i++)
  {
    if (! rtc.begin()) {
      Serial.println(F("Couldn't find RTC"));
      Serial.flush();
      delayForLowPrioTask(100); //Delay 100ms
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
    Serial.println(F("=== RTC is not working! ==="));
    return;
  }
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
  //Serial.print(F("=== RTC is set: "));
  //Serial.print(year);
  //Serial.print(F("/"));
  //Serial.print(month);
  //Serial.print(F("/"));
  //Serial.print(day);
  //Serial.print(F(" "));
  //Serial.print(hour);
  //Serial.print(F(":"));
  //Serial.print(minute);
  //Serial.print(F(":"));
  //Serial.print(second);
  //Serial.println(F(" ==="));
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
  uint8_t  sampleNum = 20;
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

void findMaxCurrent(uint8_t chNum){
  uint16_t maxValue = 0;
  uint8_t  sampleNum = 20;
  double ImaxD=0;
  double IeffD;
  double Ieff;
  double Ioutput;

  //Find max value
  for (int i = 0; i < sampleNum; i++)
  {
    if (dev.ch[chNum].listCurrent[i] > maxValue)
    {
      maxValue = dev.ch[chNum].listCurrent[i];
    }
  }
  ImaxD=maxValue;
  IeffD=ImaxD/sqrt(2);
  Ieff=(((IeffD-420.76)/-90.24)*-210.2)+210.2;
  Ioutput = Ieff*222.0/70.0;
  if (Ioutput < 0) Ioutput = 0;
  dev.ch[chNum].Iout = int(Ioutput);
}

void pwmControlCheck () {
  static uint8_t pwmStateCh1 = LOW;
  static uint8_t pwmStateCh2 = LOW;
  static uint8_t pwmStateCh3 = LOW;
  static uint8_t pwmStateCh4 = LOW;

  if (dev.firstLoadMem == 0) return; //If memory is not loaded at the first time => return.

  //Channel 1
  if (dev.ch[1].i_switchMode == AUTO_MODE) {
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
  if (dev.ch[2].i_switchMode == AUTO_MODE) {
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
  if (dev.ch[3].i_switchMode == AUTO_MODE) {
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
  if (dev.ch[4].i_switchMode == AUTO_MODE) {
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

// /**
//  * Sends packet to LORA RF
//  * 
//  * @param command The command to be sent.
//  * @param payload The payload to be sent along with the command.
//  */
// void rfSend(String command, String payload){
//   String tData = command+","+UIDStr+","+nodeType+","+payload;
//   if (txQueue.isFull() == false) {
//     txQueue.enqueue(tData); //push data to txQueue if not full
//     //Print queue size
//     // Serial.print(F("Queue size: "));
//     // Serial.println(txQueue.itemCount());
//   }
// }


