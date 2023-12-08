#ifndef APPFUNC
#include "appfunc.h"
#endif

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

class lcdScreen
{
  public:
  deviceInfo lastDev; //Last device info
  deviceInfo proDev;  //Processing device info

  void updateScreen(void){
    proDev = dev; //Update proDev
    proDev.ch[1].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[1].scheduleOper[0][1] = 30;          //<------------- DEBUG
    proDev.ch[2].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[2].scheduleOper[0][1] = 30;          //<------------- DEBUG
    proDev.ch[3].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[3].scheduleOper[0][1] = 30;          //<------------- DEBUG
    proDev.ch[4].scheduleOper[0][0] = 11;          //<------------- DEBUG
    proDev.ch[4].scheduleOper[0][1] = 30;          //<------------- DEBUG
    //Print with ANSI YELLOW color
    // Serial.println("=== TASK3: LCD UPDATE SCREEN ===");
    // Serial.println(dev.time.minute());
    if (proDev.loadFullScreenReq == 1){
      Serial.println("=== LCD FIRST UPDATE SCREEN ===");
      lastDev = proDev; //Update lastDev
      loadFullScreenReq();
      proDev.loadFullScreenReq = 0; //Reset loadFullScreenReq
      dev.loadFullScreenReq = 0; //Reset loadFullScreenReq
    }
    else{
      //Update title bar
      updateTitle(NORMAL_UPDATE);
      //Update channel
      updateChannel(1, NORMAL_UPDATE);
      updateChannel(2, NORMAL_UPDATE);
      updateChannel(3, NORMAL_UPDATE);
      updateChannel(4, NORMAL_UPDATE);
    }
  }

  void updateSemiSec(){
    static bool semiSecFlag = true;
    if (semiSecFlag){
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(10,12);
      tft.print("Time ");
      tft.print(proDev.time.hour());
      tft.print(":");
      semiSecFlag = false;
    }
    else{
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(10,12);
      tft.print("Time ");
      tft.print(proDev.time.hour());
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.print(":");
      semiSecFlag = true;
    }
  }

  void updateTitleBar(void){
    //Print Time
    DateTime now = rtc.now();
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10,12);
    tft.print("Time: ");
    tft.print(now.hour());
    tft.print(":");
    tft.print(now.minute());

