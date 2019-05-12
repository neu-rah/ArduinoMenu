#include <menu.h>
#include <menu/IO/serialOut.h>

using Out=TextFmt<TitleWrap<SerialOut<>>>;

const char* op1_text="Op 1";
const char* op2_text="Op ...";
const char* op3_text="Op 3";
const char* mainMenu_title="Main menu";
using MainMenu=
  StaticText<
    &mainMenu_title,
    StaticMenu<
      StaticText<&op1_text>,
      StaticText<&op2_text>,
      StaticText<&op3_text>
    >
  >;

NavCap<StaticNav<Out,MainMenu,NavPos<>>> nav;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AM5 serial example");
  nav.printMenu();
}

//handle serial keys to navigate menu
bool keys(int key) {
  switch(key) {
    case '+': return nav.up();
    case '-': return nav.down();
    case '*': return nav.enter();
    case '/': return nav.esc();
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) nav.printMenu();
  }
}
