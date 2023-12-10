#ifndef APPFUNC
#include "appfunc.h"
#endif





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
      Serial.println("Queue data: " + rData);
      rxQueue.enqueue(rData); //push data to rxBuff if not full
      //Print queue size
      // Serial.print("Queue size: ");
      Serial.println(rxQueue.itemCount());
    }
  }
}

void task4 (){
  //1. RX check and process
  rfRxCheck(); //Check and retrieve data from LoRa then push to rxQueue
  if (rxQueue.isEmpty() == false) {
    String rData = rxQueue.dequeue();
    //Decode rData:
    //+ First data before ',' is the command
    //+ Second data after ',' is the Gateway ID
    //+ Ramaining data is the payload
    int firstComma = rData.indexOf(',');
    int secondComma = rData.indexOf(',', firstComma + 1);
    String command = rData.substring(0, firstComma);
    String gatewayID = rData.substring(firstComma + 1, secondComma);
    String payload = rData.substring(secondComma + 1);
    // Serial.println("Command: " + command);
    // Serial.println("Gateway ID: " + gatewayID);
    // Serial.println("Payload: " + payload);
    //Process command
    


  }

  //2. TX check and process
  if (txQueue.isEmpty() == false && digitalRead(I_Slave_RDY) == HIGH) {
    String tData = txQueue.dequeue();
    // Serial.println("TX data: " + tData);
    Serial.println(tData);
    delay(100);
  }

}