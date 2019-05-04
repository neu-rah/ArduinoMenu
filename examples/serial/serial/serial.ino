#include <menu.h>
#include <menu/IO/serialOut.h>
#include <menu/fmt/text.h>
#include <menu/comp/endis.h>

/***************************************************************
2019.05.04 20:32 ATTiny45
DATA:    [====      ]  38.7% (used 99 bytes from 256 bytes)
PROGRAM: [===       ]  25.7% (used 1054 bytes from 4096 bytes)
****************************************************************/

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
    EnDis<StaticText<&op1_text>>,
    EnDis<StaticText<&op2_text>>,
    EnDis<StaticText<&op2_text>>,
    EnDis<StaticText<&op2_text>>,
    EnDis<StaticText<&op2_text>>,
    EnDis<StaticText<&op2_text>>,
    EnDis<StaticText<&op2_text>>,
    EnDis<StaticText<&op3_text>>
  >
>;

NavNode<Out,MainMenu,PosDef<>> nav;

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '\\':
      nav.enable<5>(!nav.enabled<5>());
      return true;
    case '+': return nav.up();;
    case '-': return nav.down();;
    case '*': return nav.enter();;
    case '/': return nav.esc();;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 test");
  nav.printMenu();//static menu
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) nav.printMenu();
  }
}
