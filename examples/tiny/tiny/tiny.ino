#include <menu/def/tinyArduino.h>

MenuOut<Menu::SerialFmt<>::To<SerialOutDev<>>> serialOut;

//normal option
Prompt<Text> op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
Prompt<FlashText> op2(op2_text);

// Prompt<StaticMenu<2>> mainMenu("Main menu",&op1,&op2);
const char menuTitle_text[] PROGMEM="Main menu";
Prompt<FlashText> menuTitle(menuTitle_text);
Prompt<FlashMenu<2>> mainMenu(menuTitle_text,&op1,&op2);

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
  if (Serial.available()&&keys(Serial.read()))
    serialOut.printMenu();
}
