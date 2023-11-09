#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

float boxTemp, boxHumi;


#include "AppFunc.h"
TaskHandle_t xTaskHandle_1;
TaskHandle_t xTaskHandle_2;
TaskHandle_t xTaskHandle_3;
TaskHandle_t xTaskHandle_4;
void allTaskDeclare();

void debugPrint(){
  rtcPrintDatetime(); //Print current datetime
  SHT21_Read();
  Serial.print("Temperature   : ");Serial.print(boxTemp,1);Serial.println("oC");
  Serial.print("Humidity      : ");Serial.print(boxHumi,1);Serial.println("%");
  Serial.println("-----------------------------");
  ADC_Read();
  delay(3000);
}

void setup() {
  controllerInit(); //Initialize controller

  Serial.println("\n=== START ===");
  allTaskDeclare ();
}

void loop() {

}

//-----------------------------------------------------------------------------------------------------------//
//------------------------------------     TASK ASSIGNMENT     ----------------------------------------------//
//-----------------------------------------------------------------------------------------------------------//
/////////////////////////////////
// A. Assign function to tasks //
/////////////////////////////////
//define task handles
// void task1 (void * pvParameters) {vTaskDelay( 5/portTICK_PERIOD_MS); while (1) {if(topButtonInConf == false) updateCO2Value (); vTaskDelay(10000/portTICK_PERIOD_MS);}}
// void task2 (void * pvParameters) {vTaskDelay(25/portTICK_PERIOD_MS); while (1) {scanButton(); vTaskDelay(10/portTICK_PERIOD_MS);}}
// void task3 (void * pvParameters) {vTaskDelay(45/portTICK_PERIOD_MS); while (1) {displayShowing();vTaskDelay(100/portTICK_PERIOD_MS);}}
void task4 (void * pvParameters) {vTaskDelay(65/portTICK_PERIOD_MS); while (1) {debugPrint(); vTaskDelay(1000/portTICK_PERIOD_MS);}}
//////////////////////////
// B. Declare all tasks //
//////////////////////////
void allTaskDeclare (){
 // Tasks declaration
//  xTaskCreate(
//                  task1,                 /* Function to implement the task */
//                  "Check Button & SW",   /* Name of the task */
//                  4096,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  20,                    /* Priority of the task */
//                  &xTaskHandle_1);       /* Task handle. */
//  xTaskCreate(
//                  task2,                 /* Function to implement the task */
//                  "Scan button",         /* Name of the task */
//                  4096,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  10,                    /* Priority of the task */
//                  &xTaskHandle_2);       /* Task handle. */
//  xTaskCreate(
//                  task3,                 /* Function to implement the task */
//                  "display",             /* Name of the task */
//                  4096,                  /* Stack size in words */
//                  NULL,                  /* Task input parameter */
//                  10,                    /* Priority of the task */
//                  &xTaskHandle_3);       /* Task handle. */
 xTaskCreate(
                 task4,                 /* Function to implement the task */
                 "Debug Print",   /* Name of the task */
                 4096,                  /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 5,                     /* Priority of the task */
                 &xTaskHandle_4);       /* Task handle. */
}