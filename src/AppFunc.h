#pragma once
#define APPFUNC
#include <EEPROM.h>
#include <TimerOne.h>
#include "AppPriFunc.h"

#define EEPROM_INIT_CODE     0x5A0F //EEPROM init code. If EEPROM is not initialized, this code will be written to EEPROM.
#define EEPROM_ADDR_INIT     0 //EEPROM address to store init code

#define EEPROM_ADDR_SCHEDULE 100 //EEPROM address to store schedule
#define EEPROM_SCHEDULE_MAX  40  //EEPROM address to store schedule
#define EEPROM_SCHEDULE_SIZE 15  //EEPROM address to store schedule

uint16_t reqSyncSID[EEPROM_SCHEDULE_MAX] = {0}; //Request sync SID list to gateway
uint8_t  reqSyncSIDIndex = 0; //Request sync SID list index
uint16_t alreadySyncSID[EEPROM_SCHEDULE_MAX] = {0}; //Already sync SID list
uint8_t  alreadySyncSIDIndex = 0; //Already sync SID list index


extern void rfSendToGateway(String cmdResp="C2", String payload="");
extern String waitCommandWithString(String header="A1", String str ="", uint16_t timeOutSec = 30);
extern void updateSyncTime(String message);
extern void rfRespToGateway(String cmdResp="A1", String statusResp="OK");

struct schedule_t {
  uint16_t SID = 0; //Schedule ID
  uint8_t  SVer = 0; //Schedule version
  bool     enableFlag = false; //Schedule enable flag
  uint8_t  channel = 0; //Channel number from 1 to 4. 0 is not used.
  bool     repeatUntilFlag = false; //Repeat until flag
  uint8_t  repeatMode = 0; //Repeat mode. 0: No Repeat
                           //             1: Daily Repeat
                           //             2: Weakday Repeat
                           //             3: Monthly Repeat
  uint8_t  weakdays = 0; //No repeat.
                         //Sunday repeat:    0b00000001
                         //Monday repeat:    0b00000010
                         //Tuesday repeat:   0b00000100
                         //Wednesday repeat: 0b00001000
                         //Thursday repeat:  0b00010000
                         //Friday repeat:    0b00100000
                         //Saturday repeat:  0b01000000

  uint8_t fromDay = 0; //From day of month
  uint8_t fromMonth = 0; //From month
  uint8_t fromYear = 0; //From year. 0 mean 2000 year.
  uint16_t fromTimeMin = 0; //From time in minute in day. Value from 0 to 1440.
  uint16_t toTimeMin = 0; //To time in minute in day. Value from 0 to 1440.
  uint8_t untilDay = 0; //Until day of month
  uint8_t untilMonth = 0; //Until month
  uint8_t untilYear = 0; //Until year. 0 mean 2000 year.
};
schedule_t* scheduleList = new schedule_t[EEPROM_SCHEDULE_MAX];

void checkInitEEPROM(){
  Serial.println("=== 1. Checking first initialization of EEPROM... ===");
  uint16_t initCode = EEPROM.read(EEPROM_ADDR_INIT + 1) << 8 | EEPROM.read(EEPROM_ADDR_INIT);
  //Print initCode in HEX
  Serial.print("       => initCode: ");Serial.println(initCode, HEX);
  if (initCode != EEPROM_INIT_CODE) {
    Serial.println("       => EEPROM is not initialized!");
    Serial.println("       => Initializing EEPROM...");
    //1. Write init code to EEPROM
    EEPROM.write(EEPROM_ADDR_INIT+1, EEPROM_INIT_CODE >> 8);
    EEPROM.write(EEPROM_ADDR_INIT  , EEPROM_INIT_CODE & 0x00FF);
    initCode = EEPROM.read(EEPROM_ADDR_INIT + 1) << 8 | EEPROM.read(EEPROM_ADDR_INIT);
    Serial.print("       => initCode (After): ");Serial.println(initCode, HEX);
    //2. Write 0 to all schedules (EEPROM_SCHEDULE_MAX) in EEPROM from EEPROM_ADDR_SCHEDULE. Each schedule has size of EEPROM_SCHEDULE_SIZE.
    for (int i = 0; i < EEPROM_SCHEDULE_MAX; i++) {
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 0, 0); //SID
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 1, 0); //SID
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 2, 0); //SVer
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 3, 0); //enableFlag, weakdays
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4, 0); //channel, repeatUntilFlag, repeatMode
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 5, 0); //fromDate
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 6, 0); //fromDate
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 7, 0); //fromTimeMin
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 8, 0); //fromTimeMin
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 9, 0); //toTimeMin
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 10, 0); //toTimeMin
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 11, 0); //untilDate
      EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 12, 0); //untilDate
    }
    Serial.println("       => EEPROM is initialized!");
  }
}

