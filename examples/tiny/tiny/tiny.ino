#include <Dump.h>
#include <menu.h>
using namespace AM5;

PROGMEM ConstText op1_text="Op 1";
PROGMEM ConstText op2_text="Op 2";
PROGMEM ConstText op3_text="Op 3";

template<typename T,T text>
using Op=StaticFlashTextDef<T,text,SerialOutDef<Serial>>;

PROGMEM ConstText menu_title="Main menu";

using MainMenu=Cap<
  TextFmt<
    FullPrinterDef<
      NavPosDef<
        StaticFlashTextDef<decltype(&menu_title),&menu_title,
          StaticMenuDataDef<
            Op<decltype(&op1_text),&op1_text>,
            Op<decltype(&op2_text),&op2_text>,
            Op<decltype(&op3_text),&op3_text>
          >
        >
      >
    >
  >
>;

MainMenu mainMenu;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"AM5 Static demo"<<endl;
  dumpRam(Serial, op1_text, 16);
  dumpPgm(Serial, op1_text, 16);
  mainMenu.printMenu();
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '+': return mainMenu.up();;
    case '-': return mainMenu.down();;
    case '*': return mainMenu.enter();;
    case '/': return mainMenu.esc();;
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) mainMenu.printMenu();
  }
}
