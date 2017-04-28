#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>

using namespace Menu;

//define "Op 1"
void op1Func();
const char op1Text[] PROGMEM="Op 1";
const promptShadowRaw op1InfoRaw PROGMEM={(callback)op1Func,_noStyle,op1Text,enterEvent,noStyle};
const promptShadow& op1Info=*(promptShadow*)&op1InfoRaw;
//or just this line on non PROGMEM devices like teensy or esp8266 instead of the above three
//promptShadow op1Info("Op 1",(callback)op1Func,enterEvent);
prompt op1(op1Info);

//define "Op 2"
void op2Func();
const char op2Text[] PROGMEM="Op 2";
const promptShadowRaw op2InfoRaw PROGMEM={(callback)op2Func,_noStyle,op2Text,enterEvent,noStyle};
const promptShadow& op2Info=*(promptShadow*)&op2InfoRaw;
//or just this line on non PROGMEM devices like teensy or esp8266 instead of the above three
//promptShadow op2Info("Op 2",(callback)op2Func,enterEvent);
prompt op2(op2Info);

//define the menu
prompt* const menuData[] PROGMEM={&op1,&op2};
//or just prompt* menuData[]={&op1,&op2}; on non avr devices
const char menuTitle[] PROGMEM="Main menu";
const menuNodeShadowRaw menuInfoRaw PROGMEM={
  doNothing,
  (systemStyles)(_menuData|_canNav),
  menuTitle,
  noEvent,
  wrapStyle,
  2,
  menuData
};
const menuNodeShadow& menuInfo=*(menuNodeShadow*)&menuInfoRaw;
//or just this line on non PROGMEM devices like teensy or esp8266 instead of the above three
//menuNodeShadow menuInfo("Main menu",2,menuData,(callback)doNothing,noEvent,wrapStyle);
menuNode mainMenu(menuInfo);

#define MAX_DEPTH 1

//define output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(Serial,serialTops);

//define outputs controller
menuOut* const outputs[] MEMMODE={&outSerial};//list of output devices
outputsList out(outputs,1);//outputs list controller

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH-1, Serial, out);

//implement the menu actions
void op1Func() {Serial.println("Op 1 executed");}
void op2Func() {Serial.println("Op 2 executed");}

#include <Streaming.h>

/////////////////////////////////////////////////////////////////////////////
// arduiin osketch
void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu initialized without macros.");
  /*Serial<<"active:";
  Serial<<*(prompt*)&nav.active();
  Serial<<endl;
  Serial<<"maxDepth:"<<nav.maxDepth<<endl;
  Serial<<"Selected:";
  Serial<<(*(prompt*)&nav.selected());
  Serial<<endl;
  Serial<<"active dirty:"<<nav.active().dirty<<endl;*/
  // Serial<<":"<<nav.()<<endl;
}

void loop() {
  nav.poll();
}