void loadScheduleToRAM(){
  //1. Load all Enabled schedules from EEPROM to RAM
  Serial.println("=== 2. Loading schedules from EEPROM to RAM... ===");
  for (uint8_t i = 0; i < EEPROM_SCHEDULE_MAX; i++) {
    uint8_t enableFlag = EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4) >> 7; //Read enable flag at bit 7 of byte 4
    if (enableFlag == 1) {
      scheduleList[i].SID             =  EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 1) << 8 | EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 0);
      scheduleList[i].SVer            =  EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 2);
      scheduleList[i].enableFlag      =  enableFlag;
      scheduleList[i].channel         = (EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4) & 0b01110000) >> 4;
      scheduleList[i].repeatUntilFlag = (EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4) & 0b00001000) >> 3;
      scheduleList[i].repeatMode      =  EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4) & 0b00000111;
      scheduleList[i].weakdays        =  EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 3) & 0b01111111;

      uint16_t fromDate = EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 6) << 8 | EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 5);
      scheduleList[i].fromDay         =  fromDate & 0x001F; //Get 5 LSB bits
      scheduleList[i].fromMonth       = (fromDate & 0x01E0) >> 5; //Get 4 bits from bit 5 to bit 8
      scheduleList[i].fromYear        = (fromDate & 0xFE00) >> 9; //Get 7 bits from bit 9 to bit 15
      scheduleList[i].fromTimeMin     = (EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE +  8) << 8 | EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 7)) & 0x0FFF; //Get 12 LSB bits
      scheduleList[i].toTimeMin       = (EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 10) << 8 | EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 9)) & 0x0FFF; //Get 12 LSB bits
      uint16_t untilDate = EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 12) << 8 | EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 11);
      scheduleList[i].untilDay        =  untilDate & 0x001F; //Get 5 LSB bits
      scheduleList[i].untilMonth      = (untilDate & 0x01E0) >> 5; //Get 4 bits from bit 5 to bit 8
      scheduleList[i].untilYear       = (untilDate & 0xFE00) >> 9; //Get 7 bits from bit 9 to bit 15

      //Print out all schedules to Serial
      Serial.print("\n=== [");Serial.print(i);Serial.print("] Schedule at adress ");Serial.print(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE);Serial.println(" ===");
      Serial.print("SID: ");Serial.println(scheduleList[i].SID);
      Serial.print("SVer: ");Serial.println(scheduleList[i].SVer);
      Serial.print("enableFlag: ");Serial.println(scheduleList[i].enableFlag);
      Serial.print("channel: ");Serial.println(scheduleList[i].channel);
      Serial.print("repeatUntilFlag: ");Serial.println(scheduleList[i].repeatUntilFlag);
      Serial.print("repeatMode: ");Serial.println(scheduleList[i].repeatMode);
      Serial.print("weakdays: ");Serial.println(scheduleList[i].weakdays);
      Serial.print("fromDay: ");Serial.println(scheduleList[i].fromDay);
      Serial.print("fromMonth: ");Serial.println(scheduleList[i].fromMonth);
      Serial.print("fromYear: ");Serial.println(scheduleList[i].fromYear);
      Serial.print("fromTimeMin: ");Serial.println(scheduleList[i].fromTimeMin);
      Serial.print("toTimeMin: ");Serial.println(scheduleList[i].toTimeMin);
      Serial.print("untilDay: ");Serial.println(scheduleList[i].untilDay);
      Serial.print("untilMonth: ");Serial.println(scheduleList[i].untilMonth);
      Serial.print("untilYear: ");Serial.println(scheduleList[i].untilYear);
    }
  }
  Serial.println("       => Loading schedules from EEPROM to RAM done!");
}

/**
 * Checks if the current time matches the given schedule.
 * 
 * @param schedule The schedule to check against.
 * @return True if the current time matches the schedule, false otherwise.
 */
bool __checkTimeMatch(schedule_t schedule){
  uint16_t curTimeMin = dev.time.hour()*60 + dev.time.minute();
  if (schedule.fromTimeMin <= curTimeMin && curTimeMin <= schedule.toTimeMin){
    return true;
  }
  return false;
};

/**
 * @brief Checks if the given schedule matches the current date and time.
 * 
 * @param schedule The schedule to be checked.
 * @return True if the schedule matches the current date and time, false otherwise.
 */
bool __checkDateMatch(schedule_t schedule){
  //1. Check unti date
  if (schedule.repeatUntilFlag == true) {
    if (dev.time.year() > schedule.untilYear) {
      return false;
    } else if (dev.time.year() == schedule.untilYear) {
      if (dev.time.month() > schedule.untilMonth) {
        return false;
      } else if (dev.time.month() == schedule.untilMonth) {
        if (dev.time.day() > schedule.untilDay) {
          return false;
        }
      }
    }
  }
  //2. Check from Match repeat conditions
  switch (schedule.repeatMode) {
    case 0: //No repeat
      if (schedule.fromDay == dev.time.day() && schedule.fromMonth == dev.time.month() && schedule.fromYear == dev.time.year()) {
        return true;
      }
      break;
    case 1: //Daily repeat
      return true;
    case 2: //Weakday repeat
      if (schedule.weakdays & (1 << (dev.time.dayOfTheWeek()+1))) {
        return true;
      }
      break;
    case 3: //Monthly repeat
      if (schedule.fromDay == dev.time.day()) {
        return true;
      }
      break;
  }
  return false;
}


/**
 * Controls the channel in auto mode based on the schedule list.
 * 
 * @param checkedChannel The channel to be checked (default is 1).
 */
