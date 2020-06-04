#include <Arduino.h>

/********************
Sept. 2014 ~ Oct 2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Oct. 2018 - modified by Kevin O'rourke to use interrupts on pins for the sam3x

menu with the newer adafruit GFX
output: 1.8" TFT 128*160 (ST7735 HW SPI) or
output: 1.8" TFT 128*160 (ST7735 SOFT SPI)

input: Serial + interrupts
www.r-site.net

***/

//removed as extra libs are not included into travis
// #define USE_CLICK_ENCODER
#define USE_INTERRUPT_PINS // using a joy button
#define DONT_SHOW_SUSPEND // dont show the suspended screen

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_ST7735.h>
#include <menu.h>
#include <menuIO/adafruitGfxOut.h>
#ifndef USE_INTERRUPT_PINS
  #ifdef USE_CLICK_ENCODER
    #include <TimerOne.h>
    #include <ClickEncoder.h>
    #include <menuIO/clickEncoderIn.h>
  #else
    #include <menuIO/encoderIn.h>
  #endif
  #include <menuIO/keyIn.h>
  #include <menuIO/chainStream.h>
  #include <menuIO/serialOut.h>
  #include <menuIO/serialIn.h>
#else
  #include <menuIO/interruptPins.h>
  #include <menuIO/chainStream.h>
  #include <menuIO/serialOut.h>
  #include <menuIO/serialIn.h>

#endif


using namespace Menu;

//ST7735 1.8TFT 128x160
//#define TFT_CS   A1
//#define TFT_DC   A0
//#define TFT_RST  A2



void wait_for(uint32_t  wait) {
  uint64_t  myMillis = wait * 11986;   // approx 11986000 nop nop per sec
  for (uint64_t  Counting = 0; Counting <= myMillis; Counting++) {
    __asm__("nop\n\t");
  }
}


#define SPI_SCK   13
#define SPI_DI    12
#define SPI_DO    11
#define TFT_CS    15
#define TFT_RST    0
#define TFT_DC     8

//Adafruit_ST7735 gfx(TFT_CS, TFT_DC, TFT_RST); // hardware if possible

Adafruit_ST7735 gfx = Adafruit_ST7735(TFT_CS, TFT_DC, SPI_DO, SPI_SCK, TFT_RST);// software spi on the due

#define LEDPIN LED_BUILTIN

#ifndef USE_INTERRUPT_PINS
  // rotary encoder pins
  #define encA    2
  #define encB    3
  #define encBtn  4
#else
  // interrupt pins sam3x with joy button
  #define BTN_UP     2
  #define BTN_DOWN   3
  #define BTN_SEL    5
  #define BTN_LEFT   6
  #define BTN_RIGHT  7  
#endif

result doAlert(eventMask e, prompt &item);

int test=55;

int ledCtrl=LOW;

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
class confirmExit:public menu {
public:
  confirmExit(constMEM menuNodeShadow& shadow):menu(shadow) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t p) override {
      if(idx<0){menu::printTo(root,sel,out,idx,len,p);}
      else{out.printRaw((constText*)F("Exit"),len);}
      return idx;
  }
};
class altPrompt:public prompt {
public:
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);;
  }
};

