#include <Arduino.h>

/***************************************************************************************************************************************
LILYGO TTGO T-DISPLAY DEMO
Created at August 29th, 2019 by Jeroen Maathuis (j [dot] maathuis [at] gmail [dot] com) to support the LilyGo TTGO T-display

Based on:
- ArduinoMenu TFT_eSPI example on https://github.com/neu-rah/ArduinoMenu/blob/master/examples/adafruitGfx/eTFT/TFT_eSPI/TFT_eSPI.ino.
- TTGO T-display example on https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TTGO-T-Display.ino.
- Several Button2 examples on https://github.com/LennartHennigs/Button2/tree/master/examples

External needed library's:
- TFT_eSPI
- Buttons2

Additional needed files:
- bmp.h (should be found at the same place as this sketch file)

Preparations:
- Create TFT_eSPI User Setup file as described on https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/README.MD and
  https://sites.google.com/site/jmaathuis/arduino/lilygo-ttgo-t-display-esp32/arduinomenu-demo-ttgo-t-display
- Attach a led (and a resistor if needed). This is optional.

This demo can also work on other setups with displays supported by the TFT_eSPI library. This demo is explicitly for the
TTGO T-display but I had a slightly modifies version working on a Wemos D1 mini with attached ST7735 display and some image
and text modifications as well another User Setup file for TFT_eSPI.

For a more detailed description about this demo please visit:
https://sites.google.com/site/jmaathuis/arduino/lilygo-ttgo-t-display-esp32/arduinomenu-demo-ttgo-t-display

This sketch may freely redistributed as long as you mention the sources / authors mentioned above. I would be very happy to here from
you when you have improved the sketch. You can mail me at j [dot] maathuis [at] gmail [dot] com.

****************************************************************************************************************************************/
#define PROG_VERSION "0.2" // Don't change this!

//#include <SPI.h>
#include <TFT_eSPI.h>
#include <Button2.h>
#include "bmp.h"

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_BL          4  // Display backlight control pin

#define LED_CTRL        0 // Pinnumber of external led to control. Set to 0 if there isn't a external led attached.

int ledBacklight = 80; // Initial TFT backlight intensity on a scale of 0 to 255. Initial value is 80.

#define BTN_UP 35 // Pinnumber for button for up/previous and select / enter actions (don't change this if you want to use the onboard buttons)
#define BTN_DWN 0 // Pinnumber for button for down/next and back / exit actions (don't change this if you want to use the onboard buttons)

#include <menu.h>
#include <menuIO/serialIO.h>
#include <menuIO/TFT_eSPIOut.h>
// #include <menuIO/chainStream.h>
#include <menuIO/esp8266Out.h>//must include this even if not doing web output...

using namespace Menu;

TFT_eSPI gfx = TFT_eSPI(); // Define TFT_eSPI object with the size of the screen: 135 pixels width and 240 pixels height. We will rotate it later a quarter clockwise.

Button2 btnUp(BTN_UP); // Initialize the up button
Button2 btnDwn(BTN_DWN); // Initialize the down button

result doAlert(eventMask e, prompt &item);

int test=55;

int ledCtrl=LOW; // Initial value for external connected led

// Setting PWM properties, do not change this!
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);;
  }
};

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,EXIT("<Back")
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  // ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,FIELD(ledBacklight,"Backlight: ","",0,255,10,5,doNothing,noEvent,wrapStyle) // Menu option to set the intensity of the backlight of the screen.
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  //,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table


#define Black RGB565(0,0,0)
#define Red	RGB565(255,0,0)
#define Green RGB565(0,255,0)
#define Blue RGB565(0,0,255)
#define Gray RGB565(128,128,128)
#define LighterRed RGB565(255,150,150)
#define LighterGreen RGB565(150,255,150)
#define LighterBlue RGB565(150,150,255)
#define DarkerRed RGB565(150,0,0)
#define DarkerGreen RGB565(0,150,0)
#define DarkerBlue RGB565(0,0,150)
#define Cyan RGB565(0,255,255)
#define Magenta RGB565(255,0,255)
#define Yellow RGB565(255,255,0)
#define White RGB565(255,255,255)

const colorDef<uint16_t> colors[6] MEMMODE={
  {
    {
      (uint16_t)Black,
      (uint16_t)Black
    },
    {
      (uint16_t)Black,
      (uint16_t)DarkerBlue,
      (uint16_t)DarkerBlue
    }
  },//bgColor
  {
    {
      (uint16_t)Gray,
      (uint16_t)Gray
    },
    {
      (uint16_t)White,
      (uint16_t)White,
      (uint16_t)White
    }
  },//fgColor
  {
    {
      (uint16_t)White,
      (uint16_t)Black
    },
    {
      (uint16_t)Yellow,
      (uint16_t)Yellow,
      (uint16_t)Red
    }
  },//valColor
  {
    {
      (uint16_t)White,
      (uint16_t)Black
    },
    {
      (uint16_t)White,
      (uint16_t)Yellow,
      (uint16_t)Yellow
    }
  },//unitColor
  {
    {
      (uint16_t)White,
      (uint16_t)Gray
    },
    {
      (uint16_t)Black,
      (uint16_t)Blue,
      (uint16_t)White
    }
  },//cursorColor
  {
    {
      (uint16_t)White,
      (uint16_t)Yellow
    },
    {
      (uint16_t)DarkerRed,
      (uint16_t)White,
      (uint16_t)White
    }
  },//titleColor
};

