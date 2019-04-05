#include <menu/def/tinyArduino.h>
#include <menu/IO/lcdOut.h>
#include <menu/printers.h>
#include <menu/fmt/debug.h>
#include <menu/comp/flashMenu.h>
#include <menu/panels.h>

// LCD /////////////////////////////////////////
#define RS 2
#define RW 4
#define EN A4
LiquidCrystal lcd(RS, RW, EN, A0, A1, A2, A3);

//menu output ------------------------
//by default its 16x2
using LCDRaw=LCDOutDev<decltype(lcd),lcd>;
//bind a format to the lcd
MenuOut<Menu::LCDFmt::To<LCDRaw>> lcdOut;

// normal option
Prompt<Op> op1("Op 1");

//option using flash text
const char op2_text[] PROGMEM="Op 2";
Prompt<FlashOp> op2(op2_text);

// Prompt<StaticMenu<2>> mainMenu("Main menu",&op1,&op2);
const char menuTitle_text[] PROGMEM="Main menu";
Prompt<FlashOp> menuTitle(menuTitle_text);
Prompt<Menu::FlashMenuDef<2,FlashText>> mainMenu(menuTitle_text,&op1,&op2);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"AM5 example ----"<<endl;
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcdOut<<"AM5 example ---";
  delay(1);
  lcd.clear();
  lcdOut.setTarget(mainMenu);
  lcdOut.printMenu();
}

bool keys(int key) {
  switch(key) {
    case '+': lcdOut.up();return true;
    case '-': lcdOut.down();return true;
    case '*': lcdOut.enter();return true;
    case '/': lcdOut.esc();return true;
  }
  return false;
}

void loop() {
  if (Serial.available()) {
    if (keys(Serial.read())) lcdOut.printMenu();
  }
}
