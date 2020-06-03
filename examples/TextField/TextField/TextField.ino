#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
//#include <Dump.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

//list of allowed characters
const char* const  digit="0123456789";
const char* const hexChars MEMMODE="0123456789ABCDEF";
const char* const alphaNum[] MEMMODE = {" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,+-_"};
//individual character validators
const char* constMEM validData[] MEMMODE={hexChars,hexChars,hexChars,hexChars};

const char* constMEM validIP[] MEMMODE = {"012",digit,digit,"."};
char buf0[]="000.000.000.000";

//define "Op 0" without macro
constMEM char op1Text[] MEMMODE="IP";//field name
constMEM textFieldShadowRaw op1InfoRaw MEMMODE={
  (callback)doNothing,
  (Menu::systemStyles)(_noStyle|_canNav|_parentDraw),
  op1Text,
  enterEvent,
  noStyle,
  buf0,//edit buffer
  validIP,
  4//this is the number of entries on validators array, not th efield length
};//MEMMODE static stuff
constMEM textFieldShadow& op1Info=*(textFieldShadow*)&op1InfoRaw;//hacking c++ to use progmem (hugly)
textField option0(op1Info);

char buf1[]="0000";
char name[]="                              ";//field will initialize its size by this string length
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OBJ(option0)
  ,EDIT("Hex",buf1,validData,doNothing,noEvent,noStyle)
  ,EDIT("Name",name,alphaNum,doNothing,noEvent,noStyle)
  ,EXIT("<Exit")
);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("menu 4.x text field test");Serial.flush();
  Serial.println(sizeof(validData)/sizeof(void*));
  /*Serial.println((uint16_t)buf1,HEX);
  Serial.println((uint16_t)(*(textField*)&mainMenu[1]).buffer(),HEX);
  Serial.println((uint16_t)((textFieldShadow*)mainMenu[1].shadow)->buffer,HEX);
  Serial.println((uint16_t)memPtr(((textFieldShadow*)mainMenu[1].shadow)->buffer),HEX);
  dumpRam(Serial,buf1,4);
  dumpPgm(Serial,buf1,4);
  dumpPgm(Serial,(textFieldShadowRaw*)mainMenu[1].shadow,sizeof(textFieldShadowRaw));*/
}

void loop() {
  nav.poll();
  delay(100);//delay to simulate other tasks
}
