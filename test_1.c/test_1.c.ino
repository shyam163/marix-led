
// information board on a 64x32 LED matrix to show how many days passed 
// since the last saftey incident happened
// the board is connected to the internet and gets the time from a NTP server
// the incident date is set by a button on the board
// the board is powered by a 5V power supply and the LED matrix is powered by a 5V power supply
// the board is connected to the LED matrix via a level shifters


#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include <WiFiManager.h> 
#include <TimeLib.h>


#define PANEL_RES_X 64              // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32              // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1               // Total number of panels chained one to another
#define upButton_pin 18              // pin to move the incident date a day back wards
#define up25Button_pin 21             // pin to move the incident date 25 days back wards 
#define nowButton_pin 22              // pin to move the incident date to now

WiFiUDP ntpUDP;                     // Define NTP Client to get time
NTPClient timeClient(ntpUDP);

Preferences preferences;

String formattedDate;               // Variables to save date and time
String dayStamp;
String timeStamp;

MatrixPanel_I2S_DMA *dma_display = nullptr;                        //MatrixPanel_I2S_DMA dma_display;
VirtualMatrixPanel *vdma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);                 //colors
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

uint counter=1;
int epochtime=0;
//int incidenttime=1671366600;
int incidenttime=0;
int days=0;
int  fSize=3;
int  fStart=4;
int fStarty=1;
int faster=0;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}



//void IRAM_ATTR nowPressed(){
//  incidenttime=epochtime;
//  dma_display->setCursor(5, 0);
//  dma_display->print("down");
//  delay(2000);
//  dma_display->fillScreen(dma_display->color444(0, 0, 0));
//}
 

void drawText(int colorWheelOffset)
{
  //get the current time fron ntp server

  if(counter%5000==0){
      timeClient.update(); 
      epochtime = timeClient.getEpochTime();                                                   //update time in 50 cuycles    
      dma_display->fillScreen(dma_display->color444(0, 0, 0));
  }

  days=(epochtime-incidenttime)/86400;
  //fill black : screen
  //dma_display->fillScreen(dma_display->color444(0, 0, 0));
  

  if (days < 10){
    fStart=9 ;
    fSize=3;
    fStarty=1;
  }
  else if (days < 100){
    fStart=5 ;
    fSize=2;
    fStarty=6;
  }
  else if (days < 1000){
    fStart=0 ;
    fSize=2;
    fStarty=6;
  }
  else {
    ;
  }
 
  //dma_display->clearScreen();
  dma_display->setTextSize(fSize);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Do wrap at end of line - cant do ourselves
  dma_display->setCursor(fStart,fStarty);    // start at top left, with 8 pixel of spacing
  int8_t w = 0;
  //dma_display->setFont();

 
  //dma_display->print(formattedDate);      //if you want a static display uncomment
  formattedDate = String(days);

  for (w=0; w<(formattedDate.length()); w++) {                               //
    dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));          // for ANIMATED TEXT
    dma_display->print(formattedDate[w]);                                    //
   }

  //dma_display->println();
  //dma_display->print(" ");
  //for (w=9; w<18; w++) {
  //  dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));
  //  dma_display->print("*");
  //}
  dma_display->setTextSize(1);  
  dma_display->setCursor(5,24);
  dma_display->setTextColor(dma_display->color444(5,5,5));
  dma_display->println("DAYS");



  dma_display->setTextSize(1);  
  dma_display->setCursor(35,1);
  dma_display->setTextColor(dma_display->color444(5,5,5));
  dma_display->print("SINCE");

  dma_display->setTextSize(1);  
  dma_display->setCursor(35,2);
  dma_display->setTextColor(dma_display->color444(5,5,5));
  dma_display->print("SINCE");

  days=day(incidenttime);
  if (days< 10){
    fStart=43 ;
    fSize=1;
    fStarty=12;
    dma_display->setCursor(37,12);  
    dma_display->print("0");
    
  }
  else {
    fStart=37 ;
    fSize=1;
    fStarty=12;
  }

  
  dma_display->setTextColor(dma_display->color444(15,2,2));
  dma_display->setTextSize(1);  
  //dma_display->setCursor(37,12);
  dma_display->setCursor(fStart,fStarty);
  
  dma_display->print(days);

  dma_display->setCursor(47,12);
  dma_display->print(":");

  //dma_display->drawPixel(43,19,5000);

  days=month(incidenttime);   //shouldhav been months , just using dsame vairable
 
  if (days< 10){                        //is months
    fStart=58 ;
    fSize=1;
    fStarty=12;
    dma_display->setCursor(51,12);  
    dma_display->print("0");
  }
  else{
    fStart=51 ;
    fSize=1;
    fStarty=12;
  }

  dma_display->setTextSize(1);  
  //dma_display->setCursor(51,12);
  dma_display->setCursor(fStart,fStarty);  
  dma_display->print(days);                         //is months


  dma_display->setCursor(38,21);            
  dma_display->print(year(incidenttime));

  dma_display->setCursor(38,22);           
  dma_display->print(year(incidenttime));

  dma_display->drawRect(35, 10, 29, 21, dma_display->color444(8,2,2));

  counter ++;


}


