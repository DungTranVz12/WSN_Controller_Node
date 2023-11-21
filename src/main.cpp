#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "AppFunc.h"

TaskHandle_t xTaskHandle_1;
TaskHandle_t xTaskHandle_2;
TaskHandle_t xTaskHandle_3;
TaskHandle_t xTaskHandle_4;
void allTaskDeclare();

void debugPrint(){
  //Print with ANSI YELLOW color
  Serial.println("=== TASK4 ===");
  
  rtcPrintDatetime(); //Print current datetime
  SHT21_Read();
  Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
  Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
  Serial.println("-----------------------------");
  // ADC_Read();
  delay(3000);
}

void setup() {
  controllerInit(); //Initialize controller

  Serial.println("\n=== START ALL TASKS ===");
  // allTaskDeclare ();
  Serial.println("\n=== END SETUP ===");
}

void loop() {
  task1 (); //Check button & SW
}

//-----------------------------------------------------------------------------------------------------------//
//------------------------------------     TASK ASSIGNMENT     ----------------------------------------------//
//-----------------------------------------------------------------------------------------------------------//
/////////////////////////////////
// A. Assign function to tasks //
/////////////////////////////////
//define task handles
// void multask1 (void * pvParameters) {vTaskDelay( 5/portTICK_PERIOD_MS); while (1) {if(topButtonInConf == false) updateCO2Value (); vTaskDelay(10000/portTICK_PERIOD_MS);}}
// void multask2 (void * pvParameters) {vTaskDelay(25/portTICK_PERIOD_MS); while (1) {task2(); vTaskDelay(1000/portTICK_PERIOD_MS);}}
// void multask3 (void * pvParameters) {vTaskDelay(45/portTICK_PERIOD_MS); while (1) {lcd.updateScreen();vTaskDelay(1000/portTICK_PERIOD_MS);}}
void task4 (void * pvParameters) {vTaskDelay(65/portTICK_PERIOD_MS); while (1) {debugPrint(); vTaskDelay(100/portTICK_PERIOD_MS);}}
//////////////////////////
// B. Declare all tasks //
//////////////////////////
void allTaskDeclare (){
 // Tasks declaration
//  xTaskCreate(
//                  multask1,                 /* Function to implement the task */
//                  "Check Button & SW",   /* Name of the task */
//                  4096,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  20,                    /* Priority of the task */ 
//                  &xTaskHandle_1);       /* Task handle. */
//  xTaskCreate(
//                  multask2,              /* Function to implement the task */
//                  "Voltage & Current Monitoring", /* Name of the task *///                  
//                  4096,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  10,                    /* Priority of the task */
//                  &xTaskHandle_2);       /* Task handle. */
//  xTaskCreate(
//                  multask3,                 /* Function to implement the task */
//                  "display",             /* Name of the task */
//                  4096,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  10,                    /* Priority of the task */                                    
//                  &xTaskHandle_3);       /* Task handle. */
 xTaskCreate(
                 task4,              /* Function to implement the task */
                 "Debug Print",         /* Name of the task */
                 4096,                  /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 5,                     /* Priority of the task */
                 &xTaskHandle_4);       /* Task handle. */
}