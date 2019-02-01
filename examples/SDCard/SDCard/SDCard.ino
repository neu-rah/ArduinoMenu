#include <SPI.h>
#include <SD.h>
#include <menu.h>
#include <menuIO/serialIO.h>
#include <plugin/SDMenu.h>

using namespace Menu;

//from: https://www.arduino.cc/en/Tutorial/listfiles
// The circuit:
// * SD card attached to SPI bus as follows:
// ** MOSI - pin 11
// ** MISO - pin 12
// ** CLK - pin 13
// ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
//this is for my due tft+sd shield
#define SDCARD_SS 53

//function to handle file select
// declared here and implemented bellow because we need
// to give it as event handler for `sdFolder`
// and we also need to refer to `sdFolder` inside the function
result sdFolder(eventMask event, navNode& nav, prompt &item);

SDMenu<typeof(SD)> sdFolderMenu(SD,"SD Card","/",sdFolder,enterEvent);

//implementing the handler here after sdFolder is defined...
result sdFolder(eventMask event, navNode& nav, prompt &item) {
  switch(event) {
    case enterCmd:
      if (nav.root->navFocus==&sdFolderMenu) {
        Serial.println();
        Serial.print("selected file:");
        Serial.println(sdFolderMenu.selectedFile);
        Serial.print("from folder:");
        Serial.println(sdFolderMenu.folderName);
      }
      break;
  }
  return proceed;
}

//this limit also the folders depth we can nav
#define MAX_DEPTH 15

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(sdFolderMenu)
  ,OP("Something else...",doNothing,noEvent)
  ,EXIT("<Back")
);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.print("Initializing SD card...");
  if (!SD.begin(SDCARD_SS)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

constexpr int menuFPS=25;
unsigned long nextPool=0;
void loop() {
  unsigned long now=millis();
  if(now>=nextPool) {
    nav.poll();
    nextPool=now+1000/menuFPS;
  }
}