void setup() {

  //dma_display->setFont(&FreeMonoBoldOblique12pt7b);
  pinMode(upButton_pin, INPUT_PULLUP);                                  //initialize button pins
  //  attachInterrupt(upButton_pin,upPressed, RISING);               //and ISR
  pinMode(up25Button_pin, INPUT_PULLUP);
  //  attachInterrupt(up25Button_pin,up25Pressed, RISING);
  pinMode(nowButton_pin, INPUT_PULLUP);
  //  attachInterrupt(nowButton_pin,nowPressed, RISING);

  preferences.begin("signboard", false);
  incidenttime = preferences.getUInt("incidentstored",0);
  preferences.end();
  // Module configuration19800
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,                         // module width
    PANEL_RES_Y,                         // module height
    PANEL_CHAIN     // Chain length
  );

  //mxconfig.gpio.e = 18;
  //mxconfig.clkphase = false;
  //mxconfig.driver = HUB75_I2S_CFG::FM6126A;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(200); //0-255
  dma_display->clearScreen();
 // dma_display->setRotation(2)
  dma_display->fillScreen(myWHITE);



  
  //texst setup 
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(true); // Don't wrap at end of line - will do ourselves
  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  int8_t w = 0;
  
  

  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.

  // put your setup code here, to run once:
  Serial.begin(115200);
  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("shyam_is_awesome","yesyesyes"); // password protected ap

  if(!res) {
      // didnt connect so we will reset esp
      dma_display->clearScreen();
      dma_display->setCursor(5, 0); 
      dma_display->print("Failed to connect");
      delay(1000);
      ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      //print ip on screen
      dma_display->setCursor(5, 0); 
      dma_display->clearScreen();
      dma_display->print(WiFi.localIP());
      delay(2000);
  }

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  timeClient.setTimeOffset(3600);


  // fix the screen with red
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 0, 0));
  delay(500);

  // draw a box line and circle
  //dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  //dma_display->drawLine(0, 0, dma_display->width()-1, dma_display->height()-1, dma_display->color444(15, 0, 0));
  //dma_display->drawLine(dma_display->width()-1, 0, 0, dma_display->height()-1, dma_display->color444(15, 0, 0));
  //dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
  //dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));


  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));

  //drawText(0);

  timeClient.update();
  formattedDate = timeClient.getFormattedTime();
  epochtime = timeClient.getEpochTime();
}

uint8_t wheelval = 0;


void loop() {

    // animate by going through the colour wheel for the first two lines
    drawText(wheelval);
    wheelval +=1;
    delay(10);
    if(digitalRead(upButton_pin)==LOW){
      incidenttime= incidenttime-8400;
      dma_display->setCursor(50, 0);
      dma_display->fillScreen(dma_display->color444(0, 0, 0));
      preferences.begin("signboard", false);
      preferences.putUInt("incidentstored",incidenttime);
      preferences.end();
      faster=faster+1;
    }
    else if(digitalRead(nowButton_pin)==LOW){
      incidenttime= incidenttime+8400;
      if(incidenttime>epochtime){
        incidenttime=epochtime;
      }
      dma_display->setCursor(50, 0);

      dma_display->fillScreen(dma_display->color444(0, 0, 0));
      preferences.begin("signboard", false);
      preferences.putUInt("incidentstored",incidenttime);
      preferences.end();
    }
    else{
         
    }

}
