/********************
Arduino generic menu system
U8x8 menu example
U8x8: https://github.com/olikraus/Ucglib_Arduino

Sep. 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu on U8x8 device
output: PCD8544 (Nokia 5110)
input: Serial
mcu: nano328p

*/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/U8x8Out.h>
#include <U8x8lib.h>

using namespace Menu;

#define LEDPIN LED_BUILTIN

#define U8_DC 9
#define U8_CS 8
#define U8_RST 7
#define U8_Width 84
#define U8_Height 48

U8X8_PCD8544_84X48_4W_HW_SPI u8x8(U8_CS, U8_DC , U8_RST);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";//<-- menu will edit this text

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,SUBMENU(chooseMenu)
  ,EXIT("<Back")
);

#define MAX_DEPTH 2

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,U8X8_OUT(u8x8,{0,0,10,6})
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

//initializing output and menu nav without macros
/*const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
navNode* default_serial_nodes[sizeof(default_serial_panels)/sizeof(panel)];
panelsList default_serial_panel_list(
  default_serial_panels,
  default_serial_nodes,
  sizeof(default_serial_panels)/sizeof(panel)
);

//define output device
idx_t serialTops[MAX_DEPTH]={0};
serialOut outSerial(*(Print*)&Serial,serialTops);

//define outputs controller
idx_t u8x8_tops[MAX_DEPTH];
PANELS(u8x8Panels,{0,0,U8_Width/8,U8_Height/8});
U8x8Out u8x8Out(u8x8,u8x8_tops,u8x8Panels);

menuOut* const outputs[] MEMMODE={&outSerial,&u8x8Out};//list of output devices
outputsList out(outputs,2);//outputs list controller

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, Serial, out);*/

void setup(void)
{
  pinMode(LEDPIN,OUTPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x test");
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"Menu 4.x");
  delay(1000);
}

void loop(void)
{
  nav.poll();
  delay(100);
}
