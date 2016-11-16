#include <Arduino.h>

/********************
Arduino generic menu system
U8GLib menu example
U8Glib: https://github.com/olikraus/U8glib_Arduino

Jul.2016 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
Original from: https://github.com/christophepersoz
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu on U8GLib device
output: Nokia 5110 display (PCD8544 HW SPI) + Serial
input: Serial + encoder

*/

#include <U8glib.h>
#include <menu.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/U8GLibOut.h>

using namespace Menu;

#define LEDPIN A3

// rotary encoder pins
#define encA    2
#define encB    3
#define encBtn  4

#define U8_DC 9
#define U8_CS 8
#define U8_RST 7

U8GLIB_PCD8544 u8g(U8_CS, U8_DC, U8_RST) ;

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

int ledCtrl=LOW;

result ledOn() {
  ledCtrl=HIGH;
  return proceed;
}
result ledOff() {
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
    o<<F("alert test")<<endl<<F("press [select]")<<endl<<F("to continue...");
  }
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",zZz,noEvent,wrapStyle
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

encoderIn<encA,encB> encoder;//simple quad encoder driver
encoderInStream<encA,encB> encStream(encoder,4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,options->getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
Stream* inputsList[]={&encStream,&encButton,&Serial};
chainStream<3> in(inputsList);//3 is the number of inputs

#define fontX 6
#define fontY 8
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
  ,U8GLIB_OUT(u8g,colors,fontX,fontY,{0,0,84/fontX,48/fontY})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling)
    o<<F("suspended")<<endl<<F("Press [select]")<<endl<<F("to continue");
  return proceed;
}

void setup() {
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial<<F("menu 3.0 test")<<endl;Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;

  pinMode(encBtn, INPUT_PULLUP);
  encoder.begin();

  //u8g.setFont(u8g_font_helvR08);
  //u8g.setFont(u8g_font_6x10);
  u8g.setFont(u8g_font_04b_03r);
  u8g.firstPage();
  do {
    nav.out[0].setCursor(0,0);
    nav.out[0]<<F("Menu 3.x test");
    nav.out[0].setCursor(0,1);
    nav.out[0]<<F("on U8Glib");
  } while(u8g.nextPage());
  delay(1000);
  //nav.sleepTask=alert;
}

void loop() {
  Serial<<".";
  nav.doInput();
  if (nav.changed(0)) {//only draw if menu changed for gfx device
    //change checking leaves more time for other tasks
    u8g.firstPage();
    do {
      nav.doOutput();
      digitalWrite(LEDPIN, ledCtrl);
    } while( u8g.nextPage() );
  }
  delay(100);//simulate a delay when other tasks are running
}
