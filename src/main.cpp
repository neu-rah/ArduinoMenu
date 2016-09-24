#include <Streaming.h>
//#include <AnsiStream.h>
#include <Stream.h>
#include <HardwareSerial.h>
#include "menu.h"
#include "Dump.h"
#include "dev/serialOut.h"

using namespace Menu;

#define LEDPIN A4

serialOut out(Serial);

result zZz() {Serial<<"zZz"<<endl;return proceed;}

result showEvent(eventMask e,navNode& nav,prompt& item) {
  //printEventName(e);
  nav.root->out<<e<<" on "<<item<<endl;
  return proceed;
}

int test=55;

result action1(eventMask e) {
  //printEventName(e);
  Serial<<e<<" action1 executed, proceed menu"<<endl;Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item, Stream &in, menuOut &out) {
  //printEventName(e);
  Serial<<item<<" "<<e<<" action2 executed, quiting menu"<<endl;
  return quit;
}

int ledCtrl=LOW;

result ledOn() {
  //Serial<<"ledOn"<<endl;
  ledCtrl=HIGH;
  return proceed;
}
result ledOff() {
  //Serial<<"ledOff"<<endl;
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
CHOOSE(chooseTest,chooseMenu,"Choose ",doNothing,noEvent,noStyle
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
  void printTo(idx_t i,navNode &nav,menuOut& out) override {
    out<<"special prompt!";
  }
};

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
  ,OP("Sub1",showEvent,anyEvent)
  ,OP("Sub2",showEvent,anyEvent)
  ,OP("Sub3",showEvent,anyEvent)
  ,altOP(altPrompt,"",showEvent,anyEvent)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",zZz,noEvent,wrapStyle
  ,OP("Op1",action1,anyEvent)
  ,OP("Op2",action2,enterEvent)
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
  ,OP("LED On",ledOn,enterEvent)
  ,OP("LED Off",ledOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,EXIT("<Back")
);

panel panels[]{{0,0,10,10},{10,0,10,10}};
NAVROOT(nav,mainMenu,2,Serial,out);

result idle(idleEvent e) {
  switch(e) {
    case idleStart:Serial<<"suspending menu!"<<endl;break;
    case idling:Serial<<"suspended..."<<endl;break;
    case idleEnd:Serial<<"resuming menu."<<endl;break;
  }
  return proceed;
}

/*const char title_x[] MEMMODE="Test option title";

const MEMMODE promptShadowRaw opShadowRaw_x={(callback)doNothing,title_x,anyEvent};
const promptShadow& opShadow_x=*(promptShadow*)&opShadowRaw_x;
prompt op_x(opShadow_x);*/

//const panel panels[] MEMMODE={{0,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10},{10,0,10,10}};

class memTest {
  public:
    int values[100];
    memTest(int x) {values[0]=x;}
};

const MEMMODE
memTest t(1967);

class memRef {
public:
  const memTest* mt;
  memRef(const memTest& m):mt(&m) {}
};

memRef mr(t);

void setup() {
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,HIGH);
  delay(500);
  Serial.begin(115200);
  while(!Serial);
  /*Serial
    <<ANSI::setBackgroundColor(BLUE)
    <<ANSI::fill(0,0,79,24)
    <<ANSI::home();*/
  Serial<<"menu 3.0 test"<<endl;Serial.flush();
  /*Serial<<"test:"<<mr.mt->values[0]<<endl;
  Serial<<"panels:"<<(sizeof(panels)/sizeof(panel))
    <<"{"<<panels[0].x
    <<","<<panels[0].y
    <<","<<panels[0].w
    <<","<<panels[0].h
    <<"}"<<endl;*/

  Serial<<"promptShadowRaw:"<<sizeof(promptShadowRaw)<<endl;
  Serial<<"menuNodeShadowRaw:"<<sizeof(menuNodeShadowRaw)<<endl;
  Serial<<"menuFieldShadowRaw:"<<sizeof(menuFieldShadowRaw<int>)<<endl;
  Serial<<"menuValueShadowRaw:"<<sizeof(menuValueShadowRaw<int>)<<endl;
  Serial<<"menuVariantShadowRaw:"<<sizeof(menuVariantShadowRaw<int>)<<endl;
  Serial<<"systemStyles:"<<sizeof(systemStyles)<<endl;

  //nav.poll();
  //Serial<<nav.changed()<<endl;

  Serial<<"end test --------------"<<endl;Serial.flush();
  options.idleTask=idle;
}

inline int secs() {return millis()/1000;}
//bool done=false;

void loop() {
  nav.poll();
  digitalWrite(LEDPIN, ledCtrl);
  //digitalWrite(LEDPIN,!digitalRead(LEDPIN));
/*  if (!(secs()%3)) {// test reflexivity
    if (!done) {
      done=true;
      Serial<<"changing..."<<secs()<<endl;
      selTest=secs()%2;
      chooseTest=secs()%2+1;
      test=10*(secs()%10);
    }
  } else done=false;*/
  delay(100);
}
