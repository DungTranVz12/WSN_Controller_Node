#pragma once
#define APPFUNC
#include "define.h"
#include <EEPROM.h>
#include <TimerOne.h>
#include "AppPriFunc.h"

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