result systemExit();
altMENU(confirmExit,subMenu2,"Exit?",doNothing,noEvent,wrapStyle,(Menu::_menuData|Menu::_canNav)
  ,OP("Yes",systemExit,enterEvent)
  ,EXIT("Cancel")
);

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,EXIT("<Back")
);

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,OP("Op2",doNothing,noEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",myLedOn,enterEvent)
  ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  //,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  //,EXIT("<Back")
  ,SUBMENU(subMenu2)
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table

#define ST7735_GRAY RGB565(128,128,128)

const colorDef<uint16_t> colors[6] MEMMODE={
  {{ST7735_BLACK,ST7735_BLACK},{ST7735_BLACK,ST7735_BLUE,ST7735_BLUE}},//bgColor
  {{ST7735_GRAY,ST7735_GRAY},{ST7735_WHITE,ST7735_WHITE,ST7735_WHITE}},//fgColor
  {{ST7735_WHITE,ST7735_BLACK},{ST7735_YELLOW,ST7735_YELLOW,ST7735_RED}},//valColor
  {{ST7735_WHITE,ST7735_BLACK},{ST7735_WHITE,ST7735_YELLOW,ST7735_YELLOW}},//unitColor
  {{ST7735_WHITE,ST7735_GRAY},{ST7735_BLACK,ST7735_BLUE,ST7735_WHITE}},//cursorColor
  {{ST7735_WHITE,ST7735_YELLOW},{ST7735_BLUE,ST7735_RED,ST7735_RED}},//titleColor
};

serialIn serial(Serial);

#ifndef USE_INTERRUPT_PINS
  #ifdef USE_CLICK_ENCODER
    ClickEncoder clickEncoder(encA,encB,encBtn);
    ClickEncoderStream encStream(clickEncoder,1);
    MENU_INPUTS(in,&encStream,&serial);
    void timerIsr() {clickEncoder.service();}
  #else
    encoderIn<encA,encB> encoder;//simple quad encoder driver
    encoderInStream<encA,encB> encStream(encoder,4);// simple quad encoder fake Stream
    //a keyboard with only one key as the encoder button
    keyMap encBtn_map[]={{-encBtn,defaultNavCodes[enterCmd].ch}};//negative pin numbers use internal pull-up, this is on when low
    keyIn<1> encButton(encBtn_map);//1 is the number of keys
    MENU_INPUTS(in,&encStream,&encButton,&serial);
  #endif
#else
  encoderIn<BTN_UP,BTN_DOWN,BTN_SEL,BTN_LEFT,BTN_RIGHT> encoder;//simple quad encoder driver
  encoderInStream<BTN_UP,BTN_DOWN,BTN_SEL,BTN_LEFT,BTN_RIGHT> encStream(encoder);// simple quad encoder fake Stream
  MENU_INPUTS(in,&encStream,&serial);
#endif

#define MAX_DEPTH 4
#define textScale 1
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(gfx,colors,6*textScale,9*textScale,{0,0,14,8},{14,0,14,8})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
#ifndef DONT_SHOW_SUSPEND 
  if (e==idling) {
    o.println(F("suspended..."));
    o.println(F("press [select]"));
    o.println(F("to continue"));
  }
#endif  
  return proceed;
}

result systemExit() {
  encoder.menu_running = false;//prevents the menu from running again!
  nav.reset();
  nav.idleOn();//suspend the menu system
  gfx.fillScreen(ST7735_BLACK);
  return quit;
}
//config myOptions('*','-',defaultNavCodes,false);

void setup() {
  //options=&myOptions;//can customize options
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(211000);
  while(!Serial);
  Serial.println("menu 4.x test");
  Serial.flush();

  //outGfx.usePreview=true;//reserve one panel for preview?
  //nav.showTitle=false;//show menu title?

  //pinMode(encBtn, INPUT_PULLUP);
#ifndef USE_INTERRUPT_PINS  
  #ifdef USE_CLICK_ENCODER
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
  #else
    encButton.begin();
    encoder.begin();
  #endif
  nav.idleTask=idle;//point a function to be used when menu is suspended
#else
  encoder.begin();
  nav.idleOn(); // suspend the menu system
#endif
  mainMenu[0].disable();
  SPI.begin();
  gfx.initR(INITR_BLACKTAB);
  gfx.setRotation(3);
  gfx.setTextSize(textScale);//test scalling
  gfx.setTextWrap(false);
  gfx.fillScreen(ST7735_BLACK);
  gfx.setTextColor(ST7735_RED,ST7735_BLACK);
  gfx.println("Menu 4.x test on GFX");
  delay(2000);
  gfx.fillScreen(ST7735_BLACK);
}

void loop() {

#ifndef USE_INTERRUPT_PINS
   nav.poll();//this device only draws when needed
   delay(100);//simulate a delay when other tasks are done
#else
 if((encoder.menu_running)&&(encoder.menu_interrupt)){
   encoder.menu_interrupt = false;
   nav.doInput();
   nav.doOutput(); 
   }
#endif
  digitalWrite(LEDPIN, ledCtrl);
}
