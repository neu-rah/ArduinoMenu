#include <Arduino.h>
#include <Streaming.h>

/********************
Arduino generic menu system
U8G2 menu example
U8G2: https://github.com/olikraus/u8g2

Oct. 2016 Stephen Denne https://github.com/datacute
Based on example from Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu on U8G2 device
output: Wemos D1 mini OLED Shield (SSD1306 64x48 I2C) + Serial
input: Serial
platform: espressif8266

*/

#ifdef ESP8266
  #define typeof(x) __typeof__(x)
  #include <SPI.h>
  #include <Wire.h>
#endif

#include <menu.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/u8g2Out.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA);

result zZz() {Serial<<"zZz"<<endl;return proceed;}

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial<<e<<" on "<<item<<endl;
  return proceed;
}

int test=55;

result action1(eventMask e) {
  Serial<<e<<" action1 executed, proceed menu"<<endl;Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<item<<" "<<e<<" action2 executed, quiting menu"<<endl;
  return quit;
}

int ledCtrl=HIGH;

result ledOn() {
  ledCtrl=LOW;
  return proceed;
}
result ledOff() {
  ledCtrl=HIGH;
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
  altPrompt(const promptShadow& p):prompt(p) {}
  idx_t printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len) override {
    return out.printRaw("special prompt!",len);;
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o<<"alert test"<<endl<<"press [select]"<<endl<<"to continue..."<<endl;
  }
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",zZz,noEvent,noStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
// this is a monochromatic color table
const colorDef<uint8_t> colors[] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,0},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{0,0},{1,1,1}},//titleColor
};

Stream* inputsList[]={&Serial};
chainStream<1> in(inputsList);//1 is the number of inputs

#define fontName u8g2_font_5x7_tf
#define fontX 5
#define fontY 8
#define offsetX 32
#define offsetY 16
#define MAX_DEPTH 2

/*const panel serial_panels[] MEMMODE={{0,0,40,10}};
navNode* serial_nodes[sizeof(serial_panels)/sizeof(panel)];
panelsList serial_pList(serial_panels,serial_nodes,sizeof(serial_panels)/sizeof(panel));
idx_t serial_tops[MAX_DEPTH];
serialOut outSerial(Serial,serial_tops,serial_pList);//the output device (just the serial port)

const panel panels[] MEMMODE={{0,0,84/fontX,48/fontY}};
navNode* nodes[sizeof(panels)/sizeof(panel)];
panelsList pList(panels,nodes,sizeof(panels)/sizeof(panel));
idx_t gfx_tops[MAX_DEPTH];
u8gLibOut gfx(u8g,colors,gfx_tops,pList,fontX,fontY);

menuOut* const outputs[] MEMMODE={&gfx,&outSerial};
outputsList out(outputs,2);*/

//this macro replaces all the above commented lines
MENU_OUTPUTS(out,MAX_DEPTH
  ,U8G2_OUT(u8g2,colors,fontX,fontY,offsetX,offsetY,{0,0,64/fontX,48/fontY})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  o.clear();
  switch(e) {
    case idleStart:o<<"suspending menu!"<<endl;break;
    case idling:o<<"suspended..."<<endl;break;
    case idleEnd:o<<"resuming menu."<<endl;break;
  }
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  u8g2_SetI2CAddress(u8g2.getU8g2(), 0x3d*2);
  u8g2.begin();
  u8g2.setFont(fontName);

  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;
}

void loop() {
  nav.doInput();
  if (nav.changed(0)) {//only draw if menu changed for gfx device
    //change checking leaves more time for other tasks
    u8g2.firstPage();
    do {
      nav.doOutput();
      digitalWrite(LEDPIN, ledCtrl);
    } while(u8g2.nextPage());
  }
  delay(100);//simulate a delay when other tasks are running
}