void __controlChannelInAutoModeByScheduleList(uint8_t checkedChannel = 1){
  if (dev.ch[checkedChannel].i_switchMode == AUTO_MODE) {
    //Nếu không có Remote Control bằng tay trên App thì mới kiểm tra schedule
    for (uint8_t i = 0; i < EEPROM_SCHEDULE_MAX; i++) {
      if (scheduleList[i].enableFlag == true && scheduleList[i].channel == checkedChannel) {
        if (__checkTimeMatch(scheduleList[i]) && __checkDateMatch(scheduleList[i])) {
          //Turn ON channel
          if (dev.ch[checkedChannel].inScheduleFlag == false){
            Serial.println("       => Channel "+String(checkedChannel)+" is opened");
            dev.ch[checkedChannel].autoControl = CONTROL_ON;
            dev.ch[checkedChannel].inScheduleFlag = true; //Set inScheduleFlag = true
            dev.ch[checkedChannel].remoteControlFlag = false; //Set remoteControlFlag = false
            break;
          }
        }
      }
    }
    //Nếu không có schedule nào thỏa mãn thì đóng kênh nếu trước đó nó có đang nằm trong một schedule nào đó.
    if (dev.ch[checkedChannel].inScheduleFlag == true) {
      Serial.println("       => Channel "+String(checkedChannel)+" is closed");
      dev.ch[checkedChannel].autoControl = CONTROL_OFF;
      dev.ch[checkedChannel].inScheduleFlag = false; //Set inScheduleFlag = false
      dev.ch[checkedChannel].remoteControlFlag = false; //Set remoteControlFlag = false
    }
  }
}

/**
 * Executes the first check and control in auto mode.
 * This function checks each channel and performs control operations accordingly.
 */
void firstControlInAutoMode(){
  Serial.println("=== 3. First check & control in auto mode... ===");
  //1. Check channel 1
  __controlChannelInAutoModeByScheduleList(1); //Check channel 1
  //2. Check channel 2
  __controlChannelInAutoModeByScheduleList(2); //Check channel 2
  //3. Check channel 3
  __controlChannelInAutoModeByScheduleList(3); //Check channel 3
  //4. Check channel 4
  __controlChannelInAutoModeByScheduleList(4); //Check channel 4
}

void __updateScheduleToEEPROM(schedule_t schedule, uint8_t scheduleIndex=0){
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  0, scheduleList[scheduleIndex].SID & 0x00FF); //SID
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  1, scheduleList[scheduleIndex].SID >> 8); //SID
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  2, scheduleList[scheduleIndex].SVer); //SVer
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  3, scheduleList[scheduleIndex].weakdays); //SVer
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  4, scheduleList[scheduleIndex].enableFlag << 7 | scheduleList[scheduleIndex].channel << 4 | scheduleList[scheduleIndex].repeatUntilFlag << 3 | scheduleList[scheduleIndex].repeatMode); //channel, repeatUntilFlag, repeatMode
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  5, scheduleList[scheduleIndex].fromDay & 0x00FF); //fromDate
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  6, scheduleList[scheduleIndex].fromDay >> 8); //fromDate
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  7, scheduleList[scheduleIndex].fromTimeMin & 0x00FF); //fromTimeMin
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  8, scheduleList[scheduleIndex].fromTimeMin >> 8); //fromTimeMin
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE +  9, scheduleList[scheduleIndex].toTimeMin & 0x00FF); //toTimeMin
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE + 10, scheduleList[scheduleIndex].toTimeMin >> 8); //toTimeMin
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE + 11, scheduleList[scheduleIndex].untilDay & 0x00FF); //untilDate
  EEPROM.update(EEPROM_ADDR_SCHEDULE + scheduleIndex*EEPROM_SCHEDULE_SIZE + 12, scheduleList[scheduleIndex].untilDay >> 8); //untilDate
}

void __removeScheduleNotInList(uint16_t alreadySyncSID[], uint8_t alreadySyncSIDIndex, uint16_t reqSyncSID[], uint8_t reqSyncSIDIndex){
  //A. Remove all schedule in scheduleList which does not exist in alreadySyncSID list and reqSyncSID list
  for (int i = 0; i < EEPROM_SCHEDULE_MAX; i++) {
    if (scheduleList[i].enableFlag && scheduleList[i].SID != 0) { //SID = 0 is not used
      bool foundFlag = false;
      for (int j = 0; j < alreadySyncSIDIndex; j++) {
        if (scheduleList[i].SID == alreadySyncSID[j]) {
          foundFlag = true;
          break;
        }
      }
      if (foundFlag == false) {
        for (int j = 0; j < reqSyncSIDIndex; j++) {
          if (scheduleList[i].SID == reqSyncSID[j]) {
            foundFlag = true;
            break;
          }
        }
      }
      if (foundFlag == false) {
        scheduleList[i].enableFlag = false; //Disable schedule
        //Clear enableFlag bit at bit 7 of byte 3 in EEPROM
        uint8_t byte4 = EEPROM.read(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4);
        byte4 = byte4 & 0b01111111;
        EEPROM.update(EEPROM_ADDR_SCHEDULE + i*EEPROM_SCHEDULE_SIZE + 4, byte4);
      }
    }
  }
}

