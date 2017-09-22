#include <menu.h>
#include <menuIO/serialOut.h>
//#include <Dump.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

//list of allowed characters
char* const  digit="0123456789";
char* const hexChars PROGMEM="0123456789ABCDEF";
char* const alphaNum[] PROGMEM = {" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
//individual character validators
char* constMEM validData[] PROGMEM={hexChars,hexChars,hexChars,hexChars};

char* constMEM validIP[] PROGMEM = {" 012",digit,digit,"."};
char buf0[]="   .   .   .   ";

//define "Op 0" without macro
constMEM char op1Text[] PROGMEM="IP";//field name
constMEM textFieldShadowRaw op1InfoRaw PROGMEM={
  (callback)doNothing,
  (Menu::systemStyles)(_noStyle|_canNav|_parentDraw),
  op1Text,
  enterEvent,
  noStyle,
  buf0,//edit buffer
  validIP,
  4
};//PROGMEM static stuff
constMEM textFieldShadow& op1Info=*(textFieldShadow*)&op1InfoRaw;//hacking c++ to use progmem (hugly)
textField option0(op1Info);

char buf1[]="1111";
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OBJ(option0)
  ,EDIT("Hex",buf1,validData,doNothing,noEvent,noStyle)
  ,EXIT("<Exit")
);

#define MAX_DEPTH 1

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,Serial,out);

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
