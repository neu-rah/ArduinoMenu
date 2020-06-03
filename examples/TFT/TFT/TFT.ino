/********************
Arduino generic menu system
Arduino TFT menu example
Arduino TFT: https://www.arduino.cc/en/Guide/TFT

Sep. 2017 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

menu on TFT device
output: ST7735
input: Serial
MCU: see TFT library compatibility
*/

#include <TFT.h> // Hardware-specific library
#include <SPI.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/tftOut.h>

using namespace Menu;

//display size
#define TFT_Width 160
#define TFT_Height 128

//font size plus margins
#define fontX 6
#define fontY 9

#define CS   A1
#define DC   A0
#define RST  A2

TFT tft(CS, DC, RST);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";//<-- menu will edit this text

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,OP("Op1",doNothing,noEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

#define MAX_DEPTH 1

#define ST7735_GRAY RGB565(128,128,128)

const colorDef<uint16_t> colors[6] MEMMODE={
  {{ST7735_BLACK,ST7735_BLACK},{ST7735_BLACK,ST7735_BLUE,ST7735_BLUE}},//bgColor
  {{ST7735_GRAY,ST7735_GRAY},{ST7735_WHITE,ST7735_WHITE,ST7735_WHITE}},//fgColor
  {{ST7735_WHITE,ST7735_BLACK},{ST7735_YELLOW,ST7735_YELLOW,ST7735_RED}},//valColor
  {{ST7735_WHITE,ST7735_BLACK},{ST7735_WHITE,ST7735_YELLOW,ST7735_YELLOW}},//unitColor
  {{ST7735_WHITE,ST7735_GRAY},{ST7735_BLACK,ST7735_BLUE,ST7735_WHITE}},//cursorColor
  {{ST7735_WHITE,ST7735_YELLOW},{ST7735_BLUE,ST7735_RED,ST7735_RED}},//titleColor
};

//initializing output and menu nav without macros
const panel default_serial_panels[] MEMMODE={{0,0,40,10}};
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
PANELS(ucgPanels,{0,0,TFT_Width/fontX,TFT_Height/fontY});
TFTOut tftOut(tft,colors,ucg_tops,ucgPanels,fontX,fontY);

menuOut* const outputs[] MEMMODE={&outSerial,&tftOut};//list of output devices
outputsList out(outputs,2);//outputs list controller

serialIn serial(Serial);

//define navigation root and aux objects
navNode nav_cursors[MAX_DEPTH];//aux objects to control each level of navigation
navRoot nav(mainMenu, nav_cursors, MAX_DEPTH, serial, out);

void setup(){
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Menu 4.x");
  tft.begin();
  tft.background(0,0,0);  // clear the screen with black
  tft.setRotation(3);
  tft.setTextWrap(false);
  tft.println("Menu 4.x");
  delay(1000);  // pause for dramatic effect
}

void loop(){
  nav.poll();
}
