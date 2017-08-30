#include <menu.h>
#include <menuIO/serialOut.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

//examples of validating character sequences
char* const sigNum PROGMEM="+-0123456789.";
char* const binary PROGMEM="01";
char* const numeric PROGMEM="0123456789";
char* const hexChars PROGMEM="0123456789ABCDEF";
char* const letters PROGMEM="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char* const alphaNum PROGMEM="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
char* const names PROGMEM=" 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_!#@$%&/()=+-*^~:.[]{}";

//alternative were validating functions... decided to use strings because they are easier for users to setup

//define "Op 1"
char* const validData[] PROGMEM={hexChars,hexChars};//individual character validators
const char op1Text[] PROGMEM="Name";//field name
const textFieldShadowRaw op1InfoRaw PROGMEM={doNothing,_noStyle|_canNav,op1Text,enterEvent,noStyle,2,validData};//PROGMEM static stuff
const textFieldShadow& op1Info=*(textFieldShadow*)&op1InfoRaw;//hacking c++ to use progmem (hugly)
textField op1("AA",op1Info);//text length not enforced to match validators length yet
//TODO: consider also a single validator for all field, either as a special case of this or as a separate class

//define "Op 2"
void op2Func();
const char op2Text[] PROGMEM="Op 2";
const promptShadowRaw op2InfoRaw PROGMEM={(callback)op2Func,_noStyle,op2Text,enterEvent,noStyle};
const promptShadow& op2Info=*(promptShadow*)&op2InfoRaw;
prompt op2(op2Info);

//define the menu
prompt* const menuData[] PROGMEM={&op1,&op2};
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
menuNode mainMenu(menuInfo);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

//implement the menu actions
//void op1Func() {Serial.println("Op 1 executed");}
void op2Func() {Serial.println("Op 2 executed");}

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("menu 3.x text field test");Serial.flush();
}

void loop() {
  nav.poll();
  delay(100);//simulate a delay when other tasks are done
}
