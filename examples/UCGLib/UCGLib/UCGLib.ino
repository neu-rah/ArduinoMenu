/********************
Arduino generic menu system
UCGLib menu example
UCGLib: https://github.com/olikraus/Ucglib_Arduino

Sep. 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu on UCGLib device
output: ST7735
input: Serial
mcu: nano328p
*/

#include <SPI.h>
#include <Ucglib.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/UCGLibOut.h>

using namespace Menu;

#define UC_Width 160
#define UC_Height 128

#ifdef ESP8266
  #define UC_CS   2
  #define UC_DC   3
  #define UC_RST  4
#else
  #define UC_CS   A1
  #define UC_DC   A0
  #define UC_RST  A2
#endif

//font size plus margins
#define fontX 6
#define fontY 11

Ucglib_ST7735_18x128x160_HWSPI ucg(UC_DC , UC_CS, UC_RST);

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
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

//define colors
#define BLACK {0,0,0}
#define BLUE {0,0,255}
#define GRAY {128,128,128}
#define WHITE {255,255,255}
#define YELLOW {255,255,0}
#define RED {255,0,0}

const colorDef<rgb> my_colors[6] {
  {{BLACK,BLACK},{BLACK,BLUE,BLUE}},//bgColor
  {{GRAY,GRAY},{WHITE,WHITE,WHITE}},//fgColor
  {{WHITE,BLACK},{YELLOW,YELLOW,RED}},//valColor
  {{WHITE,BLACK},{WHITE,YELLOW,YELLOW}},//unitColor
  {{WHITE,GRAY},{BLACK,BLUE,WHITE}},//cursorColor
  {{WHITE,YELLOW},{BLUE,RED,RED}},//titleColor
};

#define offsetX 0
#define offsetY 0

MENU_OUTPUTS(out,MAX_DEPTH
  ,UCG_OUT(ucg,my_colors,fontX,fontY,offsetX,offsetY,{0,0,UC_Width/fontX,UC_Height/fontY})
  ,SERIAL_OUT(Serial)
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
idx_t ucg_tops[MAX_DEPTH];
PANELS(ucgPanels,{0,0,UC_Width/fontX,UC_Height/fontY});
UCGLibOut ucgOut(ucg,my_colors,ucg_tops,ucgPanels,fontX,fontY);

menuOut* const outputs[] MEMMODE={&outSerial,&ucgOut};//list of output devices
outputsList out(outputs,2);//outputs list controller

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, Serial, out);*/

void setup(void)
{
  Serial.begin(115200);
  while(!Serial);
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.clearScreen();
  ucg.setFont(ucg_font_courB08_tr);//choose fized width font (monometric)
  ucg.setRotate90();
  ucg.setColor(255,255,255);
  ucg.setFontPosTop();
  ucg.print("Menu 4.x ucg");
  delay(1000);
  ucg.clearScreen();
  ucg.setRotate90();
  ucg.setFontPosBottom();
}

void loop(void)
{
  nav.poll();
}
