#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "FreeMono12pt7b.h"

float boxTemp, boxHumi; //Temperature and humidity inside the box.
float V_DC_IN, V_SS_SYS, V_Door; //Voltage of DC input, sensor system and door sensor.
float Vout1, Vout2, Vout3, Vout4, Iout1, Iout2, Iout3, Iout4; //Voltage and current of each channel.



struct channelInfo {
  float Vout = 0; //Voltage of each channel.
  float Iout = 0; //Current of each channel.
  uint8_t operStatus = 0; //0: OFF, 1: ON
  uint8_t operMode = 0; //0: Manual, 1: Schedule
  uint32_t scheduleOper[20]; //Schedule operation time of each channel.
};

struct  deviceInfo
{
  /* data */
  float boxTemp = 0; //Temperature inside the box.
  float boxHumi = 0; //Humidity inside the box.
  float V_DC_IN = 0; //Voltage of DC input.
  float V_SS_SYS = 0; //Voltage of sensor system.
  float V_Door = 0; //Voltage of door sensor.
  // DateTime time; //RTC time

  // Channle info
  channelInfo ch1; //Channel 1
  channelInfo ch2; //Channel 2
  channelInfo ch3; //Channel 3
  channelInfo ch4; //Channel 4
};


#include "AppFunc.h"
TaskHandle_t xTaskHandle_1;
TaskHandle_t xTaskHandle_2;
TaskHandle_t xTaskHandle_3;
TaskHandle_t xTaskHandle_4;
void allTaskDeclare();

// void task3 () {
//   pinMode(TFL_BACKLIGHT, OUTPUT);
//   digitalWrite(TFL_BACKLIGHT, HIGH); //Turn OFF LCD backlight

//   tft.begin();
//   tft.setRotation(3);
//   tft.fillScreen(ILI9341_WHITE);

//   digitalWrite(TFL_BACKLIGHT, LOW);  //Turn OFF LCD backlight
//   delay(500);
//   digitalWrite(TFL_BACKLIGHT, HIGH); //Turn ON LCD backlight

//   tft.setTextColor(ILI9341_BLUE);
//   tft.setTextSize(3);
//   tft.setCursor(85,25);
//   tft.print("HELLO WSN");
//   tft.setTextColor(ILI9341_GREEN);
//   tft.setCursor(25,200);
//   tft.setTextSize(2);
//   tft.print("CM Engineering Vietnam");    // Выводим текст
// }

void debugPrint(){
  rtcPrintDatetime(); //Print current datetime
  SHT21_Read();
  Serial.print("Temperature   : ");Serial.print(boxTemp,1);Serial.println("oC");
  Serial.print("Humidity      : ");Serial.print(boxHumi,1);Serial.println("%");
  Serial.println("-----------------------------");
  // ADC_Read();
  delay(3000);
}

void setup() {
  controllerInit(); //Initialize controller
  // task3(); //TEST LCD

  Serial.println("\n=== START ===");
  allTaskDeclare ();
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
// void task1 (void * pvParameters) {vTaskDelay( 5/portTICK_PERIOD_MS); while (1) {if(topButtonInConf == false) updateCO2Value (); vTaskDelay(10000/portTICK_PERIOD_MS);}}
void task2 (void * pvParameters) {vTaskDelay(25/portTICK_PERIOD_MS); while (1) {task2(); vTaskDelay(1000/portTICK_PERIOD_MS);}}
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
 xTaskCreate(
                 task2,                 /* Function to implement the task */
                 "Voltage & Current Monitoring", /* Name of the task *///                  
                 4096,                  /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                    /* Priority of the task */
                 &xTaskHandle_2);       /* Task handle. */
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