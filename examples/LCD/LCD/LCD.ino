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

// bind a format to the lcd
MenuOut<Menu::LCDFmt::To<LCDOutDev<lcd>/*by default its 16x2*/>> lcdOut;

// quick define menu
Prompt<StaticMenu<2>> mainMenu(
  "Main menu"
  ,new Prompt<Text>("Op 1")
  ,new Prompt<Text>("Op 2")
);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial<<"AM5 example ----"<<endl;
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcdOut<<"AM5 example ---";
  delay(300);
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
