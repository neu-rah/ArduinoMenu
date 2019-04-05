#include <menu/def/tinyArduino.h>
#include <menu/nav.h>
#include <menu/printers.h>
#include <menu/fmt/text.h>
#include <menu/fmt/debug.h>
#include <menu/fmt/titleWrap.h>
#include <menu/comp/flashMenu.h>

template<typename O>
using WrapTitle=Menu::TitleWrapFmt<O>;

MenuOut<//menu output
  Menu::Chain<//wrap inner types
    Menu::DebugFmt,//add debug info when enabled
    Menu::TextFmt,//text format, insert \n at item or title end, etc...
    WrapTitle,//surround titles with []
    Menu::TitlePrinter,
    Menu::FullPrinter,//print inner then options
    Menu::NavNode,//flat navigation control (no sub menus)
    SerialOut//use arduino default Serial port
  >::To<//device parts to be used for panel|menu|title|item
    Menu::DeviceParts<
      //install format message emitter for items,titles,menu and panel, use Menu::ID to ommit the parts
      Menu::Chain<Menu::TextAccelPrinter,Menu::TextCursorPrinter,Menu::ItemPrinter>::To,//emit format messages for accel, cursor amd item
      Menu::TitlePrinter,//emit format messages for titles (fmtTitle)
      Menu::ID,//menu parfts (not used yet)
      Menu::ID//panel parts (not used yet)
    >
  >
> serialOut;

//normal option
Prompt<Text> op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
Prompt<FlashText> op2(op2_text);

// Prompt<StaticMenu<2>> mainMenu("Main menu",&op1,&op2);
const char menuTitle_text[] PROGMEM="Main menu";
Prompt<FlashText> menuTitle(menuTitle_text);
Prompt<Menu::FlashMenuDef<2,FlashText>> mainMenu(menuTitle_text,&op1,&op2);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<"AM5 tiny example ----"<<endl;
  serialOut<<"use keys +-*/"<<endl<<endl;
  serialOut.setTarget(mainMenu);
  serialOut.printMenu();
}

bool keys(int key) {
  switch(key) {
    case '+': serialOut.up();return true;
    case '-': serialOut.down();return true;
    case '*': serialOut.enter();return true;
    case '/': serialOut.esc();return true;
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) serialOut.printMenu();
  }
}
