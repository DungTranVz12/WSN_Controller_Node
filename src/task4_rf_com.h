#ifndef APPFUNC
#include "appfunc.h"
#endif

/**
 * @brief Function to check and retrieve data from the serial port.
 * 
 * @return String The data received from the serial port.
 */
String rfRxCheck()
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
  return rData;
}