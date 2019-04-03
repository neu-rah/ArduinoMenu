#include <menu/def/tinyArduino.h>
#include <menu/printer/full.h>
#include <menu/fmt/text.h>
#include <menu/fmt/debug.h>

//serial output
MenuOut<//menu part injection MUST occur here (top level)
  Menu::DebugFmt<//add debug info to output
    Menu::FullPrinter<//print innet then options
      Menu::TitlePrinter<//print the title
        Menu::TextFmt<//text format, insert \n at item or title end, etc...
          Menu::WrapTitle<//print title surrounded by []
            SerialOut//use arduino default Serial port
          >
        >
      >
    >
  >
> serialOut;

//normal option
Prompt<Op> op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
Prompt<FlashOp> op2(op2_text);

Prompt<StaticMenu<2>> mainMenu("Main menu",&op1,&op2);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<"AM5 tiny example ----"<<endl;
  serialOut.printMenu(mainMenu);
  serialOut<<"----"<<endl;
}

void loop() {}