#define MAX_DEPTH 5

serialIn serial(Serial);

//MENU_INPUTS(in,&serial);its single, no need to `chainStream`

//define serial output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(Serial,serialTops);

#define GFX_WIDTH 240
#define GFX_HEIGHT 135
#define fontW 12
#define fontH 18

const panel panels[] MEMMODE = {{0, 0, GFX_WIDTH / fontW, GFX_HEIGHT / fontH}};
navNode* nodes[sizeof(panels) / sizeof(panel)]; //navNodes to store navigation status
panelsList pList(panels, nodes, 1); //a list of panels and nodes
idx_t eSpiTops[MAX_DEPTH]={0};
TFT_eSPIOut eSpiOut(gfx,colors,eSpiTops,pList,fontW,fontH+1);
menuOut* constMEM outputs[] MEMMODE={&outSerial,&eSpiOut};//list of output devices
outputsList out(outputs,sizeof(outputs)/sizeof(menuOut*));//outputs list controller

NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

unsigned long idleTimestamp = millis();

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling) {
      // Show the idle message once
      int xpos = gfx.width() / 2; // Half the screen width
      gfx.fillScreen(Black);

      gfx.setTextSize(5);
      gfx.setTextColor(Yellow,Black);
      gfx.setTextWrap(false);
      gfx.setTextDatum(MC_DATUM);
      gfx.drawString("IDLE", xpos, 50);
      int getFontHeight = gfx.fontHeight();

      gfx.setTextSize(2);
      gfx.setTextColor(White,Black);
      gfx.setTextDatum(MC_DATUM);
      gfx.drawString("Long press a button", xpos, 90);
      gfx.drawString("to exit", xpos, 110);
  }
  return proceed;
}

//config myOptions('*','-',defaultNavCodes,false);

void button_init()
{
    btnUp.setLongClickHandler([](Button2 & b) {
        // Select
        unsigned int time = b.wasPressedFor();
        if (time >= 1000) {
          nav.doNav(enterCmd);
        }
    });

    btnUp.setClickHandler([](Button2 & b) {
       // Up
       nav.doNav(downCmd); // It's called downCmd because it decreases the index of an array. Visually that would mean the selector goes upwards.
    });

    btnDwn.setLongClickHandler([](Button2 & b) {
        // Exit
        unsigned int time = b.wasPressedFor();
        if (time >= 1000) {
          nav.doNav(escCmd);
        }
    });

    btnDwn.setClickHandler([](Button2 & b) {
        // Down
        nav.doNav(upCmd); // It's called upCmd because it increases the index of an array. Visually that would mean the selector goes downwards.
    });
}

void button_loop()
{
    // Check for button presses
    btnUp.loop();
    btnDwn.loop();
}

void setup() {
  //options=&myOptions;//can customize options
  Serial.begin(115200); // Set Serial baudrate at 115200
  while(!Serial);
  Serial.flush();
  Serial.println();
  Serial.print("ARDUINOMENU DEMO V");
  Serial.print(PROG_VERSION);
  Serial.println(" FOR LILYGO TTGO T-DISPLAY");

  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].disable();
  //outGfx.usePreview=true;//reserve one panel for preview?
  //nav.showTitle=false;//show menu title?

  //SPI.begin(); // Leave this commented or else there will be nothing shown on the screen.
  gfx.init(); // Initialize the screen.

  Serial.print("Configuring PWM for TFT backlight... ");
  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  Serial.println("DONE");

  Serial.print("Setting PWM for TFT backlight to default intensity... ");
  ledcWrite(pwmLedChannelTFT, ledBacklight);
  Serial.println("DONE");

  gfx.setRotation(1); // Rotate display a quarter clockwise

  gfx.setTextSize(2);
  gfx.setTextWrap(false);
  gfx.fillScreen(Black);

  Serial.print("Showing bootlogo... ");
  gfx.setSwapBytes(true);
  gfx.pushImage(0, 0,  240, 135, bootlogo);
  delay(1000);
  Serial.println("DONE");

  Serial.print("Initialize buttons... ");
  button_init();
  delay(1000);
  Serial.println("DONE");

  Serial.print("Initialize external led... ");
  if(LED_CTRL > 0){
    pinMode(LED_CTRL,OUTPUT);
    digitalWrite(LED_CTRL,ledCtrl);
    delay(500);
    Serial.println("DONE");
  }
  else{
    delay(500);
    Serial.println("NOT SET UP");
  }

  delay(2000); // A little bit more delay so that you will be able to see the bootlogo.

  Serial.println("- READY -");

  gfx.fillScreen(Black); // Clear the screen to be ready to draw the menu
}

void loop() {
  button_loop();
  nav.poll();//this device only draws when needed

  // External connected led
  if(LED_CTRL > 0){ // Only set led state when a pinnumber for the external led has been provided.
    digitalWrite(LED_CTRL,ledCtrl);
  }

  // Set blacklight intenisty of screen
  ledcWrite(pwmLedChannelTFT, ledBacklight);
}
