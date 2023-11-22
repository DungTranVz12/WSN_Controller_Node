#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <ArduinoUniqueID.h>
#include "AppFunc.h"




TaskHandle_t xTaskHandle_1;
TaskHandle_t xTaskHandle_2;
TaskHandle_t xTaskHandle_3;
TaskHandle_t xTaskHandle_4;
void allTaskDeclare();

void debugPrint(){
  Serial.println("=== TASK4 ===");
  rtcPrintDatetime(); //Print current datetime
  SHT21_Read();
  Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
  Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
  Serial.println("-----------------------------");
  // ADC_Read();
  Serial.print("CH1_Vmax      : ");Serial.print(dev.ch[1].Vout,1);Serial.println("V");
  // Print dev.ch[1].listVolt in array
  Serial.print("CH1_Vmax_list : ");
  for (int i = 0; i < 40; i++)
  {
    Serial.print(dev.ch[1].listVolt[i]);
    Serial.print(" ");
  }
  delay(5000);
}

void task3 (){
  lcd.updateScreen(); //Update screen
}

void setup() {
  controllerInit(); //Initialize controller

  
  //Convert HEX UniqueID to char array then print it out
  sprintf(UID, "%02X%02X%02X%02X%02X%02X", UniqueID[0], UniqueID[1], UniqueID[2]);
  Serial.print("Unique ID: ");
  //print 6 bytes of UniqueID
  for (int i = 0; i < 6; i++)
  {
    Serial.print(UID[i]);
  }

  Serial.println("\n=== START ALL TASKS ===");
  allTaskDeclare ();
  Serial.println("\n=== END SETUP ===");

}








void loop() {
  task1 (); //Check button & SW

  //TEST
  dev.firstLoadMem = 1; //First load memory <--------------------- DEBUG
  dev.ch[1].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
  dev.ch[2].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
  dev.ch[3].autoControl = CONTROL_ON; //Auto control: CONTROL_ON
  dev.ch[4].autoControl = CONTROL_ON; //Auto control: CONTROL_ON

  
}

//-----------------------------------------------------------------------------------------------------------//
//------------------------------------     TASK ASSIGNMENT     ----------------------------------------------//
//-----------------------------------------------------------------------------------------------------------//
/////////////////////////////////
// A. Assign function to tasks //
/////////////////////////////////
//define task handles
// void multask1 (void * pvParameters) {vTaskDelay( 5/portTICK_PERIOD_MS); while (1) {if(topButtonInConf == false) updateCO2Value (); vTaskDelay(10000/portTICK_PERIOD_MS);}}
void multask2 (void * pvParameters) {vTaskDelay(25/portTICK_PERIOD_MS); while (1) {task2(); vTaskDelay(100/portTICK_PERIOD_MS);}}
void multask3 (void * pvParameters) {vTaskDelay(45/portTICK_PERIOD_MS); while (1) {task3(); vTaskDelay(1000/portTICK_PERIOD_MS);}}
void multask4 (void * pvParameters) {vTaskDelay(65/portTICK_PERIOD_MS); while (1) {debugPrint(); vTaskDelay(1000/portTICK_PERIOD_MS);}}
//////////////////////////
// B. Declare all tasks //
//////////////////////////
void allTaskDeclare (){
 // Tasks declaration
//  xTaskCreate(
//                  multask1,                 /* Function to implement the task */
//                  "Check Button & SW",   /* Name of the task */
//                  1024,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  20,                    /* Priority of the task */ 
//                  &xTaskHandle_1);       /* Task handle. */
 xTaskCreate(
                 multask2,              /* Function to implement the task */
                 "Voltage & Current Monitoring", /* Name of the task *///                  
                 512,                   /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                    /* Priority of the task */
                 &xTaskHandle_2);       /* Task handle. */
 xTaskCreate(
                 multask3,                 /* Function to implement the task */
                 "display",             /* Name of the task */
                 512,                  /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                    /* Priority of the task */                                    
                 &xTaskHandle_3);       /* Task handle. */
 xTaskCreate(
                 multask4,              /* Function to implement the task */
                 "Debug Print",         /* Name of the task */
                 512,                   /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                     /* Priority of the task */
                 &xTaskHandle_4);       /* Task handle. */
}