    //Print connection status via a circle on the top right corner of the screen
    tft.fillCircle(290, 20, 10, ILI9341_GREEN);
  }

  void updateTitle (uint8_t forcedUpdate = 0){
    if (lastDev.time != proDev.time || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(10,12);
      tft.print("Time ");
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.print(lastDev.time.hour());
      tft.print(":");
      //print minute with leading zero
      if (lastDev.time.minute() < 10)
        tft.print("0");
      tft.print(lastDev.time.minute());
      //Print Time
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(10,12);
      tft.print("Time ");
      tft.print(proDev.time.hour());
      tft.print(":");
      //print minute with leading zero
      if (proDev.time.minute() < 10)
        tft.print("0");
      tft.print(proDev.time.minute());
      lastDev.time = proDev.time;
    }
    if (lastDev.rfConnStatus != proDev.rfConnStatus || forcedUpdate == FORCED_UPDATE){
      if (proDev.rfConnStatus == 1)
        tft.fillCircle(290, 20, 10, ILI9341_GREEN);
      else
        tft.fillCircle(290, 20, 10, ILI9341_RED);
      lastDev.rfConnStatus = proDev.rfConnStatus;
    }
  }

  void updateChannel(uint8_t chNum, uint8_t forcedUpdate = 0){

    uint16_t titleH = 50;
    uint16_t titleV = 57;
    uint16_t hItemName = 20;
    uint16_t hItemValue = 100;
    uint16_t vContent = 80;
    uint16_t vSpace = 12;

    //Channel 1
    if (chNum == 1){
      titleH = 50;
      titleV = 57;
      hItemName = 20;
      hItemValue = 100;
      vContent = 80;
      vSpace = 12;
    }
    //Channel 2
    else if (chNum == 2){
      titleH = 205;
      titleV = 57;
      hItemName = 175;
      hItemValue = 265;
      vContent = 80;
      vSpace = 12;
    }
    //Channel 3
    else if (chNum == 3){
      titleH = 50;
      titleV = 147;
      hItemName = 20;
      hItemValue = 100;
      vContent = 170;
      vSpace = 12;
    }
    //Channel 4
    else if (chNum == 4){
      titleH = 205;
      titleV = 147;
      hItemName = 175;
      hItemValue = 265;
      vContent = 170;
      vSpace = 12;
    }
    else{
      return;
    }

    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.setCursor(titleH, titleV);
    //1. Update Channel
    if (lastDev.ch[chNum].operStatus != proDev.ch[chNum].operStatus || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(titleH, titleV);
      tft.print("CH"+String(chNum)+" ON");
      tft.setCursor(titleH, titleV);
      tft.print("CH"+String(chNum)+" OFF");
      //Print Channel status
      tft.setCursor(titleH, titleV);
      if (proDev.ch[chNum].operStatus == 1){
        tft.setTextColor(ILI9341_GREEN);
        tft.print("CH"+String(chNum)+" ON");
      }
      else {
        tft.setTextColor(ILI9341_RED);
        tft.print("CH"+String(chNum)+" OFF");
      }
      lastDev.ch[chNum].operStatus = proDev.ch[chNum].operStatus;
    }

    tft.setTextSize(1);
    //2. Update Voltage
    if (lastDev.ch[chNum].Vout != proDev.ch[chNum].Vout || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(hItemValue, vContent+0*vSpace);
      tft.print(lastDev.ch[chNum].Vout,0);
      tft.print(" V");
      //Print Item name
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemName, vContent+0*vSpace);
      tft.print("Voltage ");
      //Print Voltage
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemValue, vContent+0*vSpace);
      tft.print(proDev.ch[chNum].Vout,0);
      tft.print(" V");
      lastDev.ch[chNum].Vout = proDev.ch[chNum].Vout;
    }
    //3. Update Current
    if (lastDev.ch[chNum].Iout != proDev.ch[chNum].Iout || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(hItemValue, vContent+1*vSpace);
      tft.print(lastDev.ch[chNum].Iout,1);
      tft.print(" A");
      //Print Item name
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemName, vContent+1*vSpace);
      tft.print("Current ");
      //Print Current
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemValue, vContent+1*vSpace);
      tft.print(proDev.ch[chNum].Iout,1);
      tft.print(" A");
      lastDev.ch[chNum].Iout = proDev.ch[chNum].Iout;
    }
    //4. Update Mode
    if (lastDev.ch[chNum].operMode != proDev.ch[chNum].operMode || forcedUpdate == FORCED_UPDATE){
      //Clear the old text
      tft.setTextColor(ILI9341_BLUE); //Blue is the background color
      tft.setCursor(hItemValue, vContent+2*vSpace);
      tft.print("MANUAL");
      tft.setCursor(hItemValue, vContent+2*vSpace);
      tft.print("AUTO");
      //Print Item name
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemName, vContent+2*vSpace);
      tft.print("Mode ");
      //Print Mode
      tft.setTextColor(ILI9341_WHITE);
      tft.setCursor(hItemValue, vContent+2*vSpace);
      if (proDev.ch[chNum].operMode == MANUAL_MODE) {
        tft.setTextColor(ILI9341_WHITE);
        tft.print("MANUAL");
      }
      else{
        tft.setTextColor(ILI9341_GREEN);
        tft.print("AUTO");
      }
      lastDev.ch[chNum].operMode = proDev.ch[chNum].operMode;
    }
    //5. Update Schedule
    if (lastDev.ch[chNum].scheduleOper[0] != proDev.ch[chNum].scheduleOper[0] || forcedUpdate == FORCED_UPDATE){
      if (proDev.ch[chNum].operMode == AUTO_MODE) {
        //Clear the old text
        tft.setTextColor(ILI9341_BLUE); //Blue is the background color
        tft.setCursor(hItemValue, vContent+3*vSpace);
        tft.print(lastDev.ch[chNum].scheduleOper[0][0]);
        tft.print(":");
        tft.print(lastDev.ch[chNum].scheduleOper[0][1]);
        //Print Item name
        tft.setTextColor(ILI9341_WHITE);
        tft.setCursor(hItemName, vContent+3*vSpace);
        tft.print("Next OFF ");
        //Print Schedule
        tft.setTextColor(ILI9341_GREEN);
        tft.setCursor(hItemValue, vContent+3*vSpace);
        tft.print(proDev.ch[chNum].scheduleOper[0][0]);
        tft.print(":");
        tft.print(proDev.ch[chNum].scheduleOper[0][1]);
        lastDev.ch[chNum].scheduleOper[0][0] = proDev.ch[chNum].scheduleOper[0][0];
        lastDev.ch[chNum].scheduleOper[0][1] = proDev.ch[chNum].scheduleOper[0][1];
      }
      else{
        //Draw a blue rectangle to cover the old text and item name
        tft.fillRect(hItemName, vContent+3*vSpace, 130, 8, ILI9341_BLUE);
      }
    }
  }




  //   tft.print("CH1 ON");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setTextSize(1);
  //   tft.setCursor(hItemName, vContent+0*vSpace);
  //   tft.print("Voltage: ");
  //   tft.setCursor(hItemValue, vContent+0*vSpace);
  //   tft.print("110V");
  //   tft.setCursor(hItemName, vContent+1*vSpace);
  //   tft.print("Current: ");
  //   tft.setCursor(hItemValue, vContent+1*vSpace);
  //   tft.print("0.5A");
  //   tft.setCursor(hItemName, vContent+2*vSpace);
  //   tft.print("Mode: ");
  //   tft.setCursor(hItemValue, vContent+2*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("AUTO");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setCursor(hItemName, vContent+3*vSpace);
  //   tft.print("Next OFF: ");
  //   tft.setCursor(hItemValue, vContent+3*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("12:00");
  //   tft.setTextColor(ILI9341_WHITE);
  // }




  void loadFullScreenReq (void){
    //Draw Frame
    tft.fillRect(0, 0, 320, 40, ILI9341_BLUE);
    tft.fillRect(0, 40, 320, 240, ILI9341_WHITE);
    tft.fillRoundRect( 10,  50, 150, 85,10, ILI9341_BLUE); //Channel 1
    tft.fillRoundRect(165,  50, 150, 85,10, ILI9341_BLUE); //Channel 2
    tft.fillRoundRect( 10, 140, 150, 85,10, ILI9341_BLUE); //Channel 3
    tft.fillRoundRect(165, 140, 150, 85,10, ILI9341_BLUE); //Channel 4

    //Update title bar
    updateTitle(FORCED_UPDATE);

    //Update channel
    updateChannel(1, FORCED_UPDATE);
    updateChannel(2, FORCED_UPDATE);
    updateChannel(3, FORCED_UPDATE);
    updateChannel(4, FORCED_UPDATE);
  }



  //   //Channel 1
  //   uint8_t titleH = 50;
  //   uint8_t titleV = 57;
  //   uint8_t hItemName = 20;
  //   uint8_t hItemValue = 100;
  //   uint8_t vContent = 80;
  //   uint8_t vSpace = 12;
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.setTextSize(2);
  //   tft.setCursor(titleH, titleV);
  //   tft.print("CH1 ON");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setTextSize(1);
  //   tft.setCursor(hItemName, vContent+0*vSpace);
  //   tft.print("Voltage: ");
  //   tft.setCursor(hItemValue, vContent+0*vSpace);
  //   tft.print("110V");
  //   tft.setCursor(hItemName, vContent+1*vSpace);
  //   tft.print("Current: ");
  //   tft.setCursor(hItemValue, vContent+1*vSpace);
  //   tft.print("0.5A");
  //   tft.setCursor(hItemName, vContent+2*vSpace);
  //   tft.print("Mode: ");
  //   tft.setCursor(hItemValue, vContent+2*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("AUTO");
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setCursor(hItemName, vContent+3*vSpace);
  //   tft.print("Next OFF: ");
  //   tft.setCursor(hItemValue, vContent+3*vSpace);
  //   tft.setTextColor(ILI9341_GREEN);
  //   tft.print("12:00");
  //   tft.setTextColor(ILI9341_WHITE);


  //   //Channel 2
  //   tft.setTextColor(ILI9341_WHITE);
  //   tft.setTextSize(3);
  //   tft.setCursor(175, 70);
  //   tft.print("CH2");
  //   tft.setTextSize(2);
  //   tft.setCursor(175, 100);
  //   tft.print("OFF");
  //   //Channel 3 ON
  //   tft.setTextSize(3);
  //   tft.setCursor(20, 160);
  //   tft.print("CH3");
  //   tft.setTextSize(2);
  //   tft.setCursor(20, 190);
  //   tft.print("OFF");
  //   //Channel 4
  //   tft.setTextSize(3);
  //   tft.setCursor(175, 160);
  //   tft.print("CH4");
  //   tft.setTextSize(2);
  //   tft.setCursor(175, 190);
  //   tft.print("OFF");
  // }


};
lcdScreen lcd = lcdScreen();


void task3 (){
  lcd.updateScreen(); //Update screen
  lcd.updateSemiSec();
}