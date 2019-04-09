#include <menu/def/tinyArduino.h>
#include <menu/IO/serialOut.h>

using namespace Menu;

//or accept the defauls
MenuOut<AM5::SerialFmt<>::To<SerialOutDev<>>> serialOut;

using Op=Prompt<FlashText>;

const char op1_text[] PROGMEM="Op 1";
Op op1(op1_text);

const char op2_text[] PROGMEM="Op 2";
Op op2(op2_text);

const char op3_text[] PROGMEM="Op 3";
Op op3(op3_text);

const char op4_text[] PROGMEM="Op 4";
Op op4(op4_text);

const char op5_text[] PROGMEM="Op 5";
Op op5(op5_text);

// Prompt<StaticMenu<5>> mainMenu("Main menu",&op1,&op2,&op3,&op4,&op5);
const char menuTitle_text[] PROGMEM="Main menu";
Prompt<FlashText> menuTitle(menuTitle_text);
constexpr AM5::FlashData data[5] {&op1,&op2,&op3,&op4,&op5};
Prompt<AM5::FlashMenuDef<data,5,FlashText>> mainMenu(menuTitle_text);

//footprint ----------------------
//4 bytes of ram for each flash text option (as is)
//  2 - flash text pointer
//  2 - vtable pointer

void setup() {
  Serial.begin(115200);
  while(!Serial);
  serialOut<<F("AM5 tiny example ----")<<endl;
  serialOut<<F("use keys +-*/")<<endl<<endl;
  serialOut.setTarget(mainMenu);
  serialOut.printMenu();
}

bool keys(int key) {
  switch(key) {
    case '+': return serialOut.up();
    case '-': return serialOut.down();
    case '*': return serialOut.enter();
    case '/': return serialOut.esc();
  }
  return false;
}

void loop() {
  if (Serial.available()&&keys(Serial.read()))
    serialOut.printMenu();
}
