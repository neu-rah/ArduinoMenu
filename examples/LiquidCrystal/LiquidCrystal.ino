/********************
Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com
creative commons license 3.0: Attribution-ShareAlike CC BY-SA
This software is furnished "as is", without technical support, and with no
warranty, express or implied, as to its usefulness for any purpose.

Thread Safe: No
Extensible: Yes

menu output to standard arduino LCD (LiquidCrystal)
output: LCD
input: encoder and Serial
www.r-site.net
***/

#include <menu.h>
#include <menuIO/liquidCrystalOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;

// LCD /////////////////////////////////////////
#define RS 8
#define RW 3
#define EN 9
LiquidCrystal lcd(RS, RW, EN, 4, 5, 6, 7);

// Encoder /////////////////////////////////////
#define encA A2
#define encB 2
//this encoder has a button here
#define encBtn A3

encoderIn<encA,encB> encoder;//simple quad encoder driver
encoderInStream<encA,encB> encStream(encoder,4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-encBtn,options->getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
Stream* inputsList[]={&encStream,&encButton,&Serial};
chainStream<3> in(inputsList);//3 is the number of inputs

#define LEDPIN 13

result showEvent(eventMask e,navNode& nav,prompt& item) {
  Serial<<e<<" on "<<item<<endl;
  return proceed;
}

int test=55;

result action1(eventMask e,navNode& nav, prompt &item) {
  Serial<<e<<" event on "<<item<<", proceed menu"<<endl;
  Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  Serial<<item<<" "<<e<<" event on "<<item<<", quiting menu."<<endl;
  Serial.flush();
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

/*extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);*/

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o<<"alert test";
    o.setCursor(0,1);
    o<<"[select] to continue...";
  }
  return proceed;
}

result doAlert(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  nav.root->idleOn(alert);
  return proceed;
}

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  //,SUBMENU(togOp)
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

//const panel panels[] MEMMODE={{0,0,16,2}};
//navNode* nodes[sizeof(panels)/sizeof(panel)];
//panelsList pList(panels,nodes,1);

#define MAX_DEPTH 2
/*idx_t tops[MAX_DEPTH];
liquidCrystalOut outLCD(lcd,tops,pList);//output device for LCD
menuOut* outputs[]={&outLCD};//list of output devices
outputsList out(outputs,1);//outputs list with 2 outputs*/

MENU_OUTPUTS(out, MAX_DEPTH
  ,LIQUIDCRYSTAL_OUT(lcd,{0,0,16,2})
  ,NONE
);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);//the navigation root object

result idle(menuOut& o,idleEvent e) {
  switch(e) {
    case idleStart:o<<"suspending menu!";break;
    case idling:o<<"suspended...";break;
    case idleEnd:o<<"resuming menu.";break;
  }
  return proceed;
}

void setup() {
  pinMode(encBtn,INPUT_PULLUP);
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial<<"Arduino Menu Library"<<endl;Serial.flush();
  encoder.begin();
  lcd.begin(16,2);
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].enabled=disabledStatus;
  nav.showTitle=false;
  lcd.setCursor(0, 0);
  lcd<<("Menu 3.0 LCD");
  lcd.setCursor(0, 1);
  lcd<<("r-site.net");
  delay(2000);
}

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay as if other tasks are running
}
