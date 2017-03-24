#include <Arduino.h>

#include <menu.h>
#include <menuIO/serialOut.h>

#include <Streaming.h>

using namespace Menu;

//action functions
void op1Func();
void op2Func();

//define "Op 1"
promptShadow op1Info("Op 1",(callback)op1Func,enterEvent);
prompt op1(op1Info);

//define "Op 2"
promptShadow op2Info("Op 2",(callback)op2Func,enterEvent);
prompt op2(op2Info);

//define the menu
prompt* menuData[]={&op1,&op2};
menuNodeShadow menuInfo("Main menu",2,menuData,(callback)doNothing,noEvent,wrapStyle);
menuNode mainMenu(menuInfo);

#define MAX_DEPTH 1
MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

void op1Func() {Serial.println("Op 1 executed");}
void op2Func() {Serial.println("Op 2 executed");}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 3.0 test");
  Serial<<"using "<<libMemMode<<endl;
  mainMenu[0].shadow->text="Changed";
  Serial.println(mainMenu[0].getText());
  Serial<<"Sz:"<<mainMenu.sz()<<endl;
  Serial.flush();
  //pinMode(encBtn, INPUT_PULLUP);
}

void loop() {
  nav.poll();//this device only draws when needed
  delay(100);//simulate a delay when other tasks are done
}