void __extractRequestSyncAndAlreadySyncFromAB_SCHEDULE_SID_VER_Packet(String payload){
  int payloadLen = payload.length();
  int payloadIndex = 0;
  int SID = 0;
  int Sver = 0;
  int SIDIndex = 0;
  int SverIndex = 0;
  while (payloadIndex < payloadLen) {
    SIDIndex = payload.indexOf(',', payloadIndex);
    SID = payload.substring(payloadIndex, SIDIndex).toInt();
    SverIndex = payload.indexOf(',', SIDIndex + 1);
    Sver = payload.substring(SIDIndex + 1, SverIndex).toInt();
    //2. Check if SID is in scheduleList
    if (SID != 0){ //SID = 0 is not used
      for (int i = 0; i < EEPROM_SCHEDULE_MAX; i++) {
        if (scheduleList[i].enableFlag && scheduleList[i].SID == uint16_t(SID)) {
          //3. If SID is in scheduleList, check Sver
          if (scheduleList[i].SVer != Sver) {
            //4. If Sver is not match, add SID to reqSyncSID list
            reqSyncSID[reqSyncSIDIndex] = SID;
            reqSyncSIDIndex++;
          }
          else{
            //5. If Sver is match, add SID to alreadySyncSID list
            alreadySyncSID[alreadySyncSIDIndex] = SID;
            alreadySyncSIDIndex++;
          }
        }
        if (i == EEPROM_SCHEDULE_MAX - 1) { //If i = EEPROM_SCHEDULE_MAX - 1, it means that SID is not in scheduleList
          //5. If SID is not in scheduleList, add SID to reqSyncSID list
          reqSyncSID[reqSyncSIDIndex] = SID;
          reqSyncSIDIndex++;
        }
      }
    }
    payloadIndex = SverIndex + 1;
  }
}

void __request_C2_A1_packet(){
  //1. Send join request (C2) to gateway
  //2. Wait A1 response from gateway
  //3. If A1 response is received, update sync time
  //4. If A1 response is not received within 30 seconds, send join request (C2) to gateway again
  uint8_t retryCounter = 0; //Reset retryCounter
  receivedA1Flag = false; //Reset receivedA1Flag
  while (true) {
    //1. Send join request (C2) to gateway
    rfSendToGateway("C2",fwVer+",JOIN_REQ"); //Send join request to Gateway
    //2. Wait A1 response from gateway in 30s
    String A1Data = waitCommandWithString("A1","",30); //Header = A1, String = "", timeout = 30s
    //3. If A1 response is received, break loop
    if (A1Data != "") {
      updateSyncTime(A1Data); //Update sync time
      //4. Send A1 OK response to gateway
      rfRespToGateway("A1","OK"); //Send response to Gateway
      receivedA1Flag = true; //Set receivedA1Flag
      break;
    }
    else{
      //If retryCounter >= 3, break loop
      if (retryCounter >= 3) {
        break;
      }
      retryCounter++; //Increase retryCounter
    }
  }
}

