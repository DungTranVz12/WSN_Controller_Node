#ifndef APPFUNC
#include "appfunc.h"
#endif

extern void delayForLowPrioTask(uint32_t delayMs);
unsigned long rfLastTimeMark = 0; //Last time mark for RF communication
/**
 * Sends data to the gateway.
 * 
 * @param cmdResp The command response string. Default value is "C2".
 * @param payload The payload string. Default value is an empty string.
 */
void rfSendToGateway(String cmdResp, String payload){
  String tData = cmdResp+","+UIDStr+","+nodeType+","+payload;
  uint16_t counter = 0;
  while (counter < 10000) { //Try to send data for 10s
    if (digitalRead(I_Slave_RDY) == HIGH && (millis() - rfLastTimeMark) >= 100){ //Slave ready to send data
      Serial.println(tData);
      rfLastTimeMark = millis();
      break;
    }
    delayForLowPrioTask(1); //Delay 1ms
    counter += 1; //Increase counter
  }
}

/**
 * Sends a response to the gateway.
 * 
 * @param cmdResp The command response string. Default is "A1".
 * @param statusResp The status response string: OK/FAIL. Default is "OK".
 */
void rfRespToGateway(String cmdResp, String statusResp){
  String tData = "C4,"+UIDStr+","+nodeType+",CMD_RESPONSE,"+cmdResp+","+statusResp;
  uint16_t counter = 0;
  while (counter < 10000) { //Try to send data for 10s
    if (digitalRead(I_Slave_RDY) == HIGH && millis() - rfLastTimeMark >= 100){ //Slave ready to send data
      Serial.println(tData);
      rfLastTimeMark = millis();
      break;
    }
    delayForLowPrioTask(1); //Delay 1ms
    counter += 1; //Increase counter
  }
}

/**
 * @brief Checks for incoming data from the serial port and adds it to the receive buffer.
 * 
 * This function reads data from the serial port until it encounters the '`' character. 
 * The received data is then trimmed to remove leading and trailing whitespaces. 
 * If the receive buffer is not full, the data is added to the buffer using the enqueue() function.
 * 
 * @note This function assumes that the Serial object has been initialized and is available for reading.
 */

void rfRxCheck()
{
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '`'){
      //push data to rxBuff
      rfRxData.trim(); // Remove leading/trailing whitespaces
      if (rfRxData.length() > 0) {
        if (rxQueue.isFull() == false) {
          rxQueue.enqueue(rfRxData); //push data to rxBuff if not full
        }
        //Update RF status
        dev.rfConnStatus = 1; //0: Not connected, 1: Connected
        dev.rfStatusCounter = 0; //Reset RF status counter
        //TAM_BO digitalWrite(O_LED_RF, LOW); //Turn on LED RF               <--------------------- DEBUG
      }
      rfRxData = ""; //Reset rfRxData
      break;
    }
    rfRxData += String(ch);
    // delayForLowPrioTask(1); //Delay 1ms
  }
}

// String waitCommandWithString(String header, String str, uint16_t timeOutSec){
//   uint16_t timeOutCounter = 0;
//   while (1) {
//     rfRxCheck(); //Check and retrieve data from LoRa then push to rxQueue
//     if (rxQueue.isEmpty() == false) {
//       String rData = rxQueue.dequeue();
//       //Decode rData:
//       //+ First data before ',' is the header
//       //+ Second data after ',' is the Gateway ID
//       //+ Ramaining data is the payload
//       int firstComma = rData.indexOf(',');
//       int secondComma = rData.indexOf(',', firstComma + 1);
//       String header = rData.substring(0, firstComma);
//       String gatewayID = rData.substring(firstComma + 1, secondComma);
//       String payload = rData.substring(secondComma + 1);
//       // Serial.println("header: " + header);
//       // Serial.println("Gateway ID: " + gatewayID);
//       // Serial.println("Payload: " + payload);
//       //Check if header = header and str in payload
//       if (header == header && payload.indexOf(str) >= 0) {
//         return rData;
//       }
//     }
//     timeOutCounter++;
//     if (timeOutCounter >= timeOutSec*10) {
//       return "";
//     }
//     delay(100);
//   }
// }

