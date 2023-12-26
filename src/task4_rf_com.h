#ifndef APPFUNC
#include "appfunc.h"
#endif

/**
 * Sends a log message over RF communication.
 * 
 * @param logStr The log message to be sent.
 */
void rfSendLog(String logStr){
  String tData = "E4,"+UIDstr+","+nodeType+","+logStr;
  if (txQueue.isFull() == false) {
    txQueue.enqueue(tData); //push data to txQueue if not full
  }
}

/**
 * Sends data to the gateway.
 * 
 * @param cmdResp The command response string. Default value is "C2".
 * @param payload The payload string. Default value is an empty string.
 */
void rfSendToGateway(String cmdResp, String payload){
  String tData = cmdResp+","+UIDstr+","+nodeType+","+payload;
  if (txQueue.isFull() == false) {
    txQueue.enqueue(tData); //push data to txQueue if not full
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
  String rData = ""; // Data received from serial
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '`')
      break;
    rData += String(ch);
    delay(1);
  }
  rData.trim(); // Remove leading/trailing whitespaces
  //push data to rxBuff
  if (rData.length() > 0) {
    if (rxQueue.isFull() == false) {
      rxQueue.enqueue(rData); //push data to rxBuff if not full
    }
    //Update RF status
    dev.rfConnStatus = 1; //0: Not connected, 1: Connected
    dev.rfStatusCounter = 0; //Reset RF status counter
    digitalWrite(O_LED_RF, LOW); //Turn on LED RF
  }
}

String waitCommandWithString(String header, String str, uint16_t timeOutSec){
  uint16_t timeOutCounter = 0;
  while (1) {
    rfRxCheck(); //Check and retrieve data from LoRa then push to rxQueue
    if (rxQueue.isEmpty() == false) {
      String rData = rxQueue.dequeue();
      //Decode rData:
      //+ First data before ',' is the header
      //+ Second data after ',' is the Gateway ID
      //+ Ramaining data is the payload
      int firstComma = rData.indexOf(',');
      int secondComma = rData.indexOf(',', firstComma + 1);
      String header = rData.substring(0, firstComma);
      String gatewayID = rData.substring(firstComma + 1, secondComma);
      String payload = rData.substring(secondComma + 1);
      // Serial.println("header: " + header);
      // Serial.println("Gateway ID: " + gatewayID);
      // Serial.println("Payload: " + payload);
      //Check if header = header and str in payload
      if (header == header && payload.indexOf(str) >= 0) {
        return rData;
      }
    }
    timeOutCounter++;
    if (timeOutCounter >= timeOutSec*10) {
      return "";
    }
    delay(100);
  }



}



/**
 * Sends a response to the gateway.
 * 
 * @param cmdResp The command response string. Default is "A1".
 * @param statusResp The status response string: OK/FAIL. Default is "OK".
 */
void rfRespToGateway(String cmdResp, String statusResp){
  String tData = "C4,"+UIDstr+","+nodeType+",CMD_RESPONSE,"+cmdResp+","+statusResp;
  if (txQueue.isFull() == false) {
    txQueue.enqueue(tData); //push data to txQueue if not full
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
  uint8_t element[6];
  int i = 0;
  int prevIndex = 0;
  int nextIndex = 0;
  while (nextIndex >= 0) {
    nextIndex = message.indexOf(',', prevIndex);
    element[i] = message.substring(prevIndex, nextIndex).toInt();
    prevIndex = nextIndex + 1;
    i++;
  }
  //Adjust RTC
  rtc.adjust(DateTime(element[7], element[6], element[5], element[2], element[3], element[4]));
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
void task4 (){
  // //0. Check and send join request at the first time
  // // This code checks if a join request has been sent and if enough time has passed since the last join request. 
  // // If the conditions are met, it sends a join request to the Gateway and resets the lastJoinRequestTime.
  // if (joinRequestSent == false && millis() - lastJoinRequestTime >= 60000) {
  //   rfSendToGateway("C2",fwVer+",JOIN_REQ"); //Send join request to Gateway
  //   lastJoinRequestTime = millis(); //Reset lastJoinRequestTime
  // }
  // //1. RX check and process
  // rfRxCheck(); //Check and retrieve data from LoRa then push to rxQueue
  // if (rxQueue.isEmpty() == false) {
  //   String rData = rxQueue.dequeue();
  //   //Decode rData:
  //   //+ First data before ',' is the command
  //   //+ Second data after ',' is the Gateway ID
  //   //+ Ramaining data is the payload
  //   int firstComma = rData.indexOf(',');
  //   int secondComma = rData.indexOf(',', firstComma + 1);
  //   String command = rData.substring(0, firstComma);
  //   String gatewayID = rData.substring(firstComma + 1, secondComma);
  //   String payload = rData.substring(secondComma + 1);
  //   // Serial.println("Command: " + command);
  //   // Serial.println("Gateway ID: " + gatewayID);
  //   // Serial.println("Payload: " + payload);

  //   //###########################//
  //   //Process command            //
  //   //###########################//
  //   //Packet A1: Synchronization time
  //   //Rx Data:   A1,<GW_UID>,<hh>,<mm>,<ss>,<dd>,<mm>,<yyyy>
  //   if (command == "A1") {
  //     joinRequestSent = true; //Set joinRequestSent flag
  //     updateSyncTime(rData); //Update sync time
  //     rfRespToGateway("A1","OK"); //Send response to Gateway
  //   }
    
  //   //Packet AC: Set terminal status
  //   //Rx Data:   AC,<GW_UID>,"SET_TERMINAL",<CH0-3>,<ON/OFF>
  //   if (command == "AC") {
  //     remoteTerminalStatusInAutoMode(payload); //Remote terminal status in AUTO mode
  //     rfRespToGateway("AC","OK"); //Send response to Gateway
  //   }






  //   //Packet B0: Finshed setting up
  //   if (command == "B0") {
  //     rfRespToGateway("B0","OK"); //Send response to Gateway
  //   }


  // }

  //1. RX check and process
  rfRxCheck(); //Check and retrieve data from LoRa then push to rxQueue

  //2. TX check and process
  if (txQueue.isEmpty() == false && digitalRead(I_Slave_RDY) == HIGH) {
    String tData = txQueue.dequeue();
    // Serial.println("TX data: " + tData);
    Serial.println(tData);
    delay(100);
  }

}