//Kiểm tra trong scheduleList có SID nào không trùng với SID của gói Sync Schedule vừa nhận được không (syncScheduleData)
//Nếu có thì cập nhật lại dữ liệu cho schedule đó và ghi vào EEPROM
//Nếu không có thì tìm schedule đầu tiên có enableFlag = false và cập nhật lại dữ liệu cho schedule đó và ghi vào EEPROM
void __processSyncScheduleABPacket(String syncScheduleData){
  //Decode syncScheduleData and update to scheduleList which has SID = reqSyncSID[i]
  int commaIndex = syncScheduleData.indexOf(',');
  String header = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String command = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String SID = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String Sver = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String enableFlag = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String channel = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String repeatUntilFlag = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String repeatMode = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String weakdays = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String fromDate = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String fromTimeMin = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  commaIndex = syncScheduleData.indexOf(',');
  String toTimeMin = syncScheduleData.substring(0, commaIndex);
  syncScheduleData = syncScheduleData.substring(commaIndex + 1);
  String untilDate = syncScheduleData;

  // Update scheduleList with the extracted values
  // Find the schedule with SID = reqSyncSID[i] then update it
  for (int j = 0; j < EEPROM_SCHEDULE_MAX; j++) {
    if (scheduleList[j].SID == SID.toInt()) {
      //scheduleList[j].SID = SID.toInt();
      scheduleList[j].SVer = Sver.toInt();
      scheduleList[j].enableFlag = (enableFlag.toInt() == 1) ? true : false; //Convert enableFlag to bool
      scheduleList[j].channel = channel.toInt();
      scheduleList[j].repeatUntilFlag = (repeatUntilFlag.toInt() == 1) ? true : false; //Convert repeatUntilFlag to bool
      scheduleList[j].repeatMode = repeatMode.toInt();
      scheduleList[j].weakdays = weakdays.toInt();
      scheduleList[j].fromDay = fromDate.toInt() & 0x001F; //Get 5 LSB bits
      scheduleList[j].fromMonth = (fromDate.toInt() & 0x01E0) >> 5; //Get 4 bits from bit 5 to bit 8
      scheduleList[j].fromYear = (fromDate.toInt() & 0xFE00) >> 9; //Get 7 bits from bit 9 to bit 15
      scheduleList[j].fromTimeMin = fromTimeMin.toInt() & 0x0FFF; //Get 12 LSB bits
      scheduleList[j].toTimeMin = toTimeMin.toInt() & 0x0FFF; //Get 12 LSB bits
      scheduleList[j].untilDay = untilDate.toInt() & 0x001F; //Get 5 LSB bits
      scheduleList[j].untilMonth = (untilDate.toInt() & 0x01E0) >> 5; //Get 4 bits from bit 5 to bit 8
      scheduleList[j].untilYear = (untilDate.toInt() & 0xFE00) >> 9; //Get 7 bits from bit 9 to bit 15
      //Update all schedule data to EEPROM
      __updateScheduleToEEPROM(scheduleList[j], j);
      break;
    }
    //If j = EEPROM_SCHEDULE_MAX - 1, it means that SID is not in scheduleList
    //Then, find the first schedule which has enableFlag = false and update it
    if (j == EEPROM_SCHEDULE_MAX - 1) {
      for (int k = 0; k < EEPROM_SCHEDULE_MAX; k++) {
        if (scheduleList[k].enableFlag == false) {
          scheduleList[k].SID = SID.toInt();
          scheduleList[k].SVer = Sver.toInt();
          scheduleList[k].enableFlag = (enableFlag.toInt() == 1) ? true : false; //Convert enableFlag to bool
          scheduleList[k].channel = channel.toInt();
          scheduleList[k].repeatUntilFlag = (repeatUntilFlag.toInt() == 1) ? true : false; //Convert repeatUntilFlag to bool
          scheduleList[k].repeatMode = repeatMode.toInt();
          scheduleList[k].weakdays = weakdays.toInt();
          scheduleList[k].fromDay = fromDate.toInt() & 0x001F; //Get 5 LSB bits
          scheduleList[k].fromMonth = (fromDate.toInt() & 0x01E0) >> 5; //Get 4 bits from bit 5 to bit 8
          scheduleList[k].fromYear = (fromDate.toInt() & 0xFE00) >> 9; //Get 7 bits from bit 9 to bit 15
          scheduleList[k].fromTimeMin = fromTimeMin.toInt() & 0x0FFF; //Get 12 LSB bits
          scheduleList[k].toTimeMin = toTimeMin.toInt() & 0x0FFF; //Get 12 LSB bits
          scheduleList[k].untilDay = untilDate.toInt() & 0x001F; //Get 5 LSB bits
          scheduleList[k].untilMonth = (untilDate.toInt() & 0x01E0) >> 5; //Get 4 bits from bit 5 to bit 8
          scheduleList[k].untilYear = (untilDate.toInt() & 0xFE00) >> 9; //Get 7 bits from bit 9 to bit 15

          //Update all schedule data to EEPROM
          __updateScheduleToEEPROM(scheduleList[k], k);
          break;
        }
      }
    }
  }
}


//Controls a channel based on its mode (manual or auto) and returns the status of the channel.
//If the channel is in MANUAL_MODE and the contactor status is CONTROL_OFF, the function sets the status to "MANUAL_OFF".
//If the contactor status is CONTROL_ON, the status is set to "MANUAL_ON".
//If the channel is in AUTO_MODE, the function calls __controlChannelInAutoModeByScheduleList(checkedChannel) to control the channel based on a schedule list.
// + If the auto control status is CONTROL_OFF, the status is set to "AUTO_OFF".
// + If the auto control status is CONTROL_ON, the status is set to "AUTO_ON".
String __controlInAutoScheduleAndGetChannelStatus(uint8_t checkedChannel = 1){
  String channelControlStatus = "";
  if (dev.ch[checkedChannel].i_switchMode == MANUAL_MODE && dev.ch[checkedChannel].i_contactorStatus == CONTROL_OFF) {
    channelControlStatus = "MANUAL_OFF";
  }
  else if (dev.ch[checkedChannel].i_switchMode == MANUAL_MODE && dev.ch[checkedChannel].i_contactorStatus == CONTROL_ON) {
    channelControlStatus = "MANUAL_ON";
  }
  if (dev.ch[checkedChannel].i_switchMode == AUTO_MODE) {
    __controlChannelInAutoModeByScheduleList(checkedChannel); //Check channel 1
    if (dev.ch[checkedChannel].autoControl == CONTROL_OFF) {
      channelControlStatus = "AUTO_OFF";
    }
    else if (dev.ch[checkedChannel].autoControl == CONTROL_ON) {
      channelControlStatus = "AUTO_ON";
    }
  }
  return channelControlStatus;
}

int __convertStringControlStatusToInt(String channelControlStatus){
  if (channelControlStatus == "MANUAL_OFF") return 1;
  if (channelControlStatus == "MANUAL_ON") return 2;
  if (channelControlStatus == "AUTO_OFF") return 3;
  if (channelControlStatus == "AUTO_ON") return 4;
  return 0;
}