String waitCommandWithString(String header, String str, uint16_t timeOutSec) {
  unsigned long timeMark = millis();
  while (1) {
    rfRxCheck(); //Check and retrieve data from LoRa then push to rxQueue
    if (rxQueue.isEmpty() == false) {
      String rData = rxQueue.dequeue();
      // rfSendToGateway("E4", rData); //Send data to gateway (for debug) <--------------------- DEBUG
      //Decode rData:
      int firstComma = rData.indexOf(',');
      int secondComma = rData.indexOf(',', firstComma + 1);
      String receivedHeader = rData.substring(0, firstComma);
      String payload = rData.substring(secondComma + 1);
      //Check if header = header and str in payload
      if (receivedHeader == header && payload.indexOf(str) >= 0) {
        return rData;
      }
    }
    if (millis() - timeMark >= timeOutSec*1000 ) {
      return "";
    }
  }
}







/**
 * @brief Updates the synchronization time based on the message string.
 * 
 * This function splits the message string into an array of string elements separated by ','.
 * It then converts each element to an integer and uses it to adjust the real-time clock (RTC).
 * Finally, it updates the device time with the current RTC time.
 * 
 * @param message The message string containing the synchronization time elements.
 */
void updateSyncTime(String message){
  //Split message string into array of string elements separated by ','
  //Ex: A1,320DED,04,51,54,28,12,2023
  String element[8];
  int i = 0;
  int prevIndex = 0;
  int nextIndex = 0;
  while (nextIndex >= 0) {
    nextIndex = message.indexOf(',', prevIndex);
    element[i] = message.substring(prevIndex, nextIndex);
    prevIndex = nextIndex + 1;
    i++;
  }
  //Adjust RTC
  rtc.adjust(DateTime(element[7].toInt(), element[6].toInt(), element[5].toInt(), element[4].toInt(), element[3].toInt(), element[2].toInt()));
  dev.time = rtc.now();
}

/**
 * Sets the terminal status in auto mode based on the payload string.
 * The payload string is split into an array of string elements separated by ','.
 * The function checks the element values and sets the auto control status for the corresponding channel.
 * 
 * @param payload The payload string containing the terminal status information.
 */
void remoteTerminalStatusInAutoMode(String payload){
  //1. Split payload string into array of string elements separated by ','
  String element[3];
  int i = 0;
  int prevIndex = 0;
  int nextIndex = 0;
  while (nextIndex >= 0) {
    nextIndex = payload.indexOf(',', prevIndex);
    element[i] = payload.substring(prevIndex, nextIndex);
    prevIndex = nextIndex + 1;
    i++;
  }
  //2. Set terminal status
  //2.a For channel 0
  if (element[0] == "SET_TERMINAL" && element[1] == "CH0" && dev.ch[0].i_switchMode == AUTO_MODE) {
    if (element[2] == "ON")  dev.ch[0].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
    if (element[2] == "OFF") dev.ch[0].autoControl = CONTROL_OFF; //Auto control: CONTROL_OFF
  }
  //2.b For channel 1
  else if (element[0] == "SET_TERMINAL" && element[1] == "CH1" && dev.ch[1].i_switchMode == AUTO_MODE) {
    if (element[2] == "ON")  dev.ch[1].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
    if (element[2] == "OFF") dev.ch[1].autoControl = CONTROL_OFF; //Auto control: CONTROL_OFF
  }
  //2.c For channel 2
  else if (element[0] == "SET_TERMINAL" && element[1] == "CH2" && dev.ch[2].i_switchMode == AUTO_MODE) {
    if (element[2] == "ON")  dev.ch[2].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
    if (element[2] == "OFF") dev.ch[2].autoControl = CONTROL_OFF; //Auto control: CONTROL_OFF
  }
  //2.d For channel 3
  else if (element[0] == "SET_TERMINAL" && element[1] == "CH3" && dev.ch[3].i_switchMode == AUTO_MODE) {
    if (element[2] == "ON")  dev.ch[3].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
    if (element[2] == "OFF") dev.ch[3].autoControl = CONTROL_OFF; //Auto control: CONTROL_OFF
  }
}



//=============================================================================================
//=============================================================================================
// void task4 (){
//   //1. RX check and process

//   if (digitalRead(I_Slave_RDY) == HIGH){ //Slave ready to send data
//     digitalWrite(O_LED_4G, HIGH); //Turn off LED RF
//   }
//   else{
//     digitalWrite(O_LED_4G, LOW); //Turn on LED RF
//   }
// }