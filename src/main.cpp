#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <ArduinoUniqueID.h>
#include "define.h"
#include "task1_user_button_sw.h"
#include "task2_Internal_control.h"
#include "task3_lcd.h"
#include "task4_rf_com.h"
#include "AppFunc.h"


TaskHandle_t xTaskHandle_2;
TaskHandle_t xTaskHandle_3;
TaskHandle_t xTaskHandle_4;
void allTaskDeclare();
//workingFlowClass workingFlow;

void debugPrint(){
  Serial.println("=== DEBUG ===");
  rtcPrintDatetime(); //Print current datetime
  SHT21_Read();
  Serial.print("Temperature   : ");Serial.print(dev.boxTemp,1);Serial.println("oC");
  Serial.print("Humidity      : ");Serial.print(dev.boxHumi,1);Serial.println("%");
  Serial.println("-----------------------------");
  // ADC_Read();
  Serial.print("CH1_Vmax      : ");Serial.print(dev.ch[1].Vout,1);Serial.println("V");
  // Print dev.ch[1].listVolt in array
  Serial.print("CH1_Vmax_list : ");
  for (int i = 0; i < 20; i++)
  {
    Serial.print(dev.ch[1].listVolt[i]);
    Serial.print(" ");
  }
  delay(5000);
}

void controllerInit(void){
  Serial.begin(115200); //Initialize serial port
  // 1. Initialize IO
  pinMode(O_RELAY_1_LEVEL, OUTPUT);  //Set pin mode for relay 1 level
  pinMode(O_RELAY_2_LEVEL, OUTPUT);  //Set pin mode for relay 2 level
  pinMode(O_RELAY_3_LEVEL, OUTPUT);  //Set pin mode for relay 3 level
  pinMode(O_RELAY_4_LEVEL, OUTPUT);  //Set pin mode for relay 4 level
  pinMode(O_RELAY_1_PWM, OUTPUT);    //Set pin mode for relay 1 PWM
  pinMode(O_RELAY_2_PWM, OUTPUT);    //Set pin mode for relay 2 PWM
  pinMode(O_RELAY_3_PWM, OUTPUT);    //Set pin mode for relay 3 PWM
  pinMode(O_RELAY_4_PWM, OUTPUT);    //Set pin mode for relay 4 PWM
  pinMode(O_BUZZER, OUTPUT);         //Set pin mode for buzzer
  pinMode(O_LED_4G, OUTPUT);         //Set pin mode for LED 4G
  pinMode(O_LED_RF, OUTPUT);         //Set pin mode for LED RF
  pinMode(O_LED_WIFI, OUTPUT);       //Set pin mode for LED WIFI
  pinMode(O_LED_BAT, OUTPUT);        //Set pin mode for LED BAT
  pinMode(O_LED_SLA, OUTPUT);        //Set pin mode for LED SLA
  pinMode(O_LED_PWR, OUTPUT);        //Set pin mode for LED PWR
  pinMode(I_SW_CH1, INPUT);          //Set pin mode for switch channel 1
  pinMode(I_SW_CH2, INPUT);          //Set pin mode for switch channel 2
  pinMode(I_SW_CH3, INPUT);          //Set pin mode for switch channel 3
  pinMode(I_SW_CH4, INPUT);          //Set pin mode for switch channel 4
  pinMode(I_RL_CH1, INPUT);          //Set pin mode for relay channel 1 (read status)
  pinMode(I_RL_CH2, INPUT);          //Set pin mode for relay channel 2 (read status)
  pinMode(I_RL_CH3, INPUT);          //Set pin mode for relay channel 3 (read status)
  pinMode(I_RL_CH4, INPUT);          //Set pin mode for relay channel 4 (read status)
  pinMode(I_Slave_RDY, INPUT);       //Set pin mode for Slave RDY
  pinMode(BUTTON_BACK, INPUT);       //Set pin mode for button BACK
  pinMode(BUTTON_DOWN, INPUT);       //Set pin mode for button DOWN
  pinMode(BUTTON_UP, INPUT);         //Set pin mode for button UP
  pinMode(BUTTON_SELECT, INPUT);     //Set pin mode for button SELECT
  pinMode(TFL_BACKLIGHT, OUTPUT);    //Set pin mode for LCD backlight

  digitalWrite(O_LED_4G  , HIGH);    //Turn off LED 4G
  digitalWrite(O_LED_RF  , HIGH);    //Turn off LED RF
  digitalWrite(O_LED_WIFI, HIGH);    //Turn off LED WIFI
  digitalWrite(O_LED_BAT , LOW );    //Turn on LED BAT
  digitalWrite(O_LED_SLA , HIGH);    //Turn off LED SLA
  digitalWrite(O_LED_PWR , LOW );    //Turn on LED PWR
  digitalWrite(TFL_BACKLIGHT, LOW); //Turn off LCD backlight

  Timer1.initialize(1000);                 // initialize timer1, and set a 1 millisec period
  Timer1.attachInterrupt(pwmControlCheck); // attaches callback: pwmControlCheck as a timer overflow interrupt

  //2. Load config from EEPROM
  // i_contactorStatusCh1 = EEPROM.read();

  //3. Initialize RTC
  rtcInit(); //Initialize RTC

  //4. Return last state to GPIO

  //5. Button init
  buttonInit(); //Initialize button: BACK, DOWN, UP, SELECT

  //6. LCD init
  tft.begin();
  tft.setRotation(3);  //Rotate 270 degree

  //7. SHT21
  sht.begin();  //Initialize SHT21

}

void setup() {
  //1. Initialize controller
  controllerInit(); //Initialize controller

  //2. print 6 bytes of UniqueID
  sprintf(UID, "%02X%02X%02X%02X%02X%02X", (unsigned int)UniqueID[0], (unsigned int)UniqueID[1], (unsigned int)UniqueID[2]);
  UIDstr = String(UID[0]) + String(UID[1]) + String(UID[2]) + String(UID[3]) + String(UID[4]) + String(UID[5]);
  Serial.print("Unique ID: ");
  Serial.println(UIDstr);

  //workingFlow.setupFlow();
  Serial.println("\n=== START ALL TASKS ===");
  allTaskDeclare ();
  Serial.println("\n=== END SETUP ===");
  setupDoneFlag = true;

  // rfSendToGateway("A6","START_WPS_MODE"); //<--------------------- DEBUG
}

void loop() {
  // Serial.println("=== LOOP ===");
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
void multask2 (void * pvParameters) {vTaskDelay(100/portTICK_PERIOD_MS); while (1) {task2(); vTaskDelay(100/portTICK_PERIOD_MS);}}
void multask3 (void * pvParameters) {vTaskDelay(150/portTICK_PERIOD_MS); while (1) {task3(); vTaskDelay(1000/portTICK_PERIOD_MS);}}
void multask4 (void * pvParameters) {vTaskDelay(850/portTICK_PERIOD_MS); while (1) {task4(); vTaskDelay(100/portTICK_PERIOD_MS);}}
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
                 256,                   /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                    /* Priority of the task */
                 &xTaskHandle_2);       /* Task handle. */
 xTaskCreate(
                 multask3,              /* Function to implement the task */
                 "display",             /* Name of the task */
                 256,                   /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                    /* Priority of the task */                                    
                 &xTaskHandle_3);       /* Task handle. */
 xTaskCreate(
                 multask4,              /* Function to implement the task */
                 "RF communication",    /* Name of the task */
                 256,                   /* Stack size in words */
                 NULL,                  /* Task input parameter */
                 10,                     /* Priority of the task */
                 &xTaskHandle_4);       /* Task handle. */
}