void __checkChannelSwitchAndSchedule(){
  String controlStatus = "";
  uint8_t channel1ControlStatus, channel2ControlStatus, channel3ControlStatus, channel4ControlStatus;

  controlStatus = __controlInAutoScheduleAndGetChannelStatus(1);
  channel1ControlStatus = __convertStringControlStatusToInt(controlStatus);
  controlStatus = __controlInAutoScheduleAndGetChannelStatus(2);
  channel2ControlStatus = __convertStringControlStatusToInt(controlStatus);
  controlStatus = __controlInAutoScheduleAndGetChannelStatus(3);
  channel3ControlStatus = __convertStringControlStatusToInt(controlStatus);
  controlStatus = __controlInAutoScheduleAndGetChannelStatus(4);
  channel4ControlStatus = __convertStringControlStatusToInt(controlStatus);

  //Compare channel1ControlStatus with ch.lastControlStatus[1]. If they are different, update ch.lastControlStatus[1] and send to gateway
  if (channel1ControlStatus != dev.ch[1].lastControlStatus) {
    dev.ch[1].lastControlStatus = channel1ControlStatus;
    rfSendToGateway("AC","UPDATE_CHANNEL_STATUS,CH1,"+channel1ControlStatus); //Send channel 1 status to Gateway
  }
  //Compare channel2ControlStatus with ch.lastControlStatus[2]. If they are different, update ch.lastControlStatus[2] and send to gateway
  if (channel2ControlStatus != dev.ch[2].lastControlStatus) {
    dev.ch[2].lastControlStatus = channel2ControlStatus;
    rfSendToGateway("AC","UPDATE_CHANNEL_STATUS,CH2,"+channel2ControlStatus); //Send channel 2 status to Gateway
  }
  //Compare channel3ControlStatus with ch.lastControlStatus[3]. If they are different, update ch.lastControlStatus[3] and send to gateway
  if (channel3ControlStatus != dev.ch[3].lastControlStatus) {
    dev.ch[3].lastControlStatus = channel3ControlStatus;
    rfSendToGateway("AC","UPDATE_CHANNEL_STATUS,CH3,"+channel3ControlStatus); //Send channel 3 status to Gateway
  }
  //Compare channel4ControlStatus with ch.lastControlStatus[4]. If they are different, update ch.lastControlStatus[4] and send to gateway
  if (channel4ControlStatus != dev.ch[4].lastControlStatus) {
    dev.ch[4].lastControlStatus = channel4ControlStatus;
    rfSendToGateway("AC","UPDATE_CHANNEL_STATUS,CH4,"+channel4ControlStatus); //Send channel 4 status to Gateway
  }
}

//Send internal status to gateway: temperature, humidity, rfConnStatus, Vout, Iout of all channels
void __sendInternalStatus(){
  //Send temperature, humidity to gateway
  rfSendToGateway("C0","01,"+String(dev.boxTemp)+","+String(dev.boxHumi));
  //Send rfConnStatus to gateway
  rfSendToGateway("C0","08,"+String(dev.rfConnStatus));
  //Send internal status to gateway of all channels from CH1 to CH4: CH1's Vout, CH1's Iout,....
  rfSendToGateway("C0","10,"+String(dev.ch[1].Vout)+","+String(dev.ch[1].Iout)+String(dev.ch[2].Vout)+","+String(dev.ch[2].Iout)+String(dev.ch[3].Vout)+","+String(dev.ch[3].Iout)+String(dev.ch[4].Vout)+","+String(dev.ch[4].Iout));
}


void __sendWdtToSlave(){
  rfSend("10","TASK1_OK");
}


