#include <menu.h>
#include <menu/IO/serialOut.h>
#include <menu/fmt/text.h>

using namespace Menu;

using Out=TextFmt<
  SerialOut<
    decltype(Serial),
    Serial,
    FullPrinter<>
  >
>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* menu_title="Main menu";

using MainMenu=StaticText<
  &menu_title,
  StaticList<
    StaticText<&op1_text>,
    StaticText<&op2_text>,
    StaticText<&op3_text>
  >
>;

NavNode<Out,MainMenu> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 test");
  nav.printMenu();//static menu
}

void loop() {}