void __SyncScheduleGatewayAndNode(){
  // Chờ gói mới được gửi đến trong 120s.
  // Nếu gói là AB thì kiểm tra in AB.
  // Nếu gói là B0 thì kiểm tra in B0.
  // Nếu hết thời gian mà không có gói đến thì thoát khỏi quá trình JOIN NETWORK (Sẽ thực hiện sau)
  uint16_t waitTimeSec = 120; //Wait time in second
  unsigned long waitTimeMark = millis(); //Wait time mark of 120s
  reqSyncSID[EEPROM_SCHEDULE_MAX] = {0}; //Request sync SID list to gateway
  reqSyncSIDIndex = 0; //Request sync SID list index
  alreadySyncSID[EEPROM_SCHEDULE_MAX] = {0}; //Already sync SID list
  alreadySyncSIDIndex = 0; //Already sync SID list index
  bool scheduleSendFinishFlag = false; //Flag to indicate that schedule send finish or not (SCHEDULE_SEND_FINISH command)
  while (millis() - waitTimeMark <= waitTimeSec*1000){
    //Check Rx data
    if (rxQueue.isEmpty() == false) {
      String rData = rxQueue.dequeue();
      int firstComma = rData.indexOf(',');
      int secondComma = rData.indexOf(',', firstComma + 1);
      int thirdComma = rData.indexOf(',', secondComma + 1);
      String header = rData.substring(0, firstComma);
      String gatewayID = rData.substring(firstComma + 1, secondComma);
      String command = rData.substring(secondComma + 1, thirdComma);
      String payload = rData.substring(thirdComma + 1);
      // Serial.println("header: " + header);
      // Serial.println("Gateway ID: " + gatewayID);
      // Serial.println("Payload: " + payload);

      //=====================================================
      // A. GATEWAY ACTING: SEND SCHEDULE SID VER          //
      //=====================================================
      // A1. Gateway send list of Schedule ID and Schedule version to node
      // Ex: AB,<GW_UID>,SCHEDULE_SID_VER,<SID_1>,<Sver_1>,<SID_2>,<Sver_2>,….
      if (header == "AB" && command=="SCHEDULE_SID_VER") {
        waitTimeMark = millis(); //Reset waitTimeMark of 120s
        //a. Extract request sync SID and already sync SID from payload and store to reqSyncSID and alreadySyncSID
        //payload = <SID_1:uint16_t>,<Sver_1:uint8_t>,<SID_2:uint16_t>,<Sver_2:uint8_t>,….
        __extractRequestSyncAndAlreadySyncFromAB_SCHEDULE_SID_VER_Packet(payload);
        //b. Send AB OK response to gateway
        rfRespToGateway("AB","SCHEDULE_SID_VER,OK"); //Send response to Gateway
      }
      //==========================================================
      // A2. Gateway inform node that it has sent all schedule to node
      if (header == "AB" && command=="SCHEDULE_SEND_FINISH") {
        waitTimeMark = millis(); //Reset waitTimeMark of 120s
        //a. Set flag: scheduleSendFinishFlag = true
        scheduleSendFinishFlag = true; //Set scheduleSendFinishFlag
        //b. Send AB OK response to gateway
        rfRespToGateway("AB","SCHEDULE_SEND_FINISH,OK"); //Send response to Gateway
      }
      //==========================================================
      // A3. Gateway finish sending package to node. B0 is the last package.
      if (header == "B0") {
        waitTimeMark = millis(); //Reset waitTimeMark of 120s
        //I. If scheduleSendFinishFlag == false, exit join network flow
        if (scheduleSendFinishFlag == false) {
          Serial.println("       => Node has not received schedule SID and Sver from gateway!");
          Serial.println("       => Exit join network flow!");
          return;
        }
        rfRespToGateway("B0","OK"); //Send response to Gateway

        //=============================================================
        // B. NODE ACTING: CHECKS SCHEDULE AND REQ_SCHEDULE TO SYNC  //
        //=============================================================
        // B1.Remove all schedule in scheduleList which does not exist in alreadySyncSID list and reqSyncSID list
        __removeScheduleNotInList(alreadySyncSID, alreadySyncSIDIndex, reqSyncSID, reqSyncSIDIndex);

        // B2. Send request sync schedule to gateway for each SID in reqSyncSID list
        for (int i = 0; i < EEPROM_SCHEDULE_MAX; i++) {
          if (reqSyncSID[i] != 0) {
            //B2.1 Send AB "REQ_SCHEDULE" request to gateway
            rfSendToGateway("AB","REQ_SCHEDULE,"+String(reqSyncSID[i])); //Send AB,REQ_SCHEDULE,<SID> to Gateway
            //B2.2 Wait AB response from gateway
            String syncScheduleData = waitCommandWithString("AB","SEND_SCHEDULE",30); //Receive: AB,SEND_SCHEDULE,<SID>,<Sver>,<enableFlag>,<channel>,<repeatUntilFlag>,<repeatMode>,<weakdays>,<fromDate>,<fromTimeMin>,<toTimeMin>,<untilDate>
            //B2.3 if syncScheduleData is not empty, update scheduleList
            if (syncScheduleData != "") {
              __processSyncScheduleABPacket(syncScheduleData); //Process sync schedule data
              rfRespToGateway("AB","SEND_SCHEDULE,OK"); //Send response to Gateway
            } else {
              break;
            }
          }
          break;
        }
        // B3. Set joinReqDoneFlag = true
        joinReqDoneFlag = true;
      }
    }
  }
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

class workingFlowClass {
  public:
    void setupFlow(){
      //Load 100 schedules from EEPROM to RAM
      checkInitEEPROM(); //Check if EEPROM is initialized. If not, initialize it.
      loadScheduleToRAM(); //Load all schedules from EEPROM to RAM
      firstControlInAutoMode(); //Execute the first check and control in auto mode.
    }

    void joinNetworkFlow(){
      if (setupDoneFlag == true){
        __request_C2_A1_packet(); //Request C2 A1 packet
        //Nếu không nhận được A1 sau 3 lần retry thì chuyển tạm thời thoát khởi quá trình JOIN NETWORK
        if (receivedA1Flag == false) {
          Serial.println("       => Cannot join network!");
          Serial.println("       => Exit join network flow!");
          return;
        }
        //Nếu nhận được A1 thì tiếp tục quá trình JOIN NETWORK bằng việc chờ gói tin đồng bộ schedule cho đến khi nhận được gói B0.

        //=====================================================================================
        __SyncScheduleGatewayAndNode(); //Sync schedule between gateway and node
      }
    };


    unsigned long updateStatusTimeMarkMs = millis(); //Update status time mark. Send status to gateway every SEND_STATUS_PERIOD_MS
    unsigned long syncScheduleTimeMakMs = millis(); //Sync schedule time mark. Send sync schedule request to gateway every SYNC_SCHEDULE_PERIOD_MS
    void normalWorkingFlow(){
      while (true) {
        //1. Check with SEND_STATUS_PERIOD_MS
        if (millis() - updateStatusTimeMarkMs >= SEND_STATUS_PERIOD_MS){
          updateStatusTimeMarkMs = millis(); //Reset updateStatusTimeMarkMs
          if (joinReqDoneFlag == true){
            __checkChannelSwitchAndSchedule(); //Check channel's switch status. Control channel as scheduleList if switch is AUTO_MODE.
            __sendInternalStatus(); //Send internal status to gateway: temperature, humidity, rfConnStatus, Vout, Iout of all channels
            __sendWdtToSlave(); //Send WDT to slave
          }
          else{
            break; //Exit normal working flow -> join network flow again.
          }
        }
        //2. Check with SYNC_SCHEDULE_PERIOD_MS
        //Node send request sync schedule to gateway ưith command REQ_SYNC_SCHEDULE_SID_SVER
        if (millis() - syncScheduleTimeMakMs >= SYNC_SCHEDULE_PERIOD_MS){
          syncScheduleTimeMakMs = millis(); //Reset syncScheduleTimeMakMs
          //Send AB "REQ_SCHEDULE_SID_VER" request to gateway
          rfSendToGateway("AB","REQ_SCHEDULE_SID_VER"); //Send AB,REQ_SCHEDULE_SID_VER to Gateway
          //Check Sync Schedule Gateway And Node
          __SyncScheduleGatewayAndNode(); //Sync schedule between gateway and node
        }
        //3. Nhận gói Schedule Configure từ gateway
        if (rxQueue.isEmpty() == false) {
          String rData = rxQueue.dequeue();
          int firstComma = rData.indexOf(',');
          int secondComma = rData.indexOf(',', firstComma + 1);
          int thirdComma = rData.indexOf(',', secondComma + 1);
          int fourthComma = rData.indexOf(',', thirdComma + 1);
          String header = rData.substring(0, firstComma);
          String gatewayID = rData.substring(firstComma + 1, secondComma);
          String command = rData.substring(secondComma + 1, thirdComma);
          String payload = rData.substring(thirdComma + 1, fourthComma);
          // Serial.println("header: " + header);
          // Serial.println("Gateway ID: " + gatewayID);
          // Serial.println("Payload: " + payload);

          //=====================================================
          //3.1 Get SEND_SCHEDULE command from gateway
          if (header == "AB" && command=="SEND_SCHEDULE") {
            __processSyncScheduleABPacket(rData);
            rfRespToGateway("AB","SEND_SCHEDULE,OK"); //Send response to Gateway
          }
          //==========================================================
          //3.2 Get SEND_SCHEDULE command from gateway
          if (header == "AC" && command=="REMOTE_CONTROL") {
            //Payload format: <channel: CH1/CH2/CH3/CH4>,<control: AUTO_ON/AUTO_OFF>
            int commaIndex = payload.indexOf(',');
            String channel = payload.substring(0, commaIndex);
            String control = payload.substring(commaIndex + 1);
            if (channel == "CH1" && dev.ch[1].i_switchMode == AUTO_MODE) {
              if (control == "AUTO_ON") {
                dev.ch[1].remoteControlFlag = true;
                dev.ch[1].autoControl = CONTROL_ON;
              }
              else if (control == "AUTO_OFF") {
                dev.ch[1].remoteControlFlag = true;
                dev.ch[1].autoControl = CONTROL_OFF;
              }
            }
            else if (channel == "CH2" && dev.ch[2].i_switchMode == AUTO_MODE) {
              if (control == "AUTO_ON") {
                dev.ch[2].remoteControlFlag = true;
                dev.ch[2].autoControl = CONTROL_ON;
              }
              else if (control == "AUTO_OFF") {
                dev.ch[2].remoteControlFlag = true;
                dev.ch[2].autoControl = CONTROL_OFF;
              }
            }
            else if (channel == "CH3" && dev.ch[3].i_switchMode == AUTO_MODE) {
              if (control == "AUTO_ON") {
                dev.ch[3].remoteControlFlag = true;
                dev.ch[3].autoControl = CONTROL_ON;
              }
              else if (control == "AUTO_OFF") {
                dev.ch[3].remoteControlFlag = true;
                dev.ch[3].autoControl = CONTROL_OFF;
              }
            }
            else if (channel == "CH4" && dev.ch[4].i_switchMode == AUTO_MODE) {
              if (control == "AUTO_ON") {
                dev.ch[4].remoteControlFlag = true;
                dev.ch[4].autoControl = CONTROL_ON;
              }
              else if (control == "AUTO_OFF") {
                dev.ch[4].remoteControlFlag = true;
                dev.ch[4].autoControl = CONTROL_OFF;
              }
            }
            //Delay to update status at channel
            delay(1000);
            rfRespToGateway("AC","REMOTE_CONTROL,OK"); //Send response to Gateway
            __checkChannelSwitchAndSchedule(); //Node update status of channel to gateway
          }
        }
      }
    }

    

    void mainFlow(){
      joinNetworkFlow();
      __checkChannelSwitchAndSchedule(); //Check channel's switch status. Control channel as scheduleList if switch is AUTO_MODE.
      __sendInternalStatus(); //Send internal status to gateway
      __sendWdtToSlave(); //Send WDT to slave
      normalWorkingFlow(); //Loop forever if joinReqDoneFlag = true
    }




    void wpsFlow();
};
//===========================================================================================================//


